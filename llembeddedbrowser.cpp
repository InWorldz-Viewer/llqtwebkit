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
 *   Callum Prentice (callum@ubrowser.com)
 *
 * Portions created by the Initial Developer are Copyright (C) 2006
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *  Callum Prentice (callum@ubrowser.com)
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

#include "llembeddedbrowser.h"

#include "llembeddedbrowser_p.h"
#include "llembeddedbrowserwindow.h"

// singleton pattern - initialization
LLEmbeddedBrowser* LLEmbeddedBrowser::sInstance = 0;

LLEmbeddedBrowser::LLEmbeddedBrowser()
{
    d = new LLEmbeddedBrowserPrivate;
}

LLEmbeddedBrowser::~LLEmbeddedBrowser()
{
    delete d;
}

LLEmbeddedBrowser* LLEmbeddedBrowser::getInstance()
{
    if (!sInstance)
        sInstance = new LLEmbeddedBrowser;
    return sInstance;
};

void LLEmbeddedBrowser::setLastError( int errorNumIn )
{
    d->mErrorNum = errorNumIn;
}

void LLEmbeddedBrowser::clearLastError()
{
    d->mErrorNum = 0x0000;
}

int LLEmbeddedBrowser::getLastError()
{
    return d->mErrorNum;
}

std::string LLEmbeddedBrowser::getGREVersion()
{
    // take the string directly from Qt
    return std::string(QT_VERSION_STR);
}

bool LLEmbeddedBrowser::init(std::string applicationDir,
                             std::string componentDir,
                             std::string profileDir,
                             void* nativeWindowHandleIn)
{
    Q_UNUSED(componentDir);
    Q_UNUSED(profileDir);
    Q_UNUSED(nativeWindowHandleIn);
    QWebSettings::setIconDatabasePath(QString::fromStdString(applicationDir));

    // Until QtWebkit defaults to 16
    QWebSettings::globalSettings()->setFontSize(QWebSettings::DefaultFontSize, 16);
    QWebSettings::globalSettings()->setFontSize(QWebSettings::DefaultFixedFontSize, 16);

#if QT_VERSION >= 0x040500
    d->diskCache = new QNetworkDiskCache(&d->networkAccessManager);
    d->diskCache->setCacheDirectory(QString::fromStdString(applicationDir));
    d->networkAccessManager.setCache(d->diskCache);
#endif
    d->networkCookieJar = new NetworkCookieJar(&d->networkAccessManager);
    d->networkAccessManager.setCookieJar(d->networkCookieJar);
    clearLastError();
    return true;
}

// What should reset do?
bool LLEmbeddedBrowser::reset()
{
    qWarning() << __FUNCTION__ << "not implemented";
    return true;
}

bool LLEmbeddedBrowser::clearCache()
{
#if QT_VERSION >= 0x040500
    d->diskCache->clear();
    return true;
#else
	return false;
#endif
}

bool LLEmbeddedBrowser::enableProxy( bool proxyEnabledIn, std::string proxyHostNameIn, int proxyPortIn )
{
    QNetworkProxy proxy;
    if (proxyEnabledIn) {
        proxy.setType(QNetworkProxy::HttpProxy);
        QString hostName = QString::fromStdString(proxyHostNameIn);
        proxy.setHostName(hostName);
        proxy.setPort(proxyPortIn);
        //proxy.setUser(settings.value(QLatin1String("userName")).toString());
        //proxy.setPassword(settings.value(QLatin1String("password")).toString());
    }
    d->networkAccessManager.setProxy(proxy);
    return true;
}

bool LLEmbeddedBrowser::enableCookies( bool enabledIn )
{
    Q_UNUSED(enabledIn);
    qWarning() << "LLEmbeddedBrowser::enableCookies()" << "not implemented";
    /*nsCOMPtr<nsIPref> pref = do_CreateInstance( NS_PREF_CONTRACTID );
    if ( pref )
    {
        if ( enabledIn )
            pref->SetIntPref( "network.cookie.cookieBehavior", 0 );
        else
            pref->SetIntPref( "network.cookie.cookieBehavior", 2 );

        return true;
    }
    */
    return false;
}

bool LLEmbeddedBrowser::clearAllCookies()
{
    d->networkCookieJar->clear();
    return true;
}

bool LLEmbeddedBrowser::enablePlugins( bool enabledIn )
{
    QWebSettings *defaultSettings = QWebSettings::globalSettings();
    defaultSettings->setAttribute(QWebSettings::PluginsEnabled, enabledIn);
    return true;
}

void LLEmbeddedBrowser::setBrowserAgentId( std::string idIn )
{
    d->userAgentString = QString::fromStdString(idIn);
}

LLEmbeddedBrowserWindow* LLEmbeddedBrowser::createBrowserWindow(int browserWidthIn, int browserHeightIn)
{
    LLEmbeddedBrowserWindow *newWin = new LLEmbeddedBrowserWindow();
    if (newWin) {
        newWin->setSize(browserWidthIn, browserHeightIn);
        newWin->setParent(this);
        clearLastError();
        return newWin;
    };
    return 0;
}

bool LLEmbeddedBrowser::destroyBrowserWindow(LLEmbeddedBrowserWindow *browserWindowIn)
{
    delete browserWindowIn;
    clearLastError();
    return true;
}

NetworkCookieJar::NetworkCookieJar(QObject *parent)
    : QNetworkCookieJar(parent)
{
}

QList<QNetworkCookie> NetworkCookieJar::cookiesForUrl(const QUrl &url) const
{
    if (!allowCookies)
        return QList<QNetworkCookie>();
    return QNetworkCookieJar::cookiesForUrl(url);
}

bool NetworkCookieJar::setCookiesFromUrl(const QList<QNetworkCookie> &cookieList, const QUrl &url)
{
    if (!allowCookies)
        return false;
    return QNetworkCookieJar::setCookiesFromUrl(cookieList, url);
}

void NetworkCookieJar::clear() {
    setAllCookies(QList<QNetworkCookie>());
}

