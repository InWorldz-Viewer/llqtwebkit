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

#include "llnetworkaccessmanager.h"

#include <qauthenticator.h>
#include <qnetworkreply.h>
#include <qtextdocument.h>
#include <qgraphicsview.h>
#include <qgraphicsscene.h>
#include <qgraphicsproxywidget.h>
#include <qdebug.h>

#include "llembeddedbrowserwindow.h"
#include "llembeddedbrowser_p.h"

#include "ui_passworddialog.h"

LLNetworkAccessManager::LLNetworkAccessManager(LLEmbeddedBrowserPrivate* browser,QObject* parent)
    : QNetworkAccessManager(parent)
    , mBrowser(browser)
{
    connect(this, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(finishLoading(QNetworkReply*)));
    connect(this, SIGNAL(authenticationRequired(QNetworkReply*, QAuthenticator*)),
            SLOT(authenticationRequired(QNetworkReply*, QAuthenticator*)));
    connect(this, SIGNAL(sslErrors( QNetworkReply *, const QList<QSslError> &)),
            this, SLOT(sslErrorsSlot( QNetworkReply *, const QList<QSslError> &  )));
}

QNetworkReply *LLNetworkAccessManager::createRequest(Operation op, const QNetworkRequest &request,
                                         QIODevice *outgoingData)
{
	// Create a local copy of the request we can modify.
	QNetworkRequest mutable_request(request);
	
	// Set an Accept-Language header in the request, based on what the host has set through setHostLanguage.
	mutable_request.setRawHeader(QByteArray("Accept-Language"), QByteArray(mBrowser->mHostLanguage.c_str()));
	
	// and pass this through to the parent implementation
	return QNetworkAccessManager::createRequest(op, mutable_request, outgoingData);
}

void LLNetworkAccessManager::sslErrorsSlot(QNetworkReply* reply, const QList<QSslError>& errors)
{
	//qDebug() << "SSL Errors: " << errors;
	// HACK: Ignore SSL (cert errors)
	//       Temporary workaround until we work out how to use local cert file
	bool ignore_ssl_errors_hack_enabled = false;

	if ( ignore_ssl_errors_hack_enabled )
	{
		reply->ignoreSslErrors();
	};
}

void LLNetworkAccessManager::finishLoading(QNetworkReply* reply)
{
    if (reply->error() == QNetworkReply::ContentNotFoundError)
    {
        QString url = QString(reply->url().toEncoded());
        if (mBrowser)
        {
            std::string current_url = url.toStdString();
            foreach (LLEmbeddedBrowserWindow *window, mBrowser->windows)
            {
                if (window->getCurrentUri() == current_url)
                    window->load404RedirectUrl();
            }
        }
    }
}

void LLNetworkAccessManager::authenticationRequired(QNetworkReply *reply, QAuthenticator *authenticator)
{
#ifndef VANILLA_QT
    authenticator->tryAgainLater = true;
#endif
    AuthDialog authDialog;
    int i;
    for (i = 0; i < authDialogs.count(); ++i) {
        AuthDialog a = authDialogs[i];
        if (a.realm == authenticator->realm()) {
            authDialog = a;
            break;
        }
    }

    if (authDialog.realm.isEmpty()) {
        authDialog.realm = authenticator->realm();
        QGraphicsWebView *webView = mBrowser->findView(reply);
        QGraphicsScene *scene = webView->scene();
        authDialog.authenticationDialog = new QDialog;
        authDialog.passwordDialog = new Ui::PasswordDialog;
        authDialog.passwordDialog->setupUi(authDialog.authenticationDialog);
        authDialog.passwordDialog->icon->setText(QString());
        authDialog.passwordDialog->icon->setPixmap(qApp->style()->standardIcon(QStyle::SP_MessageBoxQuestion, 0, 0).pixmap(32, 32));
        authDialog.passwordDialog->userName->setFocus();

        QString message = tr("<qt>Enter username and password for \"%1\" at %2</qt>")
            .arg(Qt::escape(authenticator->realm()))
            .arg(Qt::escape(reply->url().host()));
        authDialog.passwordDialog->message->setText(message);

        authDialog.proxyWidget = scene->addWidget(authDialog.authenticationDialog);
        authDialog.proxyWidget->setWindowFlags(Qt::Window); // this makes the item a panel
        authDialog.proxyWidget->setPanelModality(QGraphicsItem::SceneModal);
        authDialog.proxyWidget->setPos((webView->boundingRect().width() - authDialog.authenticationDialog->sizeHint().width())/2,
                                       (webView->boundingRect().height() - authDialog.authenticationDialog->sizeHint().height())/2);
        authDialog.proxyWidget->setActive(true);

        authDialog.authenticationDialog->show();
        scene->setFocusItem(authDialog.proxyWidget);
        authDialogs.append(authDialog);
    } else if (authDialog.authenticationDialog->result() == QDialog::Accepted) {
        authenticator->setUser(authDialog.passwordDialog->userName->text());
        authenticator->setPassword(authDialog.passwordDialog->password->text());
        authDialog.proxyWidget->deleteLater();
        authDialog.proxyWidget = 0;
        authDialog.authenticationDialog = 0;
        authDialogs.removeAt(i);
#ifndef VANILLA_QT
        authenticator->tryAgainLater = false;
#endif
    }
}

