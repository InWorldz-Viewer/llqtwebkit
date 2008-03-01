/**
 * @file llmozlib.cpp
 * @brief Linden Lab Mozilla wrapper implementation.
 *
 * $LicenseInfo:firstyear=2006&license=viewergpl$
 *
 * Copyright (c) 2006-2007, Linden Research, Inc.
 *
 * Second Life Viewer Source Code
 * The source code in this file ("Source Code") is provided by Linden Lab
 * to you under the terms of the GNU General Public License, version 2.0
 * ("GPL"), unless you have obtained a separate licensing agreement
 * ("Other License"), formally executed by you and Linden Lab.  Terms of
 * the GPL can be found in doc/GPL-license.txt in this distribution, or
 * online at http://secondlife.com/developers/opensource/gplv2
 *
 * There are special exceptions to the terms and conditions of the GPL as
 * it is applied to this Source Code. View the full text of the exception
 * in the file doc/FLOSS-exception.txt in this software distribution, or
 * online at http://secondlife.com/developers/opensource/flossexception
 *
 * By copying, modifying or distributing this software, you acknowledge
 * that you have read and understood your obligations described above,
 * and agree to abide by those obligations.
 *
 * ALL LINDEN LAB SOURCE CODE IS PROVIDED "AS IS." LINDEN LAB MAKES NO
 * WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY,
 * COMPLETENESS OR PERFORMANCE.
 * $/LicenseInfo$
 */

#include "llembeddedbrowser.h"
#include "llembeddedbrowserwindow.h"

LLMozLib* LLMozLib::sInstance = 0;

////////////////////////////////////////////////////////////////////////////////
//
LLMozLib::LLMozLib() :
	mMaxBrowserWindows( 17 )
{
	// NOTE: browserWindowIds start at 1.  This is so that 0 can be reserved as a null ID.
	// 		mMaxBrowserWindows is actually used as the max ID that will be assigned,
	//		so the max number of windows will be one less than mMaxBrowserWindows.
	//		This wastes one pointer at the beginning of the array, but greatly simplifies the code.
	typedef LLEmbeddedBrowserWindow* LLEmbeddedBrowserWindowPtr;
	mBrowserWindowList = new LLEmbeddedBrowserWindowPtr[mMaxBrowserWindows];
	memset(mBrowserWindowList, 0, mMaxBrowserWindows * sizeof(LLEmbeddedBrowserWindowPtr));
}

////////////////////////////////////////////////////////////////////////////////
//
LLMozLib* LLMozLib::getInstance()
{
	if ( ! sInstance )
	{
		sInstance = new LLMozLib;
	};

	return sInstance;
}

////////////////////////////////////////////////////////////////////////////////
//
LLMozLib::~LLMozLib()
{
	delete[] mBrowserWindowList;
	mBrowserWindowList = NULL;
}

////////////////////////////////////////////////////////////////////////////////
//
bool LLMozLib::init( std::string applicationDirIn, std::string componentDirIn, std::string profileDirIn )
{
	return LLEmbeddedBrowser::getInstance()->init( applicationDirIn, componentDirIn, profileDirIn );
}

////////////////////////////////////////////////////////////////////////////////
//
int LLMozLib::getLastError()
{
	return LLEmbeddedBrowser::getInstance()->getLastError();
}

////////////////////////////////////////////////////////////////////////////////
//
bool LLMozLib::reset()
{
	return LLEmbeddedBrowser::getInstance()->reset();
}

////////////////////////////////////////////////////////////////////////////////
//
bool LLMozLib::clearCache()
{
	return LLEmbeddedBrowser::getInstance()->clearCache();
}

////////////////////////////////////////////////////////////////////////////////
//
bool LLMozLib::enableCookies( bool enabledIn )
{
	return LLEmbeddedBrowser::getInstance()->enableCookies( enabledIn );
}

////////////////////////////////////////////////////////////////////////////////
//
bool LLMozLib::clearAllCookies()
{
	return LLEmbeddedBrowser::getInstance()->clearAllCookies();
}

////////////////////////////////////////////////////////////////////////////////
//
bool LLMozLib::enablePlugins( bool enabledIn )
{
	return LLEmbeddedBrowser::getInstance()->enablePlugins( enabledIn );
}

////////////////////////////////////////////////////////////////////////////////
//
const std::string LLMozLib::getVersion()
{
	// return as a string for now - don't think we need to expose actual version numbers

	return "1.1.0 (Mozilla GRE:" + LLEmbeddedBrowser::getInstance()->getGREVersion() + ")";
}

////////////////////////////////////////////////////////////////////////////////
//
void LLMozLib::setBrowserAgentId( std::string idIn )
{
	LLEmbeddedBrowser::getInstance()->setBrowserAgentId( idIn );
}

////////////////////////////////////////////////////////////////////////////////
//
bool LLMozLib::enableProxy( bool proxyEnabledIn, std::string proxyHostNameIn, int proxyPortIn )
{
	return LLEmbeddedBrowser::getInstance()->enableProxy( proxyEnabledIn, proxyHostNameIn, proxyPortIn );
}

////////////////////////////////////////////////////////////////////////////////
//
int LLMozLib::createBrowserWindow( void* nativeWindowHandleIn, int browserWindowWidthIn, int browserWindowHeightIn )
{
	int id = 0;

	// arbitrary limit of mMaxBrowserWindows browser windows so we don't exhaust system resources
	for(int i = 1; (i < mMaxBrowserWindows) && (id == 0) ; i++)
	{
		if(!(mBrowserWindowList[i]))
		{
			id = i;
		}
	}

	if(id != 0)
	{
		// We haven't exceeded mMaxBrowserWindows.  Go ahead and create another one.
		LLEmbeddedBrowserWindow* browserWindow = LLEmbeddedBrowser::getInstance()->createBrowserWindow( nativeWindowHandleIn, browserWindowWidthIn, browserWindowHeightIn );

		if ( browserWindow )
		{
			mBrowserWindowList[id] = browserWindow;
			browserWindow->setWindowId( id );
		}
		else
		{
			id = 0;
		}
	}

	return id;
}

////////////////////////////////////////////////////////////////////////////////
// utility method to get an LLEmbeddedBrowserWindow* from a window id (int)
LLEmbeddedBrowserWindow* LLMozLib::getBrowserWindowFromWindowId( int browserWindowIdIn )
{
	LLEmbeddedBrowserWindow *result = NULL;

	if((browserWindowIdIn > 0) && (browserWindowIdIn < mMaxBrowserWindows))
	{
		result = mBrowserWindowList[browserWindowIdIn];
	}

	return result;
}

////////////////////////////////////////////////////////////////////////////////
//
bool LLMozLib::destroyBrowserWindow( int browserWindowIdIn )
{
	LLEmbeddedBrowserWindow* browserWindow = getBrowserWindowFromWindowId( browserWindowIdIn );
	if ( browserWindow )
	{
		LLEmbeddedBrowser::getInstance()->destroyBrowserWindow( browserWindow );

		mBrowserWindowList[browserWindowIdIn] = NULL;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////
//
bool LLMozLib::setSize( int browserWindowIdIn, int widthIn, int heightIn )
{
	LLEmbeddedBrowserWindow* browserWindow = getBrowserWindowFromWindowId( browserWindowIdIn );
	if ( browserWindow )
	{
		browserWindow->setSize( widthIn, heightIn );

		return true;
	};

	return false;
}

////////////////////////////////////////////////////////////////////////////////
//
bool LLMozLib::scrollByLines( int browserWindowIdIn, int linesIn )
{
	LLEmbeddedBrowserWindow* browserWindow = getBrowserWindowFromWindowId( browserWindowIdIn );
	if ( browserWindow )
	{
		browserWindow->scrollByLines( linesIn );

		return true;
	};

	return false;
}

////////////////////////////////////////////////////////////////////////////////
//
void LLMozLib::setBackgroundColor( int browserWindowIdIn, int redIn, int greenIn, int blueIn )
{
	LLEmbeddedBrowserWindow* browserWindow = getBrowserWindowFromWindowId( browserWindowIdIn );
	if ( browserWindow )
	{
		browserWindow->setBackgroundColor( redIn, greenIn, blueIn );
	};
}

////////////////////////////////////////////////////////////////////////////////
//
bool LLMozLib::setCaretColor( int browserWindowIdIn, const int redIn, const int greenIn, const int blueIn )
{
	LLEmbeddedBrowserWindow* browserWindow = getBrowserWindowFromWindowId( browserWindowIdIn );
	if ( browserWindow )
	{
		browserWindow->setCaretColor( redIn, greenIn, blueIn );

		return true;
	};

	return false;
}

////////////////////////////////////////////////////////////////////////////////
//
bool LLMozLib::addObserver( int browserWindowIdIn, LLEmbeddedBrowserWindowObserver* subjectIn )
{
	LLEmbeddedBrowserWindow* browserWindow = getBrowserWindowFromWindowId( browserWindowIdIn );
	if ( browserWindow )
	{
		browserWindow->addObserver( subjectIn );
	};

	return true;
}

////////////////////////////////////////////////////////////////////////////////
//
bool LLMozLib::remObserver( int browserWindowIdIn, LLEmbeddedBrowserWindowObserver* subjectIn )
{
	LLEmbeddedBrowserWindow* browserWindow = getBrowserWindowFromWindowId( browserWindowIdIn );
	if ( browserWindow )
	{
		browserWindow->remObserver( subjectIn );
	};

	return true;
}

////////////////////////////////////////////////////////////////////////////////
//
bool LLMozLib::navigateTo( int browserWindowIdIn, const std::string uriIn )
{
	LLEmbeddedBrowserWindow* browserWindow = getBrowserWindowFromWindowId( browserWindowIdIn );
	if ( browserWindow )
	{
		return browserWindow->navigateTo( uriIn ) ? true : false;
	};

	return false;
}

////////////////////////////////////////////////////////////////////////////////
//
bool LLMozLib::navigateStop( int browserWindowIdIn )
{
	LLEmbeddedBrowserWindow* browserWindow = getBrowserWindowFromWindowId( browserWindowIdIn );
	if ( browserWindow )
	{
		browserWindow->navigateStop();

		return true;
	};

	return false;
}

////////////////////////////////////////////////////////////////////////////////
//
bool LLMozLib::canNavigateBack( int browserWindowIdIn )
{
	LLEmbeddedBrowserWindow* browserWindow = getBrowserWindowFromWindowId( browserWindowIdIn );
	if ( browserWindow )
	{
		return browserWindow->canNavigateBack() ? true : false;
	};

	return false;
}

////////////////////////////////////////////////////////////////////////////////
//
bool LLMozLib::navigateBack( int browserWindowIdIn )
{
	LLEmbeddedBrowserWindow* browserWindow = getBrowserWindowFromWindowId( browserWindowIdIn );
	if ( browserWindow )
	{
		browserWindow->navigateBack();

		return true;
	};

	return false;
}

////////////////////////////////////////////////////////////////////////////////
//
bool LLMozLib::canNavigateForward( int browserWindowIdIn )
{
	LLEmbeddedBrowserWindow* browserWindow = getBrowserWindowFromWindowId( browserWindowIdIn );
	if ( browserWindow )
	{
		return browserWindow->canNavigateForward() ? true : false;
	};

	return false;
}

////////////////////////////////////////////////////////////////////////////////
//
bool LLMozLib::navigateForward( int browserWindowIdIn )
{
	LLEmbeddedBrowserWindow* browserWindow = getBrowserWindowFromWindowId( browserWindowIdIn );
	if ( browserWindow )
	{
		browserWindow->navigateForward();

		return true;
	};

	return false;
}

///////////////////////////////////////////////////////////////////////////////
//
const unsigned char* LLMozLib::grabBrowserWindow( int browserWindowIdIn )
{
	LLEmbeddedBrowserWindow* browserWindow = getBrowserWindowFromWindowId( browserWindowIdIn );
	if ( browserWindow )
	{
		return browserWindow->grabWindow();
	};

	return 0;
}

////////////////////////////////////////////////////////////////////////////////
//
const unsigned char* LLMozLib::getBrowserWindowPixels( int browserWindowIdIn )
{
	LLEmbeddedBrowserWindow* browserWindow = getBrowserWindowFromWindowId( browserWindowIdIn );
	if ( browserWindow )
	{
		return browserWindow->getPageBuffer();
	};

	return 0;
}

////////////////////////////////////////////////////////////////////////////////
//
const int LLMozLib::getBrowserWidth( int browserWindowIdIn )
{
	LLEmbeddedBrowserWindow* browserWindow = getBrowserWindowFromWindowId( browserWindowIdIn );
	if ( browserWindow )
	{
		return browserWindow->getBrowserWidth();
	};

	return 0;
}

////////////////////////////////////////////////////////////////////////////////
//
const int LLMozLib::getBrowserHeight( int browserWindowIdIn )
{
	LLEmbeddedBrowserWindow* browserWindow = getBrowserWindowFromWindowId( browserWindowIdIn );
	if ( browserWindow )
	{
		return browserWindow->getBrowserHeight();
	};

	return 0;
}

////////////////////////////////////////////////////////////////////////////////
//
const int LLMozLib::getBrowserDepth( int browserWindowIdIn )
{
	LLEmbeddedBrowserWindow* browserWindow = getBrowserWindowFromWindowId( browserWindowIdIn );
	if ( browserWindow )
	{
		return browserWindow->getBrowserDepth();
	};

	return 0;
}

////////////////////////////////////////////////////////////////////////////////
//
const int LLMozLib::getBrowserRowSpan( int browserWindowIdIn )
{
	LLEmbeddedBrowserWindow* browserWindow = getBrowserWindowFromWindowId( browserWindowIdIn );
	if ( browserWindow )
	{
		return browserWindow->getBrowserRowSpan();
	};

	return 0;
}

////////////////////////////////////////////////////////////////////////////////
//
bool LLMozLib::mouseDown( int browserWindowIdIn, int xPosIn, int yPosIn )
{
	LLEmbeddedBrowserWindow* browserWindow = getBrowserWindowFromWindowId( browserWindowIdIn );
	if ( browserWindow )
	{
		browserWindow->mouseDown( xPosIn, yPosIn );

		return true;
	};

	return false;
}

////////////////////////////////////////////////////////////////////////////////
//
bool LLMozLib::mouseUp( int browserWindowIdIn, int xPosIn, int yPosIn )
{
	LLEmbeddedBrowserWindow* browserWindow = getBrowserWindowFromWindowId( browserWindowIdIn );
	if ( browserWindow )
	{
		browserWindow->mouseUp( xPosIn, yPosIn );

		return true;
	};

	return false;
}

////////////////////////////////////////////////////////////////////////////////
//
bool LLMozLib::mouseMove( int browserWindowIdIn, int xPosIn, int yPosIn )
{
	LLEmbeddedBrowserWindow* browserWindow = getBrowserWindowFromWindowId( browserWindowIdIn );
	if ( browserWindow )
	{
		browserWindow->mouseMove( xPosIn, yPosIn );

		return true;
	};

	return false;
}

////////////////////////////////////////////////////////////////////////////////
// Send left double-click message, which is usually triggered at the OS level
// on the second rapid mouse-down event.
bool LLMozLib::mouseLeftDoubleClick( int browserWindowIdIn, int xPosIn, int yPosIn )
{
	LLEmbeddedBrowserWindow* browserWindow = getBrowserWindowFromWindowId( browserWindowIdIn );
	if ( browserWindow )
	{
		browserWindow->mouseLeftDoubleClick( xPosIn, yPosIn );

		return true;
	};

	return false;
}

////////////////////////////////////////////////////////////////////////////////
//
bool LLMozLib::keyPress( int browserWindowIdIn, int keyCodeIn )
{
	LLEmbeddedBrowserWindow* browserWindow = getBrowserWindowFromWindowId( browserWindowIdIn );
	if ( browserWindow )
	{
		browserWindow->keyPress( keyCodeIn );

		return true;
	};

	return false;
}

bool LLMozLib::unicodeInput( int browserWindowIdIn, unsigned long uni_char )
{
	LLEmbeddedBrowserWindow* browserWindow = getBrowserWindowFromWindowId( browserWindowIdIn );
	if ( browserWindow )
	{
		browserWindow->unicodeInput( uni_char );

		return true;
	};

	return false;
}

////////////////////////////////////////////////////////////////////////////////
//
bool LLMozLib::focusBrowser( int browserWindowIdIn, bool focusBrowserIn )
{
	LLEmbeddedBrowserWindow* browserWindow = getBrowserWindowFromWindowId( browserWindowIdIn );
	if ( browserWindow )
	{
		browserWindow->focusBrowser( focusBrowserIn );

		return true;
	};

	return false;
}

////////////////////////////////////////////////////////////////////////////////
//
bool LLMozLib::set404RedirectUrl(  int browser_window_in, std::string redirect_url )
{
	LLEmbeddedBrowserWindow* browser_window = getBrowserWindowFromWindowId( browser_window_in );
	if ( browser_window )
	{
		browser_window->set404RedirectUrl( redirect_url );

		return true;
	};

	return false;
}

////////////////////////////////////////////////////////////////////////////////
//
bool LLMozLib::clr404RedirectUrl( int browser_window_in )
{
	LLEmbeddedBrowserWindow* browser_window = getBrowserWindowFromWindowId( browser_window_in );
	if ( browser_window )
	{
		browser_window->clr404RedirectUrl();

		return true;
	};

	return false;
}
