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
 * The Original Code is Linden Lab Inc. (http://lindenlab.com) code.
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

#ifndef LLEMBEDDEDBROWSER_P_H
#define LLEMBEDDEDBROWSER_P_H

#include <qnetworkaccessmanager.h>
#include <qapplication.h>
#if QT_VERSION >= 0x040500
#include <qnetworkdiskcache.h>
#endif

#include "networkcookiejar.h"
#include "llembeddedbrowser.h"

class LLNetworkCookieJar : public NetworkCookieJar
{
public:
    LLNetworkCookieJar(QObject *parent = 0);
    ~LLNetworkCookieJar();

    void load(const QString &filename);
    void save();

    QList<QNetworkCookie> cookiesForUrl(const QUrl& url) const;
    bool setCookiesFromUrl(const QList<QNetworkCookie> &cookie_list, const QUrl& url);
    void clear();

    QString mCookieStorageFileName;
    bool mAllowCookies;
};

class LLEmbeddedBrowserPrivate;
class LLNetworkAccessManager: public QNetworkAccessManager
{
    Q_OBJECT
public:
    LLNetworkAccessManager(LLEmbeddedBrowserPrivate* browser, QObject* parent = 0);

public slots:
    void finishLoading(QNetworkReply* reply);

private:
    LLEmbeddedBrowserPrivate* mBrowser;

};

class LLEmbeddedBrowserPrivate
{
public:
    LLEmbeddedBrowserPrivate();
    ~LLEmbeddedBrowserPrivate();

    int mErrorNum;
    void* mNativeWindowHandle;
    LLNetworkAccessManager *mNetworkAccessManager;
    QApplication *mApplication;
#if QT_VERSION >= 0x040500
    QNetworkDiskCache *mDiskCache;
#endif
    LLNetworkCookieJar *mNetworkCookieJar;

    QString mStorageDirectory;
    QList<LLEmbeddedBrowserWindow*> windows;
};

#endif

