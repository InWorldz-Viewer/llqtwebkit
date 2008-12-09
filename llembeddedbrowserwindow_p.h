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

#ifndef LLEMBEDDEDBROWSERWINDOW_P_H
#define LLEMBEDDEDBROWSERWINDOW_P_H

#include "llwebpage.h"

#include <QtGui/QtGui>
#include <QtNetwork/QtNetwork>
#include <QtWebKit/QtWebKit>

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
        bool addObserver(T* observerIn)
        {
            if (! observerIn)
                return false;

            if (std::find(observers.begin(), observers.end(), observerIn) != observers.end())
                return false;

            observers.push_back(observerIn);

            return true;
        };

        ///////////////////////////////////////////////////////////////////////////////
        //
        bool remObserver(T* observerIn)
        {
            if (! observerIn)
                return false;

            observers.remove(observerIn);

            return true;
        };

        ///////////////////////////////////////////////////////////////////////////////
        //
        void update(observerMethod method, const EventType& msgIn)
        {
            typename std::list< T* >::iterator iter = observers.begin();

            while(iter != observers.end())
            {
                ((*iter)->*method)(msgIn);

                ++iter;
            };
        };

    protected:
        ObserverContainer observers;
};

class LLEmbeddedBrowserWindowPrivate
{
    public:
    LLEmbeddedBrowserWindowPrivate()
        : mParent(0)
        , mPage(new LLWebPage)
        , mPercentComplete(0)
        , mStatusText("")
        , mCurrentUri("")
        //, mClickHref("")
        //, mClickTarget("")
        , mNoFollowScheme("secondlife://")
        , mWindowId(0)
        , m404RedirectUrl("")
        , mEnabled(true)
        , mFlipBitmap(false)
        , mPageBuffer(NULL)
    {
    }

    ~LLEmbeddedBrowserWindowPrivate()
    {
        delete mPage;
    }

    QString userAgent() const;

    LLEmbeddedBrowserWindowEmitter< LLEmbeddedBrowserWindowObserver > mEventEmitter;
    LLEmbeddedBrowser *mParent;
    LLWebPage *mPage;
    QImage mImage;


    qint16 mPercentComplete;
    std::string mStatusText;
    std::string mCurrentUri;
    //std::string mClickHref;
    //std::string mClickTarget;
    std::string mNoFollowScheme;
    int mWindowId;
    std::string m404RedirectUrl;
    bool mEnabled;
    bool mFlipBitmap;
    unsigned char* mPageBuffer;
};

#endif

