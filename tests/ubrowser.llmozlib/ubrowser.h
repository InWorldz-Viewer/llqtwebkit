/**
 * @file ubrowser.h
 * @brief uBrowser test harness
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

#ifndef UBROWSER_H
#define UBROWSER_H

#include <string>
#include <vector>
#include <math.h>

#include "glui.h"
#include "llmozlib.h"

static void gluiCallbackWrapper( int controlIdIn );

////////////////////////////////////////////////////////////////////////////////
//
class uBrowser :
	public LLEmbeddedBrowserWindowObserver
{
	public:
		uBrowser();
		~uBrowser();

		bool init( const char* arg0, int appWindowIn );
		bool reset();
		const std::string& getName() { return mName; };
		void reshape( int widthIn, int heightIn );
		void makeChrome();
		void windowPosToTexturePos( int winXIn, int winYIn, int& texXOut, int& texYOut );
		void resetView();
		void drawGeometry( int typeIn );

		void display();
		void idle();
		void keyboard( unsigned char keyIn, int xIn, int yIn );
		void passiveMouse( int xIn, int yIn );
		void mouseButton( int button, int state, int xIn, int yIn );
		void mouseMove( int xIn, int yIn );

		void setSize( int widthIn , int heightIn );

		void gluiCallback( int controlIdIn );

		virtual void onNavigateBegin( const EventType& eventIn );
		virtual void onNavigateComplete( const EventType& eventIn );
		virtual void onUpdateProgress( const EventType& eventIn );
		virtual void onStatusTextChange( const EventType& eventIn );
		virtual void onLocationChange( const EventType& eventIn );
		virtual void onClickLinkHref( const EventType& eventIn );
		virtual void onClickLinkSecondLife( const EventType& eventIn );

	private:
		void* getNativeWindowHandle();
		void setFocusNativeWindow();
		GLenum getGLTextureFormat(int size);
		GLenum getGLTextureType(int size);

		std::string mName;
		int mAppWindow;
		int mWindowWidth;
		int mWindowHeight;
		int mTextureWidth;
		int mTextureHeight;
		int mBrowserWindowWidth;
		int mBrowserWindowHeight;
		float mTextureScaleX;
		float mTextureScaleY;
		float mViewportAspect;
		float mViewPos[ 3 ];
		float mViewRotation[ 16 ];
		unsigned char mPixelColorRB[ 3 ];
		unsigned char mPixelColorG[ 3 ];
		int mCurMouseX;
		int mCurMouseY;
		GLuint mRedBlueTexture;
		unsigned char mRedBlueTexturePixels[ 256 * 256 * 3 ];
		GLuint mGreenTexture;
		unsigned char mGreenTexturePixels[ 16 * 16 * 3 ];
		GLuint mAppTexture;
		unsigned char* mAppTexturePixels;
		int mCurObjType;
		int mSelBookmark;
		GLUI* mTopGLUIWindow;
		GLUI_Button* mNavBackButton;
		GLUI_Button* mNavStopButton;
		GLUI_Button* mNavForwardButton;
		GLUI* mTop2GLUIWindow;
		GLUI_EditText* mUrlEdit;
		GLUI_String mNavUrl;
		GLUI* mRightGLUIWindow;
		GLUI_Rotation* mViewRotationCtrl;
		GLUI_Translation* mViewScaleCtrl;
		GLUI_Translation* mViewTranslationCtrl;
		GLUI* mBottomGLUIWindow;
		GLUI_StaticText* mStatusText;
		GLUI_StaticText* mProgressText;
		const int mIdReset;
		const int mIdExit;
		const int mIdBookmarks;
		const int mIdTypeNull;
		const int mIdTypeFlat;
		const int mIdTypeBall;
		const int mIdTypeCube;
		const int mIdTypeFlag;
		const int mIdUrlEdit;
		const int mIdNavBack;
		const int mIdNavStop;
		const int mIdNavHome;
		const int mIdNavForward;
		const int mIdBrowserSmall;
		const int mIdBrowserMedium;
		const int mIdBrowserLarge;
		const int mFocusUrlEdit;
		const int mIdClearCookies;
		const std::string mHomeUrl;
		std::vector< std::pair< std::string, std::string > > mBookmarks;
		int mWindowA;
};

#endif // UBROWSER_H
