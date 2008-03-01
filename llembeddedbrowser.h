/**
 * @file llembeddedbrowser.h
 * @brief LLEmbeddedBrowser declaration.
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

#ifndef LLEMBEDDEDBROWSER_H
#define LLEMBEDDEDBROWSER_H

#include <string>
#include <map>

class LLEmbeddedBrowserWindow;
class LLEmbeddedBrowserWindowObserver;

class LLEmbeddedBrowser
{
	public:
		LLEmbeddedBrowser();
		virtual ~LLEmbeddedBrowser();

		static LLEmbeddedBrowser* getInstance();

		bool init( std::string applicationDir, std::string componentDir, std::string profileDir );
		bool reset();
		bool clearCache();
		bool enableProxy( bool proxyEnabledIn, std::string proxyHostNameIn, int proxyPortIn );
		bool enableCookies( bool enabledIn );
		bool clearAllCookies();
		bool enablePlugins( bool enabledIn );
		std::string getGREVersion();
		void setBrowserAgentId( std::string idIn );
		LLEmbeddedBrowserWindow* createBrowserWindow( void* nativeWindowHandleIn, int browserWidthIn, int browserHeightIn );
		bool destroyBrowserWindow( LLEmbeddedBrowserWindow* browserWindowIn );
		bool setCaretColor( int browserWindowIdIn, const int redIn, const int greenIn, const int blueIn );
		void setLastError( int errorNumIn );
		void clearLastError();
		int getLastError();

	private:
		static LLEmbeddedBrowser* sInstance;
		int mErrorNum;
};

#endif	// LLEMBEDDEDBROWSER_H
