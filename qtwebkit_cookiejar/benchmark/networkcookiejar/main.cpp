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

#include <QtTest/QtTest>
#include <QtNetwork/QtNetwork>
#include <networkcookiejar.h>

class CookieJarBenchmark: public QObject {
    Q_OBJECT

private slots:
    void setCookiesFromUrl();
    void cookiesForUrl();
    void player();

private:
    QNetworkCookieJar *getJar(bool populate = true);
    QList<QNetworkCookie> generateCookies(int size);
};

QNetworkCookieJar *CookieJarBenchmark::getJar(bool populate)
{
    QNetworkCookieJar *jar;
    if (qgetenv("JAR") == "CookieJar") {
        jar = new NetworkCookieJar(this);
    } else {
        jar = new QNetworkCookieJar(this);
    }
    if (!populate)
        return jar;

    // pre populate
    for (int i = 0; i < 500; ++i) {
        QList<QNetworkCookie> cookies = generateCookies(1);
        QUrl url = QUrl(QString("http://%1").arg(cookies[0].domain()));
        jar->setCookiesFromUrl(cookies, url);
    }

    return jar;
}

QList<QNetworkCookie> CookieJarBenchmark::generateCookies(int size)
{
    QList<QNetworkCookie> cookies;
    for (int i = 0; i < size; ++i) {
        QNetworkCookie cookie;

        QString tld;
        int c = qrand() % 3;
        if (c == 0) tld = "com";
        if (c == 1) tld = "net";
        if (c == 2) tld = "org";

        QString mid;
        int size = qrand() % 6 + 3;
        while (mid.count() < size)
            mid += QString(QChar::fromAscii(qrand() % 26 + 65));

        QString sub;
        c = qrand() % 3;
        if (c == 0) sub = ".";
        if (c == 1) sub = ".www.";
        if (c == 2) sub = ".foo";

        cookie.setDomain(QString("%1%2.%3").arg(sub).arg(mid).arg(tld));
        cookie.setName("a");
        cookie.setValue("b");
        cookie.setPath("/");
        cookies.append(cookie);
    }
    return cookies;
}

void CookieJarBenchmark::setCookiesFromUrl()
{
    QNetworkCookieJar *jar = getJar();
    QList<QNetworkCookie> cookies = generateCookies(1);
    QUrl url = QUrl(QString("http://%1").arg(cookies[0].domain()));
    QBENCHMARK {
        jar->setCookiesFromUrl(cookies, url);
    }
    delete jar;
}

void CookieJarBenchmark::cookiesForUrl()
{
    QNetworkCookieJar *jar = getJar();
    QList<QNetworkCookie> cookies = generateCookies(1);
    cookies[0].setDomain("www.foo.tld");
    QUrl url = QUrl("http://www.foo.tld");
    //QUrl url = QUrl(QString("http://foo%1/").arg(cookies[0].domain()));
    jar->setCookiesFromUrl(cookies, url);
    //qDebug() << cookies << url;
    int c = 0;
    QBENCHMARK {
        c += jar->cookiesForUrl(url).count();
    }
    delete jar;
}

// Grab the cookie.log file from the manualtest/browser directory
void CookieJarBenchmark::player()
{
    QBENCHMARK {
    QFile file("cookie.log");
    file.open(QFile::ReadOnly);
    QDataStream stream(&file);
    QNetworkCookieJar *jar = getJar(false);
    while (!stream.atEnd()) {
        QString command;
        QUrl url;
        stream >> command;
        stream >> url;
        //qDebug() << command << url;
        if (command == "cookiesForUrl") {
            jar->cookiesForUrl(url);
        }
        if (command == "setCookiesFromUrl") {
            QByteArray data;
            stream >> data;
            QDataStream dstream(&data, QIODevice::ReadWrite);
            qint32 c;
            dstream >> c;
            QList<QNetworkCookie> cookies;
            for (int i = 0; i < c; ++i) {
                QByteArray text;
                dstream >> text;
                cookies += QNetworkCookie::parseCookies(text);
            }
            jar->setCookiesFromUrl(cookies, url);
        }
    }
    }
}

QTEST_MAIN(CookieJarBenchmark)
#include "main.moc"
