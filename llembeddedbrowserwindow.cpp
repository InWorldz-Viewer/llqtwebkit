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
#include "llembeddedbrowserwindow_p.h"

#include "llembeddedbrowser.h"
#include "llembeddedbrowser_p.h"

#include <qwebframe.h>
#include <qwebhistory.h>
#include <qpainter.h>
#include <qevent.h>
#include <qfile.h>
#include <QGLWidget>

#ifdef STATIC_QT
	#include <qplugin.h>
	// Enable gif and jpeg plugins, since web pages look pretty bleak without gifs or jpegs.
	Q_IMPORT_PLUGIN(qgif)
	Q_IMPORT_PLUGIN(qjpeg)
#endif

//#define LLEMBEDDEDBROWSER_DEBUG

#ifdef LLEMBEDDEDBROWSER_DEBUG
#include <qdebug.h>
#endif

LLEmbeddedBrowserWindow::LLEmbeddedBrowserWindow()
{
    d = new LLEmbeddedBrowserWindowPrivate();
    d->mPage->window = this;

    d->mView = new QWebView;
    d->mView->setPage(d->mPage);
    d->mGraphicsScene = new GraphicsScene;
    d->mGraphicsScene->window = this;
    d->mGraphicsView = new QGraphicsView;
    d->mGraphicsScene->addWidget(d->mView);
    d->mGraphicsView->setScene(d->mGraphicsScene);
    d->mGraphicsScene->setStickyFocus(true);
    d->mGraphicsView->viewport()->setParent(0);
}

LLEmbeddedBrowserWindow::~LLEmbeddedBrowserWindow()
{
    delete d;
}

void LLEmbeddedBrowserWindow::setParent(LLEmbeddedBrowser* parent)
{
#ifdef LLEMBEDDEDBROWSER_DEBUG
    qDebug() << "LLEmbeddedBrowserWindow" << __FUNCTION__ << parent;
#endif
    d->mParent = parent;
    if (parent)
    {
        d->mPage->setNetworkAccessManager(parent->d->mNetworkAccessManager);
    } else
    {
        d->mPage->setNetworkAccessManager(0);
    }
}

// change the background color that gets used between pages (usually white)
void LLEmbeddedBrowserWindow::setBackgroundColor(const uint8_t red, const uint8_t green, const uint8_t blue)
{
#ifdef LLEMBEDDEDBROWSER_DEBUG
    qDebug() << "LLEmbeddedBrowserWindow" << __FUNCTION__ << red << green << blue;
#endif
    d->backgroundColor = QColor(red, green, blue);
}

// change the caret color (we have different backgrounds to edit fields - black caret on black background == bad)
void LLEmbeddedBrowserWindow::setCaretColor(const uint8_t red, const uint8_t green, const uint8_t blue)
{
#ifdef LLEMBEDDEDBROWSER_DEBUG
    qDebug() << "LLEmbeddedBrowserWindow" << __FUNCTION__ << red << green << blue;
#endif
    Q_UNUSED(red);
    Q_UNUSED(green);
    Q_UNUSED(blue);
    // QtWebKit paints the caret so we don't have to
}

//
void LLEmbeddedBrowserWindow::setEnabled(bool enabled)
{
#ifdef LLEMBEDDEDBROWSER_DEBUG
    qDebug() << "LLEmbeddedBrowserWindow" << __FUNCTION__ << enabled;
#endif
    d->mEnabled = enabled;
}

// allow consumers of this class to observe events - add themselves as an observer
bool LLEmbeddedBrowserWindow::addObserver(LLEmbeddedBrowserWindowObserver* observer)
{
#ifdef LLEMBEDDEDBROWSER_DEBUG
    qDebug() << "LLEmbeddedBrowserWindow" << __FUNCTION__ << observer;
#endif
    return d->mEventEmitter.addObserver(observer);
}

// allow consumers of this class to observe events - remove themselves as an observer
bool LLEmbeddedBrowserWindow::remObserver(LLEmbeddedBrowserWindowObserver* observer)
{
#ifdef LLEMBEDDEDBROWSER_DEBUG
    qDebug() << "LLEmbeddedBrowserWindow" << __FUNCTION__ << observer;
#endif
    return d->mEventEmitter.remObserver(observer);
}

int LLEmbeddedBrowserWindow::getObserverNumber()
{
    return d->mEventEmitter.getObserverNumber();
}

// used by observers of this class to get the current URI
const std::string& LLEmbeddedBrowserWindow::getCurrentUri()
{
    d->mCurrentUri = QString(d->mPage->mainFrame()->url().toEncoded()).toStdString();
    return d->mCurrentUri;
}

// utility method that is used by observers to retrieve data after an event
const int16_t LLEmbeddedBrowserWindow::getPercentComplete()
{
#ifdef LLEMBEDDEDBROWSER_DEBUG
    qDebug() << "LLEmbeddedBrowserWindow" << __FUNCTION__;
#endif
    return d->mPercentComplete;
}

// utility method that is used by observers to retrieve data after an event
const std::string& LLEmbeddedBrowserWindow::getStatusMsg()
{
#ifdef LLEMBEDDEDBROWSER_DEBUG
    qDebug() << "LLEmbeddedBrowserWindow" << __FUNCTION__;
#endif
    return d->mStatusText;
}

// utility method that is used by observers to retrieve data after an event
const std::string& LLEmbeddedBrowserWindow::getClickLinkHref()
{
#ifdef LLEMBEDDEDBROWSER_DEBUG
    qDebug() << "LLEmbeddedBrowserWindow" << __FUNCTION__;
#endif
    return d->mClickHref;
}

// utility method that is used by observers to retrieve data after an event
const std::string& LLEmbeddedBrowserWindow::getClickLinkTarget()
{
#ifdef LLEMBEDDEDBROWSER_DEBUG
    qDebug() << "LLEmbeddedBrowserWindow" << __FUNCTION__;
#endif
    return d->mClickTarget;
}

// render a page into memory and grab the window
unsigned char* LLEmbeddedBrowserWindow::grabWindow(int x, int y, int width, int height)
{
#ifdef LLEMBEDDEDBROWSER_DEBUG
    qDebug() << "LLEmbeddedBrowserWindow" << __FUNCTION__ << x << y << width << height;
#endif
    // only grab the window if it's enabled
    if (!d->mEnabled)
        return 0;

    d->mImage = QImage(QSize(d->mView->width(), d->mView->height()), QImage::Format_RGB32);
    if (!d->mPage->mainFrame()->url().isValid())
    {
        d->mImage.fill(d->backgroundColor.value());
    } else
    {
        QPainter painter(&d->mImage);
#if 1   // Paint from the graphics view
        QRectF r(x, y, width, height);
        QRect g(0, 0, d->mView->width(), d->mView->height());
        d->mGraphicsView->render(&painter, r, g);
#else   // Paint straight from the web page
        QRegion clip(x, y, width, height);
        d->mPage->mainFrame()->render(&painter, clip);
#endif
        painter.end();
        if (!d->mFlipBitmap)
        {
            d->mImage = d->mImage.mirrored();
        }
    }
    d->mImage = QGLWidget::convertToGLFormat(d->mImage);
    d->mPageBuffer = d->mImage.bits();
    return d->mPageBuffer;
}

// return the buffer that contains the rendered page
unsigned char* LLEmbeddedBrowserWindow::getPageBuffer()
{
#ifdef LLEMBEDDEDBROWSER_DEBUG
    qDebug() << "LLEmbeddedBrowserWindow" << __FUNCTION__;
#endif
    return d->mPageBuffer;
}

int16_t LLEmbeddedBrowserWindow::getBrowserWidth()
{
#ifdef LLEMBEDDEDBROWSER_DEBUG
    qDebug() << "LLEmbeddedBrowserWindow" << __FUNCTION__;
#endif
    return d->mImage.width();
}

int16_t LLEmbeddedBrowserWindow::getBrowserHeight()
{
#ifdef LLEMBEDDEDBROWSER_DEBUG
    qDebug() << "LLEmbeddedBrowserWindow" << __FUNCTION__;
#endif
    return d->mImage.height();
}

int16_t LLEmbeddedBrowserWindow::getBrowserDepth()
{
#ifdef LLEMBEDDEDBROWSER_DEBUG
    qDebug() << "LLEmbeddedBrowserWindow" << __FUNCTION__;
#endif
    return 4;
}

int32_t LLEmbeddedBrowserWindow::getBrowserRowSpan()
{
#ifdef LLEMBEDDEDBROWSER_DEBUG
    qDebug() << "LLEmbeddedBrowserWindow" << __FUNCTION__;
#endif
    return 4 * getBrowserWidth();
}

bool LLEmbeddedBrowserWindow::navigateTo(const std::string uri)
{
#ifdef LLEMBEDDEDBROWSER_DEBUG
    qDebug() << "LLEmbeddedBrowserWindow" << __FUNCTION__ << QString::fromStdString(uri);
#endif
    QString string = QString::fromStdString(uri);
    QUrl url;
	url = QUrl::fromEncoded(string.toLocal8Bit());
	if (QFile::exists(string))
		url = QUrl::fromLocalFile(string);
	if (url.scheme().isEmpty())
        url = QUrl(QLatin1String("http://") + string, QUrl::TolerantMode);
    navigateStop();
    d->mPage->mainFrame()->load(url);
    return true;
}

bool LLEmbeddedBrowserWindow::canNavigateBack()
{
    return d->mPage->history()->canGoBack();
}

void LLEmbeddedBrowserWindow::navigateStop()
{
#ifdef LLEMBEDDEDBROWSER_DEBUG
    qDebug() << "LLEmbeddedBrowserWindow" << __FUNCTION__;
#endif
    d->mPage->triggerAction(QWebPage::Stop);
}

void LLEmbeddedBrowserWindow::navigateBack()
{
#ifdef LLEMBEDDEDBROWSER_DEBUG
    qDebug() << "LLEmbeddedBrowserWindow" << __FUNCTION__;
#endif
    d->mPage->triggerAction(QWebPage::Back);
}

bool LLEmbeddedBrowserWindow::canNavigateForward()
{
    return d->mPage->history()->canGoForward();
}

void LLEmbeddedBrowserWindow::navigateForward()
{
#ifdef LLEMBEDDEDBROWSER_DEBUG
    qDebug() << "LLEmbeddedBrowserWindow" << __FUNCTION__;
#endif
    d->mPage->triggerAction(QWebPage::Forward);
}

void LLEmbeddedBrowserWindow::navigateReload()
{
#ifdef LLEMBEDDEDBROWSER_DEBUG
    qDebug() << "LLEmbeddedBrowserWindow" << __FUNCTION__;
#endif
    d->mPage->triggerAction(QWebPage::Reload);
}

// set the size of the browser window
bool LLEmbeddedBrowserWindow::setSize(int16_t width, int16_t height)
{
#ifdef LLEMBEDDEDBROWSER_DEBUG
    qDebug() << "LLEmbeddedBrowserWindow" << __FUNCTION__ << width << height;
#endif
    d->mPageBuffer = NULL;
    d->mImage = QImage(QSize(width, height), QImage::Format_RGB32);
    d->mGraphicsView->resize(width, height);
    d->mView->resize(width, height);
    d->mImage.fill(d->backgroundColor.rgb());
    return true;
}

bool LLEmbeddedBrowserWindow::flipWindow(bool flip)
{
#ifdef LLEMBEDDEDBROWSER_DEBUG
    qDebug() << "LLEmbeddedBrowserWindow" << __FUNCTION__ << flip;
#endif
    d->mFlipBitmap = flip;
    return true;
}

void LLEmbeddedBrowserWindow::mouseLeftDoubleClick(int16_t x, int16_t y)
{
#ifdef LLEMBEDDEDBROWSER_DEBUG
    qDebug() << "LLEmbeddedBrowserWindow" << __FUNCTION__ << x << y;
#endif
    QMouseEvent event(QEvent::MouseButtonDblClick, QPoint(x, y), Qt::LeftButton, Qt::LeftButton, 0);
    qApp->sendEvent(d->mGraphicsView->viewport(), &event);
}

void LLEmbeddedBrowserWindow::mouseDown(int16_t x, int16_t y)
{
#ifdef LLEMBEDDEDBROWSER_DEBUG
    qDebug() << "LLEmbeddedBrowserWindow" << __FUNCTION__ << x << y << d->mPage->mainFrame()->geometry();
#endif
    QMouseEvent event(QEvent::MouseButtonPress, QPoint(x, y), Qt::LeftButton, 0, 0);
    d->mCurrentMouseDown = Qt::LeftButton;
    qApp->sendEvent(d->mGraphicsView->viewport(), &event);
}

void LLEmbeddedBrowserWindow::mouseUp(int16_t x, int16_t y)
{
#ifdef LLEMBEDDEDBROWSER_DEBUG
    qDebug() << "LLEmbeddedBrowserWindow" << __FUNCTION__ << x << y;
#endif
    QMouseEvent event(QEvent::MouseButtonRelease, QPoint(x, y), Qt::LeftButton, 0, 0);
    qApp->sendEvent(d->mGraphicsView->viewport(), &event);
    d->mCurrentMouseDown = Qt::NoButton;
}

void LLEmbeddedBrowserWindow::mouseMove(int16_t x, int16_t y)
{
#ifdef LLEMBEDDEDBROWSER_DEBUG
    qDebug() << "LLEmbeddedBrowserWindow" << __FUNCTION__ << x << y;
#endif
    if (x == 0 && y == 0)
    {
        return;
    }
    QMouseEvent event(QEvent::MouseMove, QPoint(x, y), d->mCurrentMouseDown, 0, 0);
    qApp->sendEvent(d->mGraphicsView->viewport(), &event);
}

// utility methods to set an error message so something else can look at it
void LLEmbeddedBrowserWindow::scrollByLines(int16_t lines)
{
#ifdef LLEMBEDDEDBROWSER_DEBUG
    qDebug() << "LLEmbeddedBrowserWindow" << __FUNCTION__ << lines;
#endif
    int currentScrollValue = d->mPage->mainFrame()->scrollBarValue(Qt::Vertical);
    d->mPage->mainFrame()->setScrollBarValue(Qt::Vertical, currentScrollValue + lines);
}

// accept a (mozilla-style) keycode
void LLEmbeddedBrowserWindow::keyPress(int16_t key_code)
{
#ifdef LLEMBEDDEDBROWSER_DEBUG
    qDebug() << "LLEmbeddedBrowserWindow" << __FUNCTION__ << key_code;
#endif
    Qt::Key key;
    QChar text;

    switch (key_code)
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
			key = (Qt::Key)key_code;
			text = QChar(key_code);
		break;
    }

    QKeyEvent press_event(QEvent::KeyPress, key, Qt::NoModifier, text);
    qApp->sendEvent(d->mGraphicsScene, &press_event);
    QKeyEvent release_event(QEvent::KeyRelease, key, Qt::NoModifier, text);
    qApp->sendEvent(d->mGraphicsScene, &release_event);
}

// accept keyboard input that's already been translated into a unicode char.
void LLEmbeddedBrowserWindow::unicodeInput(uint32_t unicode_char)
{
#ifdef LLEMBEDDEDBROWSER_DEBUG
    qDebug() << "LLEmbeddedBrowserWindow" << __FUNCTION__ << unicode_char;
#endif
    Qt::Key key = Qt::Key_unknown;
    QChar input((uint)unicode_char);

    QKeyEvent press_event(QEvent::KeyPress, key, Qt::NoModifier, input);
    qApp->sendEvent(d->mGraphicsScene, &press_event);
    QKeyEvent release_event(QEvent::KeyRelease, key, Qt::NoModifier, input);
    qApp->sendEvent(d->mGraphicsScene, &release_event);
}

// give focus to the browser so that input keyboard events work
void LLEmbeddedBrowserWindow::focusBrowser(bool focus_browser)
{
#ifdef LLEMBEDDEDBROWSER_DEBUG
    qDebug() << "LLEmbeddedBrowserWindow" << __FUNCTION__ << focus_browser;
#endif
    QFocusEvent event(focus_browser ? QEvent::FocusIn : QEvent::FocusOut, Qt::OtherFocusReason);
    qApp->sendEvent(d->mPage, &event);
}

void LLEmbeddedBrowserWindow::setWindowId(int window_id)
{
#ifdef LLEMBEDDEDBROWSER_DEBUG
    qDebug() << "LLEmbeddedBrowserWindow" << __FUNCTION__ << window_id;
#endif
    d->mWindowId = window_id;
}

int LLEmbeddedBrowserWindow::getWindowId()
{
    return d->mWindowId;
}

std::string LLEmbeddedBrowserWindow::evaluateJavascript(std::string script)
{
#ifdef LLEMBEDDEDBROWSER_DEBUG
    qDebug() << "LLEmbeddedBrowserWindow" << __FUNCTION__ << QString::fromStdString(script);
#endif
    QString q_script = QString::fromStdString(script);
    QString result = d->mPage->mainFrame()->evaluateJavaScript(q_script).toString();
    return result.toStdString();
}

bool LLEmbeddedBrowserWindow::set404RedirectUrl(std::string redirect_url)
{
#ifdef LLEMBEDDEDBROWSER_DEBUG
    qDebug() << "LLEmbeddedBrowserWindow" << __FUNCTION__ << QString::fromStdString(redirect_url);
#endif
    d->m404RedirectUrl = redirect_url;
    return true;
}

bool LLEmbeddedBrowserWindow::clr404RedirectUrl()
{
#ifdef LLEMBEDDEDBROWSER_DEBUG
    qDebug() << "LLEmbeddedBrowserWindow" << __FUNCTION__;
#endif
    d->m404RedirectUrl = std::string();
    return true;
}

void LLEmbeddedBrowserWindow::load404RedirectUrl()
{
#ifdef LLEMBEDDEDBROWSER_DEBUG
    qDebug() << "LLEmbeddedBrowserWindow" << __FUNCTION__;
#endif
    QUrl url = QUrl(QString::fromStdString(d->m404RedirectUrl));
    navigateStop();
    d->mPage->mainFrame()->load(url);
}

void LLEmbeddedBrowserWindow::setNoFollowScheme(std::string scheme)
{
#ifdef LLEMBEDDEDBROWSER_DEBUG
    qDebug() << "LLEmbeddedBrowserWindow" << __FUNCTION__ << QString::fromStdString(scheme);
#endif
    d->mNoFollowScheme = QString::fromStdString(scheme);
    // The scheme part of the url is what is before '://'
    d->mNoFollowScheme = d->mNoFollowScheme.mid(0, d->mNoFollowScheme.indexOf("://"));
}

std::string LLEmbeddedBrowserWindow::getNoFollowScheme()
{
#ifdef LLEMBEDDEDBROWSER_DEBUG
    qDebug() << "LLEmbeddedBrowserWindow" << __FUNCTION__;
#endif
    return d->mNoFollowScheme.toStdString();
}

GraphicsScene::GraphicsScene()
    : QGraphicsScene()
{
    connect(this, SIGNAL(changed(const QList<QRectF> &)),
            this, SLOT(repaintRequestedSlot(const QList<QRectF> &)));
}

void GraphicsScene::repaintRequestedSlot(const QList<QRectF> &regions)
{
    for (int i = 0; i < regions.count(); ++i) {
        LLEmbeddedBrowserWindowEvent event(window->getWindowId(), window->getCurrentUri(),
                regions[i].x(), regions[i].y(), regions[i].width(), regions[i].height());

        window->d->mEventEmitter.update(&LLEmbeddedBrowserWindowObserver::onPageChanged, event);
    }
}

