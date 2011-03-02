/* Copyright (c) 2006-2010, Linden Research, Inc.
 *
 * LLQtWebKit Source Code
 * The source code in this file ("Source Code") is provided by Linden Lab
 * to you under the terms of the GNU General Public License, version 2.0
 * ("GPL"), unless you have obtained a separate licensing agreement
 * ("Other License"), formally executed by you and Linden Lab.  Terms of
 * the GPL can be found in GPL-license.txt in this distribution, or online at
 * http://secondlifegrid.net/technology-programs/license-virtual-world/viewerlicensing/gplv2
 *
 * There are special exceptions to the terms and conditions of the GPL as
 * it is applied to this Source Code. View the full text of the exception
 * in the file FLOSS-exception.txt in this software distribution, or
 * online at
 * http://secondlifegrid.net/technology-programs/license-virtual-world/viewerlicensing/flossexception
 *
 * By copying, modifying or distributing this software, you acknowledge
 * that you have read and understood your obligations described above,
 * and agree to abide by those obligations.
 *
 * ALL LINDEN LAB SOURCE CODE IS PROVIDED "AS IS." LINDEN LAB MAKES NO
 * WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY,
 * COMPLETENESS OR PERFORMANCE.
 */

#ifndef _WINDOWS
extern "C" {
#include <unistd.h>
}
#endif

#ifdef _WINDOWS
#include <windows.h>
#include <direct.h>
#endif

#include <iostream>
#include <stdlib.h>
#include <fstream>

#ifdef LL_OSX
// I'm not sure why STATIC_QT is getting defined, but the Q_IMPORT_PLUGIN thing doesn't seem to be necessary on the mac.
#undef STATIC_QT
#endif

#ifdef STATIC_QT
#include <QtPlugin>
Q_IMPORT_PLUGIN(qgif)
#endif

#include "llqtwebkit.h"

class sslTest :
	public LLEmbeddedBrowserWindowObserver
{
	public:
		sslTest( std::string url ) :
			mBrowserWindowWidth( 512 ),
			mBrowserWindowHeight( 512 ),
			mBrowserWindowHandle( 0 ),
			mNavigateInProgress( true )
		{
#ifdef _WINDOWS
			std::string cwd = std::string( _getcwd( NULL, 1024) );
			std::string profile_dir = cwd + "\\" + "ssltest_profile";
			void* native_window_handle = (void*)GetDesktopWindow();
			std::string ca_pem_file_loc = cwd + "\\" + "CA.pem";
#else
			std::string cwd = std::string( _getcwd( NULL, 1024) );
			std::string profile_dir = cwd + "/" + "ssltest_profile";
			void* native_window_handle = 0;
			std::string ca_pem_file_loc = cwd + "/" + "CA.pem";
#endif

			std::cout << "ssltest> === begin ===" << std::endl;
			std::cout << "ssltest> current working dir is " << cwd << std::endl;
			std::cout << "ssltest> profiles dir location is " << profile_dir << std::endl;

			LLQtWebKit::getInstance()->init( cwd, cwd, profile_dir, native_window_handle );

			LLQtWebKit::getInstance()->enableJavascript( true );
			LLQtWebKit::getInstance()->enablePlugins( true );

			mBrowserWindowHandle = LLQtWebKit::getInstance()->createBrowserWindow( mBrowserWindowWidth, mBrowserWindowHeight );
			LLQtWebKit::getInstance()->setSize( mBrowserWindowHandle, mBrowserWindowWidth, mBrowserWindowHeight );

			LLQtWebKit::getInstance()->addObserver( mBrowserWindowHandle, this );
			
			std::cout << "ssltest> Expected CA.pem file location is " << ca_pem_file_loc << std::endl;
			LLQtWebKit::getInstance()->addCAFile( ca_pem_file_loc.c_str() );

			LLQtWebKit::getInstance()->navigateTo( mBrowserWindowHandle, url );

			std::cout << "ssltest> navigating to " << url << std::endl;
		};

		bool idle( void )
		{
			LLQtWebKit::getInstance()->pump( 100 );

#if _WINDOWS	
			MSG msg;
			while ( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) )
			{
				GetMessage( &msg, NULL, 0, 0 );
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			};
#endif
			return mNavigateInProgress;
		};

		~sslTest()
		{
			LLQtWebKit::getInstance()->remObserver( mBrowserWindowHandle, this );
			LLQtWebKit::getInstance()->reset();
			std::cout << "ssltest> === end ===" << std::endl;
		};

		void onNavigateBegin( const EventType& eventIn )
		{
			mNavigateInProgress = true;
			std::cout << "Event: begin navigation to " << eventIn.getEventUri() << std::endl;
		};

		void onNavigateComplete( const EventType& eventIn )
		{
			std::cout << "Event: end navigation to " << eventIn.getEventUri() << std::endl;
			mNavigateInProgress = false;
		};

		void onUpdateProgress( const EventType& eventIn )
		{
			std::cout << "Event: progress value updated to " << eventIn.getIntValue() << std::endl;
		};

		void onStatusTextChange( const EventType& eventIn )
		{
			std::cout << "Event: status updated to " << eventIn.getStringValue() << std::endl;
		};

		void onTitleChange( const EventType& eventIn )
		{
			std::cout << "Event: title changed to  " << eventIn.getStringValue() << std::endl;
		};

		void onLocationChange( const EventType& eventIn )
		{
			std::cout << "Event: location changed to " << eventIn.getStringValue() << std::endl;
		};

	private:
		int mBrowserWindowWidth;
		int mBrowserWindowHeight;
		int mBrowserWindowHandle;
		bool mNavigateInProgress;
};

void navigateURL( std::string url )
{
	sslTest* app = new sslTest( url );

	bool result = app->idle();
	while( result )
	{
		result = app->idle();
	};
	
	delete app;
}

int main( int argc, char* argv[] )
{
	std::cout << std::endl << " --------- sslTest application starting --------- " << std::endl;

	if ( argc == 2 )
	{
		navigateURL( argv[ 1 ] );		
	}
	else
	{
		navigateURL( "https://id.aditi.lindenlab.com/openid/login" );
	};

	std::cout << " --------- sslTest application ending  --------- " << std::endl;
	
	return 0;
}
