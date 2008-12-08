/*
   Copyright (C) 2008 Torch Mobile Inc. http://www.torchmobile.com/
*/

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
 *  Benjamin Meyer (benjamin.meyer@torchmobile.com)
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

Q_IMPORT_PLUGIN(qgif)
Q_IMPORT_PLUGIN(qjpeg)
Q_IMPORT_PLUGIN(qsvg)
Q_IMPORT_PLUGIN(qtiff)
Q_IMPORT_PLUGIN(qico)

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
    mFlipBitmap( false ),
    mPageBuffer(NULL)
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
}

//
void LLEmbeddedBrowserWindow::setEnabled( bool enabledIn )
{
    // what exactly should this do?
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
    return mStatusText;
}

// utility method that is used by observers to retrieve data after an event
const std::string LLEmbeddedBrowserWindow::getClickLinkHref()
{
	std::string result;
    // This function doesn't seem to be used?
    qDebug() << __FUNCTION__ << "Not implemented";
    //return mClickHref;
    return result;
}

// utility method that is used by observers to retrieve data after an event
const std::string LLEmbeddedBrowserWindow::getClickLinkTarget()
{
	std::string result;
    // This function doesn't seem to be used?
    qDebug() << __FUNCTION__ << "Not implemented";
    //return mClickTarget;
    return result;
}

// render a page into memory and grab the window
// TODO: 0,0, browser width, browser height is always passed in right now
//       need to make this work with arbitrary rects (i.e. the dirty rect)
unsigned char* LLEmbeddedBrowserWindow::grabWindow( int xIn, int yIn, int widthIn, int heightIn )
{
    d->image = QImage(d->page->viewportSize(), QImage::Format_RGB32);
    QPainter painter(&d->image);
    QRegion clip(xIn, yIn, widthIn, heightIn);
    d->page->mainFrame()->render(&painter, clip);
    painter.end();
    if ( !mFlipBitmap )
        d->image = d->image.mirrored();
    d->image = QGLWidget::convertToGLFormat(d->image);
    mPageBuffer = d->image.bits();
    return mPageBuffer;
}

// return the buffer that contains the rendered page
unsigned char* LLEmbeddedBrowserWindow::getPageBuffer()
{
    return mPageBuffer;
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
    navigateStop();
    d->page->mainFrame()->load(url);
    return true;
}

bool LLEmbeddedBrowserWindow::canNavigateBack()
{
    return d->page->history()->canGoBack();
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
    return d->page->history()->canGoForward();
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
    mPageBuffer = NULL;
    return true;
}

bool LLEmbeddedBrowserWindow::flipWindow(bool flip)
{
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
    if (xPosIn == 0 && yPosIn == 0)
        return;
    QMouseEvent event(QEvent::MouseMove, QPoint(xPosIn, yPosIn), Qt::NoButton, 0, 0);
    qApp->sendEvent(d->page, &event);
}

// utility methods to set an error message so something else can look at it
void LLEmbeddedBrowserWindow::scrollByLines( qint16 linesIn )
{
    int currentScrollValue = d->page->mainFrame()->scrollBarValue(Qt::Vertical);
    d->page->mainFrame()->setScrollBarValue(Qt::Vertical, currentScrollValue + linesIn);
}

// accept a (mozilla-style) keycode
void LLEmbeddedBrowserWindow::keyPress( qint16 keyCode )
{
    Qt::Key key;
	QChar text;

    switch (keyCode)
	{
		case LL_DOM_VK_CANCEL:			key = Qt::Key_Cancel;		break;
		case LL_DOM_VK_HELP:			key = Qt::Key_Help;		break;
		case LL_DOM_VK_BACK_SPACE:		key = Qt::Key_Backspace;	break;
		case LL_DOM_VK_TAB:			key = Qt::Key_Tab;		break;
		case LL_DOM_VK_CLEAR:			key = Qt::Key_Clear;		break;
		case LL_DOM_VK_RETURN:			key = Qt::Key_Return;		break;
		case LL_DOM_VK_ENTER:			key = Qt::Key_Enter;		break;
		case LL_DOM_VK_SHIFT:			key = Qt::Key_Shift;		break;
		case LL_DOM_VK_CONTROL:			key = Qt::Key_Control;		break;
		case LL_DOM_VK_ALT:			key = Qt::Key_Alt;		break;
		case LL_DOM_VK_PAUSE:			key = Qt::Key_Pause;		break;
		case LL_DOM_VK_CAPS_LOCK:		key = Qt::Key_CapsLock;		break;
		case LL_DOM_VK_ESCAPE:			key = Qt::Key_Escape;		break;
		case LL_DOM_VK_SPACE:			key = Qt::Key_Space;		break;
		case LL_DOM_VK_PAGE_UP:			key = Qt::Key_PageUp;		break;
		case LL_DOM_VK_PAGE_DOWN:		key = Qt::Key_PageDown;		break;
		case LL_DOM_VK_END:			key = Qt::Key_End;		break;
		case LL_DOM_VK_HOME:			key = Qt::Key_Home;		break;
		case LL_DOM_VK_LEFT:			key = Qt::Key_Left;		break;
		case LL_DOM_VK_UP:			key = Qt::Key_Up;		break;
		case LL_DOM_VK_RIGHT:			key = Qt::Key_Right;		break;
		case LL_DOM_VK_DOWN:			key = Qt::Key_Down;		break;
		case LL_DOM_VK_PRINTSCREEN:		key = Qt::Key_Print;		break;
		case LL_DOM_VK_INSERT:			key = Qt::Key_Insert;		break;
		case LL_DOM_VK_DELETE:			key = Qt::Key_Delete;		break;
		case LL_DOM_VK_CONTEXT_MENU:	        key = Qt::Key_Menu;		break;

		default:
			key = (Qt::Key)keyCode;
			text = QChar(keyCode);
		break;
    }

    {
        QKeyEvent event( QEvent::KeyPress, key, Qt::NoModifier, text);
        qApp->sendEvent(d->page, &event);
    }
    {
        QKeyEvent event( QEvent::KeyRelease, key, Qt::NoModifier, text);
        qApp->sendEvent(d->page, &event);
    }
}

// accept keyboard input that's already been translated into a unicode char.
void LLEmbeddedBrowserWindow::unicodeInput( quint32 uni_char )
{
    Qt::Key key = Qt::Key_unknown;
    QChar input((uint)uni_char);

    {
        QKeyEvent event( QEvent::KeyPress, key, Qt::NoModifier, input);
        qApp->sendEvent(d->page, &event);
    }
    {
        QKeyEvent event( QEvent::KeyRelease, key, Qt::NoModifier, input);
        qApp->sendEvent(d->page, &event);
    }
}

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

