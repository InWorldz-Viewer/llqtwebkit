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

#include <sstream>
#include <iostream>
#include <iomanip>
#include <time.h>

#include "llqtwebkit.h"

#include "llembeddedbrowser.h"
#include "llembeddedbrowserwindow.h"

LLQtWebKit* LLQtWebKit::sInstance = 0;

////////////////////////////////////////////////////////////////////////////////
//
LLQtWebKit::LLQtWebKit() :
    mMaxBrowserWindows(16)
{
}

////////////////////////////////////////////////////////////////////////////////
//
LLQtWebKit* LLQtWebKit::getInstance()
{
    if (! sInstance)
    {
        sInstance = new LLQtWebKit;
    }

    return sInstance;
}

////////////////////////////////////////////////////////////////////////////////
//
LLQtWebKit::~LLQtWebKit()
{
}

////////////////////////////////////////////////////////////////////////////////
//
bool LLQtWebKit::init(std::string application_directory,
                          std::string component_directory,
                          std::string profile_directory,
                          void* native_window_handle)
{
    return LLEmbeddedBrowser::getInstance()->init(application_directory,
                                                  component_directory,
                                                  profile_directory,
                                                  native_window_handle);
}

////////////////////////////////////////////////////////////////////////////////
//
int LLQtWebKit::getLastError()
{
    return LLEmbeddedBrowser::getInstance()->getLastError();
}

////////////////////////////////////////////////////////////////////////////////
//
bool LLQtWebKit::reset()
{
    mBrowserWindowMap.clear();
    return LLEmbeddedBrowser::getInstance()->reset();
}

////////////////////////////////////////////////////////////////////////////////
//
bool LLQtWebKit::clearCache()
{
    return LLEmbeddedBrowser::getInstance()->clearCache();
}

////////////////////////////////////////////////////////////////////////////////
//
std::string LLQtWebKit::getVersion()
{
    const int majorVersion = 2;
    const int minorVersion = 1;

    // number of hours since "time began" for this library - used to identify builds of same version
    const int magicNumber = static_cast< int >((time(NULL) / 3600L) - (321190L));

    // return as a string for now - don't think we need to expose actual version numbers
    std::ostringstream codec;
    codec << std::setw(1) << std::setfill('0');
    codec << majorVersion << ".";
    codec << std::setw(2) << std::setfill('0');
    codec << minorVersion << ".";
    codec << std::setw(5) << std::setfill('0');
    codec << magicNumber;
    codec << " (QtWebKit version ";
    codec << LLEmbeddedBrowser::getInstance()->getGREVersion();
    codec << ")";

    return codec.str();
}

////////////////////////////////////////////////////////////////////////////////
//
void LLQtWebKit::setBrowserAgentId(std::string id)
{
    LLEmbeddedBrowser::getInstance()->setBrowserAgentId(id);
}

////////////////////////////////////////////////////////////////////////////////
//
bool LLQtWebKit::enableProxy(bool enabled, std::string host_name, int port)
{
    return LLEmbeddedBrowser::getInstance()->enableProxy(enabled, host_name, port);
}

////////////////////////////////////////////////////////////////////////////////
//
int LLQtWebKit::createBrowserWindow(int width, int height)
{
    LLEmbeddedBrowserWindow* browser_window = LLEmbeddedBrowser::getInstance()->createBrowserWindow(width, height);

    if (browser_window)
    {
        // arbitrary limit so we don't exhaust system resources
        int id(0);
        while (++id < mMaxBrowserWindows)
        {
            std::pair< BrowserWindowMapIter, bool > result = mBrowserWindowMap.insert(std::make_pair(id, browser_window));

            // find first place the insert succeeds and use that index as the id
            if (result.second)
            {
                browser_window->setWindowId(id);
                return id;
            }
        }
    }

    return 0;
}

////////////////////////////////////////////////////////////////////////////////
//
bool LLQtWebKit::destroyBrowserWindow(int browser_window_id)
{
    // don't use the utility method here since we need the iteratorator to remove the entry from the map
    BrowserWindowMapIter iterator = mBrowserWindowMap.find(browser_window_id);
    LLEmbeddedBrowserWindow* browser_window = (*iterator).second;

    if (browser_window)
    {
        LLEmbeddedBrowser::getInstance()->destroyBrowserWindow(browser_window);
    }

    mBrowserWindowMap.erase(iterator);

    return true;
}

////////////////////////////////////////////////////////////////////////////////
//
bool LLQtWebKit::setBackgroundColor(int browser_window_id, const int red, const int green, const int blue)
{
    LLEmbeddedBrowserWindow* browser_window = getBrowserWindowFromWindowId(browser_window_id);
    if (browser_window)
    {
        browser_window->setBackgroundColor(red, green, blue);
        return true;
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////
//
bool LLQtWebKit::setCaretColor(int browser_window_id, const int red, const int green, const int blue)
{
    LLEmbeddedBrowserWindow* browser_window = getBrowserWindowFromWindowId(browser_window_id);
    if (browser_window)
    {
        browser_window->setCaretColor(red, green, blue);
        return true;
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////
//
bool LLQtWebKit::setEnabled(int browser_window_id, bool enabled)
{
    LLEmbeddedBrowserWindow* browser_window = getBrowserWindowFromWindowId(browser_window_id);
    if (browser_window)
    {
        browser_window->setEnabled(enabled);
        return true;
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////
//
bool LLQtWebKit::setSize(int browser_window_id, int width, int height)
{
    LLEmbeddedBrowserWindow* browser_window = getBrowserWindowFromWindowId(browser_window_id);
    if (browser_window)
    {
        browser_window->setSize(width, height);
        return true;
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////
//
bool LLQtWebKit::scrollByLines(int browser_window_id, int lines)
{
    LLEmbeddedBrowserWindow* browser_window = getBrowserWindowFromWindowId(browser_window_id);
    if (browser_window)
    {
        browser_window->scrollByLines(lines);
        return true;
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////
//
bool LLQtWebKit::addObserver(int browser_window_id, LLEmbeddedBrowserWindowObserver* subject)
{
    LLEmbeddedBrowserWindow* browser_window = getBrowserWindowFromWindowId(browser_window_id);
    if (browser_window)
    {
        browser_window->addObserver(subject);
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////
//
bool LLQtWebKit::remObserver(int browser_window_id, LLEmbeddedBrowserWindowObserver* subject)
{
    LLEmbeddedBrowserWindow* browser_window = getBrowserWindowFromWindowId(browser_window_id);
    if (browser_window)
    {
        browser_window->remObserver(subject);
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////
//
bool LLQtWebKit::navigateTo(int browser_window_id, const std::string uri)
{
    LLEmbeddedBrowserWindow* browser_window = getBrowserWindowFromWindowId(browser_window_id);
    if (browser_window)
    {
        return browser_window->navigateTo(uri) ? true : false;
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////
//
bool LLQtWebKit::userAction(int browser_window_id, EUserAction action)
{
    LLEmbeddedBrowserWindow* browser_window = getBrowserWindowFromWindowId(browser_window_id);
    if (browser_window)
    {
        return browser_window->userAction(action);
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////
//
bool LLQtWebKit::userActionIsEnabled(int browser_window_id, EUserAction action)
{
    LLEmbeddedBrowserWindow* browser_window = getBrowserWindowFromWindowId(browser_window_id);
    if (browser_window)
    {
        return browser_window->userActionIsEnabled(action);
    }
    return false;
}

///////////////////////////////////////////////////////////////////////////////
//
const unsigned char* LLQtWebKit::grabBrowserWindow(int browser_window_id)
{
    LLEmbeddedBrowserWindow* browser_window = getBrowserWindowFromWindowId(browser_window_id);
    if (browser_window)
    {
        return browser_window->grabWindow(0, 0, browser_window->getBrowserWidth(), browser_window->getBrowserHeight());
    }

    return 0;
}

////////////////////////////////////////////////////////////////////////////////
//
const unsigned char* LLQtWebKit::getBrowserWindowPixels(int browser_window_id)
{
    LLEmbeddedBrowserWindow* browser_window = getBrowserWindowFromWindowId(browser_window_id);
    if (browser_window)
    {
        return browser_window->getPageBuffer();
    }

    return 0;
}

////////////////////////////////////////////////////////////////////////////////
//
bool LLQtWebKit::flipWindow(int browser_window_id, bool flip)
{
    LLEmbeddedBrowserWindow* browser_window = getBrowserWindowFromWindowId(browser_window_id);
    if (browser_window)
    {
        browser_window->flipWindow(flip);
        return true;
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////
//
int LLQtWebKit::getBrowserWidth(int browser_window_id)
{
    LLEmbeddedBrowserWindow* browser_window = getBrowserWindowFromWindowId(browser_window_id);
    if (browser_window)
    {
        return browser_window->getBrowserWidth();
    }

    return 0;
}

////////////////////////////////////////////////////////////////////////////////
//
int LLQtWebKit::getBrowserHeight(int browser_window_id)
{
    LLEmbeddedBrowserWindow* browser_window = getBrowserWindowFromWindowId(browser_window_id);
    if (browser_window)
    {
        return browser_window->getBrowserHeight();
    }

    return 0;
}

////////////////////////////////////////////////////////////////////////////////
//
int LLQtWebKit::getBrowserDepth(int browser_window_id)
{
    LLEmbeddedBrowserWindow* browser_window = getBrowserWindowFromWindowId(browser_window_id);
    if (browser_window)
    {
        return browser_window->getBrowserDepth();
    }

    return 0;
}

////////////////////////////////////////////////////////////////////////////////
//
int LLQtWebKit::getBrowserRowSpan(int browser_window_id)
{
    LLEmbeddedBrowserWindow* browser_window = getBrowserWindowFromWindowId(browser_window_id);
    if (browser_window)
    {
        return browser_window->getBrowserRowSpan();
    }

    return 0;
}

////////////////////////////////////////////////////////////////////////////////
//
bool LLQtWebKit::mouseDown(int browser_window_id, int x, int y)
{
    LLEmbeddedBrowserWindow* browser_window = getBrowserWindowFromWindowId(browser_window_id);
    if (browser_window)
    {
        browser_window->mouseDown(x, y);
        return true;
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////
//
bool LLQtWebKit::mouseUp(int browser_window_id, int x, int y)
{
    LLEmbeddedBrowserWindow* browser_window = getBrowserWindowFromWindowId(browser_window_id);
    if (browser_window)
    {
        browser_window->mouseUp(x, y);
        return true;
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////
//
bool LLQtWebKit::mouseMove(int browser_window_id, int x, int y)
{
    LLEmbeddedBrowserWindow* browser_window = getBrowserWindowFromWindowId(browser_window_id);
    if (browser_window)
    {
        browser_window->mouseMove(x, y);
        return true;
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////
//
bool LLQtWebKit::mouseLeftDoubleClick(int browser_window_id, int x, int y)
{
    LLEmbeddedBrowserWindow* browser_window = getBrowserWindowFromWindowId(browser_window_id);
    if (browser_window)
    {
        browser_window->mouseLeftDoubleClick(x, y);
        return true;
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////
//
bool LLQtWebKit::keyPress(int browser_window_id, int key_code)
{
    LLEmbeddedBrowserWindow* browser_window = getBrowserWindowFromWindowId(browser_window_id);
    if (browser_window)
    {
        browser_window->keyPress(key_code);
        return true;
    }

    return false;
}

bool LLQtWebKit::unicodeInput(int browser_window_id, unsigned long uni_char)
{
    LLEmbeddedBrowserWindow* browser_window = getBrowserWindowFromWindowId(browser_window_id);
    if (browser_window)
    {
        browser_window->unicodeInput(uni_char);
        return true;
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////
//
bool LLQtWebKit::focusBrowser(int browser_window_id, bool focus_browser)
{
    LLEmbeddedBrowserWindow* browser_window = getBrowserWindowFromWindowId(browser_window_id);
    if (browser_window)
    {
        browser_window->focusBrowser(focus_browser);
        return true;
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////
//
void LLQtWebKit::setNoFollowScheme(int browser_window_id, std::string scheme)
{
    LLEmbeddedBrowserWindow* browser_window = getBrowserWindowFromWindowId(browser_window_id);
    if (browser_window)
    {
        browser_window->setNoFollowScheme(scheme);
    }
}

////////////////////////////////////////////////////////////////////////////////
//
std::string LLQtWebKit::getNoFollowScheme(int browser_window_id)
{
    LLEmbeddedBrowserWindow* browser_window = getBrowserWindowFromWindowId(browser_window_id);
    if (browser_window)
    {
        return browser_window->getNoFollowScheme();
    }

    return ("");
}

////////////////////////////////////////////////////////////////////////////////
//
void LLQtWebKit::pump(int max_milliseconds)
{
    LLEmbeddedBrowser::getInstance()->pump(max_milliseconds);
}

////////////////////////////////////////////////////////////////////////////////
//
bool LLQtWebKit::enableCookies(bool enabled)
{
    return LLEmbeddedBrowser::getInstance()->enableCookies(enabled);
}

////////////////////////////////////////////////////////////////////////////////
//
bool LLQtWebKit::clearAllCookies()
{
    return LLEmbeddedBrowser::getInstance()->clearAllCookies();
}

////////////////////////////////////////////////////////////////////////////////
//
bool LLQtWebKit::enablePlugins(bool enabled)
{
    return LLEmbeddedBrowser::getInstance()->enablePlugins(enabled);
}

////////////////////////////////////////////////////////////////////////////////
//
std::string LLQtWebKit::evaluateJavascript(int browser_window_id, const std::string script)
{
    LLEmbeddedBrowserWindow* browser_window = getBrowserWindowFromWindowId(browser_window_id);
    if (browser_window)
    {
        return browser_window->evaluateJavascript(script);
    }

    return "";
}

////////////////////////////////////////////////////////////////////////////////
//
bool LLQtWebKit::set404RedirectUrl(int browser_window_in, std::string redirect_url)
{
    LLEmbeddedBrowserWindow* browser_window = getBrowserWindowFromWindowId(browser_window_in);
    if (browser_window)
    {
        browser_window->set404RedirectUrl(redirect_url);
        return true;
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////
//
bool LLQtWebKit::clr404RedirectUrl(int browser_window_in)
{
    LLEmbeddedBrowserWindow* browser_window = getBrowserWindowFromWindowId(browser_window_in);
    if (browser_window)
    {
        browser_window->clr404RedirectUrl();
        return true;
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////
//
void LLQtWebKit::prependHistoryUrl(int browser_window_id, std::string url)
{
    LLEmbeddedBrowserWindow* browser_window = getBrowserWindowFromWindowId(browser_window_id);
    if (browser_window)
    {
        browser_window->prependHistoryUrl(url);
    }
}

////////////////////////////////////////////////////////////////////////////////
//
void LLQtWebKit::clearHistory(int browser_window_id)
{
	LLEmbeddedBrowserWindow* browser_window = getBrowserWindowFromWindowId(browser_window_id);
	if (browser_window)
	{
		browser_window->clearHistory();
	}
}

std::string LLQtWebKit::dumpHistory(int browser_window_id)
{
	LLEmbeddedBrowserWindow* browser_window = getBrowserWindowFromWindowId(browser_window_id);
	if (browser_window)
	{
		return browser_window->dumpHistory();
	}

	return NULL;
}


////////////////////////////////////////////////////////////////////////////////
// utility method to get an LLEmbeddedBrowserWindow* from a window id (int)
LLEmbeddedBrowserWindow* LLQtWebKit::getBrowserWindowFromWindowId(int browser_window_id)
{
    BrowserWindowMapIter iterator = mBrowserWindowMap.find(browser_window_id);

    if (iterator != mBrowserWindowMap.end())
        return (*iterator).second;
    else
        return 0;
}

LLEmbeddedBrowserWindowObserver::~LLEmbeddedBrowserWindowObserver()
{
}

void LLEmbeddedBrowserWindowObserver::onCursorChanged(const EventType&)
{
}

void LLEmbeddedBrowserWindowObserver::onPageChanged(const EventType&)
{
}

void LLEmbeddedBrowserWindowObserver::onNavigateBegin(const EventType&)
{
}

void LLEmbeddedBrowserWindowObserver::onNavigateComplete(const EventType&)
{
}

void LLEmbeddedBrowserWindowObserver::onUpdateProgress(const EventType&)
{
}

void LLEmbeddedBrowserWindowObserver::onStatusTextChange(const EventType&)
{
}

void LLEmbeddedBrowserWindowObserver::onLocationChange(const EventType&)
{
}

void LLEmbeddedBrowserWindowObserver::onClickLinkHref(const EventType&)
{
}

void LLEmbeddedBrowserWindowObserver::onClickLinkNoFollow(const EventType&)
{
}

