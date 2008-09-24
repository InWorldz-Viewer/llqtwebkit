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

#include "llembeddedbrowser.h"
#include "llembeddedbrowserwindow.h"

////////////////////////////////////////////////////////////////////////////////
//
LLEmbeddedBrowserWindow::LLEmbeddedBrowserWindow() :
    mParent( 0 )
    /*mWebBrowser( nsnull ),
    mBaseWindow( nsnull ),
    mWindowId( 0 ),
    mPercentComplete( 0 ),
    mBrowserWidth( 0 ),
    mBrowserHeight( 0 ),
    mBrowserDepth( 4 ),
    mPageBuffer( 0 ),
    mEnabled( true ),
    mCurrentUri( "" ),
    mStatusText( "" ),
    mClickHref( "" ),
    mClickTarget( "" ),
    mNoFollowScheme( "secondlife://" ),
    mBkgRed( 0xff ),
    mBkgGreen( 0xff ),
    mBkgBlue( 0xff ),
    mCaretRed( 0x0 ),
    mCaretGreen( 0x0 ),
    mCaretBlue( 0x0 ),
    m404RedirectUrl( "" ),
    mFlipBitmap( false )*/
{
}

////////////////////////////////////////////////////////////////////////////////
//
LLEmbeddedBrowserWindow::~LLEmbeddedBrowserWindow()
{
    /*if ( mWebNav )
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
        mPageBuffer = 0;
    };*/
}

////////////////////////////////////////////////////////////////////////////////
//
/*
nsresult LLEmbeddedBrowserWindow::createBrowser( void* nativeWindowHandleIn, qint32 widthIn, qint32 heightIn, nsIWebBrowser **aBrowser )
{
    NS_ENSURE_ARG_POINTER(aBrowser);
    *aBrowser = nsnull;

    nsresult rv;
    mWebBrowser = do_CreateInstance( NS_WEBBROWSER_CONTRACTID, &rv );
    if ( ! mWebBrowser )
    {
        return NS_ERROR_FAILURE;
    }

    (void)mWebBrowser->SetContainerWindow( NS_STATIC_CAST( nsIWebBrowserChrome*, this ) );

    nsCOMPtr<nsIDocShellTreeItem> dsti = do_QueryInterface( mWebBrowser );
    dsti->SetItemType( nsIDocShellTreeItem::typeContentWrapper );

    mBaseWindow = do_QueryInterface( mWebBrowser );

    mBaseWindow->InitWindow( nativeWindowHandleIn, nsnull,  0, 0, mBrowserWidth, mBrowserHeight );
    mBaseWindow->Create();

    nsCOMPtr< nsIWebProgressListener > listener( NS_STATIC_CAST( nsIWebProgressListener*, this ) );
    nsCOMPtr< nsIWeakReference > thisListener( do_GetWeakReference( listener ) );
    mWebBrowser->AddWebBrowserListener( thisListener, NS_GET_IID( nsIWebProgressListener ) );

#if LL_DARWIN
    // Without this, the mac doesn't get upates for animating gifs, mouseovers, etc.
    mBaseWindow->SetVisibility( true );
#else
    mBaseWindow->SetVisibility( false );
#endif

    nsresult result;
    mWebNav = do_QueryInterface( mWebBrowser, &result );
    if ( NS_FAILED( result ) || ! mWebNav )
    {
        return NS_ERROR_FAILURE;
    };

    setSize( widthIn, heightIn );

    if ( mWebBrowser )
    {
        *aBrowser = mWebBrowser;
        NS_ADDREF( *aBrowser );

        return NS_OK;
    };

    return NS_ERROR_FAILURE;
}
*/

////////////////////////////////////////////////////////////////////////////////
// called when the page loading progress changes - emits event to consumer
/*
NS_IMETHODIMP LLEmbeddedBrowserWindow::OnProgressChange( nsIWebProgress* progress, nsIRequest* request,
                                                    qint32 curSelfProgress, qint32 maxSelfProgress,
                                                        qint32 curTotalProgress, qint32 maxTotalProgress )
{
    mPercentComplete = static_cast< qint16 >
        ( static_cast< float >( curTotalProgress * 100.0f ) / static_cast< float >( maxTotalProgress ) );

    if ( mPercentComplete < 0 )
        mPercentComplete = 0;

    if ( mPercentComplete > 100 )
        mPercentComplete = 100;

    LLEmbeddedBrowserWindowEvent event( getWindowId(), getCurrentUri(), mPercentComplete );
    mEventEmitter.update( &LLEmbeddedBrowserWindowObserver::onUpdateProgress, event );

    return NS_OK;
}


////////////////////////////////////////////////////////////////////////////////
// called when the browser state changes - as described below - emits event to consumer
NS_IMETHODIMP LLEmbeddedBrowserWindow::OnStateChange( nsIWebProgress* progress, nsIRequest* request,
                                                    quint32 progressStateFlags, nsresult status )
{
     if ( ( progressStateFlags & STATE_START ) && ( progressStateFlags & STATE_IS_DOCUMENT ) && ( status == NS_OK ) )
    {
        // TODO: move this to a better place.
        enableToolkitObserver( false );
        enableToolkitObserver( true );

        // page load is starting so remove listener that catches "click" events
        nsCOMPtr< nsIDOMWindow > window;
        nsresult result = progress->GetDOMWindow( getter_AddRefs( window ) );
        if ( result == NS_OK )
        {
            nsCOMPtr< nsIDOMEventTarget > target = do_QueryInterface( window );
            if ( target )
                target->RemoveEventListener(NS_ConvertUTF8toUTF16( "click" ), this, true );
        };

        // set the listener to we can catch nsURIContentListener events
        if ( mWebBrowser )
        {
            mWebBrowser->SetParentURIContentListener( NS_STATIC_CAST( nsIURIContentListener*, this ) );
        };

        // emit event that navigation is beginning
        mStatusText = std::string( "Browser loaded" );
        LLEmbeddedBrowserWindowEvent event( getWindowId(), getCurrentUri(), mStatusText );
        mEventEmitter.update( &LLEmbeddedBrowserWindowObserver::onNavigateBegin, event );

        // about to move to a different page so have to stop grabbing a page
        // but done one final grab in case the app doesn't ever call grabWindow again
        grabWindow( 0, 0, mBrowserWidth, mBrowserHeight );
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
                target->AddEventListener(NS_ConvertUTF8toUTF16( "click" ), this, true );
        };

        // pick up raw HTML response status code
        quint32 responseStatus = 0;
        if ( request )
        {
            nsCOMPtr< nsIHttpChannel > httpChannel = do_QueryInterface( request );
            if ( httpChannel )
            {
                httpChannel->GetResponseStatus( &responseStatus );
            };
        };

        // emit event that navigation is finished
        mStatusText = std::string( "Done" );
        LLEmbeddedBrowserWindowEvent event( getWindowId(), getCurrentUri(), mStatusText, (int)responseStatus );
        mEventEmitter.update( &LLEmbeddedBrowserWindowObserver::onNavigateComplete, event );

        // also set the flag here since back/forward navigation doesn't call progress change
        grabWindow( 0, 0, mBrowserWidth, mBrowserHeight );
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

    LLEmbeddedBrowserWindowEvent event( getWindowId(), getCurrentUri(), mStatusText );
    mEventEmitter.update( &LLEmbeddedBrowserWindowObserver::onStatusTextChange, event );

    return NS_OK;
}

////////////////////////////////////////////////////////////////////////////////
// call when the location changes - e.g. when a site redirects - emits event to consumer
// TODO: ought to check that this change is on the top frame and
// indicate this to the consumer of this class
NS_IMETHODIMP LLEmbeddedBrowserWindow::OnLocationChange( nsIWebProgress* webProgress,
                                                        nsIRequest* request,
                                                            nsIURI* location )
{
    if ( request )
    {
        nsCOMPtr< nsIHttpChannel > http_channel = do_QueryInterface( request );
        if ( http_channel )
        {
            quint32 response_status = 0;
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

    LLEmbeddedBrowserWindowEvent event( getWindowId(), getCurrentUri() );
    mEventEmitter.update( &LLEmbeddedBrowserWindowObserver::onLocationChange, event );

    return NS_OK;
}
*/

////////////////////////////////////////////////////////////////////////////////
// change the background color that gets used between pages (usually white)
void LLEmbeddedBrowserWindow::setBackgroundColor( const quint8 redIn, const quint8 greenIn, const quint8 blueIn )
{
    /*
    mBkgRed = redIn;
    mBkgGreen = greenIn;
    mBkgBlue = blueIn;
    */
}

////////////////////////////////////////////////////////////////////////////////
// change the caret color (we have different backgrounds to edit fields - black caret on black background == bad)
void LLEmbeddedBrowserWindow::setCaretColor( const quint8 redIn, const quint8 greenIn, const quint8 blueIn )
{
    /*
    mCaretRed = redIn;
    mCaretGreen = greenIn;
    mCaretBlue = blueIn;
    */
}

////////////////////////////////////////////////////////////////////////////////
//
void LLEmbeddedBrowserWindow::setEnabled( bool enabledIn )
{
    //mEnabled = enabledIn;
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
    //return mCurrentUri;
}

////////////////////////////////////////////////////////////////////////////////
// utility method that is used by observers to retrieve data after an event
const qint16 LLEmbeddedBrowserWindow::getPercentComplete()
{
    //return mPercentComplete;
}

////////////////////////////////////////////////////////////////////////////////
// utility method that is used by observers to retrieve data after an event
const std::string& LLEmbeddedBrowserWindow::getStatusMsg()
{
    //return mStatusText;
}

////////////////////////////////////////////////////////////////////////////////
// utility method that is used by observers to retrieve data after an event
const std::string& LLEmbeddedBrowserWindow::getClickLinkHref()
{
    //return mClickHref;
}

////////////////////////////////////////////////////////////////////////////////
// utility method that is used by observers to retrieve data after an event
const std::string& LLEmbeddedBrowserWindow::getClickLinkTarget()
{
    //return mClickTarget;
}
/*
////////////////////////////////////////////////////////////////////////////////
// called when the status text is changed - emits event to consumer
NS_IMETHODIMP LLEmbeddedBrowserWindow::OnStatusChange( nsIWebProgress* aWebProgress,
                                                        nsIRequest* aRequest,
                                                            nsresult aStatus,
                                                                const PRUnichar* aMessage )
{
    mStatusText = std::string( NS_ConvertUTF16toUTF8( aMessage ).get() );

    LLEmbeddedBrowserWindowEvent event( getWindowId(), getCurrentUri(), mStatusText );
    mEventEmitter.update( &LLEmbeddedBrowserWindowObserver::onStatusTextChange, event );

    return NS_OK;
}

////////////////////////////////////////////////////////////////////////////////
// implement this if you want to do something when the security state changtes
NS_IMETHODIMP LLEmbeddedBrowserWindow::OnSecurityChange( nsIWebProgress* aWebProgress,
                                                    nsIRequest* aRequest,
                                                        quint32 state )
{
    return NS_OK;
}
*/
////////////////////////////////////////////////////////////////////////////////
// render a page into memory and grab the window
// TODO: 0,0, browser width, browser height is always passed in right now
//       need to make this work with arbitrary rects (i.e. the dirty rect)
unsigned char* LLEmbeddedBrowserWindow::grabWindow( int xIn, int yIn, int widthIn, int heightIn )
{
    /*
    // sanity check
    if ( ! mWebBrowser )
        return 0;

    // only grab the window if it's enabled
    if ( ! mEnabled )
        return false;

    // get the docshell
    nsCOMPtr< nsIDocShell > docShell = do_GetInterface( mWebBrowser );
    if ( ! docShell )
        return false;


    // get pres context
    nsCOMPtr< nsPresContext > presContext;
    nsresult result = docShell->GetPresContext( getter_AddRefs( presContext ) );
    if ( NS_FAILED( result ) || ( ! presContext ) )
        return false;

    // get view manager
    nsIViewManager* viewManager = presContext->GetViewManager();
    if ( ! viewManager )
        return false;

    // get the view
    nsIScrollableView* scrollableView = NULL;
    viewManager->GetRootScrollableView( &scrollableView );
    nsIView* view = NULL;
    if ( scrollableView )
        scrollableView->GetScrolledView( view );
    else
        viewManager->GetRootView( view );

    // get the rectangle we want to render in twips (this looks odd but takees care of scrolling too)
    nsRect rect = view->GetBounds() - view->GetPosition() - view->GetPosition();
    if ( rect.IsEmpty() )
        return 0;

    float p2t = presContext->PixelsToTwips();
    rect.width = NSIntPixelsToTwips( widthIn, p2t );
    rect.height = NSIntPixelsToTwips( heightIn, p2t );

    // render the page
    nsCOMPtr< nsIRenderingContext > context;
    result = viewManager->RenderOffscreen( view, rect, false, false, NS_RGB( mBkgRed, mBkgGreen, mBkgBlue  ), getter_AddRefs( context ) );
    if ( NS_FAILED( result ) )
        return 0;

    // retrieve the surface we rendered to
    nsIDrawingSurface* surface = nsnull;
    context->GetDrawingSurface( &surface );
    if ( ! surface )
        return 0;

    // lock the surface and retrieve a pointer to the rendered data and current row span
    quint8* data;
    qint32 rowLen;
    // sometime rowspan ! width in pixels * bytes per pixel so save row span value and use in application
    result = surface->Lock( xIn, yIn, widthIn, heightIn, reinterpret_cast< void** >( &data ), &mBrowserRowSpan, &rowLen, NS_LOCK_SURFACE_READ_ONLY );
    if ( NS_FAILED ( result ) )
        return 0;

    // save row span - it *can* change during the life of the app
    mBrowserDepth = rowLen / mBrowserWidth;

    // create memory buffer here so it can be deleted and recreated elsewhere
    if ( ! mPageBuffer )
        mPageBuffer = new unsigned char[ mBrowserRowSpan * mBrowserHeight ];

    // save the pixels and optionally invert them 
    // (it's useful the SL client to get bitmaps that are inverted compared
    // to the way that Mozilla renders them - allow to optionally flip
    if ( mFlipBitmap )
    {
        for( int y = mBrowserHeight - 1; y > -1; --y )
        {
            memcpy( mPageBuffer + y * mBrowserRowSpan, 
                        data + ( mBrowserHeight - y - 1 ) * mBrowserRowSpan, 
                            mBrowserRowSpan );
        };
    }
    else
    {
        memcpy( mPageBuffer, data, mBrowserRowSpan * mBrowserHeight );
    };

    // release and destroy the surface we rendered to
    surface->Unlock();
    context->DestroyDrawingSurface( surface );

    renderCaret();

    return mPageBuffer;
    */
}

////////////////////////////////////////////////////////////////////////////////
// all this just to render a caret!
/*
bool LLEmbeddedBrowserWindow::renderCaret()
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

    bool collapsed;
    nsRect coords;
    nsIView* caretView;
    caret->GetCaretCoordinates( nsICaret::eTopLevelWindowCoordinates, selection, &coords, &collapsed, &caretView );

    float twips2Pixls = presShell->GetPresContext()->TwipsToPixels();

    qint32 caretX = NSTwipsToIntPixels( coords.x, twips2Pixls );
    qint32 caretY = NSTwipsToIntPixels( coords.y, twips2Pixls );
    qint32 caretHeight = NSTwipsToIntPixels( coords.height, twips2Pixls );

    if ( caretX > -1 && caretX < mBrowserWidth && caretY > -1 && caretY < mBrowserHeight )
    {
        if ( mPageBuffer )
        {
            for( int y = 1; y < caretHeight - 1; ++y )
            {
                qint32 base_pos = caretY + y;
                if ( mFlipBitmap )
                    base_pos = mBrowserHeight - ( caretY + y );

                // sometimes the caret seems valid when it really isn't - cap it to size of screen
                if ( caretY + y + caretHeight < mBrowserHeight )
                {
                    mPageBuffer[ base_pos * getBrowserRowSpan() + ( caretX + 1 ) * mBrowserDepth + 0 ] = mCaretBlue;
                    mPageBuffer[ base_pos * getBrowserRowSpan() + ( caretX + 1 ) * mBrowserDepth + 1 ] = mCaretGreen;
                    mPageBuffer[ base_pos * getBrowserRowSpan() + ( caretX + 1 ) * mBrowserDepth + 2 ] = mCaretRed;
                };
            };
        };
    };

    return NS_OK;
}*/

////////////////////////////////////////////////////////////////////////////////
// return the buffer that contains the rendered page
unsigned char* LLEmbeddedBrowserWindow::getPageBuffer()
{
    //return mPageBuffer;
}

////////////////////////////////////////////////////////////////////////////////
//
qint16 LLEmbeddedBrowserWindow::getBrowserWidth()
{
    //return mBrowserWidth;
}

////////////////////////////////////////////////////////////////////////////////
//
qint16 LLEmbeddedBrowserWindow::getBrowserHeight()
{
    //return mBrowserHeight;
}

////////////////////////////////////////////////////////////////////////////////
//
qint16 LLEmbeddedBrowserWindow::getBrowserDepth()
{
    //return mBrowserDepth;
}

////////////////////////////////////////////////////////////////////////////////
//
qint32 LLEmbeddedBrowserWindow::getBrowserRowSpan()
{
    //return mBrowserRowSpan;
}

////////////////////////////////////////////////////////////////////////////////
//
bool LLEmbeddedBrowserWindow::navigateTo( const std::string uriIn )
{
    /*
    if ( mWebNav )
    {
        mWebNav->LoadURI( reinterpret_cast< const PRUnichar* >( NS_ConvertUTF8toUTF16( uriIn.c_str() ).get() ),
            nsIWebNavigation::LOAD_FLAGS_NONE,
                nsnull, nsnull, nsnull );

        return true;
    };
    */
    return false;
};

////////////////////////////////////////////////////////////////////////////////
//
bool LLEmbeddedBrowserWindow::canNavigateBack()
{
 /*
    if ( ! mWebNav )
    {
        return false;
    };

    bool canGoBack = false;

    nsresult result = mWebNav->GetCanGoBack( &canGoBack );
    if ( NS_FAILED( result ) )
    {
        return false;
    };

    return canGoBack;
    */
};

////////////////////////////////////////////////////////////////////////////////
//
void LLEmbeddedBrowserWindow::navigateStop()
{
    /*
    if ( mWebNav )
        mWebNav->Stop( nsIWebNavigation::STOP_ALL );
    */
};

////////////////////////////////////////////////////////////////////////////////
//
void LLEmbeddedBrowserWindow::navigateBack()
{
    /*
    if ( mWebNav )
        mWebNav->GoBack();
    */
};

////////////////////////////////////////////////////////////////////////////////
//
bool LLEmbeddedBrowserWindow::canNavigateForward()
{
    /*
    if ( ! mWebNav )
        return false;

    bool canGoForward = false;

    nsresult result = mWebNav->GetCanGoForward( &canGoForward );
    if ( NS_FAILED( result ) )
    {
        return false;
    };

    return canGoForward;
    */
};

////////////////////////////////////////////////////////////////////////////////
//
void LLEmbeddedBrowserWindow::navigateForward()
{
    /*
    if ( mWebNav )
        mWebNav->GoForward();
        */
};

////////////////////////////////////////////////////////////////////////////////
//
void LLEmbeddedBrowserWindow::navigateReload()
{
    /*
    // maybe need a cache version of this too?
    if ( mWebNav )
        mWebNav->Reload( nsIWebNavigation::LOAD_FLAGS_BYPASS_CACHE );
        */
};

////////////////////////////////////////////////////////////////////////////////
// set the size of the browser window
bool LLEmbeddedBrowserWindow::setSize( qint16 widthIn, qint16 heightIn )
{
    /*
    if ( mBaseWindow )
    {
        // if there is a buffer already, get rid of it (it will get created as required in grabWindow())
        if ( mPageBuffer )
        {
            delete[] mPageBuffer;
            mPageBuffer = 0;
        };

        // record new size (important: may change after grabWindow() is called);
        mBrowserWidth = widthIn;
        mBrowserHeight = heightIn;
        mBrowserRowSpan = mBrowserWidth * mBrowserDepth;

        // On the Mac, these calls do strange things to the main viewer window, and they don't seem necessary in any case.
        #ifdef WIN32
        // this is the actual OS (on Win32) Window so it needs to be hidden
        mBaseWindow->SetVisibility( false );

        // move WAY off screen (and in a place that makes the combobox hack work)
        mBaseWindow->SetPosition( 8000, -6000 );
        #endif

        // tell Mozilla about the new size
        mBaseWindow->SetSize( widthIn, heightIn, false );

        return true;
    };

    return false;
    */
}

////////////////////////////////////////////////////////////////////////////////
//
bool LLEmbeddedBrowserWindow::flipWindow( bool flip )
{
    mFlipBitmap = flip;

    return true;
}

////////////////////////////////////////////////////////////////////////////////
// higher level mouse event
void LLEmbeddedBrowserWindow::mouseLeftDoubleClick( qint16 xPosIn, qint16 yPosIn )
{
    // Internally Mozilla represents double-click as a 2-count mouse down event.
    // TODO: support triple-click
    //const quint32 clickCount = 2;
    //sendMozillaMouseEvent( NS_MOUSE_LEFT_BUTTON_DOWN, xPosIn, yPosIn, clickCount );
}

////////////////////////////////////////////////////////////////////////////////
// higher level mouse event
void LLEmbeddedBrowserWindow::mouseDown( qint16 xPosIn, qint16 yPosIn )
{
    //const quint32 clickCount = 1;
    //sendMozillaMouseEvent( NS_MOUSE_LEFT_BUTTON_DOWN, xPosIn, yPosIn, clickCount );
}

////////////////////////////////////////////////////////////////////////////////
// higher level mouse event
void LLEmbeddedBrowserWindow::mouseUp( qint16 xPosIn, qint16 yPosIn )
{
    //const quint32 clickCount = 1;
    //sendMozillaMouseEvent( NS_MOUSE_LEFT_BUTTON_UP, xPosIn, yPosIn, clickCount );
}

////////////////////////////////////////////////////////////////////////////////
// higher level mouse event
void LLEmbeddedBrowserWindow::mouseMove( qint16 xPosIn, qint16 yPosIn )
{
    //const quint32 clickCount = 1;    // ignored?
    //sendMozillaMouseEvent( NS_MOUSE_MOVE, xPosIn, yPosIn, clickCount );
}

////////////////////////////////////////////////////////////////////////////////
// utility methods to set an error message so something else can look at it
void LLEmbeddedBrowserWindow::scrollByLines( qint16 linesIn )
{
    /*
    if ( mWebBrowser )
    {
        nsCOMPtr< nsIDOMWindow > window;
        nsresult result = mWebBrowser->GetContentDOMWindow( getter_AddRefs( window ) );

        if ( ! NS_FAILED( result ) && window )
        {
            result = window->ScrollByLines( linesIn );
        };
    };
    */
}
/*
////////////////////////////////////////////////////////////////////////////////
// synthesizes a mouse event and sends into the embedded instance
// eventIn - NS_MOUSE_LEFT_BUTTON_DOWN, NS_MOUSE_LEFT_BUTTON_UP, etc.
// xPosIn, yPosIn - coordinates (in browser window space)
// clickCountIn - use 1 for single click, 2 for double-click, etc.
bool LLEmbeddedBrowserWindow::sendMozillaMouseEvent( qint16 eventIn, qint16 xPosIn, qint16 yPosIn, quint32 clickCountIn )
{
    if ( ! mEnabled )
        return false;

    if ( ! mWebBrowser )
        return false;

    nsCOMPtr< nsIDocShell > docShell = do_GetInterface( mWebBrowser );
    if ( ! docShell )
        return false;

    nsCOMPtr< nsPresContext > presContext;
    nsresult result = docShell->GetPresContext( getter_AddRefs( presContext ) );
    if ( NS_FAILED( result ) || ( ! presContext ) )
        return false;

    nsIViewManager* viewManager = presContext->GetViewManager();
    if ( ! viewManager )
        return false;

    nsIView* rootView;
    result = viewManager->GetRootView( rootView );
    if ( NS_FAILED( result ) || ( ! rootView ) )
        return false;

    nsCOMPtr< nsIWidget > widget = rootView->GetWidget();
    if ( ! widget )
        return false;

    nsMouseEvent mouseEvent( true, eventIn, widget, nsMouseEvent::eReal );
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
        return false;

    return true;
};
*/
////////////////////////////////////////////////////////////////////////////////
// higher level keyboard functions

// accept a (mozilla-style) keycode
void LLEmbeddedBrowserWindow::keyPress( qint16 keyCode )
{
    //sendMozillaKeyboardEvent( 0, keyCode );
}

// accept keyboard input that's already been translated into a unicode char.
void LLEmbeddedBrowserWindow::unicodeInput( quint32 uni_char )
{
    //sendMozillaKeyboardEvent( uni_char, 0 );
}
/*
////////////////////////////////////////////////////////////////////////////////
// synthesizes a keyboard event and sends into the embedded instance
bool LLEmbeddedBrowserWindow::sendMozillaKeyboardEvent( quint32 uni_char, quint32 ns_vk_code )
{
    if ( ! mEnabled )
        return false;

    if ( ! mWebBrowser )
        return false;

    nsCOMPtr< nsIDocShell > docShell = do_GetInterface( mWebBrowser );
    if ( ! docShell )
        return false;

    nsCOMPtr< nsPresContext > presContext;
    docShell->GetPresContext( getter_AddRefs( presContext ) );
    if ( ! presContext )
        return false;

    nsIViewManager* viewManager = presContext->GetViewManager();
    if ( ! viewManager )
        return false;

    nsIView* rootView;
    viewManager->GetRootView( rootView );
    if ( ! rootView )
        return false;

    nsCOMPtr< nsIWidget > widget = rootView->GetWidget();
    if ( ! widget )
        return false;

    nsKeyEvent keyEvent( true, NS_KEY_PRESS, widget );
    keyEvent.keyCode = ns_vk_code;
    keyEvent.charCode = uni_char;
    keyEvent.isChar = uni_char ? true : false;
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
    nsresult result = viewManager->DispatchEvent( &keyEvent, &status );
    if ( NS_FAILED( result ) )
        return false;

    return true;
}
*/
////////////////////////////////////////////////////////////////////////////////
// override nsIWebBrowserChrome::HandleEvent ()
    /*
NS_IMETHODIMP LLEmbeddedBrowserWindow::HandleEvent( nsIDOMEvent* anEvent )
{
    nsCOMPtr< nsIDOMEventTarget > eventTarget;
    anEvent->GetTarget( getter_AddRefs( eventTarget ) );

    nsCOMPtr<nsIDOMElement> linkElement ( do_QueryInterface ( eventTarget ) );
    if ( linkElement )
    {
        // look for an href link
        nsString name;
        linkElement->GetAttribute( NS_ConvertUTF8toUTF16( "href" ), name );
        mClickHref = std::string( NS_ConvertUTF16toUTF8( name ).get() );

        // look for a target element
        linkElement->GetAttribute( NS_ConvertUTF8toUTF16( "target" ), name );
        mClickTarget = std::string( NS_ConvertUTF16toUTF8( name ).get() );

        // if the href link contains something
        if ( mClickHref.length() )
        {
            LLEmbeddedBrowserWindowEvent event( getWindowId(), getCurrentUri(), mClickHref, mClickTarget );
            mEventEmitter.update( &LLEmbeddedBrowserWindowObserver::onClickLinkHref, event );
        };
    };

    return NS_OK;
};
    */

////////////////////////////////////////////////////////////////////////////////
// give focus to the browser so that input keyboard events work
void LLEmbeddedBrowserWindow::focusBrowser( bool focusBrowserIn )
{
    /*
    if ( mWebBrowser )
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
    };*/
}

////////////////////////////////////////////////////////////////////////////////
//
void LLEmbeddedBrowserWindow::setWindowId( int windowIdIn )
{
    //mWindowId = windowIdIn;
}

////////////////////////////////////////////////////////////////////////////////
//
int LLEmbeddedBrowserWindow::getWindowId()
{
    //printf("## Getting id for %p and it is %d\n", this, mWindowId );

    //return mWindowId;
}

////////////////////////////////////////////////////////////////////////////////
// add / remove the toolkit (and therefore 'page changed') observer
    /*
bool LLEmbeddedBrowserWindow::enableToolkitObserver( bool enableIn )
{
    //TODO: AddObserver fails if I grab the toolkit this way.
    //static NS_DEFINE_CID(kNS_TOOLKIT_CID, NS_TOOLKIT_CID);
    //nsresult result1;
    //nsCOMPtr< nsIToolkit > toolkit = do_GetService( kNS_TOOLKIT_CID, &result1 );
    //if ( ( result1 == NS_OK ) && toolkit )
    //{
    //    if ( toolkit->AddObserver( this ) )
    //    {
    //        return true;
    //    };
    //};
    //return false;

    // TODO: this is horrible but seems to work - need a better way to get the toolkit
    nsCOMPtr< nsIDocShell > docShell = do_GetInterface( mWebBrowser );
    if ( ! docShell )
        return false;

    nsCOMPtr< nsPresContext > presContext;
    nsresult result = docShell->GetPresContext( getter_AddRefs( presContext ) );
    if ( NS_FAILED( result ) || ( ! presContext ) )
        return false;

    nsIViewManager* viewManager = presContext->GetViewManager();
    if ( ! viewManager )
        return false;

    nsIView* rootView;
    result = viewManager->GetRootView( rootView );
    if ( NS_FAILED( result ) || ( ! rootView ) )
        return false;

    nsCOMPtr< nsIWidget > widget = rootView->GetWidget();
    if ( ! widget )
        return false;

    nsCOMPtr< nsIToolkit > mToolkit = widget->GetToolkit();
    if ( ! mToolkit )
        return false;

    if ( enableIn )
        mToolkit->AddObserver( this );
    else
        mToolkit->RemoveObserver( this );

    return true;
}
    */

////////////////////////////////////////////////////////////////////////////////
// something on the page changed - e.g. a new page loaded, scrolling, user
// input or as the result of some kind of animation.
// NOTE: we don't call grabWindow here as this can stall graphics apps -
//       we merely pass back an event to the app and let it decide when/if
//       to call grabWindow()
    /*
NS_METHOD LLEmbeddedBrowserWindow::NotifyInvalidated( nsIWidget *aWidget, qint32 x, qint32 y, qint32 width, qint32 height )
{
//    printf("LLEmbeddedBrowserWindow::NotifyInvalidated(%p, %d, %d, %d, %d)\n", (void*)aWidget, (int)x, (int)y, (int)width, (int)height);

    // try to match widget-window against ourselves to see if we need to update the texture
    // only works using native widgets (on Windows) at the moment - needs to be moved to platform agnostic code ASAP
    #ifdef WIN32

    // this is horrible beyond words but it seems to work...
    // nsToolkit tells us that a widget changed and we need to see if it's this instance
    // so we can emit an event that causes the parent app to update the browser texture
    nsIWidget* mainWidget;
    mBaseWindow->GetMainWidget( &mainWidget );

    HWND nativeWidget = (HWND)aWidget->GetNativeData( NS_NATIVE_WIDGET );
    HWND nativeWidgetChild = 0;
    while ( ::GetParent( nativeWidget ) )
    {
        nativeWidgetChild = nativeWidget;
        nativeWidget = ::GetParent( nativeWidget );
    };

    if ( ( (HWND)mainWidget->GetNativeData( NS_NATIVE_WIDGET ) ) == nativeWidgetChild )
    {
        LLEmbeddedBrowserWindowEvent event( getWindowId(), getCurrentUri(), x, y, width, height );
        mEventEmitter.update( &LLEmbeddedBrowserWindowObserver::onPageChanged, event );
    };

    // other platforms will always update - desperately inefficient but you'll see something.
    #else
        LLEmbeddedBrowserWindowEvent event( getWindowId(), getCurrentUri(), x, y, width, height );
        mEventEmitter.update( &LLEmbeddedBrowserWindowObserver::onPageChanged, event );
    #endif

    return NS_OK;
}
    */

////////////////////////////////////////////////////////////////////////////////
//
std::string LLEmbeddedBrowserWindow::evaluateJavascript( std::string scriptIn )
{
    /*
    nsCOMPtr< nsIScriptGlobalObjectOwner > theGlobalObjectOwner( do_GetInterface( mWebBrowser ) );

    if ( theGlobalObjectOwner )
    {
        nsIScriptGlobalObject* theGlobalObject;
        theGlobalObject = theGlobalObjectOwner->GetScriptGlobalObject();

        nsIScriptContext* theScriptContext = theGlobalObject->GetContext();

        bool IsUndefined;
        nsString output;
        nsresult result = theScriptContext->EvaluateString(NS_ConvertUTF8toUTF16(scriptIn.c_str()),
           nsnull, nsnull, "", 1, nsnull, &output, &IsUndefined);

        if( NS_FAILED( result ) )
            return "";

        return std::string( NS_ConvertUTF16toUTF8( output ).get() );
    }
    */
    return "";
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

void LLEmbeddedBrowserWindow::setNoFollowScheme( std::string schemeIn )
{
}

std::string LLEmbeddedBrowserWindow::getNoFollowScheme()
{
}

