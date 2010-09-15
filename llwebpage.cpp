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

#include "llwebpage.h"

#include <qnetworkrequest.h>
#include <qwebframe.h>
#include <qgraphicswebview.h>
#include <qevent.h>
#include <qdebug.h>
#include <qmessagebox.h>
#include <qwebelement.h>
#include <qgraphicsproxywidget.h>

#include "llqtwebkit.h"
#include "llembeddedbrowserwindow.h"
#include "llembeddedbrowserwindow_p.h"

LLWebPage::LLWebPage(QObject *parent)
    : QWebPage(parent)
    , window(0)
    , mHostLanguage( "en" )
{
    connect(this, SIGNAL(loadProgress(int)),
            this, SLOT(loadProgressSlot(int)));
    connect(this, SIGNAL(statusBarMessage(const QString &)),
            this, SLOT(statusBarMessageSlot(const QString &)));
    connect(mainFrame(), SIGNAL(urlChanged(const QUrl&)),
            this, SLOT(urlChangedSlot(const QUrl&)));
    connect(this, SIGNAL(loadStarted()),
            this, SLOT(loadStarted()));
    connect(this, SIGNAL(loadFinished(bool)),
            this, SLOT(loadFinished(bool)));
    connect(this, SIGNAL(windowCloseRequested()),
            this, SLOT(windowCloseRequested()));
    connect(this, SIGNAL(geometryChangeRequested(const QRect&)),
            this, SLOT(geometryChangeRequested(const QRect&)));
    connect(mainFrame(), SIGNAL(titleChanged(const QString&)),
            this, SLOT(titleChangedSlot(const QString&)));
    connect(mainFrame(), SIGNAL(javaScriptWindowObjectCleared()),
            this, SLOT(extendNavigatorObject()));
}

void LLWebPage::loadProgressSlot(int progress)
{
    if (!window)
        return;
    window->d->mPercentComplete = progress;
    LLEmbeddedBrowserWindowEvent event(window->getWindowId());
	event.setEventUri(window->getCurrentUri());
	event.setIntValue(progress);
    window->d->mEventEmitter.update(&LLEmbeddedBrowserWindowObserver::onUpdateProgress, event);
}

void LLWebPage::statusBarMessageSlot(const QString& text)
{
    if (!window)
        return;
    window->d->mStatusText = text.toStdString();
    LLEmbeddedBrowserWindowEvent event(window->getWindowId());
	event.setEventUri(window->getCurrentUri());
	event.setStringValue(window->d->mStatusText);
    window->d->mEventEmitter.update(&LLEmbeddedBrowserWindowObserver::onStatusTextChange, event);
}

void LLWebPage::titleChangedSlot(const QString& text)
{
    if (!window)
        return;
    window->d->mTitle = text.toStdString();
	LLEmbeddedBrowserWindowEvent event(window->getWindowId());
	event.setEventUri(window->getCurrentUri());
	event.setStringValue(window->d->mTitle);
	window->d->mEventEmitter.update(&LLEmbeddedBrowserWindowObserver::onTitleChange, event);
}

void LLWebPage::urlChangedSlot(const QUrl& url)
{
    Q_UNUSED(url);

    if (!window)
        return;

	LLEmbeddedBrowserWindowEvent event(window->getWindowId());
	event.setEventUri(window->getCurrentUri());
    window->d->mEventEmitter.update(&LLEmbeddedBrowserWindowObserver::onLocationChange, event);
}

bool LLWebPage::event(QEvent *event)
{
    bool result = QWebPage::event(event);

    if (event->type() == QEvent::GraphicsSceneMousePress)
		currentPoint = ((QGraphicsSceneMouseEvent*)event)->pos().toPoint();
    else if(event->type() == QEvent::GraphicsSceneMouseRelease)
        currentPoint = QPoint();

    return result;
}

bool LLWebPage::acceptNavigationRequest(QWebFrame* frame, const QNetworkRequest& request, NavigationType type)
{
    if (!window)
        return false;

    if (request.url().scheme() == window->d->mNoFollowScheme)
    {
        QString encodedUrl = request.url().toEncoded();
        // QUrl is turning foo:///home/bar into foo:/home/bar for some reason while Firefox does not
        // http://bugs.webkit.org/show_bug.cgi?id=24695
        if (!encodedUrl.startsWith(window->d->mNoFollowScheme + "://")) {
            encodedUrl = encodedUrl.mid(window->d->mNoFollowScheme.length() + 1);
            encodedUrl = window->d->mNoFollowScheme + "://" + encodedUrl;
        }
        std::string rawUri = encodedUrl.toStdString();
		LLEmbeddedBrowserWindowEvent event(window->getWindowId());
		event.setEventUri(rawUri);
		window->d->mEventEmitter.update(&LLEmbeddedBrowserWindowObserver::onClickLinkNoFollow, event);

//	 	qDebug() << "LLWebPage::acceptNavigationRequest: sending onClickLinkNoFollow, NavigationType is " << type << ", url is " << QString::fromStdString(rawUri) ;
		return false;
    }
	
	
    return true;
}


void LLWebPage::loadStarted()
{
    if (!window)
        return;
	LLEmbeddedBrowserWindowEvent event(window->getWindowId());
	event.setEventUri(window->getCurrentUri());
    window->d->mEventEmitter.update(&LLEmbeddedBrowserWindowObserver::onNavigateBegin, event);
}

void LLWebPage::loadFinished(bool)
{
    if (!window)
        return;
	LLEmbeddedBrowserWindowEvent event(window->getWindowId());
	event.setEventUri(window->getCurrentUri());
    window->d->mEventEmitter.update(&LLEmbeddedBrowserWindowObserver::onNavigateComplete, event);
}

void LLWebPage::windowCloseRequested()
{
    if (!window)
        return;
    LLEmbeddedBrowserWindowEvent event(window->getWindowId());
    window->d->mEventEmitter.update(&LLEmbeddedBrowserWindowObserver::onWindowCloseRequested, event);
}

void LLWebPage::geometryChangeRequested(const QRect& geom)
{
    if (!window)
        return;
    
	LLEmbeddedBrowserWindowEvent event(window->getWindowId());
	// empty UUID indicates this is targeting the main window
//	event.setStringValue(window->getUUID());
	event.setRectValue(geom.x(), geom.y(), geom.width(), geom.height());
    window->d->mEventEmitter.update(&LLEmbeddedBrowserWindowObserver::onWindowGeometryChangeRequested, event);	
}

QString LLWebPage::chooseFile(QWebFrame* parentFrame, const QString& suggestedFile)
{
    Q_UNUSED(parentFrame);
    Q_UNUSED(suggestedFile);

	LLEmbeddedBrowserWindowEvent event(window->getWindowId());
	event.setEventUri(window->getCurrentUri());
	event.setStringValue("*.png;*.jpg");
    std::string filename_chosen = window->d->mEventEmitter.updateAndReturn( &LLEmbeddedBrowserWindowObserver::onRequestFilePicker, event );

    return QString::fromStdString( filename_chosen );
}

void LLWebPage::javaScriptAlert(QWebFrame* frame, const QString& msg)
{
    Q_UNUSED(frame);
    QMessageBox *msgBox = new QMessageBox;
    msgBox->setWindowTitle(tr("JavaScript Alert - %1").arg(mainFrame()->url().host()));
    msgBox->setText(msg);
    msgBox->addButton(QMessageBox::Ok);

    QGraphicsProxyWidget *proxy = webView->scene()->addWidget(msgBox);
    proxy->setWindowFlags(Qt::Window); // this makes the item a panel (and will make it get a window 'frame')
    proxy->setPanelModality(QGraphicsItem::SceneModal);
    proxy->setPos((webView->boundingRect().width() - msgBox->sizeHint().width())/2,
                  (webView->boundingRect().height() - msgBox->sizeHint().height())/2);
    proxy->setActive(true); // make it the active item

    connect(msgBox, SIGNAL(finished(int)), proxy, SLOT(deleteLater()));
    msgBox->show();

    webView->scene()->setFocusItem(proxy);
}

bool LLWebPage::javaScriptConfirm(QWebFrame* frame, const QString& msg)
{
    Q_UNUSED(frame);
    Q_UNUSED(msg);
    qWarning() << "LLWebPage::" << __FUNCTION__ << "not implemented" << msg << "returning true";
    return true;
}

bool LLWebPage::javaScriptPrompt(QWebFrame* frame, const QString& msg, const QString& defaultValue, QString* result)
{
    Q_UNUSED(frame);
    Q_UNUSED(msg);
    Q_UNUSED(defaultValue);
    Q_UNUSED(result);
    qWarning() << "LLWebPage::" << __FUNCTION__ << "not implemented" << msg << defaultValue << "returning false";
    return false;
}

void LLWebPage::extendNavigatorObject()
{
	QString q_host_language = QString::fromStdString( mHostLanguage );

    mainFrame()->evaluateJavaScript(QString("navigator.hostLanguage=\"%1\"").arg( q_host_language ));
}

QWebPage *LLWebPage::createWindow(WebWindowType type)
{
    Q_UNUSED(type);
	QWebPage *result = NULL;
	
	if(window)
	{
		result = window->createWindow();
	}
	
	return result;
}

void LLWebPage::setHostLanguage(const std::string& host_language)
{
	mHostLanguage = host_language;
}
