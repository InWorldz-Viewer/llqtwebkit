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
 *   Callum Prentice (callum@lindenlab.com)
 *
 * Portions created by the Initial Developer are Copyright (C) 2006
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
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

#include <windows.h>
#include <gl\gl.h>
#include "llqtwebkit.h"

int mAppWindowWidth = 1024;
int mAppWindowHeight = 1024;
int gTextureWidth = 1024;
int gTextureHeight = 1024;
int gBrowserWindowId = 0;
WPARAM gWParam = 0;
LPARAM gLParam = 0L;
int gcharcode = 0;

/////////////////////////////////////////////////////////////////////////////////
//
void resize_gl_screen( int width, int height )
{
    if ( height == 0 )
        height = 1;

    mAppWindowWidth = width;
    mAppWindowHeight = height;

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();

    glViewport( 0, 0, width, height );
    glOrtho( 0.0f, width, height, 0.0f, -1.0f, 1.0f );

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
}

/////////////////////////////////////////////////////////////////////////////////
//
void init_qt( HWND hWnd )
{
    char cur_directory[ MAX_PATH ] = "";
    GetCurrentDirectory( sizeof( cur_directory ) - 1, cur_directory );

    std::string app_dir = std::string( cur_directory );
    std::string component_dir = app_dir;

    #ifdef _DEBUG
    std::string profile_dir = app_dir + "\\debug\\" + "win32gl_profile";
    #else
    std::string profile_dir = app_dir + "\\release\\" + "win32gl_profile";
    #endif

    LLQtWebKit::getInstance()->init( app_dir, component_dir, profile_dir, hWnd );

    int browser_width = gTextureWidth;
    int browser_height = gTextureHeight;
    gBrowserWindowId = LLQtWebKit::getInstance()->createBrowserWindow( browser_width, browser_height );

    LLQtWebKit::getInstance()->setSize( gBrowserWindowId, browser_width, browser_height );

    LLQtWebKit::getInstance()->setBrowserAgentId( "win32gl" );

    LLQtWebKit::getInstance()->flipWindow( gBrowserWindowId, false );

    std::string home_url( "http://google.com" );
    home_url = "http://www.keybr.com";
    //home_url = "file:///C:/Work/llqtwebkit-4.6/tests/testgl/testpage.html";
    LLQtWebKit::getInstance()->navigateTo( gBrowserWindowId, home_url );
}

/////////////////////////////////////////////////////////////////////////////////
//
void reset_qt()
{
    LLQtWebKit::getInstance()->reset();
}

/////////////////////////////////////////////////////////////////////////////////
//
void draw_gl_scene( GLuint texture_handle )
{
    LLQtWebKit::getInstance()->pump( 100 );

    LLQtWebKit::getInstance()->grabBrowserWindow( gBrowserWindowId );

    const unsigned char* pixels = LLQtWebKit::getInstance()->getBrowserWindowPixels( gBrowserWindowId );
    if ( pixels )
    {
        glTexSubImage2D( GL_TEXTURE_2D, 0,
            0, 0,
                gTextureWidth, gTextureHeight,
                        GL_RGBA,
                            GL_UNSIGNED_BYTE,
                                pixels );
    };

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glLoadIdentity();

    glBindTexture( GL_TEXTURE_2D, texture_handle );

    glEnable( GL_TEXTURE_2D );
    glColor3f( 1.0f, 1.0f, 1.0f );
    glBegin( GL_QUADS );
        glTexCoord2f( 1.0f, 0.0f );
        glVertex2d( mAppWindowWidth, 0 );

        glTexCoord2f( 0.0f, 0.0f );
        glVertex2d( 0, 0 );

        glTexCoord2f( 0.0f, 1.0f );
        glVertex2d( 0, mAppWindowHeight );

        glTexCoord2f( 1.0f, 1.0f );
        glVertex2d( mAppWindowWidth, mAppWindowHeight );
    glEnd();
}

/////////////////////////////////////////////////////////////////////////////////
//
void mouse_event( int x, int y, LLQtWebKit::EMouseEvent mev )
{
    LLQtWebKit::getInstance()->mouseEvent( gBrowserWindowId,
                                                mev, LLQtWebKit::MB_MOUSE_BUTTON_LEFT,
                                                    x, y, LLQtWebKit::KM_MODIFIER_NONE );
}

/////////////////////////////////////////////////////////////////////////////////
//
void keyboard_event( int char_code, LLQtWebKit::EKeyEvent kev )
{
    unsigned long key = (unsigned long)char_code;
    char utf8str[2];
	utf8str[0]= (char)char_code;
	utf8str[1]= 0;
    unsigned long native_scan_code = (unsigned long)( ( gLParam >> 16 ) & 0xff );
    unsigned long native_virtual_key = ( unsigned long )( gWParam );

	unsigned long native_modifiers = LLQtWebKit::KM_MODIFIER_NONE;
    LLQtWebKit::EKeyboardModifier modifiers = LLQtWebKit::KM_MODIFIER_NONE;
    if ( GetKeyState( VK_SHIFT ) )
    {
        modifiers = LLQtWebKit::KM_MODIFIER_SHIFT;
        native_modifiers = LLQtWebKit::KM_MODIFIER_SHIFT;
    };

    LLQtWebKit::getInstance()->keyboardEvent( gBrowserWindowId, kev, key, utf8str, modifiers, native_scan_code, native_virtual_key, native_modifiers );
}

/////////////////////////////////////////////////////////////////////////////////
//
LRESULT CALLBACK window_proc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	static int last_char_code = 0;

    switch ( uMsg )
    {
        case WM_LBUTTONDOWN:
        {
            int x_pos = ( LOWORD( lParam ) * gTextureWidth ) / mAppWindowWidth;
            int y_pos = ( HIWORD( lParam ) * gTextureHeight ) / mAppWindowHeight;
            mouse_event( x_pos, y_pos, LLQtWebKit::ME_MOUSE_DOWN );
            return 0;
        };

        case WM_LBUTTONUP:
        {
            int x_pos = ( LOWORD( lParam ) * gTextureWidth ) / mAppWindowWidth;
            int y_pos = ( HIWORD( lParam ) * gTextureHeight ) / mAppWindowHeight;
            mouse_event( x_pos, y_pos, LLQtWebKit::ME_MOUSE_UP );
            return 0;
        };

        case WM_MOUSEMOVE:
        {
            int x_pos = ( LOWORD( lParam ) * gTextureWidth ) / mAppWindowWidth;
            int y_pos = ( HIWORD( lParam ) * gTextureHeight ) / mAppWindowHeight;
            mouse_event( x_pos, y_pos, LLQtWebKit::ME_MOUSE_MOVE );
            return 0;
        };

        case WM_CLOSE:
        {
            PostQuitMessage( 0 );
            return 0;
        };

        case WM_CHAR:
        {
			// pick up the character code here - keyboard event grabs virtual/scan 
			// key codes set during WM_KEYDOWN
			last_char_code = (int)wParam;
            keyboard_event( last_char_code, LLQtWebKit::KE_KEY_DOWN );
			return 0;
        };

        case WM_KEYDOWN:
        {
			// no keyboard_event here because we don't know the character code here.
			// just save the wParam and lParam values - WM_CHAR will get sent next 
			// and call keyboard_event with char code.
			gLParam = lParam;
			gWParam = wParam;
            return 0;
        };

        case WM_KEYUP:
        {
			// don't know what char code to send here - no WM_CHAR after a WM_KEYUP
			// in the same way WM_KEYDOWN works.
            keyboard_event( last_char_code, LLQtWebKit::KE_KEY_UP );

			// not sure if we need yo update these here too?
			gLParam = lParam;
			gWParam = wParam;
			return 0;
        };

        case WM_SIZE:
        {
            int new_width = LOWORD( lParam );
            int new_height = HIWORD( lParam );
            resize_gl_screen( new_width, new_height );
            return 0;
        };
    };

    return DefWindowProc( hWnd, uMsg, wParam, lParam );
}

/////////////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv)
{
    return WinMain(NULL, NULL, NULL, 0);
}

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
    WNDCLASS wc;
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = (WNDPROC)window_proc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon( NULL, IDI_WINLOGO );
    wc.hCursor = LoadCursor( NULL, IDC_ARROW );
    wc.hbrBackground = NULL;
    wc.lpszMenuName = NULL;
    wc.lpszClassName = "Win32GL";
    RegisterClass( &wc );

    RECT window_rect;
    SetRect( &window_rect, 0, 0, mAppWindowWidth, mAppWindowHeight );

    DWORD ex_style = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
    DWORD style = WS_OVERLAPPEDWINDOW;
    AdjustWindowRectEx( &window_rect, style, FALSE, ex_style );

    HWND hWnd = CreateWindowEx( ex_style, "Win32GL", "Win32GL LLQtWebKit test", style | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
                            80, 0, window_rect.right - window_rect.left, window_rect.bottom - window_rect.top,
                                NULL, NULL, hInstance, NULL );

    static  PIXELFORMATDESCRIPTOR pfd =
    {
        sizeof( PIXELFORMATDESCRIPTOR ),
        1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, PFD_TYPE_RGBA,
        32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 0, 0, PFD_MAIN_PLANE, 0, 0, 0, 0
    };

    HDC hDC = GetDC( hWnd );

    GLuint pixel_format = ChoosePixelFormat( hDC, &pfd );
    SetPixelFormat( hDC, pixel_format, &pfd );
    HGLRC hRC = wglCreateContext( hDC );
    wglMakeCurrent( hDC, hRC );

    ShowWindow( hWnd,SW_SHOW );
    SetForegroundWindow( hWnd );
    SetFocus( hWnd );

    resize_gl_screen( mAppWindowWidth, mAppWindowHeight );

    GLuint texture_handle = 0;
    glGenTextures( 1, &texture_handle );

    glBindTexture( GL_TEXTURE_2D, texture_handle );
    glTexImage2D( GL_TEXTURE_2D, 0, 3, gTextureWidth, gTextureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, 0 );
    glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR );
    init_qt( hWnd );

    bool done = false;
    while( ! done )
    {
        MSG msg;
        if ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
        {
            if ( WM_QUIT == msg.message )
            {
                done = true;
            }
            else
            {
                TranslateMessage( &msg );
                DispatchMessage( &msg );
            };
        }
        else
        {
            draw_gl_scene( texture_handle );
            SwapBuffers( hDC );
        };
    };

    wglMakeCurrent( NULL,NULL );
    wglDeleteContext( hRC );
    ReleaseDC( hWnd,hDC );
    DestroyWindow( hWnd );
    UnregisterClass( "Win32GL", hInstance );

    reset_qt();

    return 0;
}
