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

#include "llwebpageopenshim.h"

#include <qnetworkrequest.h>
#include <qwebframe.h>
#include <qdebug.h>

#include "llqtwebkit.h"
#include "llembeddedbrowserwindow.h"
#include "llembeddedbrowserwindow_p.h"

LLWebPageOpenShim::LLWebPageOpenShim(QObject *parent)
    : QWebPage(parent)
    , window(0)
{
//	qDebug() << "LLWebPageOpenShim created";
}

LLWebPageOpenShim::~LLWebPageOpenShim()
{
//	qDebug() << "LLWebPageOpenShim destroyed";
}

bool LLWebPageOpenShim::acceptNavigationRequest(QWebFrame* frame, const QNetworkRequest& request, NavigationType type)
{
    if (!window)
	{
        return false;
	}

//	qDebug() << "LLWebPageOpenShim::acceptNavigationRequest called, NavigationType is " << type 
//		<< ", web frame is " << frame 
//		<< ", frame->page is " << frame->page()
//		<< ", url is " << request.url() 
//		<< ", target is " << QString::fromStdString(window->getClickLinkTarget())
//		;
	
    if (request.url().scheme() == QString("file"))
	{
		// For some reason, I'm seeing a spurious call to this function with a file:/// URL that points to the current working directory.
		// Ignoring file:/// URLs here isn't a perfect solution (since it could potentially break content in local HTML files),
		// but it's the best I could come up with for now.
		
		return false;
	}

	// Turn the request into an event that makes it look like the user clicked on an "_external" link.
	std::string click_href = QString(request.url().toEncoded()).toStdString();
	std::string click_target = window->getClickLinkTarget();
	int link_target_type = window->targetToTargetType(click_target);

	// build event based on the data we have and emit it
	LLEmbeddedBrowserWindowEvent event( window->getWindowId(),
										window->getCurrentUri(),	// Should this be the empty string instead?
										click_href,
										click_target,
										link_target_type );

	window->d->mEventEmitter.update( &LLEmbeddedBrowserWindowObserver::onClickLinkHref, event );

    return false;
}
