/*
   Copyright (C) 2008 Torch Mobile Inc. http://www.torchmobile.com/
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
 *  Benjamin Meyer (benjamin.meyer@torchmobile.com)
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

#include <QtCore/qvariant.h>

// singleton pattern - initialization
LLEmbeddedBrowser* LLEmbeddedBrowser::sInstance = 0;

LLEmbeddedBrowser::LLEmbeddedBrowser()
    : d(new LLEmbeddedBrowserPrivate)
{
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
}

void LLEmbeddedBrowser::setLastError(int error_number)
{
    d->mErrorNum = error_number;
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

bool LLEmbeddedBrowser::init(std::string application_directory,
                             std::string component_directory,
                             std::string profile_directory,
                             void* native_window_handle)
{
    Q_UNUSED(component_directory);
    Q_UNUSED(profile_directory);
    Q_UNUSED(native_window_handle);
    QWebSettings::setIconDatabasePath(QString::fromStdString(application_directory));

    // Until QtWebkit defaults to 16
    QWebSettings::globalSettings()->setFontSize(QWebSettings::DefaultFontSize, 16);
    QWebSettings::globalSettings()->setFontSize(QWebSettings::DefaultFixedFontSize, 16);

    d->mApplicationDirectory = QString::fromStdString(application_directory);
    return reset();
}

bool LLEmbeddedBrowser::reset()
{
    foreach(LLEmbeddedBrowserWindow *window, d->windows)
        delete window;
    d->windows.clear();
    delete d->mNetworkAccessManager;
    d->mNetworkAccessManager = new LLNetworkAccessManager(d);
#if QT_VERSION >= 0x040500
    d->mDiskCache = new QNetworkDiskCache(d->mNetworkAccessManager);
    QString location = QDesktopServices::storageLocation(QDesktopServices::CacheLocation)
                            + QLatin1String("/browser");
    d->mDiskCache->setCacheDirectory(location);
    d->mNetworkAccessManager->setCache(d->mDiskCache);
#endif
    d->mNetworkCookieJar = new LLNetworkCookieJar(d->mNetworkAccessManager);
    d->mNetworkAccessManager->setCookieJar(d->mNetworkCookieJar);
    clearLastError();
    return true;
}

bool LLEmbeddedBrowser::clearCache()
{
#if QT_VERSION >= 0x040500
    if (d->mDiskCache)
    {
        d->mDiskCache->clear();
        return true;
    }
#endif
    return false;
}

bool LLEmbeddedBrowser::enableProxy(bool enabled, std::string host_name, int port)
{
    QNetworkProxy proxy;
    if (enabled)
    {
        proxy.setType(QNetworkProxy::HttpProxy);
        QString q_host_name = QString::fromStdString(host_name);
        proxy.setHostName(q_host_name);
        proxy.setPort(port);
    }
    d->mNetworkAccessManager->setProxy(proxy);
    return true;
}

bool LLEmbeddedBrowser::enableCookies(bool enabled)
{
    if (!d->mNetworkCookieJar)
        return false;
    d->mNetworkCookieJar->mAllowCookies = enabled;
    return false;
}

bool LLEmbeddedBrowser::clearAllCookies()
{
    if (!d->mNetworkCookieJar)
        return false;
    d->mNetworkCookieJar->clear();
    return true;
}

bool LLEmbeddedBrowser::enablePlugins(bool enabled)
{
    QWebSettings *defaultSettings = QWebSettings::globalSettings();
    defaultSettings->setAttribute(QWebSettings::PluginsEnabled, enabled);
    return true;
}

void LLEmbeddedBrowser::setBrowserAgentId(std::string id)
{
    d->mUserAgentString = QString::fromStdString(id);
}

LLEmbeddedBrowserWindow* LLEmbeddedBrowser::createBrowserWindow(int width, int height)
{
    LLEmbeddedBrowserWindow *newWin = new LLEmbeddedBrowserWindow();
    if (newWin)
    {
        newWin->setSize(width, height);
        newWin->setParent(this);
        clearLastError();
        d->windows.append(newWin);
        return newWin;
    }
    return 0;
}

bool LLEmbeddedBrowser::destroyBrowserWindow(LLEmbeddedBrowserWindow* browser_window)
{
    // check if exists in windows list
    if (d->windows.removeOne(browser_window))
    {
        delete browser_window;
        clearLastError();
        return true;
    }
    return false;
}

int LLEmbeddedBrowser::getWindowCount() const
{
    return d->windows.size();
}

void LLEmbeddedBrowser::pump(int max_milliseconds)
{
    qApp->processEvents(QEventLoop::AllEvents, max_milliseconds);
}

LLNetworkCookieJar::LLNetworkCookieJar(QObject* parent)
    : NetworkCookieJar(parent)
    , mAllowCookies(true)
{
}

QList<QNetworkCookie> LLNetworkCookieJar::cookiesForUrl(const QUrl& url) const
{
    if (!mAllowCookies)
        return QList<QNetworkCookie>();
    return NetworkCookieJar::cookiesForUrl(url);
}

bool LLNetworkCookieJar::setCookiesFromUrl(const QList<QNetworkCookie> &cookie_list, const QUrl& url)
{
    if (!mAllowCookies)
        return false;
    return NetworkCookieJar::setCookiesFromUrl(cookie_list, url);
}

void LLNetworkCookieJar::clear()
{
    setAllCookies(QList<QNetworkCookie>());
}

LLNetworkAccessManager::LLNetworkAccessManager(LLEmbeddedBrowserPrivate* browser,QObject* parent)
    : QNetworkAccessManager(parent)
    , mBrowser(browser)
{
    connect(this, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(finishLoading(QNetworkReply*)));
}

void LLNetworkAccessManager::finishLoading(QNetworkReply* reply)
{
    if (reply->error() == QNetworkReply::ContentNotFoundError)
    {
        QString url = QString(reply->url().toEncoded());
        if (mBrowser)
        {
            std::string current_url = url.toStdString();
            foreach (LLEmbeddedBrowserWindow *window, mBrowser->windows)
            {
                if (window->getCurrentUri() == current_url)
                    window->load404RedirectUrl();
            }
        }
    }
}


