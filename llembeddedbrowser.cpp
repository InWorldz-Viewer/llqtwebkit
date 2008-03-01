/**
 * @file llembeddedbrowser.cpp
 * @brief LLEmbeddedBrowser implementation.
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

// Windows specific switches
#ifdef WIN32
	// appears to be required by LibXUL/Mozilla code to avoid crashes in debug versions of their code (undef'd at end of this file)
	#ifdef _DEBUG
		#define DEBUG 1
	#endif
#endif

#include "llembeddedbrowser.h"
#include "llembeddedbrowserwindow.h"

#ifdef WIN32
	#pragma warning( disable : 4265 ) // "class has virtual functions, but destructor is not virtual"
	#pragma warning( disable : 4291 ) // (no matching operator delete found; memory will not be freed if initialization throws an exception)
#endif	// WIN32

#include "nsBuildID.h"
#include "nsNetCID.h"
#include "nsCWebBrowser.h"
#include "nsGUIEvent.h"
#include "nsICacheService.h"
#include "nsICookieManager.h"
#include "nsICaret.h"
#include "nsIComponentRegistrar.h"
#include "nsIContent.h"
#include "nsIDOMDocument.h"
#include "nsIDOMElement.h"
#include "nsIDOMWindow.h"
#include "nsIDocShell.h"
#include "nsIDocShellTreeItem.h"
#include "nsIDocument.h"
#include "nsIFactory.h"
#include "nsIFrame.h"
#include "nsIInterfaceRequestorUtils.h"
#include "nsIPref.h"
#include "nsIPromptService.h"
#include "nsIScrollableView.h"
#include "nsISelection.h"
#include "nsISelectionController.h"
#include "nsIWebBrowserChrome.h"
#include "nsIWebBrowserChromeFocus.h"
#include "nsIWebBrowserFocus.h"
#include "nsPresContext.h"
#include "nsProfileDirServiceProvider.h"
#include "nsXPCOMGlue.h"
#include "nsXULAppAPI.h"
#include "nsWidgetsCID.h" // for NS_APPSHELL_CID
#include "nsIAppShell.h"
static NS_DEFINE_CID(kAppShellCID, NS_APPSHELL_CID);

// singleton pattern - initialization
LLEmbeddedBrowser* LLEmbeddedBrowser::sInstance = 0;

////////////////////////////////////////////////////////////////////////////////
//
LLEmbeddedBrowser::LLEmbeddedBrowser() :
	mErrorNum( 0 )
{
}

////////////////////////////////////////////////////////////////////////////////
//
LLEmbeddedBrowser::~LLEmbeddedBrowser()
{
}

////////////////////////////////////////////////////////////////////////////////
//
LLEmbeddedBrowser* LLEmbeddedBrowser::getInstance()
{
    if ( ! sInstance )
    {
        sInstance = new LLEmbeddedBrowser;
    };

	return sInstance;
};

////////////////////////////////////////////////////////////////////////////////
//
void LLEmbeddedBrowser::setLastError( int errorNumIn )
{
	mErrorNum = errorNumIn;
}

////////////////////////////////////////////////////////////////////////////////
//
void LLEmbeddedBrowser::clearLastError()
{
	mErrorNum = 0x0000;
}

////////////////////////////////////////////////////////////////////////////////
//
int LLEmbeddedBrowser::getLastError()
{
	return mErrorNum;
}

////////////////////////////////////////////////////////////////////////////////
//
static nsIAppShell *sAppShell = nsnull;

bool LLEmbeddedBrowser::init( std::string applicationDir, std::string componentDir, std::string profileDir )
{
	nsCOMPtr< nsILocalFile > applicationDirNative;
	nsresult result = NS_NewNativeLocalFile( nsCString( applicationDir.c_str() ), PR_FALSE, getter_AddRefs( applicationDirNative ) );
	if ( NS_FAILED( result ) )
	{
		setLastError( 0x1000 );
		return false;
	};

	nsCOMPtr< nsILocalFile > componentDirNative;
	result = NS_NewNativeLocalFile( nsCString( componentDir.c_str() ), PR_FALSE, getter_AddRefs( componentDirNative ) );
	if ( NS_FAILED( result ) )
	{
		setLastError( 0x1001 );
		return false;
	};

	result = XRE_InitEmbedding( componentDirNative, applicationDirNative, nsnull, nsnull, 0 );
	if ( NS_FAILED( result ) )
	{
		setLastError( 0x1002 );
		return false;
	};

	nsCOMPtr< nsILocalFile > profileDirNative;
	result = NS_NewNativeLocalFile( nsCString( profileDir.c_str() ), PR_TRUE, getter_AddRefs( profileDirNative ) );
	if ( NS_FAILED( result ) )
	{
		setLastError( 0x1007 );
		return false;
	};
	nsCOMPtr< nsProfileDirServiceProvider > locProvider;
	NS_NewProfileDirServiceProvider( PR_TRUE, getter_AddRefs( locProvider ) );
	if ( ! locProvider )
	{
		setLastError( 0x1003 );
		XRE_TermEmbedding();
		return PR_FALSE;
	};

	result = locProvider->Register();
	if ( NS_FAILED( result ) )
	{
		setLastError( 0x1004 );
		XRE_TermEmbedding();
		return PR_FALSE;
	};

	result = locProvider->SetProfileDir( profileDirNative );
	if ( NS_FAILED( result ) )
	{
		setLastError( 0x1005 );
		XRE_TermEmbedding();
		return PR_FALSE;
	};

	nsCOMPtr<nsIPref> pref = do_CreateInstance( NS_PREF_CONTRACTID );
	if ( pref )
	{
		//pref->SetBoolPref( "viewmanager.do_doublebuffering", PR_FALSE );
		pref->SetBoolPref( "security.warn_entering_secure", PR_FALSE );
		pref->SetBoolPref( "security.warn_entering_weak", PR_FALSE );
		pref->SetBoolPref( "security.warn_leaving_secure", PR_FALSE );
		pref->SetBoolPref( "security.warn_submit_insecure", PR_FALSE );
		pref->SetBoolPref( "network.protocol-handler.warn-external-default", PR_FALSE );
	}
	else
	{
		setLastError( 0x1006 );
	};

	// disable proxy by default
	enableProxy( false, "", 0 );

#ifdef LL_LINUX
	// Essential on Linux/GTK to add the gecko pump to the GTK event
	// loop.  Might be harmless/good on other platforms too.
	nsCOMPtr<nsIAppShell> appShell;
	appShell = do_CreateInstance(kAppShellCID);
	if (!appShell)
	{
		setLastError( 0x1008 );
		return PR_FALSE;
	}
	sAppShell = appShell.get();
	NS_ADDREF(sAppShell);
	sAppShell->Create(0, nsnull);
	sAppShell->Spinup();
#endif // LL_LINUX

	clearLastError();

	return true;
}

////////////////////////////////////////////////////////////////////////////////
//
bool LLEmbeddedBrowser::reset()
{
	if (sAppShell) {
		// Shutdown the appshell service.
		sAppShell->Spindown();
		NS_RELEASE(sAppShell);
		sAppShell = NULL;
	}

	XRE_TermEmbedding();

	return true;
}

////////////////////////////////////////////////////////////////////////////////
//
std::string LLEmbeddedBrowser::getGREVersion()
{
	// take the string directly from Mozilla
	return std::string( GRE_BUILD_ID );
}

////////////////////////////////////////////////////////////////////////////////
//
void LLEmbeddedBrowser::setBrowserAgentId( std::string idIn )
{
	nsCOMPtr<nsIPref> pref = do_CreateInstance( NS_PREF_CONTRACTID );
	if ( pref )
	{
		pref->SetCharPref( "general.useragent.extra.* ", idIn.c_str() );
	};
}

////////////////////////////////////////////////////////////////////////////////
//
LLEmbeddedBrowserWindow* createWindow(  void* nativeWindowHandleIn, int widthIn, int heightIn, PRUint32 aChromeFlags, nsIWebBrowserChrome* aParent, nsIWebBrowserChrome** aNewWindow )
{
	LLEmbeddedBrowserWindow* chrome = new LLEmbeddedBrowserWindow();
	if ( ! chrome )
	{
		return 0;
	};

	CallQueryInterface( NS_STATIC_CAST( nsIWebBrowserChrome*, chrome ), aNewWindow );

	NS_ADDREF( *aNewWindow );

	chrome->SetChromeFlags( aChromeFlags );

	nsCOMPtr< nsIWebBrowser > newBrowser;

	chrome->createBrowser( nativeWindowHandleIn, widthIn, heightIn, getter_AddRefs( newBrowser ) );
	if ( ! newBrowser )
	{
		return 0;
	};

	return chrome;
}

////////////////////////////////////////////////////////////////////////////////
//
LLEmbeddedBrowserWindow* LLEmbeddedBrowser::createBrowserWindow( void* nativeWindowHandleIn, int browserWidthIn, int browserHeightIn )
{
    nsCOMPtr< nsIWebBrowserChrome > chrome;
    LLEmbeddedBrowserWindow* newWin = createWindow( nativeWindowHandleIn, browserWidthIn, browserHeightIn, nsIWebBrowserChrome::CHROME_ALL, nsnull, getter_AddRefs( chrome ) );
    if ( newWin && chrome )
    {
        nsCOMPtr< nsIWebBrowser > newBrowser;
        chrome->GetWebBrowser( getter_AddRefs( newBrowser ) );
        nsCOMPtr< nsIWebNavigation > webNav( do_QueryInterface ( newBrowser ) );
	webNav->LoadURI( NS_ConvertUTF8toUTF16( "about:blank" ).get(), nsIWebNavigation::LOAD_FLAGS_NONE, nsnull, nsnull, nsnull );


		clearLastError();

		return newWin;
    };

	setLastError( 0x2001 );

	return 0;
};

////////////////////////////////////////////////////////////////////////////////
//
bool LLEmbeddedBrowser::destroyBrowserWindow( LLEmbeddedBrowserWindow* browserWindowIn )
{
	nsCOMPtr< nsIWebBrowser > webBrowser;
	nsCOMPtr< nsIWebNavigation > webNavigation;

	browserWindowIn->GetWebBrowser( getter_AddRefs( webBrowser ) );
	webNavigation = do_QueryInterface( webBrowser );
	if ( webNavigation )
	{
		webNavigation->Stop( nsIWebNavigation::STOP_ALL );
	};

	nsCOMPtr< nsIWebBrowser > browser = nsnull;
	browserWindowIn->GetWebBrowser( getter_AddRefs( browser ) );
	nsCOMPtr< nsIBaseWindow > browserAsWin = do_QueryInterface( browser );
	if ( browserAsWin )
	{
		browserAsWin->Destroy();
	};

	browserWindowIn->SetWebBrowser( nsnull );

	NS_RELEASE( browserWindowIn );

	delete browserWindowIn;

	clearLastError();

	return true;
}

////////////////////////////////////////////////////////////////////////////////
//
bool LLEmbeddedBrowser::clearCache()
{
	nsCOMPtr< nsICacheService > cacheService = do_GetService( NS_CACHESERVICE_CONTRACTID );
	if (! cacheService)
		return false;

	cacheService->EvictEntries( nsICache::STORE_ANYWHERE );

	return true;
}

///////////////////////////////////////////////////////////////////////////////
//
bool LLEmbeddedBrowser::enableProxy( bool proxyEnabledIn, std::string proxyHostNameIn, int proxyPortIn )
{
	nsCOMPtr<nsIPref> pref = do_CreateInstance( NS_PREF_CONTRACTID );
	if ( pref )
	{
		if ( proxyEnabledIn )
			pref->SetIntPref( "network.proxy.type", 1 );
		else
			pref->SetIntPref( "network.proxy.type", 0 );

		pref->SetCharPref( "network.proxy.ssl", proxyHostNameIn.c_str() );
		pref->SetIntPref( "network.proxy.ssl_port", proxyPortIn );

		pref->SetCharPref( "network.proxy.ftp", proxyHostNameIn.c_str() );
		pref->SetIntPref( "network.proxy.ftp_port", proxyPortIn );

		pref->SetCharPref( "network.proxy.gopher", proxyHostNameIn.c_str() );
		pref->SetIntPref( "network.proxy.gopher_port", proxyPortIn );

		pref->SetCharPref( "network.proxy.http", proxyHostNameIn.c_str() );
		pref->SetIntPref( "network.proxy.http_port", proxyPortIn );

		pref->SetBoolPref( "network.proxy.share_proxy_settings", true );

		return true;
	};

	return false;
}

////////////////////////////////////////////////////////////////////////////////
//
bool LLEmbeddedBrowser::enableCookies( bool enabledIn )
{
	nsCOMPtr<nsIPref> pref = do_CreateInstance( NS_PREF_CONTRACTID );
	if ( pref )
	{
		if ( enabledIn )
			pref->SetIntPref( "network.cookie.cookieBehavior", 0 );
		else
			pref->SetIntPref( "network.cookie.cookieBehavior", 2 );

		return true;
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////////
//
bool LLEmbeddedBrowser::clearAllCookies()
{
	nsCOMPtr< nsICookieManager > cookieManager = do_GetService( NS_COOKIEMANAGER_CONTRACTID );
	if ( ! cookieManager )
		return false;

	cookieManager->RemoveAll();

	return true;
}

////////////////////////////////////////////////////////////////////////////////
//
bool LLEmbeddedBrowser::enablePlugins( bool enabledIn )
{
	nsCOMPtr<nsIPref> pref = do_CreateInstance( NS_PREF_CONTRACTID );
	if ( pref )
	{
		if ( enabledIn )
		{
			pref->SetBoolPref( "plugin.scan.plid.all", PR_TRUE );
			pref->SetBoolPref( "xpinstall-enabled", PR_TRUE );
		}
		else
		{
			pref->SetBoolPref( "plugin.scan.plid.all", PR_FALSE );
			pref->SetBoolPref( "xpinstall-enabled", PR_FALSE );
		};

		return true;
	};

	return false;
}

// Windows specific switches
#ifdef WIN32

	#pragma warning( 3 : 4265 ) // "class has virtual functions, but destructor is not virtual"
	#pragma warning( 3 : 4291 ) // (no matching operator delete found; memory will not be freed if initialization throws an exception)

	// #define required by this file for LibXUL/Mozilla code to avoid crashes in their debug code
	#ifdef _DEBUG
		#undef DEBUG
	#endif

#endif	// WIN32
