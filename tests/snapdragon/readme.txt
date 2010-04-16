======================== Snap Dragon (2010-04-16) ========================

------------ Contact ------------
Callum Prentice - callum@lindenlab.com

------------ About ------------
First attempt to make an auto-testing app that renders sites via URLs in
a script and examines the contents to find out if things rendered correctly.

The only test right now is to look for mostly white (0xFF) or black (0x00) pixels
but this will be expanded to include testing images against previous "known good"
versions.

------------ To Do ------------
* Make it work on Mac

* Make it work on Linux

* Add some kind iof support for comparing image against "known good" version

* Add option to overlay image details (NOTE: might be able to use javascript://
  URLs to do this directly). For example, "This page should display a warning
  that JavaScript is disabled" when add a line SET JAVASCRIPT FALSE in the script.

* Fix restriction that strings in SET command must not contain white space

* Loads more cool stuff :)


------------ Script commands ------------

Command:
	#
e.g.
	#### This is a test script
Description:
	any line starting with a # is treated as a comment


Command:
	SET <variable> <value>
e.g. 
	SET BROWSERWIDTH 950
Description:	
	Set a variable to a value and update state of LLQtWebKit
	No whitespace allowed in variable or value (to be fixed)
	Possible variables:
		SET BROWSERWIDTH <nuber pixels>
		SET BROWSERHEIGHT <nuber pixels>
		SET JAVASCRIPTENABLED <true|false>
		SET PLUGINSENABLED <true|false>
		SET FLIPWINDOW <true|false>
		SET HOSTLANGUAGE <language string>
		SET USERAGENTSTRINGADDITION <user agent string added to browser's one>
		SET EXTERNALTARGETNAME <target window name>
		SET WINDOWOPENBEHAVIOR <ignore|self>
	See sample script for example usage


Command:
	DUMPSETTINGS <message>
e.g.
	DUMP About to try Flash URLs
Description:
	Writes current LLQtWebKit settings to output along with the message


Command:
	NAVIGATE <url>
e.g. 
	NAVIGATE http://www.google.com
Description:
	Begins navigation to specified URL
	Script continues after navigation begins


Command:
	WAITUNTILLOADED
e.g.
	WAITUNTILLOADED
Description:
	Forces script to wait for the current navigation to complete
	Events are still processed.
	No timeout - if a navigate complete event is not received, script will stall


Command:
	PAUSE <seconds>
e.g.
	PAUSE 5
Description:
	Pause the script for <seconds> seconds.
	Events are still processed.
	Useful for Flash movies where you want to wait for movie to load


Command:
	WRITETGA <filename>
e.g.
	WRITE google.tga
	WRITE *
Description:
	Writes the current pixel buffer being rendered into out to a Targa file
	Use * as the filename to use the last navigated URL as the basis of filename


Command:
	CHECKTGA <filename>
e.g.
	CHECKTGA youtube_no_script.tga
	CHECKTGA *
Description:
	Checks a Targa file for "correctness"
	Currently this means not containing too many white or black pixels - more to come
	Use * as the filename to use the last navigated URL as the basis of filename


Command:
	EXIT
e.g.
	EXIT
Description:
	Exits script 
	Useful for testing - add an EXIT command after first navigate

