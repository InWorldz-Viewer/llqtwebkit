/**
 * @file ubrowser.cpp
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

#include "ubrowser.h"

#ifdef _WINDOWS
#include <windows.h>
#elif defined(__APPLE__)
	// Grr... glui.h #defines Byte for some reason, which conflicts with the Carbon headers.
	#ifdef Byte
		#undef Byte
	#endif
	#include <Carbon/Carbon.h>
#elif LL_LINUX
#include <gtk/gtk.h>
#endif

#include <iostream>
#include <sstream>
#include <time.h>

// connects GLUI callbacks to the app class
uBrowser* gInstance = 0;

////////////////////////////////////////////////////////////////////////////////
//
uBrowser::uBrowser() :
	mName( "uBrowser - version 1.0.4" ),
	mWindowA( 0 ),
	mBrowserWindowWidth( 800 ),
	mBrowserWindowHeight( 800 ),
	mTextureWidth( 0 ),
	mTextureHeight( 0 ),
	mTextureScaleX( 0.0f ),
	mTextureScaleY( 0.0f ),
	mViewportAspect( 0.0f ),
	mIdReset( 0x0010 ),
	mIdBookmarks( 0x0011 ),
	mIdExit( 0x0012 ),
	mIdTypeNull( 0x0013 ),
	mIdTypeFlat( 0x0014 ),
	mIdTypeBall( 0x0015 ),
	mIdTypeCube( 0x0016 ),
	mIdTypeFlag( 0x0017 ),
	mIdUrlEdit( 0x0018 ),
	mIdNavBack( 0x0019 ),
	mIdNavStop( 0x001a ),
	mIdNavHome( 0x001b ),
	mIdNavForward( 0x001c ),
	mIdBrowserSmall( 0x001d ),
	mIdBrowserMedium( 0x001e ),
	mIdBrowserLarge( 0x001f ),
	mFocusUrlEdit( 0x0020 ),
	mIdClearCookies( 0x0021 ),
	mStatusText( 0 ),
	mProgressText( 0 ),
	mHomeUrl( "http://www.google.com" )
{
	// use this to connect GLUI callbacks to this class
	gInstance = this;

	// list of bookmarks that demonstrate cool stuff!
	mBookmarks.push_back( std::pair< std::string, std::string >( "Select a bookmark", "" ) );
	mBookmarks.push_back( std::pair< std::string, std::string >( "Second Life Home Page", "http://www.secondlife.com" ) );
	mBookmarks.push_back( std::pair< std::string, std::string >( "Mozilla Home Page", "http://www.mozilla.org" ) );
	mBookmarks.push_back( std::pair< std::string, std::string >( "Google Home", "http://www.google.com" ) );
	mBookmarks.push_back( std::pair< std::string, std::string >( "Google Maps", "http://maps.google.com" ) );
	mBookmarks.push_back( std::pair< std::string, std::string >( "Boing Boing", "http://www.boingboing.net" ) );
	mBookmarks.push_back( std::pair< std::string, std::string >( "Digg", "http://www.digg.com" ) );
	mBookmarks.push_back( std::pair< std::string, std::string >( "Yahoo! User Interface Library slider", "http://developer.yahoo.net/yui/slider/examples/slider.html" ) );
	mBookmarks.push_back( std::pair< std::string, std::string >( "Canvascape", "http://www.abrahamjoffe.com.au/ben/canvascape/" ) );
	mBookmarks.push_back( std::pair< std::string, std::string >( "SVG Tetris (arrow keys and spacebar)", "http://www.croczilla.com/svg/samples/svgtetris/svgtetris.svg" ) );
	mBookmarks.push_back( std::pair< std::string, std::string >( "SVG Opacity & Hover", "http://www.croczilla.com/svg/samples/opacity1/opacity1.xml" ) );
	mBookmarks.push_back( std::pair< std::string, std::string >( "SVG Interactive lines", "http://www.croczilla.com/svg/samples/xbl-shapes2/xbl-shapes2.xml" ) );
	mBookmarks.push_back( std::pair< std::string, std::string >( "SVG Interactive shapes", "http://www.croczilla.com/svg/samples/xbl1/xbl1.xml" ) );
	mBookmarks.push_back( std::pair< std::string, std::string >( "XUL SQL Viewer", "http://www.georgenava.com/xul/dineros/dineros.php" ) );
	mBookmarks.push_back( std::pair< std::string, std::string >( "XUL Email App", "http://www.georgenava.com/xul/gmail/gmail.php" ) );
	mBookmarks.push_back( std::pair< std::string, std::string >( "XUL Outliner App", "http://www.georgenava.com/xul/outliner/outliner.php" ) );
	mBookmarks.push_back( std::pair< std::string, std::string >( "XUL Video Poker", "http://www.georgenava.com/xul/poker/videopoker.php" ) );
	mBookmarks.push_back( std::pair< std::string, std::string >( "XUL Visual Designer Studio", "http://www.georgenava.com/xul/studio/visualdesigner.php" ) );
	mBookmarks.push_back( std::pair< std::string, std::string >( "XUL Photo Viewer", "http://www.georgenava.com/xul/photo/photoviewer.php" ) );
	mBookmarks.push_back( std::pair< std::string, std::string >( "XUL Widgets", "http://www.hevanet.com/acorbin/xul/top.xul" ) );
	mBookmarks.push_back( std::pair< std::string, std::string >( "XUL Mozilla Amazon Browser (MAB)", "http://www.faser.net/mab/chrome/content/mab.xul" ) );
	mBookmarks.push_back( std::pair< std::string, std::string >( "DHTML Lemmings!", "http://www.elizium.nu/scripts/lemmings/" ) );
	mBookmarks.push_back( std::pair< std::string, std::string >( "DHTML Demos", "http://www.dhteumeuleu.com/" ) );
	mBookmarks.push_back( std::pair< std::string, std::string >( "about:config", "about:config" ) );
	mBookmarks.push_back( std::pair< std::string, std::string >( "S3 404 test", "http://world.secondlife.com/parcel/jumk" ) );
	mBookmarks.push_back( std::pair< std::string, std::string >( "Sky Web test", "http://sky.webdev.lindenlab.com/app/login/iframe_test_for_callum.php" ) );
	mBookmarks.push_back( std::pair< std::string, std::string >( "Project Draw", "http://draw.labs.autodesk.com/ADDraw/draw.html" ) );
	mBookmarks.push_back( std::pair< std::string, std::string >( "Leyla Test", " https://secondlife.com/app/login/?show_login_form=True&firstname=JTester&lastname=Tester&location=last&region=&grid=Siva&channel=Second%20Life%20Release" ) );
	mBookmarks.push_back( std::pair< std::string, std::string >( "JavaScript call to turn on page editing mode", "javascript:document.body.contentEditable='true'; document.designMode='on'; void 0" ) );

}

////////////////////////////////////////////////////////////////////////////////
//
uBrowser::~uBrowser()
{
	// clean up - don't generally get here since we quit from a GLUT app with exit..!!
	LLMozLib::getInstance()->remObserver( mWindowA, this );

	LLMozLib::getInstance()->reset();
}

////////////////////////////////////////////////////////////////////////////////
//
bool uBrowser::init( const char* arg0, int appWindowIn )
{
	// save the GLUT window handle since we need to reset it sometimes.
	// (GLUT/GLUI dependency)
	mAppWindow = appWindowIn;

	// build GUI
	makeChrome();

	glClearColor( 0.0f, 0.0f, 0.0f, 0.5f );
	glClearDepth( 1.0f );
	glEnable( GL_DEPTH_TEST );
	glDepthFunc( GL_LEQUAL );
	glEnable( GL_TEXTURE_2D );

	// create the red/blue texture used in picking
	glGenTextures( 1, &mRedBlueTexture );
	for( int y = 0; y < 256; ++y )
	{
		for( int x = 0; x < 256 * 3 ; x += 3 )
		{
			mRedBlueTexturePixels[ y * 256 * 3 + x + 0 ] = ( x / 3 );
			mRedBlueTexturePixels[ y * 256 * 3 + x + 1 ] = 0;
			mRedBlueTexturePixels[ y * 256 * 3 + x + 2 ] = y;
		};
	}
	glBindTexture( GL_TEXTURE_2D, mRedBlueTexture );
	glTexImage2D( GL_TEXTURE_2D, 0,
		GL_RGB,
			256, 256,
				0, GL_RGB, GL_UNSIGNED_BYTE, mRedBlueTexturePixels );

	// create the green texture used in picking
	glGenTextures( 1, &mGreenTexture );
	for( int i = 0; i < 16 * 16 * 3; i += 3 )
	{
		mGreenTexturePixels[ i + 0 ] = 0;
		mGreenTexturePixels[ i + 1 ] = ( unsigned char )( i / 3 );
		mGreenTexturePixels[ i + 2 ] = 0;
	}
	glBindTexture( GL_TEXTURE_2D, mGreenTexture );
	glTexImage2D( GL_TEXTURE_2D, 0,
		GL_RGB,
			16, 16,
				0, GL_RGB, GL_UNSIGNED_BYTE, mGreenTexturePixels );

	// create the texture used to display the browser data
	glGenTextures( 1, &mAppTexture );
	glBindTexture( GL_TEXTURE_2D, mAppTexture );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexImage2D( GL_TEXTURE_2D, 0,
		GL_RGB,
			mTextureWidth, mTextureHeight,
				0, GL_RGB, GL_UNSIGNED_BYTE, 0 );

	resetView();

	// start with the flat rectangle so we don't freak people out :)
	mCurObjType = mIdTypeFlat;

#ifndef LL_LINUX
	std::string applicationDir = std::string( arg0 ).substr( 0, std::string( arg0 ).find_last_of("\\/") );
	std::string componentDir( applicationDir );
	// NOTE: if this has the same name as an existing file in the directory, the browser won't initialize.
	// Don't set it to 'ubrowser' on the mac, since that's the name of the executable...
	std::string profileDir( applicationDir + "\\" + "ubrowserprofile" );
#else
	// Must use absolute paths on Linux
	std::string applicationDir = getcwd(NULL,0);
	std::string componentDir( applicationDir );
	std::string profileDir( applicationDir + "/" + "ubrowserprofile" );
#endif // LL_LINUX

	if ( ! LLMozLib::getInstance()->init( applicationDir, componentDir, profileDir ) )
	{
		std::cerr << "Failed to initialize browser window, last error is 0x" << std::hex << LLMozLib::getInstance()->getLastError() << std::endl;
		exit(1);
	}

	LLMozLib::getInstance()->enableProxy( false, "", 0 );

	mWindowA = LLMozLib::getInstance()->createBrowserWindow( getNativeWindowHandle(), mBrowserWindowWidth, mBrowserWindowHeight );

	setSize( mBrowserWindowWidth, mBrowserWindowHeight );

	LLMozLib::getInstance()->setBackgroundColor( mWindowA, 0x00, 0x00, 0x00 );

	LLMozLib::getInstance()->setCaretColor( mWindowA, 0x00, 0x00, 0x00 );

	LLMozLib::getInstance()->enableCookies( true );

	LLMozLib::getInstance()->addObserver( mWindowA, this );

	LLMozLib::getInstance()->navigateTo( mWindowA, mHomeUrl );

	LLMozLib::getInstance()->setBrowserAgentId( "[uBrowser]" );

	LLMozLib::getInstance()->set404RedirectUrl( mWindowA, "http://user.lindenlab.com/~callum/404.html" );

	std::cout << "LLMozLib version: " << LLMozLib::getInstance()->getVersion() << std::endl;

	return true;
}

////////////////////////////////////////////////////////////////////////////////
//
bool uBrowser::reset()
{
	return true;
}

////////////////////////////////////////////////////////////////////////////////
//
void uBrowser::reshape( int widthIn, int heightIn )
{
	// save these as we'll need them later
	mWindowWidth = widthIn;
	mWindowHeight = heightIn;

	// just a rough calculation
	mUrlEdit->set_w( mWindowWidth - 400 );
	mStatusText->set_w( mWindowWidth - 100 );

	// update viewport (the active window inside the chrome stuff)
	int viewportX, viewportY;
	int viewportHeight, viewportWidth;
	GLUI_Master.get_viewport_area( &viewportX, &viewportY, &viewportWidth, &viewportHeight );
	glViewport( viewportX, viewportY, viewportWidth, viewportHeight );

	// need this when we come to display
	mViewportAspect = ( float )( viewportWidth ) / ( float)( viewportHeight );

	// GLUI requires this
	if ( glutGetWindow() != mAppWindow )
		glutSetWindow( mAppWindow );

	// trigger re-display
	glutPostRedisplay();
};

////////////////////////////////////////////////////////////////////////////////
//
void uBrowser::drawGeometry( int typeIn )
{
	// plain old flat rectangle
	if ( typeIn == mIdTypeFlat )
	{
		glColor3f( 1.0f, 1.0f, 1.0f );
		glBegin( GL_QUADS );
			glTexCoord2f( 0.0f, 0.0f ); glVertex3f( -1.0f, -1.0f,  0.0f );
			glTexCoord2f( 1.0f, 0.0f ); glVertex3f(  1.0f, -1.0f,  0.0f );
			glTexCoord2f( 1.0f, 1.0f ); glVertex3f(  1.0f,  1.0f,  0.0f );
			glTexCoord2f( 0.0f, 1.0f ); glVertex3f( -1.0f,  1.0f,  0.0f );
		glEnd();
	}
	else
	// a ball - just because you can :) draw my own ball since the glu/glut ones don't get me
	// enough control over the texture coordinate details
	if ( typeIn == mIdTypeBall )
	{
		// this is based on some code from here: http://astronomy.swin.edu.au/~pbourke/opengl/sphere/
		const int numElems = 40;
		const GLfloat  piDiv2 = 1.57079632679489661923f;
		const GLfloat piMul2 = 6.28318530717958647692f;

		for( int j = 0; j < numElems / 2; ++j )
		{
			GLfloat theta1 = j * ( piMul2 ) / numElems - piDiv2;
			GLfloat theta2 = ( j + 1 ) * ( piMul2 ) / numElems - piDiv2;

			glBegin( GL_TRIANGLE_STRIP );

			for( int i = 0; i <= numElems; i++ )
			{
				GLfloat theta3 = i * piMul2 / numElems;

				GLfloat pointX = cos( theta2 ) * cos( theta3 );
				GLfloat pointY = sin( theta2 );
				GLfloat pointZ = cos( theta2 ) * sin( theta3 );

				glNormal3f( pointX, pointY, pointZ );
				glTexCoord2f( i / ( GLfloat )numElems, 2 * ( j + 1 ) / ( GLfloat )numElems );
				glVertex3f( pointX, pointY, pointZ );

				pointX = cos( theta1 ) * cos( theta3 );
				pointY = sin( theta1 );
				pointZ = cos( theta1 ) * sin( theta3 );

				glNormal3f( pointX, pointY, pointZ );
				glTexCoord2f( i / ( GLfloat )numElems, 2 * j / ( GLfloat )numElems);
				glVertex3f( pointX, pointY, pointZ );
			};
			glEnd();
		};
	}
	else
	// same browser on all six faces - of course, what really should happen is one on each face
	// but that's for later
	if ( typeIn == mIdTypeCube )
	{
		const GLfloat delta = 0.75f;

		glColor3f( 1.0f, 1.0f, 1.0f );
		glBegin( GL_QUADS );
			glTexCoord2f( 0.0f, 0.0f ); glVertex3f( -delta, -delta,  -delta );
			glTexCoord2f( 1.0f, 0.0f ); glVertex3f(  delta, -delta,  -delta );
			glTexCoord2f( 1.0f, 1.0f ); glVertex3f(  delta,  delta,  -delta );
			glTexCoord2f( 0.0f, 1.0f ); glVertex3f( -delta,  delta,  -delta );

			glTexCoord2f( 0.0f, 0.0f ); glVertex3f( -delta, -delta,  delta );
			glTexCoord2f( 0.0f, 1.0f ); glVertex3f(  delta, -delta,  delta );
			glTexCoord2f( 1.0f, 1.0f ); glVertex3f(  delta,  delta,  delta );
			glTexCoord2f( 1.0f, 0.0f ); glVertex3f( -delta,  delta,  delta );

			glTexCoord2f( 0.0f, 0.0f ); glVertex3f( -delta,  -delta, -delta );
			glTexCoord2f( 1.0f, 0.0f ); glVertex3f( -delta,  -delta,  delta );
			glTexCoord2f( 1.0f, 1.0f ); glVertex3f(  delta,  -delta,  delta );
			glTexCoord2f( 0.0f, 1.0f ); glVertex3f(  delta,  -delta, -delta );

			glTexCoord2f( 0.0f, 0.0f ); glVertex3f( -delta,  delta, -delta );
			glTexCoord2f( 0.0f, 1.0f ); glVertex3f( -delta,  delta,  delta );
			glTexCoord2f( 1.0f, 1.0f ); glVertex3f(  delta,  delta,  delta );
			glTexCoord2f( 1.0f, 0.0f ); glVertex3f(  delta,  delta, -delta );

			glTexCoord2f( 0.0f, 0.0f ); glVertex3f( -delta, -delta, -delta );
			glTexCoord2f( 1.0f, 0.0f ); glVertex3f( -delta,  delta, -delta );
			glTexCoord2f( 1.0f, 1.0f ); glVertex3f( -delta,  delta,  delta );
			glTexCoord2f( 0.0f, 1.0f ); glVertex3f( -delta, -delta,  delta );

			glTexCoord2f( 0.0f, 0.0f ); glVertex3f( delta, -delta, -delta );
			glTexCoord2f( 0.0f, 1.0f ); glVertex3f( delta,  delta, -delta );
			glTexCoord2f( 1.0f, 1.0f ); glVertex3f( delta,  delta,  delta );
			glTexCoord2f( 1.0f, 0.0f ); glVertex3f( delta, -delta,  delta );
		glEnd();
	}
	else
	// waving flag - again - not useful but it looks great..
	if ( typeIn == mIdTypeFlag )
	{
		// this is based on some code from here: http://nehe.gamedev.net
		const int resolution = 45;
		static GLfloat points[ resolution ][ resolution ][ 3 ];
		static int wiggle = 0;
		static bool init = false;

		if ( ! init )
		{
			for( int x = 0; x < resolution; x++ )
			{
				for( int y = 0; y < resolution; y++ )
				{
					points[ x ][ y ][ 0 ] = ( GLfloat )( ( x / ( ( GLfloat )( resolution ) / 2.0f ) ) - 1.0f );
					points[ x ][ y ][ 1 ] = ( GLfloat )( ( y / ( ( GLfloat )( resolution ) / 2.0f ) ) - 1.0f );
					points[ x ][ y ][ 2 ] = ( GLfloat )( cos ( ( ( ( ( x + y ) / 5.0f ) * 40.0f ) / 360.0f ) * 3.141592654 * 2.0f ) * 0.1f );
				};
			};
			init = true;
		};

		glColor3f( 1.0f, 1.0f, 1.0f );
		glBegin( GL_QUADS );
		for( int x = 0; x < resolution - 1; x++ )
		{
			for( int y = resolution - 2; y > -1; --y )
			{
				int xCoord1 = x;
				int yCoord1 = y;

				int xCoord2 = x;
				int yCoord2 = y + 1;

				int xCoord3 = x + 1;
				int yCoord3 = y + 1;

				int xCoord4 = x + 1;
				int yCoord4 = y;

				glTexCoord2f( ( GLfloat )( xCoord1 ) / ( GLfloat )( resolution - 1 ),
					( GLfloat )( yCoord1 ) / ( GLfloat )( resolution - 1 ) );
				glVertex3f( points[ xCoord1 ][ yCoord1 ][ 0 ], points[ xCoord1 ][ yCoord1 ][ 1 ], points[ xCoord1 ][ yCoord1 ][ 2 ] );

				glTexCoord2f( ( GLfloat )( xCoord2 ) / ( GLfloat )( resolution - 1 ),
					( GLfloat )( yCoord2 ) / ( GLfloat )( resolution - 1 ) );
				glVertex3f( points[ xCoord2 ][ yCoord2 ][ 0 ], points[ xCoord2 ][ yCoord2 ][ 1 ], points[ xCoord2 ][ yCoord2 ][ 2 ] );

				glTexCoord2f( ( GLfloat )( xCoord3 ) / ( GLfloat )( resolution - 1 ),
					( GLfloat )( yCoord3 ) / ( GLfloat )( resolution - 1 ) );
				glVertex3f( points[ xCoord3 ][ yCoord3 ][ 0 ], points[ xCoord3 ][ yCoord3 ][ 1 ], points[ xCoord3 ][ yCoord3 ][ 2 ] );

				glTexCoord2f( ( GLfloat )( xCoord4 ) / ( GLfloat )( resolution - 1 ),
					( GLfloat )( yCoord4 ) / ( GLfloat )( resolution - 1 ) );
				glVertex3f( points[ xCoord4 ][ yCoord4 ][ 0 ], points[ xCoord4 ][ yCoord4 ][ 1 ], points[ xCoord4 ][ yCoord4 ][ 2 ] );
			};
		};
		glEnd();

		if ( ++wiggle == 3 )
		{
			for( int y = 0; y < resolution; ++y )
			{
				GLfloat swap = points[ 0 ][ y ][ 2 ];
				for( int x = 0; x < ( resolution - 1 ); ++x )
				{
					points[ x ][ y ][ 2 ] = points[ x + 1 ][ y ][ 2 ];
				};
				points[ resolution - 1 ][ y ][ 2 ] =swap;
			};
			wiggle = 0;
		};
	};
}

////////////////////////////////////////////////////////////////////////////////
//
void uBrowser::display()
{
	// let's start with a clean slate
	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// scale all texture so that they fit the geometry exactly
	glMatrixMode( GL_TEXTURE );
	glPushMatrix();
	glLoadIdentity();
	glScalef( mTextureScaleX, mTextureScaleY, 1.0f );

	// set up OpenGL view
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glFrustum( -mViewportAspect * 0.04f, mViewportAspect * 0.04f, -0.04f, 0.04f, 0.1f, 50.0f );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	glTranslatef( 0.0, 0.0, -3.0f );
	glTranslatef( mViewPos[ 0 ], mViewPos[ 1 ], -mViewPos[ 2 ] );
	glMultMatrixf( mViewRotation );
	glRotatef( 180.0f, 1.0f, 0.0f, 0.0f );
	glScalef( 1.0f, 1.0f, 1.0f );

	// red blue pattern
	glBindTexture( GL_TEXTURE_2D, mRedBlueTexture );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	drawGeometry( mCurObjType );

	// read colors and get red/blue value
	glReadPixels( mCurMouseX, mCurMouseY, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, mPixelColorRB );

	// green texture mask
	glBindTexture( GL_TEXTURE_2D, mGreenTexture );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glMatrixMode( GL_TEXTURE );
	glPushMatrix();
	glScalef( mTextureScaleX * 256.0f, mTextureScaleX * 256.0f, 1.0f );   // scale the scale by the scale :)
	drawGeometry( mCurObjType );
	glPopMatrix();
	glMatrixMode( GL_MODELVIEW );

	// read colors and get green value
	glReadPixels( mCurMouseX, mCurMouseY, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, mPixelColorG );

	// draw the browser texture
	glBindTexture( GL_TEXTURE_2D, mAppTexture );

	if ( mWindowA )
	{
		const unsigned char* pixels = LLMozLib::getInstance()->grabBrowserWindow( mWindowA );
		if ( pixels )
		{
			glTexSubImage2D( GL_TEXTURE_2D, 0,
				0, 0,
					// because sometimes the rowspan != width * bytes per pixel (mBrowserWindowWidth)
					LLMozLib::getInstance()->getBrowserRowSpan( mWindowA ) / LLMozLib::getInstance()->getBrowserDepth( mWindowA ),
						mBrowserWindowHeight,
							getGLTextureFormat(LLMozLib::getInstance()->getBrowserDepth( mWindowA )),
								getGLTextureType(LLMozLib::getInstance()->getBrowserDepth( mWindowA )),
									LLMozLib::getInstance()->getBrowserWindowPixels( mWindowA ) );
		};
	};
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	drawGeometry( mCurObjType );

	// restore matrix after texture scale
	glMatrixMode( GL_TEXTURE );
	glPopMatrix();

	glutSwapBuffers();
}

////////////////////////////////////////////////////////////////////////////////
//
void uBrowser::idle()
{
	// use this to throttle updates
	clock_t currentTime = clock();
	static clock_t startTime = currentTime;

	// time to update page
	if ( ( currentTime - startTime ) > ( CLOCKS_PER_SEC / 50 ) )
	{
		// grab the contents of the rendered page
		LLMozLib::getInstance()->grabBrowserWindow( mWindowA );

		// enable/disable back button depending on whether we can go back or not
		if ( LLMozLib::getInstance()->canNavigateBack( mWindowA ) )
			mNavBackButton->enable();
		else
			mNavBackButton->disable();

		// enable/disable back button depending on whether we can go back or not
		if ( LLMozLib::getInstance()->canNavigateForward( mWindowA ) )
			mNavForwardButton->enable();
		else
			mNavForwardButton->disable();

#ifdef LL_LINUX
		// pump the GTK+Gecko event queue for a (limited) while.
		gtk_main_iteration_do(0);
		for (int iter=0; iter<10; ++iter)
			if (gtk_events_pending())
				gtk_main_iteration();
#endif // LL_LINUX

		// reset timer
		startTime = currentTime;
	};

	// GLUI needs this
	if ( glutGetWindow() != mAppWindow )
		glutSetWindow( mAppWindow );

	// lots of updates for smooth motion
	glutPostRedisplay();
}

////////////////////////////////////////////////////////////////////////////////
//
void uBrowser::resetView()
{
	mViewRotationCtrl->reset();

	mViewScaleCtrl->set_x( 0.0f );
	mViewScaleCtrl->set_y( 0.0f );
	mViewScaleCtrl->set_z( 0.0f );

	mViewTranslationCtrl->set_x( 0.0f );
	mViewTranslationCtrl->set_y( 0.0f );
	mViewTranslationCtrl->set_z( 0.0f );

	LLMozLib::getInstance()->clearCache();
}

////////////////////////////////////////////////////////////////////////////////
//
void uBrowser::makeChrome()
{
	// top UI bar
	mTopGLUIWindow = GLUI_Master.create_glui_subwindow( mAppWindow, GLUI_SUBWINDOW_TOP );

	mNavBackButton = mTopGLUIWindow->add_button( "<<<", mIdNavBack, gluiCallbackWrapper );
	mNavBackButton->set_w( 56 );

	mTopGLUIWindow->add_column( false );
	GLUI_Button* button = mTopGLUIWindow->add_button( "STOP", mIdNavStop, gluiCallbackWrapper );
	button->set_w( 56 );

	mTopGLUIWindow->add_column( false );
	button = mTopGLUIWindow->add_button( "HOME", mIdNavHome, gluiCallbackWrapper );
	button->set_w( 56 );

	mTopGLUIWindow->add_column( false );
	mNavForwardButton = mTopGLUIWindow->add_button( ">>>", mIdNavForward, gluiCallbackWrapper );
	mNavForwardButton->set_w( 56 );

	mTopGLUIWindow->add_column( false );
#if !defined(LL_LINUX)
	// Friendly to older GLUI versions.
	mUrlEdit = mTopGLUIWindow->add_edittext( "Url:", mNavUrl, mIdUrlEdit, gluiCallbackWrapper );
#else
	mUrlEdit = mTopGLUIWindow->add_edittext( "Url:", GLUI_EDITTEXT_TEXT, mNavUrl, mIdUrlEdit, gluiCallbackWrapper );
#endif // LL_LINUX

	mTopGLUIWindow->add_column( false );
	mTopGLUIWindow->add_button( "Focus", mFocusUrlEdit, gluiCallbackWrapper );

	mTopGLUIWindow->set_main_gfx_window( mAppWindow );

	// top UI bar (second part)
	mTop2GLUIWindow = GLUI_Master.create_glui_subwindow( mAppWindow, GLUI_SUBWINDOW_TOP );

	mTop2GLUIWindow->add_column( false );
	GLUI_Listbox* bookmarkList = mTop2GLUIWindow->add_listbox( "Bookmarks", &mSelBookmark, mIdBookmarks, gluiCallbackWrapper );
	for( unsigned int each = 0; each < mBookmarks.size(); ++each )
	{
		bookmarkList->add_item( each, const_cast< char* >( mBookmarks[ each ].first.c_str() ) );
	};
	mTop2GLUIWindow->set_main_gfx_window( mAppWindow );

	// bottom UI bar
	mBottomGLUIWindow = GLUI_Master.create_glui_subwindow( mAppWindow, GLUI_SUBWINDOW_BOTTOM );

	mStatusText = mBottomGLUIWindow->add_statictext( "" );

	mBottomGLUIWindow->add_column( false );
	mProgressText = mBottomGLUIWindow->add_statictext( "" );
	mProgressText->set_alignment( GLUI_ALIGN_RIGHT );

	mBottomGLUIWindow->set_main_gfx_window( mAppWindow );

	// right side UI bar
	mRightGLUIWindow = GLUI_Master.create_glui_subwindow( mAppWindow, GLUI_SUBWINDOW_RIGHT );

	GLUI_Listbox* objTypelist = mRightGLUIWindow->add_listbox( "Type", &mCurObjType );
	objTypelist->add_item( mIdTypeFlat, "Flat" );
	objTypelist->add_item( mIdTypeCube, "Cube" );
	objTypelist->add_item( mIdTypeBall, "Ball" );
	objTypelist->add_item( mIdTypeFlag, "Flag" );
	mRightGLUIWindow->add_statictext( "" );

	mViewRotationCtrl = mRightGLUIWindow->add_rotation( "Rotation", mViewRotation );

	mViewTranslationCtrl = mRightGLUIWindow->add_translation( "Translate", GLUI_TRANSLATION_XY, mViewPos );
	mViewTranslationCtrl->set_speed( 0.01f );

	mViewScaleCtrl = mRightGLUIWindow->add_translation( "Scale", GLUI_TRANSLATION_Z, &mViewPos[ 2 ] );
	mViewScaleCtrl->set_speed( 0.05f );

	mRightGLUIWindow->add_statictext( "" );

	mRightGLUIWindow->add_statictext( "" );
	button = mRightGLUIWindow->add_button( "Small", mIdBrowserSmall, gluiCallbackWrapper );
	button->set_w( 64 );

	button = mRightGLUIWindow->add_button( "Medium", mIdBrowserMedium, gluiCallbackWrapper );
	button->set_w( 64 );

	button = mRightGLUIWindow->add_button( "Large", mIdBrowserLarge, gluiCallbackWrapper );
	button->set_w( 64 );

	GLUI_StaticText* text = mRightGLUIWindow->add_statictext( "Browser window size" );
	text->set_alignment( GLUI_ALIGN_CENTER );
	mRightGLUIWindow->add_statictext( "" );

	mRightGLUIWindow->add_statictext( "" );
	button = mRightGLUIWindow->add_button( "Clear Cookies", mIdClearCookies, gluiCallbackWrapper );
	button->set_w( 64 );

	mRightGLUIWindow->add_statictext( "" );
	button = mRightGLUIWindow->add_button( "Reset", mIdReset, gluiCallbackWrapper );
	button->set_w( 64 );

	mRightGLUIWindow->add_statictext( "" );
	button = mRightGLUIWindow->add_button( "Exit", mIdExit, gluiCallbackWrapper );
	button->set_w( 64 );

	mRightGLUIWindow->set_main_gfx_window( mAppWindow );
}

////////////////////////////////////////////////////////////////////////////////
//
void uBrowser::windowPosToTexturePos( int winXIn, int winYIn, int& texXOut, int& texYOut )
{
	// this is how we convert from the color in the textures draw in the first 2 passes to an
	// XY location in the coordinate space of the texture
	mCurMouseX = winXIn;
	mCurMouseY = mWindowHeight - winYIn;	// opposite Ycoordinate systems..

	// red gives 0..255 on X and blue value gives 0.255 on Y
	// green divides each distrete value by 16 giving 256 * 16 (4096) resolution in each of X & Y
	texXOut = ( mPixelColorRB[ 0 ] * 16 + ( mPixelColorG[ 1 ] & 0x0f ) ) / ( 4096 / mTextureWidth );
	texYOut = ( mPixelColorRB[ 2 ] * 16 + ( mPixelColorG[ 1 ] >> 4 ) ) / ( 4096 / mTextureHeight  );
}

////////////////////////////////////////////////////////////////////////////////
//
void uBrowser::keyboard( unsigned char keyIn, int xIn, int yIn )
{
	// pass on the keypress to Mozilla - will need something more sophisticated here one day
	LLMozLib::getInstance()->keyPress( mWindowA, keyIn );
};

////////////////////////////////////////////////////////////////////////////////
//
void uBrowser::passiveMouse( int xIn, int yIn )
{
	// called when the mouse is moving and button isn't pressed
	int x, y;
	windowPosToTexturePos( xIn, yIn, x, y );

	LLMozLib::getInstance()->mouseMove( mWindowA, x, y );
}

////////////////////////////////////////////////////////////////////////////////
//
void uBrowser::mouseButton( int button, int state, int xIn, int yIn )
{
	int x, y;
	windowPosToTexturePos( xIn, yIn, x, y );

	if ( button == GLUT_LEFT_BUTTON )
	{
		if ( state == GLUT_DOWN )
		{
			// send event to mozilla
			LLMozLib::getInstance()->mouseDown( mWindowA, x, y );
		}
		else
		if ( state == GLUT_UP )
		{
			// send event to mozilla
			LLMozLib::getInstance()->mouseUp( mWindowA, x, y );

			// this seems better than sending focus on mouse down (still need to improve this)
			LLMozLib::getInstance()->focusBrowser( mWindowA, true );

			// turn off the URL edit widget so it's "obvious" that you need to press focus to reenable it.
			// (focus restriction in GLUI i wasn't able to work around)
			mUrlEdit->disable();
		};
	};
}

////////////////////////////////////////////////////////////////////////////////
//
void uBrowser::mouseMove( int xIn , int yIn )
{
	// called when mouse moves and button is down
	int x, y;
	windowPosToTexturePos( xIn, yIn, x, y );

	LLMozLib::getInstance()->mouseMove( mWindowA, x, y );
}

////////////////////////////////////////////////////////////////////////////////
//
void uBrowser::setSize( int widthIn , int heightIn )
{
	// text the embedded browser that things changed
	LLMozLib::getInstance()->setSize( mWindowA, widthIn, heightIn );

	// calculate the next power of 2 bigger than reqquested size for width and height
	for ( mTextureWidth = 1; mTextureWidth < widthIn; mTextureWidth <<= 1 )
	{
	};

	for ( mTextureHeight = 1; mTextureHeight < heightIn; mTextureHeight <<= 1 )
	{
	};

	// save the height and width
	mBrowserWindowWidth = widthIn;
	mBrowserWindowHeight = heightIn;

	// we scale all textures by this much so that they fit the geometry
	mTextureScaleX = ( GLfloat )mBrowserWindowWidth / ( GLfloat )mTextureWidth;
	mTextureScaleY = ( GLfloat )mBrowserWindowHeight / ( GLfloat )mTextureHeight;

	// delete the old texture handle and create a new one
	glBindTexture( GL_TEXTURE_2D, 0 );
	glDeleteTextures( 1, &mAppTexture );
	glGenTextures( 1, &mAppTexture );
	glBindTexture( GL_TEXTURE_2D, mAppTexture );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, mTextureWidth, mTextureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, 0 );
}

////////////////////////////////////////////////////////////////////////////////
//
void uBrowser::gluiCallback( int controlIdIn )
{
	if ( controlIdIn == mIdExit )
	{
		// clean up code - since GLUT doesn't let us exit the event loop, we'll do it here
		// to make sure it works (deleting the browser instance was a major problem before)
		LLMozLib::getInstance()->remObserver( mWindowA, this );
		LLMozLib::getInstance()->destroyBrowserWindow( mWindowA );
		LLMozLib::getInstance()->reset();

		// write something out to the GLUT console to indicate we're all done
		std::cout << "Application finished. Buh-bye!" << std::endl;

		// only way out with GLUT
		exit( 0 );
	}
	else
	if ( controlIdIn == mIdReset )
	{
		if ( glutGetWindow() != mAppWindow )
			glutSetWindow( mAppWindow );

		resetView();

		glutPostRedisplay();
	}
	else
	if ( controlIdIn == mIdClearCookies )
	{
		LLMozLib::getInstance()->clearAllCookies();

		glutPostRedisplay();
	}
	else
	if ( controlIdIn == mIdBrowserSmall )
	{
		setSize( 400, 400 );

		if ( glutGetWindow() != mAppWindow )
			glutSetWindow( mAppWindow );

		glutPostRedisplay();
	}
	else
	if ( controlIdIn == mIdBrowserMedium )
	{
		setSize( 800, 800 );

		if ( glutGetWindow() != mAppWindow )
			glutSetWindow( mAppWindow );

		glutPostRedisplay();
	}
	else
	if ( controlIdIn == mIdBrowserLarge )
	{
		setSize( 1024, 1024 );

		if ( glutGetWindow() != mAppWindow )
			glutSetWindow( mAppWindow );

		glutPostRedisplay();
	}
	else
	if ( controlIdIn == mIdNavBack )
	{
		LLMozLib::getInstance()->navigateBack( mWindowA );
	}
	else
	if ( controlIdIn == mIdNavStop )
	{
		LLMozLib::getInstance()->navigateStop( mWindowA );
	}
	else
	if ( controlIdIn == mIdNavHome )
	{
		LLMozLib::getInstance()->navigateTo( mWindowA, mHomeUrl.c_str() );
	}
	else
	if ( controlIdIn == mIdNavForward )
	{
		LLMozLib::getInstance()->navigateForward( mWindowA );
	}
	else
	if ( controlIdIn == mIdUrlEdit )
	{
		LLMozLib::getInstance()->navigateTo( mWindowA, mUrlEdit->get_text() );
	}
	else
	if ( controlIdIn == mIdBookmarks )
	{
		LLMozLib::getInstance()->navigateTo( mWindowA, mBookmarks[ mSelBookmark ].second.c_str() );
	}
	else
	// silly hack needed to get around a limitation of GLUI
	// I really need a callback when caret enters the edit field so I can do this automatically
	if ( controlIdIn == mFocusUrlEdit )
	{
		mUrlEdit->enable();
		LLMozLib::getInstance()->focusBrowser( mWindowA, false );
		setFocusNativeWindow();
	};
}

////////////////////////////////////////////////////////////////////////////////
//
void uBrowser::onNavigateBegin( const EventType& eventIn )
{
	// could do something here like start a throbber :)
}

////////////////////////////////////////////////////////////////////////////////
//
void uBrowser::onClickLinkHref( const EventType& eventIn )
{
	std::cout << "onClickLinkHref fired with value of " << eventIn.getStringValue() << std::endl;
};

////////////////////////////////////////////////////////////////////////////////
//
void uBrowser::onClickLinkSecondLife( const EventType& eventIn )
{
	std::cout << "onClickLinkSecondLife fired with value of " << eventIn.getStringValue() << std::endl;
};

////////////////////////////////////////////////////////////////////////////////
//
void uBrowser::onNavigateComplete( const EventType& eventIn )
{
	// could do something here like stop a throbber :)
}

////////////////////////////////////////////////////////////////////////////////
//
void uBrowser::onUpdateProgress( const EventType& eventIn )
{
	// observed event - page progress changes
	glutSetWindow( mBottomGLUIWindow->get_glut_window_id() );

	std::ostringstream conv;
	conv << "[" << eventIn.getIntValue() << "% loaded]";
	mProgressText->set_text( const_cast< char* >( conv.str().c_str() ) );
}

////////////////////////////////////////////////////////////////////////////////
//
void uBrowser::onStatusTextChange( const EventType& eventIn )
{
	// observed event - status text changes, either during load or when you mouse over a link or a script does it
	glutSetWindow( mBottomGLUIWindow->get_glut_window_id() );

	// NOTE: only display the first 100 chars since anything longer breaks the display of percent loaded
	// on the right hand side - normally, you'd want to use the whole thing.
	mStatusText->set_text( const_cast< char*>( eventIn.getStringValue().substr( 0, 100 ).c_str() ) );
}

////////////////////////////////////////////////////////////////////////////////
//
void uBrowser::onLocationChange( const EventType& eventIn )
{
	// observed event - URL location changes - e.g. when a site redirects somewhere else
	// (ought to check that this is the top frame or this will be wrong)
	glutSetWindow( mTopGLUIWindow->get_glut_window_id() );

	mUrlEdit->set_text( const_cast< char*>( eventIn.getStringValue().c_str() ) );
}

////////////////////////////////////////////////////////////////////////////////
//
void* uBrowser::getNativeWindowHandle()
{
	// My implementation of the embedded browser needs a native window handle
	// Can't get this via GLUT so had to use this hack
#ifdef _WINDOWS

	return FindWindow( NULL, mName.c_str() );

#elif defined(__APPLE__)
	// The Mozilla libraries need a "native window ref" for some reason.  They don't care which window it is, though.
	// The GLUT window doesn't have a Carbon WindowRef, because it's a Cocoa window.  This code will create a window just to pacify
	// the mozilla libraries.
	static WindowRef dummyWindow = NULL;
	if(dummyWindow == NULL)
	{
		// Create a window just for this purpose.
		Rect window_rect = {100, 100, 200, 200};

		dummyWindow = NewCWindow(
			NULL,
			&window_rect,
			"\p",
			false,				// Create the window invisible.
			zoomDocProc,		// Window with a grow box and a zoom box
			kLastWindowOfClass,		// create it behind other windows
			false,					// no close box
			0);
	}
	return (void*)(dummyWindow);

#elif LL_LINUX
	gtk_disable_setlocale();
	gtk_init(NULL, NULL);

        GtkWidget *win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_widget_realize(win);
	gtk_window_set_title(GTK_WINDOW(win), "Raw gecko window");

	GTK_WIDGET_UNSET_FLAGS(GTK_WIDGET(win), GTK_NO_WINDOW);

	//gtk_widget_show_all(GTK_WIDGET(win));

	return win;
#else
#error uBrowser::getNativeWindowHandle() needs an implementation for this platform!
#endif
}

////////////////////////////////////////////////////////////////////////////////
//
void uBrowser::setFocusNativeWindow()
{
	// need to set focus to the browser window so that keyboard events work
#ifdef _WINDOWS
SetFocus( FindWindow( NULL, mName.c_str() ) );
#elif defined(__APPLE__)
// Don't think this is necessary on the Mac...
#elif LL_LINUX
// Don't think this really works like this on Linux...
#else
	#error uBrowser::setFocusNativeWindow() needs an implementation for this platform!
#endif
}

////////////////////////////////////////////////////////////////////////////////
//
GLenum uBrowser::getGLTextureFormat(int size)
{
#ifdef _WINDOWS
	return (size == 3) ? GL_BGR_EXT : GL_BGRA_EXT;
#elif defined(__APPLE__)
	return GL_BGRA;
#elif LL_LINUX
	return GL_BGRA;
#else
	#error uBrowser::getGLTextureFormat() needs an implementation for this platform!
#endif
}

////////////////////////////////////////////////////////////////////////////////
//
GLenum uBrowser::getGLTextureType(int size)
{
#ifdef _WINDOWS
	return GL_UNSIGNED_BYTE;
#elif defined(__APPLE__)
	#ifdef __ppc__
		// PowerPC Mac
		return GL_UNSIGNED_INT_8_8_8_8_REV;
	#else
		// Intel Mac
		return GL_UNSIGNED_INT_8_8_8_8;
	#endif
#elif LL_LINUX
	return GL_UNSIGNED_BYTE;
#else
	#error uBrowser::getGLTextureType() needs an implementation for this platform!
#endif
}

////////////////////////////////////////////////////////////////////////////////
//
static void gluiCallbackWrapper( int controlIdIn )
{
	// dispatch GLUI callbacks into my class cos I like it that way :)
	if ( gInstance )
		gInstance->gluiCallback( controlIdIn );
}
