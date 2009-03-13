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

#include <QtGui/QtGui>
#include <llmozlib2.h>

class WebPage : public QWidget, LLEmbeddedBrowserWindowObserver
{
    Q_OBJECT

signals:
    void locationChanged(const QString &);
    void canGoBack(bool);
    void canGoForward(bool);

public:
    WebPage(QWidget *parent = 0);
    ~WebPage();

    void onPageChanged(const EventType& event);
    void onNavigateBegin(const EventType& event);
    void onNavigateComplete(const EventType& event);
    void onUpdateProgress(const EventType& event);
    void onStatusTextChange(const EventType& event);
    void onLocationChange(const EventType& event);
    void onClickLinkHref(const EventType& event);
    void onClickLinkNoFollow(const EventType& event);

public slots:
    void goBack();
    void goForward();
    void reload();
    void loadUrl(const QString &);

protected:
    void resizeEvent(QResizeEvent *event);
    void paintEvent(QPaintEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
private:
    int mBrowserWindowId;
};

WebPage::WebPage(QWidget *parent)
    : QWidget(parent)
{
    std::string applicationDir = std::string();
    std::string componentDir = applicationDir;
    std::string profileDir = applicationDir + "\\" + "testGL_profile";
    LLMozLib::getInstance()->init(applicationDir, componentDir, profileDir, 0);

    mBrowserWindowId = LLMozLib::getInstance()->createBrowserWindow(width(), height());

    // observer events that LLMozLib emits
    LLMozLib::getInstance()->addObserver( mBrowserWindowId, this );

    // append details to agent string
    LLMozLib::getInstance()->setBrowserAgentId("testqtapp");

    // don't flip bitmap
    LLMozLib::getInstance()->flipWindow( mBrowserWindowId, true );

    // go to the "home page"
    LLMozLib::getInstance()->navigateTo( mBrowserWindowId, "http://www.google.com" );
}

WebPage::~WebPage()
{
    // unhook observer
    LLMozLib::getInstance()->remObserver( mBrowserWindowId, this );

    // clean up
    LLMozLib::getInstance()->reset();
}

void WebPage::onPageChanged(const EventType& event)
{
    LLMozLib::getInstance()->grabBrowserWindow( mBrowserWindowId );
    qDebug() << __FUNCTION__ << QString::fromStdString(event.getEventUri());
    update();
}

void WebPage::onNavigateBegin(const EventType& event)
{
    qDebug() << __FUNCTION__ << QString::fromStdString(event.getEventUri());
}

void WebPage::onNavigateComplete(const EventType& event)
{
    qDebug() << __FUNCTION__ << QString::fromStdString(event.getEventUri());
}

void WebPage::onUpdateProgress(const EventType& event)
{
    qDebug() << __FUNCTION__ << event.getIntValue();
}

void WebPage::onStatusTextChange(const EventType& event)
{
    qDebug() << __FUNCTION__;
}

void WebPage::onLocationChange(const EventType& event)
{
    qDebug() << __FUNCTION__;
    emit locationChanged(QString::fromStdString(event.getEventUri()));
    //void canGoBack(bool);
    //void canGoForward(bool);
}

void WebPage::onClickLinkHref(const EventType& event)
{
    qDebug() << __FUNCTION__;
}

void WebPage::onClickLinkNoFollow(const EventType& event)
{
    qDebug() << __FUNCTION__;
}


void WebPage::resizeEvent(QResizeEvent *event)
{
    LLMozLib::getInstance()->setSize(mBrowserWindowId, event->size().width(), event->size().height());
    QWidget::resizeEvent(event);
}

void WebPage::paintEvent(QPaintEvent *event)
{
    int width = LLMozLib::getInstance()->getBrowserWidth(mBrowserWindowId);
    int height = LLMozLib::getInstance()->getBrowserHeight(mBrowserWindowId);
    const unsigned char* pixels = LLMozLib::getInstance()->getBrowserWindowPixels(mBrowserWindowId);
    QImage image(pixels, width, height, QImage::Format_RGB32);
    qDebug() << "paint" << width << height << pixels << image.isNull();
    QPainter painter(this);
    painter.drawImage(QPoint(0, 0), image);
}

void WebPage::mouseDoubleClickEvent(QMouseEvent *event)
{
    LLMozLib::getInstance()->mouseLeftDoubleClick(mBrowserWindowId, event->x(), event->y());
}

void WebPage::mouseMoveEvent(QMouseEvent *event)
{
    LLMozLib::getInstance()->mouseMove(mBrowserWindowId, event->x(), event->y());
}

void WebPage::mousePressEvent(QMouseEvent *event)
{
    LLMozLib::getInstance()->mouseDown(mBrowserWindowId, event->x(), event->y());
}

void WebPage::mouseReleaseEvent(QMouseEvent *event)
{
    LLMozLib::getInstance()->mouseUp(mBrowserWindowId, event->x(), event->y());
    LLMozLib::getInstance()->focusBrowser(mBrowserWindowId, true);
}

void WebPage::keyPressEvent(QKeyEvent *event)
{
}

void WebPage::keyReleaseEvent(QKeyEvent *event)
{
    LLMozLib::getInstance()->unicodeInput(mBrowserWindowId, event->text().at(0).unicode());
}

void WebPage::goBack()
{
    LLMozLib::getInstance()->navigateBack(mBrowserWindowId);
}

void WebPage::goForward()
{
    LLMozLib::getInstance()->navigateForward(mBrowserWindowId);
}

void WebPage::reload()
{
    LLMozLib::getInstance()->navigateReload(mBrowserWindowId);
}

void WebPage::loadUrl(const QString &url)
{
    LLMozLib::getInstance()->navigateTo(mBrowserWindowId, url.toStdString());
}

#include "ui_window.h"

class Window : public QDialog, public Ui_Dialog
{
    Q_OBJECT
public:
    Window(QWidget *parent = 0);

public slots:
    void loadUrl();
};

Window::Window(QWidget *parent)
    : QDialog(parent)
{
    setupUi(this);
    connect(webpage, SIGNAL(locationChanged(const QString &)),
            location, SLOT(setText(const QString &)));
    connect(webpage, SIGNAL(canGoBack(bool)),
            backButton, SLOT(setEnabled(bool)));
    connect(webpage, SIGNAL(canGoForward(bool)),
            forwardButton, SLOT(setEnabled(bool)));
    connect(backButton, SIGNAL(clicked()),
            webpage, SLOT(goBack()));
    connect(forwardButton, SIGNAL(clicked()),
            webpage, SLOT(goForward()));
    connect(reloadButton, SIGNAL(clicked()),
            webpage, SLOT(reload()));
    connect(location, SIGNAL(returnPressed()),
            this, SLOT(loadUrl()));
}

void Window::loadUrl()
{
    webpage->loadUrl(location->text());
}


int main(int argc, char **argv)
{
    QApplication application(argc, argv);
    Window window;
    window.show();
    return application.exec();
}

#include "main.moc"
