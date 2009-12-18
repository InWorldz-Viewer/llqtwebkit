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
 *  Benjamin Meyer (benjamin.meyer@torchmobile.com)
 *
 * Portions created by the Initial Developer are Copyright (C) 2006
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
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
    authenticator->tryAgainLater = true;
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
        authenticator->tryAgainLater = false;
    }
}

