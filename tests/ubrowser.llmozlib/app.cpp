/**
 * @file app.cpp
 * @brief Application stub class for uBrowser test harness
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

#include "glui.h"

uBrowser* theUBrowser;

////////////////////////////////////////////////////////////////////////////////
//
void glutReshape( int widthIn, int heightIn )
{
	if ( theUBrowser )
		theUBrowser->reshape( widthIn, heightIn );
};

////////////////////////////////////////////////////////////////////////////////
//
void glutDisplay()
{
	if ( theUBrowser )
		theUBrowser->display();
};

////////////////////////////////////////////////////////////////////////////////
//
void glutIdle()
{
	if ( theUBrowser )
		theUBrowser->idle();
};

////////////////////////////////////////////////////////////////////////////////
//
void glutKeyboard( unsigned char keyIn, int xIn, int yIn )
{
	if ( theUBrowser )
		theUBrowser->keyboard( keyIn, xIn, yIn );
};

////////////////////////////////////////////////////////////////////////////////
//
void glutSpecialKeyboard( int keyIn, int xIn, int yIn )
{
	// appears that you need this defined even if it's empty
	// passing NULL for the handler func ptr crashes this app
};

////////////////////////////////////////////////////////////////////////////////
//
void glutPassiveMouse( int xIn, int yIn )
{
	if ( theUBrowser )
		theUBrowser->passiveMouse( xIn, yIn );
}

////////////////////////////////////////////////////////////////////////////////
//
void glutMouseMove( int xIn , int yIn )
{
	if ( theUBrowser )
		theUBrowser->mouseMove( xIn, yIn );
}

////////////////////////////////////////////////////////////////////////////////
//
void glutMouseButton( int buttonIn, int stateIn, int xIn, int yIn )
{
	if ( theUBrowser )
		theUBrowser->mouseButton( buttonIn, stateIn, xIn, yIn );
}

////////////////////////////////////////////////////////////////////////////////
//
int main( int argc, char* argv[] )
{
	theUBrowser = new uBrowser;

	if ( theUBrowser )
	{
		glutInit( &argc, argv );
		glutInitDisplayMode( GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB );

		glutInitWindowPosition( 80, 0 );
		glutInitWindowSize( 1024, 900 );

		int appWindow = glutCreateWindow( theUBrowser->getName().c_str() );

		glutDisplayFunc( glutDisplay );

		GLUI_Master.set_glutReshapeFunc( glutReshape );
		GLUI_Master.set_glutKeyboardFunc( glutKeyboard );
		GLUI_Master.set_glutMouseFunc( glutMouseButton );
		GLUI_Master.set_glutSpecialFunc( glutSpecialKeyboard );

		glutPassiveMotionFunc( glutPassiveMouse );
		glutMotionFunc( glutMouseMove );

		glutSetWindow( appWindow );

		theUBrowser->init( argv[ 0 ], appWindow );

		GLUI_Master.set_glutIdleFunc( glutIdle );

		glutMainLoop();

		if ( theUBrowser )
			delete theUBrowser;
	};

	return 1;
}
