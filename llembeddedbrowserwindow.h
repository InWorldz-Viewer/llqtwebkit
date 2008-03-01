/**
 * @file llembeddedbrowserwindow.h
 * @brief LLEmbeddedBrowserWindow and associated helpers declaration.
 *
 * $LicenseInfo:firstyear=2006&license=viewergpl$
 *
 * Copyright (c) 2006-2007, Linden Research, Inc.
 *
 * Second Life Viewer Source Code
 * The source code in this file ("Source Code") is provided by Linden Lab
 * to you under the terms of the GNU General Public License, version 2.0
 * ("GPL"), unless you have obtained a separate licensing agreement
 * ("Other License"), formally executed by you and Linden Lab.  Terms of
 * the GPL can be found in doc/GPL-license.txt in this distribution, or
 * online at http://secondlife.com/developers/opensource/gplv2
 *
 * There are special exceptions to the terms and conditions of the GPL as
 * it is applied to this Source Code. View the full text of the exception
 * in the file doc/FLOSS-exception.txt in this software distribution, or
 * online at http://secondlife.com/developers/opensource/flossexception
 *
 * By copying, modifying or distributing this software, you acknowledge
 * that you have read and understood your obligations described above,
 * and agree to abide by those obligations.
 *
 * ALL LINDEN LAB SOURCE CODE IS PROVIDED "AS IS." LINDEN LAB MAKES NO
 * WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY,
 * COMPLETENESS OR PERFORMANCE.
 * $/LicenseInfo$
 */

#ifndef LLEMBEDDEDBROWSERWINDOW_H
#define LLEMBEDDEDBROWSERWINDOW_H

// Mozilla code has non-virtual destructors
#ifdef WIN32
#pragma warning( disable : 4265 ) // "class has virtual functions, but destructor is not virtual"
#endif

#include "nsIBaseWindow.h"
#include "nsIDOMEventListener.h"
#include "nsIDOMEventTarget.h"
#include "nsIInterfaceRequestor.h"
#include "nsIWebBrowserChrome.h"
#include "nsIWebNavigation.h"
#include "nsIWebProgressListener.h"
#include "nsIURIContentListener.h"
#include "nsWeakReference.h"
#include "nsIWebBrowser.h"

#include <string>
#include <list>
#include <algorithm>

#include "llmozlib.h"

///////////////////////////////////////////////////////////////////////////////
// manages the process of sotring and emitting events that the consumer
// of the embedding class can observe
template< class T >
class LLEmbeddedBrowserWindowEmitter
{
	public:
		LLEmbeddedBrowserWindowEmitter() { };
		~LLEmbeddedBrowserWindowEmitter() { };

		typedef typename T::EventType EventType;
		typedef std::list< T* > ObserverContainer;
		typedef void( T::*observerMethod )( const EventType& );

		///////////////////////////////////////////////////////////////////////////////
		//
		bool addObserver( T* observerIn )
		{
			if ( ! observerIn )
				return false;

			if ( std::find( observers.begin(), observers.end(), observerIn ) != observers.end() )
				return false;

			observers.push_back( observerIn );

			return true;
		};

		///////////////////////////////////////////////////////////////////////////////
		//
		bool remObserver( T* observerIn )
		{
			if ( ! observerIn )
				return false;

			observers.remove( observerIn );

			return true;
		};

		///////////////////////////////////////////////////////////////////////////////
		//
		void update( observerMethod method, const EventType& msgIn )
		{
			typename std::list< T* >::iterator iter = observers.begin();

			while( iter != observers.end() )
			{
				( ( *iter )->*method )( msgIn );

				++iter;
			};
		};

	protected:
		ObserverContainer observers;
};

class LLEmbeddedBrowser;

////////////////////////////////////////////////////////////////////////////////
// class for a "window" that holds a browser - there can be lots of these
class LLEmbeddedBrowserWindow :
	public nsIInterfaceRequestor,
	public nsIWebBrowserChrome,
	public nsIWebProgressListener,
	public nsIURIContentListener,
	public nsSupportsWeakReference,
	public nsIDOMEventListener
{
	public:
		LLEmbeddedBrowserWindow();
		virtual ~LLEmbeddedBrowserWindow();

		NS_DECL_ISUPPORTS
		NS_DECL_NSIINTERFACEREQUESTOR
		NS_DECL_NSIWEBBROWSERCHROME
		NS_DECL_NSIWEBPROGRESSLISTENER
		NS_DECL_NSIURICONTENTLISTENER
		NS_DECL_NSIDOMEVENTLISTENER

		nsresult createBrowser( void* nativeWindowHandleIn, PRInt32 widthIn, PRInt32 heightIn, nsIWebBrowser** aBrowser );
		PRBool setSize( PRInt16 widthIn, PRInt16 heightIn );
		void focusBrowser( PRBool focusBrowserIn );
		void scrollByLines( PRInt16 linesIn );
		void setWindowId( int windowIdIn );
		int getWindowId();

		const PRInt16 getPercentComplete();
		const std::string& getStatusMsg();
		const std::string& getCurrentUri();
		const std::string& getClickLinkHref();

		unsigned char* grabWindow();
		unsigned char* getPageBuffer();
		PRInt16 getBrowserWidth();
		PRInt16 getBrowserHeight();
		PRInt16 getBrowserDepth();
		PRInt32 getBrowserRowSpan();

		void setBackgroundColor( PRUint8 redIn, PRUint8 greenIn, PRUint8 blueIn );
		void setCaretColor( const PRUint8 redIn, const PRUint8 greenIn, const PRUint8 blueIn );

		void navigateStop();
		PRBool navigateTo( const std::string uriIn );
		PRBool canNavigateBack();
		void navigateBack();
		PRBool canNavigateForward();
		void navigateForward();

		bool set404RedirectUrl( std::string redirect_url );
		bool clr404RedirectUrl();

		void mouseDown( PRInt16 xPosIn, PRInt16 yPosIn );
		void mouseUp( PRInt16 xPosIn, PRInt16 yPosIn );
		void mouseMove( PRInt16 xPosIn, PRInt16 yPosIn );
		void mouseLeftDoubleClick( PRInt16 xPosIn, PRInt16 yPosIn );
		void keyPress( PRInt16 keyCode );
		void unicodeInput( PRUint32 uni_char );

		bool addObserver( LLEmbeddedBrowserWindowObserver* observerIn );
		bool remObserver( LLEmbeddedBrowserWindowObserver* observerIn );

	private:
		PRBool sendMozillaMouseEvent( PRInt16 eventIn, PRInt16 xPosIn, PRInt16 yPosIn, PRUint32 clickCountIn );
		PRBool sendMozillaKeyboardEvent( PRUint32 keyIn, PRUint32 ns_vk_code );
		PRBool renderCaret();
		LLEmbeddedBrowserWindowEmitter< LLEmbeddedBrowserWindowObserver > mEventEmitter;
		PRInt16 mPercentComplete;
		std::string mStatusText;
		std::string mCurrentUri;
		std::string mClickHref;
		std::string m404RedirectUrl;

		nsCOMPtr< nsIWebBrowser > mWebBrowser;
		nsCOMPtr< nsIBaseWindow > mBaseWindow;
		nsCOMPtr< nsIWebNavigation > mWebNav;

		int mWindowId;

		PRUint8 mBackgroundRed;
		PRUint8 mBackgroundGreen;
		PRUint8 mBackgroundBlue;
		PRUint8 mCaretRed;
		PRUint8 mCaretGreen;
		PRUint8 mCaretBlue;

		unsigned char* mPageBuffer;
		size_t mPageBufferSize;
		PRInt32 mBrowserRowSpan;
		PRInt16 mBrowserWidth;
		PRInt16 mBrowserHeight;
		PRInt16 mBrowserDepth;
};

#ifdef WIN32
#pragma warning( 3 : 4265 ) // "class has virtual functions, but destructor is not virtual"
#endif

#endif // LLEMBEDEDDBROWSERWINDOW_H
