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

#include "webpage.h"
#include "llmozlib2.h"
#include "llembeddedbrowserwindow.h"
#include "llembeddedbrowserwindow_p.h"

WebPage::WebPage(QObject *parent)
    : QWebPage(parent)
{
    connect(this, SIGNAL(loadProgress(int)),
            this, SLOT(loadProgressSlot(int)));
}

void WebPage::loadProgressSlot(int progress) {
    qDebug() << QString::fromStdString(window->getCurrentUri()) << "progress" << progress;
    window->mPercentComplete = progress;
    LLEmbeddedBrowserWindowEvent event( window->getWindowId(), window->getCurrentUri(), progress);
    window->mEventEmitter.update( &LLEmbeddedBrowserWindowObserver::onUpdateProgress, event );

    int width = window->getBrowserWidth();
    int height = window->getBrowserHeight();
    LLEmbeddedBrowserWindowEvent event2( window->getWindowId(), window->getCurrentUri(), 0, 0, width, height );
    window->mEventEmitter.update( &LLEmbeddedBrowserWindowObserver::onPageChanged, event2 );
}

QString WebPage::userAgentForUrl(const QUrl &url) const
{
    QString setAgent = window->d->userAgent();
    if (!setAgent.isEmpty())
        return setAgent;
    return QWebPage::userAgentForUrl(url);
}

