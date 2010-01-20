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
    connect(mainFrame(), SIGNAL(titleChanged(const QString&)),
            this, SLOT(titleChangedSlot(const QString&)));
}

void LLWebPage::loadProgressSlot(int progress)
{
    if (!window)
        return;
    window->d->mPercentComplete = progress;
    LLEmbeddedBrowserWindowEvent event(window->getWindowId(), window->getCurrentUri(), progress);
    window->d->mEventEmitter.update(&LLEmbeddedBrowserWindowObserver::onUpdateProgress, event);
}

void LLWebPage::statusBarMessageSlot(const QString& text)
{
    if (!window)
        return;
    window->d->mStatusText = text.toStdString();
    LLEmbeddedBrowserWindowEvent event(window->getWindowId(), window->getCurrentUri(), window->d->mStatusText);
    window->d->mEventEmitter.update(&LLEmbeddedBrowserWindowObserver::onStatusTextChange, event);
}

void LLWebPage::titleChangedSlot(const QString& text)
{
    if (!window)
        return;
    window->d->mTitle = text.toStdString();
	LLEmbeddedBrowserWindowEvent event(window->getWindowId(), window->getCurrentUri(), window->d->mTitle);
	window->d->mEventEmitter.update(&LLEmbeddedBrowserWindowObserver::onTitleChange, event);
}

void LLWebPage::urlChangedSlot(const QUrl& url)
{
    Q_UNUSED(url);

    if (!window)
        return;

    LLEmbeddedBrowserWindowEvent event(window->getWindowId(), window->getCurrentUri());
    window->d->mEventEmitter.update(&LLEmbeddedBrowserWindowObserver::onLocationChange, event);
}

bool LLWebPage::event(QEvent *event)
{
    bool result = QWebPage::event(event);

    if (event->type() == QEvent::GraphicsSceneMousePress)
		currentPoint = ((QGraphicsSceneMouseEvent*)event)->pos().toPoint();
    else
    if (event->type() == QEvent::GraphicsSceneMouseRelease)
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
        LLEmbeddedBrowserWindowEvent event(window->getWindowId(), rawUri, rawUri);
	window->d->mEventEmitter.update(&LLEmbeddedBrowserWindowObserver::onClickLinkNoFollow, event);
        return false;
    }
    bool accepted = QWebPage::acceptNavigationRequest(frame, request, type);
    if (accepted && type == QWebPage::NavigationTypeLinkClicked)
    {
		// save URL
        QUrl url = request.url();
        window->d->mClickHref = QString(url.toEncoded()).toStdString();

		// save target attribute
		QWebHitTestResult hitTest = mainFrame()->hitTestContent(currentPoint);
		QString linkTarget = hitTest.linkElement().attribute("target");
		window->d->mClickTarget = linkTarget.toStdString();

		// start off with no target specified
		int link_target_type = LLQtWebKit::LTT_TARGET_UNKNOWN;

		// user clicks on a link with a target that matches the one set as "External"
		if ( window->d->mClickTarget.empty() )
		{
			link_target_type = LLQtWebKit::LTT_TARGET_NONE;
		}
		else
		if ( window->d->mClickTarget == window->d->mExternalTargetName )
		{
			link_target_type = LLQtWebKit::LTT_TARGET_EXTERNAL;
		}
		else
		// user clicks on a link with a target that matches the one set as "Blank"
		if ( window->d->mClickTarget == window->d->mBlankTargetName )
		{
			link_target_type = LLQtWebKit::LTT_TARGET_BLANK;
		}
		else
		{
			// default action for a target we haven't specified is to open in current window
			// and fire an event as if it was a normal click
			window->navigateTo( window->d->mClickHref );

			link_target_type = LLQtWebKit::LTT_TARGET_OTHER;
		};

		// build event based on the data we have and emit it
		LLEmbeddedBrowserWindowEvent event( window->getWindowId(),
											window->getCurrentUri(),
											window->d->mClickHref,
											window->d->mClickTarget,
											link_target_type );

		window->d->mEventEmitter.update( &LLEmbeddedBrowserWindowObserver::onClickLinkHref, event );
	};

    return accepted;
}

void LLWebPage::loadStarted()
{
    if (!window)
        return;
    LLEmbeddedBrowserWindowEvent event(window->getWindowId(), window->getCurrentUri());
    window->d->mEventEmitter.update(&LLEmbeddedBrowserWindowObserver::onNavigateBegin, event);
}

void LLWebPage::loadFinished(bool)
{
    if (!window)
        return;
    LLEmbeddedBrowserWindowEvent event(window->getWindowId(),
            window->getCurrentUri());
    window->d->mEventEmitter.update(&LLEmbeddedBrowserWindowObserver::onNavigateComplete, event);
}

QString LLWebPage::chooseFile(QWebFrame* parentFrame, const QString& suggestedFile)
{
    Q_UNUSED(parentFrame);
    Q_UNUSED(suggestedFile);
    qWarning() << "LLWebPage::" << __FUNCTION__ << "not implemented" << "Returning empty string";
    return QString();
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

