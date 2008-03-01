#!/bin/sh

if [ '!' -d glui-2.35 ]; then
	if [ '!' -f glui-2.35.zip ]; then
		echo "Please download the zip file from this link and try again:"
		echo 'http://prdownloads.sourceforge.net/glui/glui-2.35.zip?download'
	fi

	if [ -f glui-2.35.zip ]; then
		unzip glui-2.35.zip
	fi
fi

if [ -d glui-2.35 ]; then
	cd glui-2.35/src 
	mkdir -p lib
	CPPFLAGS="-arch ppc -arch i386" 
	export CPPFLAGS 
	make -f makefile lib/libglui.a
	ranlib lib/libglui.a
fi
