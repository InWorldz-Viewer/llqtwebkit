#!/bin/bash

# This script adds or updates libllmozlib.dylib in mozilla-universal-darwin.tgz.
sandbox_location="$1"


if [ -f build/Release/libllmozlib.dylib ]; then
	echo "libllmozlib.dylib found"
else
	echo "libllmozlib.dylib not found, please build the Release target in llmozlib.xcodeproj"
	exit 1
fi

if [ -d "$sandbox_location" ]; then
	echo "Using sandbox at $sandbox_location"
else
	echo "Sandbox $sandbox_location not found, exiting."
	exit 1
fi


# exit on any error
set -e

echo "Unpacking mozilla-universal-darwin-original.tgz..."

# expand the runtime archive
rm -rf mozilla-universal-darwin
mkdir -p mozilla-universal-darwin
(cd mozilla-universal-darwin && tar -zxf ../mozilla-universal-darwin-original.tgz)

echo "Adding libllmozlib.dylib..."

# copy in the latest built version of llmozlib
cp ./build/Release/libllmozlib.dylib mozilla-universal-darwin

echo "Removing unneeded pieces..."

# remove bits of the mozilla runtime that we don't use
rm -rf mozilla-universal-darwin/Info.plist
rm -rf mozilla-universal-darwin/Resources
rm -rf mozilla-universal-darwin/run-mozilla.sh
rm -rf mozilla-universal-darwin/updater.app
rm -rf mozilla-universal-darwin/xpicleanup
rm -rf mozilla-universal-darwin/xulrunner
rm -rf mozilla-universal-darwin/xulrunner-bin

echo "Creating mozilla-universal-darwin.tgz..."

# tar up the runtime and llmozlib 
rm -f ./mozilla-universal-darwin.tgz
tar -zcf ./mozilla-universal-darwin.tgz \
	-C mozilla-universal-darwin . 

#rm -rf mozilla-universal-darwin

#echo '###############################'
#echo 'please copy mozilla-universal-darwin.tgz to indra/newview in your sandbox.'

echo "Copying mozilla-universal-darwin.tgz to $sandbox_location/indra/newview..."
cp mozilla-universal-darwin.tgz $sandbox_location/indra/newview/

echo "Copying stub library to $sandbox_location/libraries/universal-darwin/..."
cp build/Release_stub/libllmozlib.dylib $sandbox_location/libraries/universal-darwin/lib_debug/libllmozlib.dylib
cp build/Release_stub/libllmozlib.dylib $sandbox_location/libraries/universal-darwin/lib_release/libllmozlib.dylib

echo "done!"

