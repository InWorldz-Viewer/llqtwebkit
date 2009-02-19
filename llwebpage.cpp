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
 * The Original Code is Torch Mobile Inc. (http://www.torchmobile.com/) code
 *
 * The Initial Developer of the Original Code is:
 *   Benjamin Meyer (benjamin.meyer@torchmobile.com)
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

#include "llwebpage.h"
#include "llmozlib2.h"
#include "llembeddedbrowserwindow.h"
#include "llembeddedbrowserwindow_p.h"

//#define LINKTARGETPATCH

LLWebPage::LLWebPage(QObject *parent)
    : QWebPage(parent)
{
    connect(this, SIGNAL(loadProgress(int)),
            this, SLOT(loadProgressSlot(int)));
    connect(this, SIGNAL(statusBarMessage(const QString &)),
            this, SLOT(statusBarMessageSlot(const QString &)));
    connect(mainFrame(), SIGNAL(urlChanged(const QUrl&)),
            this, SLOT(urlChangedSlot(const QUrl&)));
    connect(this, SIGNAL(repaintRequested(const QRect &)),
            this, SLOT(repaintRequestedSlot(const QRect &)));
    connect(this, SIGNAL(scrollRequested(int, int, const QRect &)),
            this, SLOT(scrollRequestedSlot(int, int, const QRect &)));
    connect(this, SIGNAL(loadFinished(bool)),
            this, SLOT(loadFinished(bool)));
}

void LLWebPage::loadProgressSlot(int progress)
{
    window->d->mPercentComplete = progress;
    LLEmbeddedBrowserWindowEvent event(window->getWindowId(), window->getCurrentUri(), progress);
    window->d->mEventEmitter.update(&LLEmbeddedBrowserWindowObserver::onUpdateProgress, event);
}

void LLWebPage::statusBarMessageSlot(const QString& text)
{
    window->d->mStatusText = text.toStdString();
    LLEmbeddedBrowserWindowEvent event(window->getWindowId(), window->getCurrentUri(), window->d->mStatusText);
    window->d->mEventEmitter.update(&LLEmbeddedBrowserWindowObserver::onStatusTextChange, event);
}

void LLWebPage::urlChangedSlot(const QUrl& url)
{
    Q_UNUSED(url);
    LLEmbeddedBrowserWindowEvent event(window->getWindowId(), window->getCurrentUri());
    window->d->mEventEmitter.update(&LLEmbeddedBrowserWindowObserver::onLocationChange, event);
}

void LLWebPage::repaintRequestedSlot(const QRect& dirty_rect)
{
    LLEmbeddedBrowserWindowEvent event(window->getWindowId(), window->getCurrentUri(),
            dirty_rect.x(), dirty_rect.y(), dirty_rect.width(), dirty_rect.height());

    window->d->mEventEmitter.update(&LLEmbeddedBrowserWindowObserver::onPageChanged, event);
}

void LLWebPage::scrollRequestedSlot(int dx, int dy, const QRect& rect_to_scroll)
{
    Q_UNUSED(dx);
    Q_UNUSED(dy);
    Q_UNUSED(rect_to_scroll);
    LLEmbeddedBrowserWindowEvent event(window->getWindowId(), window->getCurrentUri(),
            0, 0, window->getBrowserWidth(), window->getBrowserHeight());
            //dx, dy, rect_to_scroll.width(), rect_to_scroll.height());
    window->d->mEventEmitter.update(&LLEmbeddedBrowserWindowObserver::onPageChanged, event);
}

bool LLWebPage::event(QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress)
        currentPoint = ((QMouseEvent*)event)->pos();
    else if (event->type() == QEvent::MouseButtonRelease)
        currentPoint = QPoint();
    return QWebPage::event(event);
}

bool LLWebPage::acceptNavigationRequest(QWebFrame* frame, const QNetworkRequest& request, NavigationType type)
{
    if (request.url().scheme() == window->d->mNoFollowScheme)
    {
        std::string rawUri = QString(request.url().toEncoded()).toStdString();
        LLEmbeddedBrowserWindowEvent event(window->getWindowId(), rawUri, rawUri);
	window->d->mEventEmitter.update(&LLEmbeddedBrowserWindowObserver::onClickLinkNoFollow, event);
        return false;
    }
    bool accepted = QWebPage::acceptNavigationRequest(frame, request, type);
    if (accepted && type == QWebPage::NavigationTypeLinkClicked) {
        QUrl url = request.url();
        window->d->mClickHref = QString(url.toEncoded()).toStdString();
#ifdef LINKTARGETPATCH
        QWebHitTestResult hitTest = mainFrame()->hitTestContent(currentPoint);
        window->d->mClickTarget = hitTest.linkTarget().toStdString();
#else
        window->d->mClickTarget = std::string();
#endif
        LLEmbeddedBrowserWindowEvent event(window->getWindowId(),
                                           window->getCurrentUri(),
                                           window->d->mClickHref,
                                           window->d->mClickTarget);
        window->d->mEventEmitter.update(&LLEmbeddedBrowserWindowObserver::onClickLinkHref, event);
    }
    return accepted;
}

void LLWebPage::loadFinished(bool)
{
    LLEmbeddedBrowserWindowEvent event(window->getWindowId(),
            window->getCurrentUri());
    window->d->mEventEmitter.update(&LLEmbeddedBrowserWindowObserver::onNavigateComplete, event);
}

QString LLWebPage::chooseFile(QWebFrame* parentFrame, const QString& suggestedFile)
{
    qWarning() << "LLWebPage::" << __FUNCTION__ << "not implemented" << "Returning empty string";
    return QString();
}

void LLWebPage::javaScriptAlert(QWebFrame* frame, const QString& msg)
{
    qWarning() << "LLWebPage::" << __FUNCTION__ << "not implemented" << msg;
}

bool LLWebPage::javaScriptConfirm(QWebFrame* frame, const QString& msg)
{
    qWarning() << "LLWebPage::" << __FUNCTION__ << "not implemented" << msg << "returning true";
    return true;
}

bool LLWebPage::javaScriptPrompt(QWebFrame* frame, const QString& msg, const QString& defaultValue, QString* result)
{
    qWarning() << "LLWebPage::" << __FUNCTION__ << "not implemented" << msg << defaultValue << "returning false";
    return false;
}
