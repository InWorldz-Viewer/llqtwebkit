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

#ifndef LL_WINDOWS
extern "C" {
#include <unistd.h>
}
#endif

#ifdef LL_WINDOWS
#include <windows.h>
#include <direct.h>
#include <conio.h>
#define PATH_SEPARATOR "\\"
#endif

#ifdef LL_OSX
#define PATH_SEPARATOR "/"
#endif

#ifdef LL_LINUX
#define PATH_SEPARATOR "/"
#endif

#include <iostream>
#include <algorithm>
#include <cctype>
#include <fstream>
#include <iterator>
#include <sstream>
#include <vector>
#include <time.h>

#ifdef STATIC_QT
#ifndef LL_OSX
#include <QtPlugin>
Q_IMPORT_PLUGIN(qgif)
#endif
#endif

#include "llqtwebkit.h"

// true == write everything, false = write most common output
bool gNoisyOutput = false;

////////////////////////////////////////////////////////////////////////////////
//
const std::string getTimeStampString()
{
	time_t raw_time;
	time( &raw_time );
	static char ctime_buffer[ 128 ];
	ctime_s( ctime_buffer, sizeof( ctime_buffer ) / sizeof( char ), &raw_time );
	std::string time_stamp = std::string( ctime_buffer );
	time_stamp = time_stamp.substr( 0, time_stamp.length() - 1 );

	return "=== " +  time_stamp + " ===" ;
}

////////////////////////////////////////////////////////////////////////////////
//
class snapDragon :
	public LLEmbeddedBrowserWindowObserver
{
	public:
		snapDragon( int argc, char* argv[] ) :
			mIsDone( false )
		{
			// current working dir
			char tmp_path[ MAX_PATH + 1 ];
			const std::string working_dir = std::string( _getcwd( tmp_path, MAX_PATH ) );

			// default location of dirs
			mApplicationDir = working_dir + PATH_SEPARATOR;
			mProfileDir = working_dir + PATH_SEPARATOR + "sd_browser_profile" + PATH_SEPARATOR;
			mImageOutputDir = working_dir + PATH_SEPARATOR;

			// log file directory and pathname (path plus filename)
			const std::string logfile_dir = working_dir + PATH_SEPARATOR;
			const std::string logfile_pathname = logfile_dir + "output.log";

			// some things can be changed on the command line
			for( int i = 1; i < argc; ++i )
			{
				if ( *argv[ i ] == '-' )
				{
					std::string arg = std::string( argv[ i ] + 1 );

					if ( caseInsensitiveCompare( arg, "IMGDIR" ) )
					{
						if ( i < argc - 1 )
						{
							mImageOutputDir = std::string( argv[ i + 1 ] );
							std::cout << "Setting image output directory to " << mImageOutputDir << " using command line" << std::endl;
						};
					};
				};
			};

			// default values
			mBrowserWidth = 512;
			mBrowserHeight = 512;
			mJavaScriptEnabled = true;
			mPluginsEnabled = true;
			mFlipWindow = false;
			mHostLanguage = "en-us";
			mUserAgentStringAddition = "User Agent String Addition";
			mExternalTargetName = "External Window";
			mWindowOpenBehavior = LLQtWebKit::WOB_IGNORE;
			mLastNavigatedURL = "";

			// dirs we will use
			std::cout << "Current working dir is " << working_dir << std::endl;
			std::cout << "Application directory is " << mApplicationDir << std::endl;
			std::cout << "Profile directory is " << mProfileDir << std::endl;
			std::cout << "Image directory is " << mImageOutputDir << std::endl;
		};

		~snapDragon()
		{
		};

		bool init()
		{
			LLQtWebKit::getInstance()->init( mApplicationDir, mApplicationDir, mProfileDir, GetDesktopWindow() );
			
//			LLQtWebKit::getInstance()->init( applicationDir, componentDir, profileDir, getNativeWindowHandle() );

			LLQtWebKit::getInstance()->setHostLanguage( mHostLanguage );

			LLQtWebKit::getInstance()->enableJavascript( mJavaScriptEnabled );

			LLQtWebKit::getInstance()->enablePlugins( mPluginsEnabled );

			mBrowserId = LLQtWebKit::getInstance()->createBrowserWindow( mBrowserWidth, mBrowserHeight );
			LLQtWebKit::getInstance()->setSize( mBrowserId, mBrowserWidth, mBrowserHeight );

			LLQtWebKit::getInstance()->addObserver( mBrowserId, this );

			LLQtWebKit::getInstance()->setBrowserAgentId( mUserAgentStringAddition );

			LLQtWebKit::getInstance()->flipWindow( mBrowserId, mFlipWindow );

			LLQtWebKit::getInstance()->setExternalTargetName( mBrowserId, mExternalTargetName );

			LLQtWebKit::getInstance()->setWindowOpenBehavior( mBrowserId, mWindowOpenBehavior );

			dumpLLQtWebKitSettings();

			return true;
		};

		////////////////////////////////////////////////////////////////////////////////
		//
		void reset()
		{
			LLQtWebKit::getInstance()->remObserver( mBrowserId, this );

			LLQtWebKit::getInstance()->reset();
		};

		////////////////////////////////////////////////////////////////////////////////
		//
		void dumpLLQtWebKitSettings()
		{
			std::cout << "LLQtWebKit application dir is " << mApplicationDir << std::endl;
			std::cout << "LLQtWebKit profile dir is " << mProfileDir << std::endl;
			std::cout << "LLQtWebKit host language setting is " << mHostLanguage << std::endl;
			std::cout << "LLQtWebKit JavaScript enabled setting is " << mJavaScriptEnabled << std::endl;
			std::cout << "LLQtWebKit plugins enabled setting is " << mPluginsEnabled << std::endl;
			int browser_depth = LLQtWebKit::getInstance()->getBrowserDepth( mBrowserId );
			std::cout << "LLQtWebKit browser dimensions are " << mBrowserWidth << " * " << mBrowserHeight << " * " << browser_depth << std::endl;
			std::cout << "LLQtWebKit user agent string addition is " << mUserAgentStringAddition << std::endl;
			std::cout << "LLQtWebKit flip window setting is " << mFlipWindow << std::endl;
			std::cout << "LLQtWebKit external target name is " << mExternalTargetName  << std::endl;
			std::cout << "LLQtWebKit window open setting is " << mWindowOpenBehavior  << std::endl;
			std::cout << "LLQtWebKit version: " << LLQtWebKit::getInstance()->getVersion() << std::endl;
		}

		////////////////////////////////////////////////////////////////////////////////
		//
		bool update()
		{
			LLQtWebKit::getInstance()->grabBrowserWindow( mBrowserId );
			LLQtWebKit::getInstance()->pump( 100 );

			MSG msg;
			while ( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) ) 
			{
				GetMessage( &msg, NULL, 0, 0 );
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			};
			
			return true;
		};

		////////////////////////////////////////////////////////////////////////////////
		//
		void navigateTo( const std::string& url )
		{
			mLastNavigatedURL = url;

			LLQtWebKit::getInstance()->navigateTo( mBrowserId, url.c_str() );
		};

		////////////////////////////////////////////////////////////////////////////////
		//
		void waitUntilLoaded()
		{
			while ( ! mIsDone )
			{
				update();
			};
		};

		////////////////////////////////////////////////////////////////////////////////
		//
		void pause( int seconds )
		{
			time_t start_time = time( 0 );
			
			while ( time( 0 ) < start_time + (time_t)seconds )
			{
				update();
			};
		};

		////////////////////////////////////////////////////////////////////////////////
		//
		std::string urlToFilename( const std::string& url )
		{
			std::string filename( "img-" );

			for( size_t i = 0; i < url.length(); ++i )
			{
				unsigned char c = url[ i ];
				if ( isalnum( c )  )
					filename += c;
				else
					filename += '_';
			};

			filename += ".tga";

			return filename;
		};

		////////////////////////////////////////////////////////////////////////////////
		//
		bool readScript()
		{
			std::string script_filename( "script.txt" );

			std::cout << "Reading script from " << script_filename << std::endl;

			std::ifstream file_handle( script_filename.c_str() );
			if ( ! file_handle.is_open() )
			{
				std::cout << "ERROR: Can't open script file" << std::endl;
				return false;
			};

			while( ! file_handle.eof() )
			{
				std::string line;
				std::getline( file_handle, line );
				if ( file_handle.eof() )
					break;

				if ( line.length() > 0 && line[ 0 ] != '#' )
				{
					trim( line );
					mScriptLines.insert( mScriptLines.begin(), line );
				};
			};

			file_handle.close();

			std::cout << "Read " << mScriptLines.size() << " (non-comment) script lines" << std::endl;

			return true;
		};

		////////////////////////////////////////////////////////////////////////////////
		//
		bool executeNextLine()
		{
			// no more lines to execute
			if ( mScriptLines.empty() )
			{
				std::cout << "No more script lines left" << std::endl;
				return false;
			};
			std::string next_line = mScriptLines.back();
			mScriptLines.pop_back();

			if ( gNoisyOutput )
				std::cout << "next script line is " << next_line << std::endl;

			// split line into tokens
			std::stringstream strstr( next_line );
			std::istream_iterator< std::string > iter( strstr );
			std::istream_iterator< std::string > end;
			std::vector< std::string > tokens( iter, end );

			// line is empty or no more script
			if ( tokens.empty() )
				return false;

			std::string command = tokens[ 0 ];

			if ( caseInsensitiveCompare( command, "SET" ) )
			{

				if ( tokens.size() >= 3 )
				{
					std::string variable = tokens[ 1 ];
					std::string value = tokens[ 2 ];

					if ( caseInsensitiveCompare( variable, "BROWSERWIDTH" ) )
					{
						std::cout << "Executing [" << command << "] [" << variable << "] = [" << value << "]" << std::endl;
						mBrowserWidth = stringToInt( value );
						LLQtWebKit::getInstance()->setSize( mBrowserId, mBrowserWidth, mBrowserHeight );
					}
					else
					if ( caseInsensitiveCompare( variable, "BROWSERHEIGHT" ) )
					{
						std::cout << "Executing [" << command << "] [" << variable << "] = [" << value << "]" << std::endl;
						mBrowserHeight = stringToInt( value );
						LLQtWebKit::getInstance()->setSize( mBrowserId, mBrowserWidth, mBrowserHeight );
					}
					else
					if ( caseInsensitiveCompare( variable, "JAVASCRIPTENABLED" ) )
					{
						std::cout << "Executing [" << command << "] [" << variable << "] = [" << value << "]" << std::endl;
						varBoolVal( value, mJavaScriptEnabled );
						LLQtWebKit::getInstance()->enableJavascript( mJavaScriptEnabled );
					}
					else
					if ( caseInsensitiveCompare( variable, "PLUGINSENABLED" ) )
					{
						std::cout << "Executing [" << command << "] [" << variable << "] = [" << value << "]" << std::endl;
						varBoolVal( value, mPluginsEnabled );
						LLQtWebKit::getInstance()->enablePlugins( mPluginsEnabled );
					}
					else
					if ( caseInsensitiveCompare( variable, "FLIPWINDOW" ) )
					{
						std::cout << "Executing [" << command << "] [" << variable << "] = [" << value << "]" << std::endl;
						varBoolVal( value, mFlipWindow );
						LLQtWebKit::getInstance()->flipWindow( mBrowserId, mFlipWindow );
					}
					else
					if ( caseInsensitiveCompare( variable, "HOSTLANGUAGE" ) )
					{
						std::cout << "Executing [" << command << "] [" << variable << "] = [" << value << "]" << std::endl;
						mHostLanguage = value;
						LLQtWebKit::getInstance()->setHostLanguage( mHostLanguage );
					}
					else
					if ( caseInsensitiveCompare( variable, "USERAGENTSTRINGADDITION" ) )
					{
						std::cout << "Executing [" << command << "] [" << variable << "] = [" << value << "]" << std::endl;
						mUserAgentStringAddition = value;
						LLQtWebKit::getInstance()->setBrowserAgentId( mUserAgentStringAddition );
					}
					else
					if ( caseInsensitiveCompare( variable, "EXTERNALTARGETNAME" ) )
					{
						std::cout << "Executing [" << command << "] [" << variable << "] = [" << value << "]" << std::endl;
						mExternalTargetName = value;
						LLQtWebKit::getInstance()->setExternalTargetName( mBrowserId, mExternalTargetName );
					}
					else
					if ( caseInsensitiveCompare( variable, "WINDOWOPENBEHAVIOR" ) )
					{
						std::cout << "Executing [" << command << "] [" << variable << "] = [" << value << "]" << std::endl;
						if ( caseInsensitiveCompare( value, "IGNORE" ) )
						{
							mWindowOpenBehavior = LLQtWebKit::WOB_IGNORE;
						}
						else
						{
							mWindowOpenBehavior = LLQtWebKit::WOB_REDIRECT_TO_SELF;
						};
						LLQtWebKit::getInstance()->setWindowOpenBehavior( mBrowserId, mWindowOpenBehavior );
					}
					else
					{
						std::cout << "Unknown parameter for " << command << "] [" << variable << "] = [" << value << "]" << std::endl;
					};
				}
				else
				{
					std::cout << "Script error: not enough values for command: " << command << std::endl;
				};

				return true;
			}
			else
			if ( caseInsensitiveCompare( command, "NAVIGATE" ) )
			{
				if ( tokens.size() == 2 )
				{
					std::string url = tokens[ 1 ];
					std::cout << "Executing " << command << " to [" << url << "]" << std::endl;
					navigateTo( url );
				}
				else
				{
					std::cout << "Script error: not enough values for command: " << command << std::endl;
				};

				return true;
			}
			else
			if ( caseInsensitiveCompare( command, "WAITUNTILLOADED" ) )
			{
				std::cout << "Executing " << command << std::endl;

				waitUntilLoaded();

				return true;
			}
			else
			if ( caseInsensitiveCompare( command, "PAUSE" ) )
			{
				std::string value = tokens[ 1 ];
			
				std::cout << "Executing " << command << " " << value << std::endl;
				
				int seconds = stringToInt( value );

				pause( seconds );				

				return true;
			}
			else
			if ( caseInsensitiveCompare( command, "WRITETGA" ) )
			{
				if ( tokens.size() == 2 )
				{
					std::string filename = tokens[ 1 ];

					// * means make a filename from the URL
					if ( filename == "*" )
					{
						filename = urlToFilename( mLastNavigatedURL );
					};

					std::cout << "Executing " << command << " to file " << filename << std::endl;

					writeTargaImage( filename );
				}
				else
				{
					std::cout << "Script error: not enough values for command: " << command << std::endl;
				};

				return true;
			}
			else
			if ( caseInsensitiveCompare( command, "CHECKTGA" ) )
			{
				if ( tokens.size() == 2 )
				{
					std::string filename = tokens[ 1 ];

					// * means make a filename from the URL
					if ( filename == "*" )
					{
						filename = urlToFilename( mLastNavigatedURL );
					};

					std::cout << "Executing " << command << " to file " << filename << std::endl;

					checkTargaImage( filename );
				}
				else
				{
					std::cout << "Script error: not enough values for command: " << command << std::endl;
				};

				return true;
			}
			else
			if ( caseInsensitiveCompare( command, "DUMPSETTINGS" ) )
			{
				std::cout << "Dumping settings: ";
				for( size_t i = 1; i < tokens.size(); ++i )
				{
					std::cout << tokens[ i ];
					std::cout << " ";
				};
				std::cout << std::endl;

				dumpLLQtWebKitSettings();

				return true;
			}
			else
			if ( caseInsensitiveCompare( command, "EXIT" ) )
			{
				std::cout << "Executing " << command << std::endl;
				return false;
			};

			std::cout << "Unrecognised line found in script - finishing" << std::endl;

			return false;
		};

		////////////////////////////////////////////////////////////////////////////////
		// (Why Targa I hear you ask - simplest file format that can be written without
		// external dependency and supports 32bit image depth (otherwise, PPM would work)
		void writeTargaImage( const std::string& filename )
		{
			std::cout << "Writing output image (Targa) to " << filename << std::endl;

			std::ofstream img_stream( filename.c_str(), std::ios::binary | std::ios::out );
			if ( img_stream )
			{
				LLQtWebKit::getInstance()->grabBrowserWindow( mBrowserId );
				const unsigned char* pixels = LLQtWebKit::getInstance()->getBrowserWindowPixels( mBrowserId );
				if ( pixels )
				{
					const int image_x_origin = 0;
					const int image_y_origin = 0;
					int image_width = mBrowserWidth;
					int image_height = mBrowserHeight;
					const int image_depth = LLQtWebKit::getInstance()->getBrowserDepth( mBrowserId );

					// number of characters in identification field.
					img_stream << (unsigned char)0x00;

					// color map type (0 == none)
					img_stream << (unsigned char)0x00;

					// image type (2 == uncompressed, true-color image)
					img_stream << (unsigned char)0x02;

					// color map information (unused)
					img_stream << (unsigned char)0x00;
					img_stream << (unsigned char)0x00;
					img_stream << (unsigned char)0x00;
					img_stream << (unsigned char)0x00;
					img_stream << (unsigned char)0x00;

					// image origin
					img_stream << (unsigned char)( image_x_origin & 0x00ff );
					img_stream << (unsigned char)( ( image_x_origin & 0xff00 ) / 0x100 );
					img_stream << (unsigned char)( image_y_origin & 0x00ff );
					img_stream << (unsigned char)( ( image_y_origin & 0xff00 ) / 0x100 );

					// image size
					img_stream << (unsigned char)( image_width & 0x00ff );
					img_stream << (unsigned char)( ( image_width & 0xff00 ) / 0x100 );
					img_stream << (unsigned char)( image_height & 0x00ff );
					img_stream << (unsigned char)( ( image_height & 0xff00 ) / 0x100 );

					// bits in output image - always use 24 - we don't care about alpha in output
					img_stream << (unsigned char)( 24 );

					// set bit 5 to indicate origin is at top
					img_stream << (unsigned char)( 0x20 );

					for( int i = 0; i < image_width * image_height * image_depth; i += image_depth )
					{
						const unsigned char red = *( pixels + i + 0 );
						const unsigned char green = *( pixels + i + 1 );
						const unsigned char blue = *( pixels + i + 2 );

						img_stream << blue;
						img_stream << green;
						img_stream << red;
					};

					img_stream.close();
				};
			}
			else
			{
				return;
			};
		};

		////////////////////////////////////////////////////////////////////////////////
		//
		void checkTargaImage( const std::string& filename )
		{
			std::cout << "Checking image file: " << filename << std::endl;

			std::ifstream file_handle( filename.c_str(), std::ios::in | std::ios::binary );
			if ( file_handle.is_open() )
			{
				file_handle.seekg( 0, std::ios::end );
	  			std::ios::pos_type file_size = file_handle.tellg();
				std::cout << "File size is ..." << file_size << std::endl;

				if ( file_size < 1 )
				{
					std::cout << "File size bad - bailing..." << std::endl;
					file_handle.close();
					return;
				};

				std::cout << "Reading file..." << std::endl;
				unsigned char* file_buffer = new unsigned char[ file_size ];
				file_handle.seekg( 18, std::ios::beg );  // skip Targa header
				file_handle.read( (char*)file_buffer, (int)file_size - 18 );

				if ( file_handle.gcount() < 1 )
				{
					std::cout << "File read bad - bailing..." << std::endl;
					delete [] file_buffer;
					file_handle.close();
					return;
				};

				std::cout << "File read ok - analysing..." << std::endl;
				unsigned long pixel_count = 0L;
				for( int i = 0; i < file_size; ++i )
				{
					pixel_count += (unsigned long)( *( file_buffer + i ) );
				};
				std::cout << "File pixel checksum is ..." << pixel_count << std::endl;

				int browser_depth = LLQtWebKit::getInstance()->getBrowserDepth( mBrowserId );
				unsigned long num_pixels = (unsigned long )mBrowserWidth * (unsigned long )mBrowserHeight * (unsigned long )browser_depth;

				// thresholds - outside these limits, the pixel count of the file *might* be wrong
				unsigned long low_threshold = ( num_pixels / 100L ) * 5L;
				unsigned long high_threshold = ( ( num_pixels * 255L ) / 100L ) * 95L;

				std::cout << "-->     num pixels = " << num_pixels << std::endl;
				std::cout << "-->  low threshold = " << low_threshold << std::endl;
				std::cout << "--> high threshold = " << high_threshold << std::endl;

				if ( pixel_count < low_threshold || pixel_count > high_threshold )
				{
					std::cout << "@@ Possible image problem from browser mis-render or crash for file: " << filename << std::endl;
				};

				delete [] file_buffer;

				file_handle.close();
			};
		};

		////////////////////////////////////////////////////////////////////////////////
		// virtual
		void onPageChanged( const EventType& )
		{
			// this can be very noisy and only useful sometimes - can be turned on via command line
			if ( gNoisyOutput )
				std::cout << "Event: page changed" << std::endl;
		};

		////////////////////////////////////////////////////////////////////////////////
		// virtual
		void onNavigateBegin( const EventType& eventIn )
		{
			mIsDone = false;
			std::cout << "Event: begin navigation to " << eventIn.getEventUri() << std::endl;
		};

		////////////////////////////////////////////////////////////////////////////////
		// virtual
		void onNavigateComplete( const EventType& eventIn )
		{
			mIsDone = true;
			
			LLQtWebKit::getInstance()->grabBrowserWindow( mBrowserId );
		
			std::cout << "Event: end navigation to " << eventIn.getEventUri() << std::endl;
		};

		////////////////////////////////////////////////////////////////////////////////
		// virtual
		void onUpdateProgress( const EventType& eventIn )
		{
			// this can be very noisy and only useful sometimes - can be turned on via command line
			if ( gNoisyOutput )
				std::cout << "Event: progress value updated to " << eventIn.getIntValue() << std::endl;
		};

		////////////////////////////////////////////////////////////////////////////////
		// virtual
		void onLocationChange( const EventType& eventIn )
		{
			std::cout << "Event: location changed to " << eventIn.getStringValue() << std::endl;
		};

		////////////////////////////////////////////////////////////////////////////////
		// virtual
		void onFrameCreated( const EventType& )
		{
			std::cout << "Event: frame created" << std::endl;
		};

		////////////////////////////////////////////////////////////////////////////////
		// (note strings care passed by value, not by reference)
		bool caseInsensitiveCompare( std::string s1, std::string s2 )
		{
			// convert *copy* of input strings to lower case and take account of locale/collate
			std::transform( s1.begin(), s1.end(), s1.begin(), std::bind1st( std::mem_fun( &std::ctype<char>::tolower ), &std::use_facet< std::ctype<char> >( std::cout.getloc() ) ) );
			std::transform( s2.begin(), s2.end(), s2.begin(), std::bind1st( std::mem_fun( &std::ctype<char>::tolower ), &std::use_facet< std::ctype<char> >( std::cout.getloc() ) ) );

			if ( s1 == s2 )
				return true;
			else
				return false;
		};

		////////////////////////////////////////////////////////////////////////////////
		//
		std::string& trim( std::string& str )
		{
			str.erase( std::find_if( str.rbegin(), str.rend(), std::not1( std::ptr_fun< int, int >( std::isspace ))).base(), str.end());
			str.erase( str.begin(), std::find_if( str.begin(), str.end(), std::not1( std::ptr_fun< int, int >( std::isspace ))));
        	return str;
		};

		////////////////////////////////////////////////////////////////////////////////
		//
		int stringToInt( std::string& str )
		{
			int value_int;
			std::stringstream codec( str );
			codec >> value_int;

			return value_int;
		};

		////////////////////////////////////////////////////////////////////////////////
		//
		void varBoolVal( std::string& str, bool& var )
		{
			if ( caseInsensitiveCompare( str, "TRUE" ) )
			{
				var = true;
			}
			else
			{
				var = false;
			};
		};

	private:
		bool mIsDone;
		int mBrowserId;
		int mBrowserWidth;
		int mBrowserHeight;
		bool mJavaScriptEnabled;
		bool mPluginsEnabled;
		bool mFlipWindow;
		std::string mLastNavigatedURL;
		std::string mApplicationDir;
		std::string mProfileDir;
		std::string mImageOutputDir;
		std::string mHostLanguage;
		std::string mUserAgentStringAddition;
		std::string mExternalTargetName;
		LLQtWebKit::WindowOpenBehavior mWindowOpenBehavior;
		std::vector< std::string > mScriptLines;
};

////////////////////////////////////////////////////////////////////////////////
//
int main( int argc, char* argv[] )
{
	std::cout << "SnapDragon application starting at " << getTimeStampString() << std::endl;

	snapDragon* sd = new snapDragon( argc, argv );
	if ( sd->readScript() )
	{
		std::cout << "Snap dragon initialized ok" << std::endl;

		if ( sd->init() )
		{
			while( sd->update() )
			{
				if ( ! sd->executeNextLine() )
				{
					break;
				};
			};
		};

		sd->reset();

		delete sd;
	}
	else
	{
		std::cout << "ERROR: unable to initialize Snap Dragon" << std::endl;
	};

	std::cout << "SnapDragon application ending at " << getTimeStampString() << std::endl;

	return 0;
}
