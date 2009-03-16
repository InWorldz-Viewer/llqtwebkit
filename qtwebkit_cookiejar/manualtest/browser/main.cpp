/*
   Copyright (C) 2009 Torch Mobile Inc. http://www.torchmobile.com/
*/

/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is Torch Mobile Inc. (http://www.torchmobile.com/) code
 *
 * The Initial Developer of the Original Code is:
 *   Benjamin Meyer (benjamin.meyer@torchmobile.com)
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

#include <QtGui/QtGui>
#include <QtWebKit/QtWebKit>
#include <QtNetwork/QtNetwork>
#include <networkcookiejar.h>

QFile file;
QDataStream stream;

class CookieLog : public NetworkCookieJar {

    Q_OBJECT

public:
    CookieLog(QObject *parent = 0) : NetworkCookieJar(parent)
    {
        file.setFileName("cookie.log");
        file.open(QFile::WriteOnly);
        stream.setDevice(&file);
    };

    virtual QList<QNetworkCookie> cookiesForUrl(const QUrl & url) const
    {
        stream << QString("cookiesForUrl") << url;
        QList<QNetworkCookie> cookies = NetworkCookieJar::cookiesForUrl(url);
        //stream << "#" << cookies;
        file.flush();
        return cookies;
    }

    virtual bool setCookiesFromUrl(const QList<QNetworkCookie> &cookieList, const QUrl &url)
    {
        QByteArray data;
        QDataStream dstream(&data, QIODevice::ReadWrite);
        qint32 c = cookieList.count();
        dstream << c;
        qDebug() << cookieList.count();
        for (int i = 0; i < c; ++i)
            dstream << cookieList[i].toRawForm();
        dstream.device()->close();
        stream << QString("setCookiesFromUrl") << url << data;// << cookieList;
        bool set = NetworkCookieJar::setCookiesFromUrl(cookieList, url);
        file.flush();
        return set;
    }

};

int main(int argc, char**argv) {
    QApplication application(argc, argv);
    QWebView view;
    QString url = "http://www.google.com";
    if (argc > 1)
        url = argv[1];
    view.load(QUrl(url));
    view.page()->networkAccessManager()->setCookieJar(new CookieLog());
    view.show();
    return application.exec();
}

#include "main.moc"
