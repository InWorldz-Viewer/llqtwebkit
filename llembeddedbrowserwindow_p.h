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

#ifndef LLEMBEDDEDBROWSERWINDOW_P_H
#define LLEMBEDDEDBROWSERWINDOW_P_H

#include "llwebpage.h"

#include <qgraphicsscene.h>
#include <qgraphicsview.h>
#include <qwebview.h>

///////////////////////////////////////////////////////////////////////////////
// manages the process of storing and emitting events that the consumer
// of the embedding class can observe
template< class T >
class LLEmbeddedBrowserWindowEmitter
{
    public:
        LLEmbeddedBrowserWindowEmitter() { };
        ~LLEmbeddedBrowserWindowEmitter() { };

        typedef typename T::EventType EventType;
        typedef std::list< T* > ObserverContainer;
        typedef void(T::*observerMethod)(const EventType&);

        ///////////////////////////////////////////////////////////////////////////////
        //
        bool addObserver(T* observer)
        {
            if (! observer)
                return false;

            if (std::find(observers.begin(), observers.end(), observer) != observers.end())
                return false;

            observers.push_back(observer);

            return true;
        }

        ///////////////////////////////////////////////////////////////////////////////
        //
        bool remObserver(T* observer)
        {
            if (! observer)
                return false;

            observers.remove(observer);

            return true;
        }

        ///////////////////////////////////////////////////////////////////////////////
        //
        void update(observerMethod method, const EventType& msg)
        {
            typename std::list< T* >::iterator iter = observers.begin();

            while(iter != observers.end())
            {
                ((*iter)->*method)(msg);
                ++iter;
            }
        }

        int getObserverNumber()
        {
            return observers.size();
        }

    protected:
        ObserverContainer observers;
};

#include "llqtwebkit.h"
#include "llembeddedbrowserwindow.h"
#include <qgraphicssceneevent.h>

class LLGraphicsScene : public QGraphicsScene
{
    Q_OBJECT

public:
    LLGraphicsScene();
    LLEmbeddedBrowserWindow *window;

    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent) {
        QGraphicsScene::mouseMoveEvent(mouseEvent);
        mouseEvent->setAccepted(true);
        mouseEvent->setButtons(Qt::LeftButton);
    }

private slots:
    void repaintRequestedSlot(const QList<QRectF> &);
    friend class LLEmbeddedBrowserWindow;
};


class LLWebView : public QWebView
{
    Q_OBJECT

public:
    LLWebView(QWidget *parent = 0);
    LLEmbeddedBrowserWindow *window;

    static QUrl guessUrlFromString(const QString &string);

protected:
    bool event(QEvent *event);

    Qt::CursorShape currentShape;
};

class LLEmbeddedBrowserWindowPrivate
{
    public:
    LLEmbeddedBrowserWindowPrivate()
        : mParent(0)
        , mPage(new LLWebPage)
        , mView(0)
        , mGraphicsScene(0)
        , mGraphicsView(0)
        , mCurrentMouseButtonState(Qt::NoButton)
        , mPercentComplete(0)
        , mStatusText("")
        , mTitle("")
        , mCurrentUri("")
        , mClickHref("")
        , mClickTarget("")
        , mNoFollowScheme("secondlife")
        , mWindowId(-1)
        , m404RedirectUrl("")
        , mEnabled(true)
        , mFlipBitmap(false)
        , mPageBuffer(NULL)
    {
    }

    ~LLEmbeddedBrowserWindowPrivate()
    {
        mGraphicsScene->window = 0;
        mPage->window = 0;
        mView->deleteLater();
        mGraphicsScene->deleteLater();
        mGraphicsView->viewport()->setParent(mGraphicsView);
        mGraphicsView->deleteLater();
    }

    LLEmbeddedBrowserWindowEmitter< LLEmbeddedBrowserWindowObserver > mEventEmitter;
    QImage mImage;
    LLEmbeddedBrowser *mParent;
    LLWebPage *mPage;

    LLWebView *mView;
    LLGraphicsScene *mGraphicsScene;
    QGraphicsView *mGraphicsView;
    Qt::MouseButtons mCurrentMouseButtonState;

    int16_t mPercentComplete;
    std::string mStatusText;
    std::string mTitle;
    std::string mCurrentUri;
    std::string mClickHref;
    std::string mClickTarget;
    QString mNoFollowScheme;
    int mWindowId;
    std::string m404RedirectUrl;
    bool mEnabled;
    bool mFlipBitmap;
    unsigned char* mPageBuffer;
    QColor backgroundColor;
};


#endif

