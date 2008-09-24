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

#include "llembeddedbrowser.h"

#include "llembeddedbrowserwindow.h"

#include <QtWebKit/QtWebKit>
#include <QtGui/QtGui>

// singleton pattern - initialization
LLEmbeddedBrowser* LLEmbeddedBrowser::sInstance = 0;

LLEmbeddedBrowser::LLEmbeddedBrowser() :
    mErrorNum( 0 ),
    mNativeWindowHandle( 0 )
{
}

LLEmbeddedBrowser::~LLEmbeddedBrowser()
{
}

LLEmbeddedBrowser* LLEmbeddedBrowser::getInstance()
{
    if (!sInstance)
        sInstance = new LLEmbeddedBrowser;
    return sInstance;
};

void LLEmbeddedBrowser::setLastError( int errorNumIn )
{
    mErrorNum = errorNumIn;
}

void LLEmbeddedBrowser::clearLastError()
{
    mErrorNum = 0x0000;
}

int LLEmbeddedBrowser::getLastError()
{
    return mErrorNum;
}

std::string LLEmbeddedBrowser::getGREVersion()
{
    // take the string directly from Qt
    return std::string(QT_VERSION_STR);
}

bool LLEmbeddedBrowser::init(std::string applicationDir,
                             std::string componentDir,
                             std::string profileDir,
                             void* nativeWindowHandleIn)
{
    qWarning() << "LLEmbeddedBrowser::init()" << "not implemented";
    QApplication *application = new QApplication(0, 0);
    /*
    mNativeWindowHandle = nativeWindowHandleIn;

    NS_ConvertUTF8toUTF16 applicationDirUTF16(applicationDir.c_str());
    NS_ConvertUTF8toUTF16 componentDirUTF16(componentDir.c_str());
    NS_ConvertUTF8toUTF16 profileDirUTF16(profileDir.c_str());

    nsCOMPtr< nsILocalFile > applicationDirNative;
    nsresult result = NS_NewLocalFile( applicationDirUTF16, PR_FALSE, getter_AddRefs( applicationDirNative ) );
    if ( NS_FAILED( result ) )
    {
        setLastError( 0x1000 );
        return false;
    };

    nsCOMPtr< nsILocalFile > componentDirNative;
    result = NS_NewLocalFile( componentDirUTF16 , PR_FALSE, getter_AddRefs( componentDirNative ) );
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
    result = NS_NewLocalFile( profileDirUTF16 , PR_TRUE, getter_AddRefs( profileDirNative ) );
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

    // Originally from Linux version but seems to help other platforms too
    nsresult rv;
    nsCOMPtr<nsIAppShell> appShell;
    NS_DEFINE_CID(kAppShellCID, NS_APPSHELL_CID);
    appShell = do_CreateInstance(kAppShellCID, &rv);
    if (!appShell)
    {
        setLastError( 0x1008 );
        return PR_FALSE;
    }
    sAppShell = appShell.get();
    NS_ADDREF(sAppShell);
    sAppShell->Create(0, nsnull);
    sAppShell->Spinup();

    clearLastError();
*/
    return true;
}

bool LLEmbeddedBrowser::reset()
{
    qWarning() << "LLEmbeddedBrowser::reset()" << "not implemented";
    //XRE_TermEmbedding();
    return true;
}

bool LLEmbeddedBrowser::clearCache()
{
    qWarning() << "LLEmbeddedBrowser::clearCache()" << "not implemented";
    return true;
}

bool LLEmbeddedBrowser::enableProxy( bool proxyEnabledIn, std::string proxyHostNameIn, int proxyPortIn )
{
    qWarning() << "LLEmbeddedBrowser::enableProxy()" << "not implemented";
    /*nsCOMPtr<nsIPref> pref = do_CreateInstance( NS_PREF_CONTRACTID );
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
*/
    return false;
}

bool LLEmbeddedBrowser::enableCookies( bool enabledIn )
{
    qWarning() << "LLEmbeddedBrowser::enableCookies()" << "not implemented";
    /*nsCOMPtr<nsIPref> pref = do_CreateInstance( NS_PREF_CONTRACTID );
    if ( pref )
    {
        if ( enabledIn )
            pref->SetIntPref( "network.cookie.cookieBehavior", 0 );
        else
            pref->SetIntPref( "network.cookie.cookieBehavior", 2 );

        return true;
    }
    */
    return false;
}

bool LLEmbeddedBrowser::clearAllCookies()
{
    qWarning() << "LLEmbeddedBrowser::clearAllCookies()" << "not implemented";
    /*nsCOMPtr< nsICookieManager > cookieManager = do_GetService( NS_COOKIEMANAGER_CONTRACTID );
    if ( ! cookieManager )
        return false;

    cookieManager->RemoveAll();
*/
    return true;
}

bool LLEmbeddedBrowser::enablePlugins( bool enabledIn )
{
    qWarning() << "LLEmbeddedBrowser::enablePlugins()" << "not implemented";
    /*
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
            pref->SetBoolPref( "plugin.scan.4xPluginFolder", PR_FALSE );
            pref->SetCharPref( "plugin.scan.Quicktime", "20.0" );
            pref->SetCharPref( "plugin.scan.Acrobat", "99.0" );
            pref->SetCharPref( "plugin.scan.SunJRE", "99.0" );
            pref->SetCharPref( "plugin.scan.WindowsMediaPlayer", "99.0" );
        };

        return true;
    };
    */
    return false;
}

void LLEmbeddedBrowser::setBrowserAgentId( std::string idIn )
{
    qWarning() << "LLEmbeddedBrowser::setBrowserAgentId()" << "not implemented";
    /*
    nsCOMPtr<nsIPref> pref = do_CreateInstance( NS_PREF_CONTRACTID );
    if ( pref )
    {
        pref->SetCharPref( "general.useragent.extra.* ", idIn.c_str() );
    };
    */
}

LLEmbeddedBrowserWindow* LLEmbeddedBrowser::createBrowserWindow(int browserWidthIn, int browserHeightIn)
{
    LLEmbeddedBrowserWindow *newWin = new LLEmbeddedBrowserWindow();
    if (newWin) {
        newWin->setParent(this);
        clearLastError();
        return newWin;
    };
    return 0;
}

bool LLEmbeddedBrowser::destroyBrowserWindow(LLEmbeddedBrowserWindow *browserWindowIn)
{
    delete browserWindowIn;
    clearLastError();
    return true;
}


