/**
 * @file llembeddedbrowserwindow.cpp
 * @brief LLEmbeddedBrowserWindow implementation.
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

// required by LibXUL/Mozilla code to avoid crashes in their debug code, but only on Windows.
// undef'd at end of this
#ifdef _DEBUG
	#ifdef WIN32
		#define DEBUG 1
	#endif
#endif

#include "llembeddedbrowserwindow.h"

// Mozilla code has non-virtual destructors
#ifdef WIN32
#pragma warning( disable : 4291 ) // (no matching operator delete found; memory will not be freed if initialization throws an exception)
#pragma warning( disable : 4265 ) // "class has virtual functions, but destructor is not virtual"
#endif

#include "nsCWebBrowser.h"
#include "nsGUIEvent.h"
#include "nsICaret.h"
#include "nsIContent.h"
#include "nsIDOMDocument.h"
#include "nsIDOMElement.h"
#include "nsIDOMWindow.h"
#include "nsIDOMEvent.h"
#include "nsIDocShell.h"
#include "nsIDocShellTreeItem.h"
#include "nsIDocument.h"
#include "nsIFrame.h"
#include "nsIHttpChannel.h"
#include "nsIInterfaceRequestorUtils.h"
#include "nsIScrollableView.h"
#include "nsISelection.h"
#include "nsISelectionController.h"
#include "nsIWebBrowserChrome.h"
#include "nsIWebBrowserChromeFocus.h"
#include "nsIWebBrowserFocus.h"
#include "nsIWebProgress.h"
#include "nsIWebProgressListener.h"
#include "nsPresContext.h"
#include "nsProfileDirServiceProvider.h"
#include "nsXPCOMGlue.h"
#include "nsXULAppAPI.h"

#ifdef WIN32
#pragma warning( 3 : 4265 ) // "class has virtual functions, but destructor is not virtual"
#endif

////////////////////////////////////////////////////////////////////////////////
//
LLEmbeddedBrowserWindow::LLEmbeddedBrowserWindow() :
	mWebBrowser( nsnull ),
	mBaseWindow( nsnull ),
	mWindowId( 0 ),
	mPercentComplete( 0 ),
	mBrowserWidth( 0 ),
	mBrowserHeight( 0 ),
	mBrowserDepth( 4 ),
	mPageBuffer( NULL ),
	mPageBufferSize ( 0 ),
	mCurrentUri( "" ),
	mStatusText( "" ),
	mClickHref( "" ),
	mBackgroundRed( 0xff ),
	mBackgroundGreen( 0xff ),
	mBackgroundBlue( 0xff ),
	mCaretRed( 0x0 ),
	mCaretGreen( 0x0 ),
	mCaretBlue( 0x0 ),
	m404RedirectUrl( "" )
{
}

////////////////////////////////////////////////////////////////////////////////
//
LLEmbeddedBrowserWindow::~LLEmbeddedBrowserWindow()
{
	if ( mWebNav )
	{
		mWebNav->Stop ( nsIWebNavigation::STOP_ALL );
		mWebNav = nsnull;
	};

	if ( mBaseWindow )
	{
		mBaseWindow->Destroy();
		mBaseWindow = nsnull;
	};

	if ( mPageBuffer )
	{
		delete[] mPageBuffer;
		mPageBuffer = NULL;
		mPageBufferSize = 0;
	};
}

////////////////////////////////////////////////////////////////////////////////
//
nsresult LLEmbeddedBrowserWindow::createBrowser( void* nativeWindowHandleIn, PRInt32 widthIn, PRInt32 heightIn, nsIWebBrowser **aBrowser )
{
	NS_ENSURE_ARG_POINTER(aBrowser);
	*aBrowser = nsnull;

	mWebBrowser = do_CreateInstance( NS_WEBBROWSER_CONTRACTID );
	if ( ! mWebBrowser )
		return NS_ERROR_FAILURE;

    (void)mWebBrowser->SetContainerWindow(NS_STATIC_CAST(nsIWebBrowserChrome*, this));

    nsCOMPtr<nsIDocShellTreeItem> dsti = do_QueryInterface(mWebBrowser);
    dsti->SetItemType(nsIDocShellTreeItem::typeContentWrapper);

    mBaseWindow = do_QueryInterface(mWebBrowser);

    mBaseWindow->InitWindow( nativeWindowHandleIn, nsnull,  0, 0, mBrowserWidth, mBrowserHeight );
    mBaseWindow->Create();

    nsCOMPtr<nsIWebProgressListener> listener(NS_STATIC_CAST(nsIWebProgressListener*, this));
    nsCOMPtr<nsIWeakReference> thisListener(do_GetWeakReference(listener));
    (void)mWebBrowser->AddWebBrowserListener(thisListener,
        NS_GET_IID(nsIWebProgressListener));

	nsresult result;
	// set the listener to we can catch nsURIContentListener events
	if ( mWebBrowser )
	{
		result = mWebBrowser->SetParentURIContentListener( NS_STATIC_CAST( nsIURIContentListener*, this ) );
		if ( NS_FAILED( result ) )
		{
		};
	};

	mWebNav = do_QueryInterface( mWebBrowser, &result );
	if ( NS_FAILED( result ) || ! mWebNav )
	{
	};

	mBaseWindow->SetVisibility( PR_FALSE );

	setSize( widthIn, heightIn );

    if (mWebBrowser)
    {
		*aBrowser = mWebBrowser;
		NS_ADDREF(*aBrowser);
		return NS_OK;
    };

    return NS_ERROR_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////
//
NS_IMPL_ADDREF( LLEmbeddedBrowserWindow )
NS_IMPL_RELEASE( LLEmbeddedBrowserWindow )

////////////////////////////////////////////////////////////////////////////////
//
NS_INTERFACE_MAP_BEGIN( LLEmbeddedBrowserWindow )
	NS_INTERFACE_MAP_ENTRY_AMBIGUOUS( nsISupports, nsIWebBrowserChrome )
	NS_INTERFACE_MAP_ENTRY( nsIInterfaceRequestor )
	NS_INTERFACE_MAP_ENTRY( nsIWebBrowserChrome )
	NS_INTERFACE_MAP_ENTRY( nsIWebProgressListener )
	NS_INTERFACE_MAP_ENTRY( nsIURIContentListener )
	NS_INTERFACE_MAP_ENTRY( nsISupportsWeakReference )
NS_INTERFACE_MAP_END

////////////////////////////////////////////////////////////////////////////////
//
NS_IMETHODIMP LLEmbeddedBrowserWindow::GetInterface( const nsIID &aIID, void** aInstancePtr )
{
	if ( aIID.Equals( NS_GET_IID( nsIDOMWindow ) ) )
	{
		if ( mWebBrowser )
		{
			return mWebBrowser->GetContentDOMWindow( ( nsIDOMWindow** )aInstancePtr );
		};

		return NS_ERROR_NOT_INITIALIZED;
	};

	return QueryInterface( aIID, aInstancePtr );
}

////////////////////////////////////////////////////////////////////////////////
// called when something changes the status text - emits event to consumer
NS_IMETHODIMP LLEmbeddedBrowserWindow::SetStatus( PRUint32 aType, const PRUnichar* aStatus )
{
	mStatusText = std::string( NS_ConvertUTF16toUTF8( aStatus ).get() );

	LLEmbeddedBrowserWindowEvent event( getWindowId(), mStatusText );
	mEventEmitter.update( &LLEmbeddedBrowserWindowObserver::onStatusTextChange, event );

	return NS_OK;
}

////////////////////////////////////////////////////////////////////////////////
//
NS_IMETHODIMP LLEmbeddedBrowserWindow::GetWebBrowser( nsIWebBrowser** aWebBrowser )
{
	NS_ENSURE_ARG_POINTER( aWebBrowser );

	*aWebBrowser = mWebBrowser;

	NS_IF_ADDREF( *aWebBrowser );

	return NS_OK;
}

////////////////////////////////////////////////////////////////////////////////
//
NS_IMETHODIMP LLEmbeddedBrowserWindow::SetWebBrowser( nsIWebBrowser* aWebBrowser )
{
	NS_ENSURE_ARG_POINTER( aWebBrowser );

	mWebBrowser = aWebBrowser;

	return NS_OK;
}

////////////////////////////////////////////////////////////////////////////////
//
NS_IMETHODIMP LLEmbeddedBrowserWindow::GetChromeFlags( PRUint32* aChromeMask )
{
	return NS_ERROR_NOT_IMPLEMENTED;
}

////////////////////////////////////////////////////////////////////////////////
//
NS_IMETHODIMP LLEmbeddedBrowserWindow::SetChromeFlags( PRUint32 aChromeMask )
{
	return NS_ERROR_NOT_IMPLEMENTED;
}

////////////////////////////////////////////////////////////////////////////////
//
NS_IMETHODIMP LLEmbeddedBrowserWindow::DestroyBrowserWindow()
{
	return NS_OK;
}

////////////////////////////////////////////////////////////////////////////////
//
NS_IMETHODIMP LLEmbeddedBrowserWindow::SizeBrowserTo( PRInt32 aCX, PRInt32 aCY )
{
	return NS_OK;
}

////////////////////////////////////////////////////////////////////////////////
//
NS_IMETHODIMP LLEmbeddedBrowserWindow::ShowAsModal()
{
	return NS_ERROR_NOT_IMPLEMENTED;
}

////////////////////////////////////////////////////////////////////////////////
//
//
NS_IMETHODIMP LLEmbeddedBrowserWindow::IsWindowModal( PRBool* retval )
{
	*retval = PR_FALSE;

	return NS_OK;
}

////////////////////////////////////////////////////////////////////////////////
//
NS_IMETHODIMP LLEmbeddedBrowserWindow::ExitModalEventLoop( nsresult aStatus )
{
	return NS_OK;
}

////////////////////////////////////////////////////////////////////////////////
// called when the page loading progress changes - emits event to consumer
NS_IMETHODIMP LLEmbeddedBrowserWindow::OnProgressChange( nsIWebProgress* progress, nsIRequest* request,
													PRInt32 curSelfProgress, PRInt32 maxSelfProgress,
														PRInt32 curTotalProgress, PRInt32 maxTotalProgress )
{
	mPercentComplete = static_cast< PRInt16 >
		( static_cast< float >( curTotalProgress * 100.0f ) / static_cast< float >( maxTotalProgress ) );

	if ( mPercentComplete < 0 )
		mPercentComplete = 0;

	if ( mPercentComplete > 100 )
		mPercentComplete = 100;

	LLEmbeddedBrowserWindowEvent event( getWindowId(), mPercentComplete );
	mEventEmitter.update( &LLEmbeddedBrowserWindowObserver::onUpdateProgress, event );

	return NS_OK;
}

////////////////////////////////////////////////////////////////////////////////
// called when the browser state changes - as described below - emits event to consumer
NS_IMETHODIMP LLEmbeddedBrowserWindow::OnStateChange( nsIWebProgress* progress, nsIRequest* request,
													PRUint32 progressStateFlags, nsresult status )
{
 	if ( ( progressStateFlags & STATE_START ) && ( progressStateFlags & STATE_IS_DOCUMENT ) && ( status == NS_OK ) )
	{
		// page load is starting so remove listener that catches "click" events
		nsCOMPtr< nsIDOMWindow > window;
		nsresult result = progress->GetDOMWindow( getter_AddRefs( window ) );
		if ( result == NS_OK )
		{
			nsCOMPtr< nsIDOMEventTarget > target = do_QueryInterface( window );
			if ( target )
				target->RemoveEventListener(NS_ConvertUTF8toUTF16( "click" ), this, PR_TRUE );
		};

		// emit event that navigation is beginning
		mStatusText = std::string( "Browser loaded" );
		LLEmbeddedBrowserWindowEvent event( getWindowId() );
		mEventEmitter.update( &LLEmbeddedBrowserWindowObserver::onNavigateBegin, event );
	};

	if ( ( progressStateFlags & STATE_STOP ) && ( progressStateFlags & STATE_IS_WINDOW ) && ( status == NS_OK ) )
	{
		// page load is complete so add listener that catches "click" events
		nsCOMPtr< nsIDOMWindow > window;
		nsresult result = progress->GetDOMWindow( getter_AddRefs( window ) );
		if ( result == NS_OK )
		{
			nsCOMPtr< nsIDOMEventTarget > target = do_QueryInterface( window );
			if ( target )
				target->AddEventListener(NS_ConvertUTF8toUTF16( "click" ), this, PR_TRUE );
		};

		// emit event that navigation is finished
		mStatusText = std::string( "Done" );
		LLEmbeddedBrowserWindowEvent event( getWindowId() );
		mEventEmitter.update( &LLEmbeddedBrowserWindowObserver::onNavigateComplete, event );
	};

	if ( progressStateFlags & STATE_REDIRECTING )
	{
		mStatusText = std::string( "Redirecting..." );
	};

	if ( progressStateFlags & STATE_TRANSFERRING )
	{
		mStatusText = std::string( "Transferring..." );
	};

	if ( progressStateFlags & STATE_NEGOTIATING )
	{
		mStatusText = std::string( "Negotiating..." );
	};

	LLEmbeddedBrowserWindowEvent event( getWindowId(), mStatusText );
	mEventEmitter.update( &LLEmbeddedBrowserWindowObserver::onStatusTextChange, event );

	return NS_OK;
}

////////////////////////////////////////////////////////////////////////////////
// call when the location changes - e.g. when a site redirects - emits event to consumer
// TODO: ought to check that this change is on the top frame and
// indicate this to the consumer of this class
NS_IMETHODIMP LLEmbeddedBrowserWindow::OnLocationChange( nsIWebProgress* progress,
															nsIRequest* request,
																nsIURI* location )
{
	if ( request )
	{
		nsCOMPtr< nsIHttpChannel > http_channel = do_QueryInterface( request );
		if ( http_channel )
		{
			PRUint32 response_status = 0;
			http_channel->GetResponseStatus( &response_status );

			if ( response_status == 404 )
			{
				if ( ! m404RedirectUrl.empty() )
				{
					if ( mWebNav )
					{
						mWebNav->LoadURI( reinterpret_cast< const PRUnichar* >
							( NS_ConvertUTF8toUTF16( m404RedirectUrl.c_str() ).get() ),
								nsIWebNavigation::LOAD_FLAGS_REPLACE_HISTORY,
									nsnull, nsnull, nsnull );
					};
				};
			};
		};
	};

	nsCAutoString newURI;
	location->GetSpec( newURI );

	mCurrentUri = newURI.get();

	LLEmbeddedBrowserWindowEvent event( getWindowId(), mCurrentUri );
	mEventEmitter.update( &LLEmbeddedBrowserWindowObserver::onLocationChange, event );

	return NS_OK;
}

////////////////////////////////////////////////////////////////////////////////
// allow consumers of this class to observe events - add themselves as an observer
bool LLEmbeddedBrowserWindow::addObserver( LLEmbeddedBrowserWindowObserver* observerIn )
{
	return mEventEmitter.addObserver( observerIn );
}

////////////////////////////////////////////////////////////////////////////////
// allow consumers of this class to observe events - remove themselves as an observer
bool LLEmbeddedBrowserWindow::remObserver( LLEmbeddedBrowserWindowObserver* observerIn )
{
	return mEventEmitter.remObserver( observerIn );
}

////////////////////////////////////////////////////////////////////////////////
// used by observers of this class to get the current URI
const std::string& LLEmbeddedBrowserWindow::getCurrentUri()
{
	return mCurrentUri;
}

////////////////////////////////////////////////////////////////////////////////
// utility method that is used by observers to retrieve data after an event
const PRInt16 LLEmbeddedBrowserWindow::getPercentComplete()
{
	return mPercentComplete;
}

////////////////////////////////////////////////////////////////////////////////
// utility method that is used by observers to retrieve data after an event
const std::string& LLEmbeddedBrowserWindow::getStatusMsg()
{
	return mStatusText;
}

////////////////////////////////////////////////////////////////////////////////
// utility method that is used by observers to retrieve data after an event
const std::string& LLEmbeddedBrowserWindow::getClickLinkHref()
{
	return mClickHref;
}

////////////////////////////////////////////////////////////////////////////////
// called when the status text is changed - emits event to consumer
NS_IMETHODIMP LLEmbeddedBrowserWindow::OnStatusChange( nsIWebProgress* aWebProgress,
														nsIRequest* aRequest,
															nsresult aStatus,
																const PRUnichar* aMessage )
{
	mStatusText = std::string( NS_ConvertUTF16toUTF8( aMessage ).get() );

	LLEmbeddedBrowserWindowEvent event( getWindowId(), mStatusText );
	mEventEmitter.update( &LLEmbeddedBrowserWindowObserver::onStatusTextChange, event );

	return NS_OK;
}

////////////////////////////////////////////////////////////////////////////////
// implement this if you want to do something when the security state changtes
NS_IMETHODIMP LLEmbeddedBrowserWindow::OnSecurityChange( nsIWebProgress* aWebProgress,
													nsIRequest* aRequest,
														PRUint32 state )
{
	return NS_OK;
}

////////////////////////////////////////////////////////////////////////////////
//
unsigned char* LLEmbeddedBrowserWindow::grabWindow()
{
	nsCOMPtr< nsIDocShell > docShell = do_GetInterface( mWebBrowser );
	if ( ! docShell )
		return PR_FALSE;

	nsCOMPtr< nsPresContext > presContext;
	nsresult result = docShell->GetPresContext( getter_AddRefs( presContext ) );
	if ( NS_FAILED( result ) || ( ! presContext ) )
		return PR_FALSE;

	nsIViewManager* viewManager = presContext->GetViewManager();
	if ( ! viewManager )
		return PR_FALSE;

	nsIScrollableView* scrollableView = NULL;
	viewManager->GetRootScrollableView( &scrollableView );

	nsIView* view = NULL;
	if ( scrollableView )
		scrollableView->GetScrolledView( view );
	else
		viewManager->GetRootView( view );

	nsRect rect = view->GetBounds() - view->GetPosition() - view->GetPosition();
	if ( rect.IsEmpty() )
		return 0;

	float p2t = presContext->PixelsToTwips();
	rect.width = NSIntPixelsToTwips( mBrowserWidth, p2t );
	rect.height = NSIntPixelsToTwips( mBrowserHeight, p2t );

	nsCOMPtr< nsIRenderingContext > context;
	result = viewManager->RenderOffscreen( view, rect, PR_FALSE, PR_FALSE, NS_RGB( mBackgroundRed, mBackgroundGreen, mBackgroundBlue ), getter_AddRefs( context ) );
	if ( NS_FAILED( result ) )
		return 0;

	nsIDrawingSurface* surface = nsnull;
	context->GetDrawingSurface( &surface );
	if ( ! surface )
		return 0;

	float t2p = presContext->TwipsToPixels();
	PRInt32 width = NSTwipsToIntPixels( rect.width, t2p );
	PRInt32 height = NSTwipsToIntPixels( rect.height, t2p );

	PRUint8* data;
	PRInt32 rowLen;

	// sometime rowspan ! width in pixels * bytes per pixel so save row span value and use in application
	result = surface->Lock( 0, 0, width, height, reinterpret_cast< void** >( &data ), &mBrowserRowSpan, &rowLen, NS_LOCK_SURFACE_READ_ONLY );
	if ( NS_FAILED ( result ) )
		return 0;

	PRUint32 bytesPerPix = rowLen / width;
	nsPixelFormat format;
	surface->GetPixelFormat( &format );
	mBrowserDepth = bytesPerPix;
	size_t bufferSize = mBrowserRowSpan * mBrowserHeight;

	if( (mPageBuffer) && ( mPageBufferSize < bufferSize ) )
	{
		// the page buffer exists, but it's too small.  Delete it so it'll be recreated.
		delete[] mPageBuffer;
		mPageBuffer = NULL;
		mPageBufferSize = 0;
	}

	// create it here so it can be deleted and recreated elsewhere
	if ( ! mPageBuffer )
	{
		mPageBufferSize = bufferSize;
		mPageBuffer = new unsigned char[ mPageBufferSize ];
	}

	memcpy( mPageBuffer, data, bufferSize );

	surface->Unlock();
	context->DestroyDrawingSurface( surface );

	renderCaret();

	return mPageBuffer;
}

////////////////////////////////////////////////////////////////////////////////
// return the buffer that contains the rendered page
unsigned char* LLEmbeddedBrowserWindow::getPageBuffer()
{
	return mPageBuffer;
}

////////////////////////////////////////////////////////////////////////////////
//
PRInt16 LLEmbeddedBrowserWindow::getBrowserWidth()
{
	return mBrowserWidth;
}

////////////////////////////////////////////////////////////////////////////////
//
PRInt16 LLEmbeddedBrowserWindow::getBrowserHeight()
{
	return mBrowserHeight;
}

////////////////////////////////////////////////////////////////////////////////
//
PRInt16 LLEmbeddedBrowserWindow::getBrowserDepth()
{
	return mBrowserDepth;
}

////////////////////////////////////////////////////////////////////////////////
//
PRInt32 LLEmbeddedBrowserWindow::getBrowserRowSpan()
{
	return mBrowserRowSpan;
}

////////////////////////////////////////////////////////////////////////////////
//
PRBool LLEmbeddedBrowserWindow::navigateTo( const std::string uriIn )
{
	if ( mWebNav )
	{
		mWebNav->LoadURI( reinterpret_cast< const PRUnichar* >( NS_ConvertUTF8toUTF16( uriIn.c_str() ).get() ),
			nsIWebNavigation::LOAD_FLAGS_NONE,
				nsnull, nsnull, nsnull );

		return PR_TRUE;
	};

	return PR_FALSE;
};

////////////////////////////////////////////////////////////////////////////////
//
PRBool LLEmbeddedBrowserWindow::canNavigateBack()
{
	if ( ! mWebNav )
		return PR_FALSE;

	PRBool canGoBack = PR_FALSE;

	nsresult result = mWebNav->GetCanGoBack( &canGoBack );
	if ( NS_FAILED( result ) )
	{
		return PR_FALSE;
	};

	return canGoBack;
};

////////////////////////////////////////////////////////////////////////////////
//
void LLEmbeddedBrowserWindow::navigateStop()
{
	if ( mWebNav )
		mWebNav->Stop( nsIWebNavigation::STOP_ALL );
};

////////////////////////////////////////////////////////////////////////////////
//
void LLEmbeddedBrowserWindow::navigateBack()
{
	if ( mWebNav )
		mWebNav->GoBack();
};

////////////////////////////////////////////////////////////////////////////////
//
PRBool LLEmbeddedBrowserWindow::canNavigateForward()
{
	if ( ! mWebNav )
		return PR_FALSE;

	PRBool canGoForward = PR_FALSE;

	nsresult result = mWebNav->GetCanGoForward( &canGoForward );
	if ( NS_FAILED( result ) )
	{
		return PR_FALSE;
	};

	return canGoForward;
};

////////////////////////////////////////////////////////////////////////////////
//
void LLEmbeddedBrowserWindow::navigateForward()
{
	if ( mWebNav )
		mWebNav->GoForward();
};

////////////////////////////////////////////////////////////////////////////////
//
PRBool LLEmbeddedBrowserWindow::setSize( PRInt16 widthIn, PRInt16 heightIn )
{
	if ( widthIn > 2048 )
		widthIn = 2048;

	if ( heightIn > 2048 )
		heightIn = 2048;

	if ( mBaseWindow )
	{
		// if there is a buffer already, get rid of it (it will get created as required in grabWindow())
		if ( mPageBuffer )
		{
			delete[] mPageBuffer;
			mPageBuffer = NULL;
			mPageBufferSize = 0;
		};

		// record new size
		mBrowserWidth = widthIn;
		mBrowserHeight = heightIn;

		// On the Mac, these calls do strange things to the main viewer window, and they don't seem necessary in any case.
#if WIN32
		// this is the actual OS (on Win32) Window so it needs to be hidden
		mBaseWindow->SetVisibility( PR_FALSE );

		// move off screen (and in a place that makes the combobox hack work
		mBaseWindow->SetPosition( 8000, -6000 );
#endif

		// tell Mozilla about the new size
		mBaseWindow->SetSize( widthIn, heightIn, PR_FALSE );

		return PR_TRUE;
	};

	return PR_FALSE;
}

////////////////////////////////////////////////////////////////////////////////
// higher level mouse event
void LLEmbeddedBrowserWindow::mouseDown( PRInt16 xPosIn, PRInt16 yPosIn )
{
	const PRUint32 clickCount = 1;
	sendMozillaMouseEvent( NS_MOUSE_LEFT_BUTTON_DOWN, xPosIn, yPosIn, clickCount );
}

////////////////////////////////////////////////////////////////////////////////
// higher level mouse event
void LLEmbeddedBrowserWindow::mouseUp( PRInt16 xPosIn, PRInt16 yPosIn )
{
	const PRUint32 clickCount = 1;
	sendMozillaMouseEvent( NS_MOUSE_LEFT_BUTTON_UP, xPosIn, yPosIn, clickCount );
}

////////////////////////////////////////////////////////////////////////////////
// higher level mouse event
void LLEmbeddedBrowserWindow::mouseMove( PRInt16 xPosIn, PRInt16 yPosIn )
{
	const PRUint32 clickCount = 1;	// ignored?
	sendMozillaMouseEvent( NS_MOUSE_MOVE, xPosIn, yPosIn, clickCount );
}

////////////////////////////////////////////////////////////////////////////////
// higher level mouse event
void LLEmbeddedBrowserWindow::mouseLeftDoubleClick( PRInt16 xPosIn, PRInt16 yPosIn )
{
	// Internally Mozilla represents double-click as a 2-count mouse down event.
	// TODO: support triple-click
	const PRUint32 clickCount = 2;
	sendMozillaMouseEvent( NS_MOUSE_LEFT_BUTTON_DOWN, xPosIn, yPosIn, clickCount );
}

////////////////////////////////////////////////////////////////////////////////
// utility methods to set an error message so something else can look at it
void LLEmbeddedBrowserWindow::scrollByLines( PRInt16 linesIn )
{
	if ( mWebBrowser )
	{
		nsCOMPtr< nsIDOMWindow > window;
		nsresult result = mWebBrowser->GetContentDOMWindow( getter_AddRefs( window ) );

		if ( ! NS_FAILED( result ) && window )
		{
			result = window->ScrollByLines( linesIn );
		};
	};
}

////////////////////////////////////////////////////////////////////////////////
// synthesizes a mouse event and sends into the embedded instance
//
// eventIn - NS_MOUSE_LEFT_BUTTON_DOWN, NS_MOUSE_LEFT_BUTTON_UP, etc.
// xPosIn, yPosIn - coordinates (in browser window space?)
// clickCountIn - use 1 for single click, 2 for double-click, etc.
PRBool LLEmbeddedBrowserWindow::sendMozillaMouseEvent( PRInt16 eventIn, PRInt16 xPosIn, PRInt16 yPosIn, PRUint32 clickCountIn )
{
	nsCOMPtr< nsIDocShell > docShell = do_GetInterface( mWebBrowser );
	if ( ! docShell )
		return PR_FALSE;

	nsCOMPtr< nsPresContext > presContext;
	nsresult result = docShell->GetPresContext( getter_AddRefs( presContext ) );
	if ( NS_FAILED( result ) || ( ! presContext ) )
		return PR_FALSE;

	nsIViewManager* viewManager = presContext->GetViewManager();
	if ( ! viewManager )
		return PR_FALSE;

	nsIView* rootView;
	result = viewManager->GetRootView( rootView );
	if ( NS_FAILED( result ) || ( ! rootView ) )
		return PR_FALSE;

	nsCOMPtr< nsIWidget > widget = rootView->GetWidget();
	if ( ! widget )
		return PR_FALSE;

	nsMouseEvent mouseEvent( PR_TRUE, eventIn, widget, nsMouseEvent::eReal );
	mouseEvent.clickCount = clickCountIn;
	mouseEvent.isShift = 0;
	mouseEvent.isControl = 0;
	mouseEvent.isAlt = 0;
	mouseEvent.isMeta = 0;
	mouseEvent.widget = widget;
	mouseEvent.nativeMsg = nsnull;
	mouseEvent.point.x = xPosIn;
	mouseEvent.point.y = yPosIn;
	mouseEvent.refPoint.x = xPosIn;
	mouseEvent.refPoint.y = yPosIn;
	mouseEvent.flags = 0;

	nsEventStatus status;
	result = viewManager->DispatchEvent( &mouseEvent, &status );
	if ( NS_FAILED( result ) )
		return PR_FALSE;

	return PR_TRUE;
};

////////////////////////////////////////////////////////////////////////////////
// higher level keyboard functions

// accept a (mozilla-style) keycode
void LLEmbeddedBrowserWindow::keyPress( PRInt16 keyCode )
{
	sendMozillaKeyboardEvent( 0, keyCode );
}

// accept keyboard input that's already been translated into a unicode char.
void LLEmbeddedBrowserWindow::unicodeInput( PRUint32 uni_char )
{
	sendMozillaKeyboardEvent( uni_char, 0 );
}

////////////////////////////////////////////////////////////////////////////////
// synthesizes a keyboard event and sends into the embedded instance
PRBool LLEmbeddedBrowserWindow::sendMozillaKeyboardEvent( PRUint32 uni_char, PRUint32 ns_vk_code )
{
	nsresult result = NS_OK;

	nsCOMPtr< nsIDocShell > docShell = do_GetInterface( mWebBrowser );
	if ( ! docShell )
		return PR_FALSE;

	nsCOMPtr< nsPresContext > presContext;
	docShell->GetPresContext( getter_AddRefs( presContext ) );
	if ( ! presContext )
		return PR_FALSE;

	nsIViewManager* viewManager = presContext->GetViewManager();
	if ( ! viewManager )
		return PR_FALSE;

	nsIView* rootView;
	viewManager->GetRootView( rootView );
	if ( ! rootView )
		return PR_FALSE;

	nsCOMPtr< nsIWidget > widget = rootView->GetWidget();
	if ( ! widget )
		return PR_FALSE;

	nsKeyEvent keyEvent( PR_TRUE, NS_KEY_PRESS, widget );
	keyEvent.keyCode = ns_vk_code;
	keyEvent.charCode = uni_char;
	keyEvent.isChar = uni_char ? PR_TRUE : PR_FALSE;
	keyEvent.isShift = 0;
	keyEvent.isControl = 0;
	keyEvent.isAlt = 0;
	keyEvent.isMeta = 0;
	keyEvent.widget = widget;
	keyEvent.nativeMsg = nsnull;
	keyEvent.point.x = 0;
	keyEvent.point.y = 0;
	keyEvent.refPoint.x = 0;
	keyEvent.refPoint.y = 0;
	keyEvent.flags = 0;

	nsEventStatus status;
	result = viewManager->DispatchEvent( &keyEvent, &status );
	if ( NS_FAILED( result ) )
		return PR_FALSE;

	return PR_TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// all this just to render a caret!
PRBool LLEmbeddedBrowserWindow::renderCaret()
{
	nsCOMPtr< nsIWebBrowserFocus > focus = do_QueryInterface( mWebBrowser );

	nsCOMPtr< nsIDOMElement > focusedElement;
	focus->GetFocusedElement( getter_AddRefs( focusedElement ) );
	if ( ! focusedElement )
		return NS_ERROR_FAILURE;

	nsCOMPtr<nsIContent> focusedContent = do_QueryInterface( focusedElement );

	nsCOMPtr< nsIDOMWindow > domWindow;
	mWebBrowser->GetContentDOMWindow( getter_AddRefs( domWindow ) );
	if ( ! domWindow )
		return NS_ERROR_FAILURE;

	nsCOMPtr< nsIDOMDocument > domDocument;
	domWindow->GetDocument( getter_AddRefs( domDocument ) );
	if ( ! domDocument )
		return NS_ERROR_FAILURE;

	nsCOMPtr< nsIDocument> document = do_QueryInterface( domDocument );
	if ( ! document )
		return NS_ERROR_FAILURE;

	nsIPresShell* presShell = document->GetShellAt( 0 );
	if ( ! presShell )
		return NS_ERROR_FAILURE;

	nsCOMPtr< nsICaret > caret;
	presShell->GetCaret( getter_AddRefs( caret ) );

	nsIFrame* frame = nsnull;
	presShell->GetPrimaryFrameFor( focusedContent, &frame );
	if ( ! frame )
		return NS_ERROR_FAILURE;

	nsCOMPtr<nsISelectionController> selCtrl;
	frame->GetSelectionController( presShell->GetPresContext(), getter_AddRefs( selCtrl ) );

	nsCOMPtr<nsISelection> selection;
	selCtrl->GetSelection( nsISelectionController::SELECTION_NORMAL, getter_AddRefs( selection ) );

	PRBool collapsed;
	nsRect coords;
	nsIView* caretView;
	caret->GetCaretCoordinates( nsICaret::eTopLevelWindowCoordinates, selection, &coords, &collapsed, &caretView );

	float twips2Pixls = presShell->GetPresContext()->TwipsToPixels();

	PRInt32 caretX = NSTwipsToIntPixels( coords.x, twips2Pixls );
	PRInt32 caretY = NSTwipsToIntPixels( coords.y, twips2Pixls );
	PRInt32 caretHeight = NSTwipsToIntPixels( coords.height, twips2Pixls );

	if ( caretX > -1 && caretX < mBrowserWidth && caretY > -1 && caretY < mBrowserHeight )
	{
		if ( mPageBuffer )
		{
			for( int y = 1; y < caretHeight - 1; ++y )
			{
				// sometimes the caret seems valid when it really isn't - cap it to size of screen
				if ( caretY + y + caretHeight < mBrowserHeight )
				{
					mPageBuffer[ ( caretY + y ) * getBrowserRowSpan() + ( caretX + 1 ) * mBrowserDepth + 0 ] = mCaretBlue;
					mPageBuffer[ ( caretY + y ) * getBrowserRowSpan() + ( caretX + 1 ) * mBrowserDepth + 1 ] = mCaretGreen;
					mPageBuffer[ ( caretY + y ) * getBrowserRowSpan() + ( caretX + 1 ) * mBrowserDepth + 2 ] = mCaretRed;
				};
			};
		};
	};

	return NS_OK;
}

////////////////////////////////////////////////////////////////////////////////
//
void LLEmbeddedBrowserWindow::setBackgroundColor( PRUint8 redIn, PRUint8 greenIn, PRUint8 blueIn )
{
	mBackgroundRed = redIn;
	mBackgroundGreen = greenIn;
	mBackgroundBlue = blueIn;
}

////////////////////////////////////////////////////////////////////////////////
// change the caret color (we have different backgrounds to edit fields - black caret on black background == bad)
void LLEmbeddedBrowserWindow::setCaretColor( const PRUint8 redIn, const PRUint8 greenIn, const PRUint8 blueIn )
{
	mCaretRed = redIn;
	mCaretGreen = greenIn;
	mCaretBlue = blueIn;
}

////////////////////////////////////////////////////////////////////////////////
// override nsIWebBrowserChrome::HandleEvent ()
NS_IMETHODIMP LLEmbeddedBrowserWindow::HandleEvent( nsIDOMEvent* anEvent )
{
	nsCOMPtr< nsIDOMEventTarget > eventTarget;
	anEvent->GetTarget( getter_AddRefs( eventTarget ) );

	nsCOMPtr<nsIDOMElement> linkElement ( do_QueryInterface ( eventTarget ) );
	if ( linkElement )
	{
		nsString name;
		linkElement->GetAttribute( NS_ConvertUTF8toUTF16( "href" ), name );
		mClickHref = std::string( NS_ConvertUTF16toUTF8( name ).get() );

		LLEmbeddedBrowserWindowEvent event( getWindowId(), mClickHref );
		mEventEmitter.update( &LLEmbeddedBrowserWindowObserver::onClickLinkHref, event );
	};

	return NS_OK;
};

////////////////////////////////////////////////////////////////////////////////
// override nsIURIContentListener methods
NS_IMETHODIMP LLEmbeddedBrowserWindow::OnStartURIOpen( nsIURI *aURI, PRBool *_retval )
{
	nsCAutoString newURI;
	aURI->GetSpec( newURI );
	std::string rawUri = newURI.get();

	if ( rawUri.substr( 0, std::string( "secondlife://" ).length() ) == "secondlife://" )
	{
		LLEmbeddedBrowserWindowEvent event( getWindowId(), rawUri );
		mEventEmitter.update( &LLEmbeddedBrowserWindowObserver::onClickLinkSecondLife, event );

		// tell browser we're handling things and don't follow link
		*_retval = PR_TRUE;
	}
	else
	{
		// tell browser to proceed as normal
		*_retval = PR_FALSE;
	};

	return NS_OK;
}

////////////////////////////////////////////////////////////////////////////////
//
NS_IMETHODIMP LLEmbeddedBrowserWindow::DoContent( const char *aContentType,
													PRBool aIsContentPreferred,
														nsIRequest *aRequest,
															nsIStreamListener **aContentHandler,
																PRBool *_retval )
{
	return NS_ERROR_NOT_IMPLEMENTED;
}

////////////////////////////////////////////////////////////////////////////////
//
NS_IMETHODIMP LLEmbeddedBrowserWindow::IsPreferred( const char *aContentType,
														char **aDesiredContentType,
															PRBool *_retval )
{
	// important (otherwise, links try to open in a new window and trigger the window watcher code)
	*_retval = PR_TRUE;
	return NS_OK;
}

////////////////////////////////////////////////////////////////////////////////
//
NS_IMETHODIMP LLEmbeddedBrowserWindow::CanHandleContent( const char *aContentType,
															PRBool aIsContentPreferred,
																char **aDesiredContentType,
																	PRBool *_retval )
{
	return NS_ERROR_NOT_IMPLEMENTED;
}

////////////////////////////////////////////////////////////////////////////////
//
NS_IMETHODIMP LLEmbeddedBrowserWindow::GetLoadCookie( nsISupports * *aLoadCookie )
{
	return NS_ERROR_NOT_IMPLEMENTED;
}

////////////////////////////////////////////////////////////////////////////////
//
NS_IMETHODIMP LLEmbeddedBrowserWindow::SetLoadCookie( nsISupports * aLoadCookie )
{
	return NS_ERROR_NOT_IMPLEMENTED;
}

////////////////////////////////////////////////////////////////////////////////
//
NS_IMETHODIMP LLEmbeddedBrowserWindow::GetParentContentListener( nsIURIContentListener** aParentContentListener )
{
	return NS_ERROR_NOT_IMPLEMENTED;
}

////////////////////////////////////////////////////////////////////////////////
//
NS_IMETHODIMP LLEmbeddedBrowserWindow::SetParentContentListener( nsIURIContentListener* aParentContentListener )
{
	return NS_ERROR_NOT_IMPLEMENTED;
}

////////////////////////////////////////////////////////////////////////////////
// give focus to the browser so that input keyboard events work
void LLEmbeddedBrowserWindow::focusBrowser( PRBool focusBrowserIn )
{
	if ( focusBrowserIn )
	{
		nsCOMPtr< nsIWebBrowserFocus > focus( do_GetInterface( mWebBrowser ) );
		focus->Activate();
	}
	else
	{
		nsCOMPtr< nsIWebBrowserFocus > focus( do_GetInterface( mWebBrowser ) );
		focus->Deactivate();
	};
}

////////////////////////////////////////////////////////////////////////////////
//
void LLEmbeddedBrowserWindow::setWindowId( int windowIdIn )
{
	mWindowId = windowIdIn;
}

////////////////////////////////////////////////////////////////////////////////
//
int LLEmbeddedBrowserWindow::getWindowId()
{
	return mWindowId;
}

////////////////////////////////////////////////////////////////////////////////
//
bool LLEmbeddedBrowserWindow::set404RedirectUrl( std::string redirect_url )
{
	m404RedirectUrl = redirect_url;

	return true;
}

////////////////////////////////////////////////////////////////////////////////
//
bool LLEmbeddedBrowserWindow::clr404RedirectUrl()
{
	m404RedirectUrl = std::string( "" );

	return true;
}

// #define required by this file for LibXUL/Mozilla code to avoid crashes in their debug code
#ifdef _DEBUG
	#ifdef WIN32
		#undef DEBUG
	#endif
#endif
