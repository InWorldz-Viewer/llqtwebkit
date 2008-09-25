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

#include "llembeddedbrowserwindow.h"
#include "llembeddedbrowser_p.h"
#include "llembeddedbrowserwindow_p.h"

#include "llembeddedbrowser.h"
#include "webpage.h"

#include <QtGui/QtGui>
#include <QtWebKit/QtWebKit>
#include <QtOpenGL/QtOpenGL>

QString LLEmbeddedBrowserWindowPrivate::userAgent() const
{
    if (!mParent)
        return QString();
    return mParent->d->userAgentString;
}

LLEmbeddedBrowserWindow::LLEmbeddedBrowserWindow()
    :
    mWindowId( 0 ),
    mPercentComplete( 0 ),
    mEnabled( true ),
    mCurrentUri( "" ),
    mStatusText( "" ),
    mClickHref( "" ),
    mClickTarget( "" ),
    mNoFollowScheme( "secondlife://" ),
    m404RedirectUrl( "" ),
    mFlipBitmap( false )
{
    d = new LLEmbeddedBrowserWindowPrivate();
    d->page->window = this;
}

LLEmbeddedBrowserWindow::~LLEmbeddedBrowserWindow()
{
    delete d;
}

void LLEmbeddedBrowserWindow::setParent(LLEmbeddedBrowser* parentIn)
{
    d->mParent = parentIn;
    d->page->setNetworkAccessManager(&parentIn->d->networkAccessManager);
}

// called when the page loading progress changes - emits event to consumer
/*
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

// change the background color that gets used between pages (usually white)
void LLEmbeddedBrowserWindow::setBackgroundColor( const quint8 redIn, const quint8 greenIn, const quint8 blueIn )
{
    qDebug() << __FUNCTION__ << "Not implemented";
    /*
    mBkgRed = redIn;
    mBkgGreen = greenIn;
    mBkgBlue = blueIn;
    */
}

// change the caret color (we have different backgrounds to edit fields - black caret on black background == bad)
void LLEmbeddedBrowserWindow::setCaretColor( const quint8 redIn, const quint8 greenIn, const quint8 blueIn )
{
    qDebug() << __FUNCTION__ << "Not implemented";
    /*
    mCaretRed = redIn;
    mCaretGreen = greenIn;
    mCaretBlue = blueIn;
    */
}

//
void LLEmbeddedBrowserWindow::setEnabled( bool enabledIn )
{
    qDebug() << __FUNCTION__ << "Not implemented";
    //mEnabled = enabledIn;
}

// allow consumers of this class to observe events - add themselves as an observer
bool LLEmbeddedBrowserWindow::addObserver( LLEmbeddedBrowserWindowObserver* observerIn )
{
    return mEventEmitter.addObserver( observerIn );
}

// allow consumers of this class to observe events - remove themselves as an observer
bool LLEmbeddedBrowserWindow::remObserver( LLEmbeddedBrowserWindowObserver* observerIn )
{
    return mEventEmitter.remObserver( observerIn );
}

// used by observers of this class to get the current URI
const std::string LLEmbeddedBrowserWindow::getCurrentUri()
{
    qDebug() << __FUNCTION__ << "Not implemented" << d->page->mainFrame()->url();
    return d->page->mainFrame()->url().toString().toStdString();
}

// utility method that is used by observers to retrieve data after an event
const qint16 LLEmbeddedBrowserWindow::getPercentComplete()
{
    return mPercentComplete;
}

// utility method that is used by observers to retrieve data after an event
const std::string LLEmbeddedBrowserWindow::getStatusMsg()
{
    qDebug() << __FUNCTION__ << "Not implemented";
    //return mStatusText;
}

// utility method that is used by observers to retrieve data after an event
const std::string LLEmbeddedBrowserWindow::getClickLinkHref()
{
    qDebug() << __FUNCTION__ << "Not implemented";
    //return mClickHref;
}

// utility method that is used by observers to retrieve data after an event
const std::string LLEmbeddedBrowserWindow::getClickLinkTarget()
{
    qDebug() << __FUNCTION__ << "Not implemented";
    //return mClickTarget;
}
/*
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
*/
// render a page into memory and grab the window
// TODO: 0,0, browser width, browser height is always passed in right now
//       need to make this work with arbitrary rects (i.e. the dirty rect)
unsigned char* LLEmbeddedBrowserWindow::grabWindow( int xIn, int yIn, int widthIn, int heightIn )
{
    qDebug() << __FUNCTION__ << mFlipBitmap;
    d->image = QImage(d->page->viewportSize(), QImage::Format_RGB32);
    QPainter painter(&d->image);
    d->page->mainFrame()->render(&painter);
    painter.end();
    if ( !mFlipBitmap )
        d->image = d->image.mirrored();
    d->image = QGLWidget::convertToGLFormat(d->image);
    return d->image.bits();
}

// return the buffer that contains the rendered page
unsigned char* LLEmbeddedBrowserWindow::getPageBuffer()
{
    qDebug() << __FUNCTION__;
    d->image = QImage(d->page->viewportSize(), QImage::Format_RGB32);
    QPainter painter(&d->image);
    d->page->mainFrame()->render(&painter);
    painter.end();
    if ( !mFlipBitmap )
        d->image = d->image.mirrored();
    d->image = QGLWidget::convertToGLFormat(d->image);
    return d->image.bits();
}

qint16 LLEmbeddedBrowserWindow::getBrowserWidth()
{
    return d->image.width();
}

qint16 LLEmbeddedBrowserWindow::getBrowserHeight()
{
    return d->image.height();
}

qint16 LLEmbeddedBrowserWindow::getBrowserDepth()
{
    return 4;
}

qint32 LLEmbeddedBrowserWindow::getBrowserRowSpan()
{
    return 4 * getBrowserWidth();
}

bool LLEmbeddedBrowserWindow::navigateTo( const std::string uriIn )
{
    QUrl url = QUrl(QString::fromStdString(uriIn));
    qDebug() << __FUNCTION__ << url;
    d->page->mainFrame()->load(url);
    return true;
}

bool LLEmbeddedBrowserWindow::canNavigateBack()
{
    d->page->history()->canGoBack();
}

void LLEmbeddedBrowserWindow::navigateStop()
{
    d->page->triggerAction(QWebPage::Stop);
}

void LLEmbeddedBrowserWindow::navigateBack()
{
    d->page->triggerAction(QWebPage::Back);
}

bool LLEmbeddedBrowserWindow::canNavigateForward()
{
    d->page->history()->canGoForward();
}

void LLEmbeddedBrowserWindow::navigateForward()
{
    d->page->triggerAction(QWebPage::Forward);
}

void LLEmbeddedBrowserWindow::navigateReload()
{
    d->page->triggerAction(QWebPage::Reload);
}

// set the size of the browser window
bool LLEmbeddedBrowserWindow::setSize(qint16 widthIn, qint16 heightIn)
{
    d->page->setViewportSize(QSize(widthIn, heightIn));
    return true;
}

bool LLEmbeddedBrowserWindow::flipWindow(bool flip)
{
    qDebug() << __FUNCTION__ << "Not implemented" << flip;
    mFlipBitmap = flip;
    return true;
}

void LLEmbeddedBrowserWindow::mouseLeftDoubleClick( qint16 xPosIn, qint16 yPosIn )
{
    QMouseEvent event(QEvent::MouseButtonDblClick, QPoint(xPosIn, yPosIn), Qt::LeftButton, 0, 0);
    qApp->sendEvent(d->page, &event);
}

void LLEmbeddedBrowserWindow::mouseDown(qint16 xPosIn, qint16 yPosIn)
{
    QMouseEvent event(QEvent::MouseButtonPress, QPoint(xPosIn, yPosIn), Qt::LeftButton, 0, 0);
    qApp->sendEvent(d->page, &event);
}

void LLEmbeddedBrowserWindow::mouseUp(qint16 xPosIn, qint16 yPosIn)
{
    QMouseEvent event(QEvent::MouseButtonRelease, QPoint(xPosIn, yPosIn), Qt::LeftButton, 0, 0);
    qApp->sendEvent(d->page, &event);
}

void LLEmbeddedBrowserWindow::mouseMove( qint16 xPosIn, qint16 yPosIn )
{
    QMouseEvent event(QEvent::MouseMove, QPoint(xPosIn, yPosIn), Qt::NoButton, 0, 0);
    qApp->sendEvent(d->page, &event);
}

// utility methods to set an error message so something else can look at it
void LLEmbeddedBrowserWindow::scrollByLines( qint16 linesIn )
{
    int currentScrollValue = d->page->mainFrame()->scrollBarValue(Qt::Vertical);
    d->page->mainFrame()->setScrollBarValue(Qt::Vertical, currentScrollValue + linesIn);
}

// higher level keyboard functions

// accept a (mozilla-style) keycode
void LLEmbeddedBrowserWindow::keyPress( qint16 keyCode )
{
    {
        QKeyEvent event( QEvent::KeyPress, 0, Qt::NoModifier, QChar(keyCode));
        qDebug() << event.text() << QChar(keyCode);
        qApp->sendEvent(d->page, &event);
    }
    {
        QKeyEvent event( QEvent::KeyRelease, 0, Qt::NoModifier, QChar(keyCode));
        qApp->sendEvent(d->page, &event);
    }
}

// accept keyboard input that's already been translated into a unicode char.
void LLEmbeddedBrowserWindow::unicodeInput( quint32 uni_char )
{
    qDebug() << __FUNCTION__ << "Not implemented";
}
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

// give focus to the browser so that input keyboard events work
void LLEmbeddedBrowserWindow::focusBrowser( bool focusBrowserIn )
{
    QFocusEvent event(focusBrowserIn ? QEvent::FocusIn : QEvent::FocusOut, Qt::OtherFocusReason);
    qApp->sendEvent(d->page, &event);
}

void LLEmbeddedBrowserWindow::setWindowId( int windowIdIn )
{
    mWindowId = windowIdIn;
}

int LLEmbeddedBrowserWindow::getWindowId()
{
    return mWindowId;
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

std::string LLEmbeddedBrowserWindow::evaluateJavascript( std::string scriptIn )
{
    QString script = QString::fromStdString(scriptIn);
    QString result = d->page->mainFrame()->evaluateJavaScript(script).toString();
    return result.toStdString();
}

bool LLEmbeddedBrowserWindow::set404RedirectUrl( std::string redirect_url )
{
    m404RedirectUrl = redirect_url;
    return true;
}

bool LLEmbeddedBrowserWindow::clr404RedirectUrl()
{
    m404RedirectUrl = std::string();
    return true;
}

void LLEmbeddedBrowserWindow::setNoFollowScheme( std::string schemeIn )
{
}

std::string LLEmbeddedBrowserWindow::getNoFollowScheme()
{
    return std::string();
}

