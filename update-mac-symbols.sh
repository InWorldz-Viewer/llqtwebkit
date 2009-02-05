#!/bin/sh

# this script does the necessary magic to extract the desired list of symbols into
# the llmozlib2.exp file.

# exit on any error
set -e

# build the "All Symbols" version
xcodebuild -project llmozlib2.xcodeproj -target llmozlib2 -configuration 'All Symbols' build

# extract the symbols associated with the classes "LLMozLib" and "LLEmbeddedBrowserWindowObserver"
otool -vT 'build/All Symbols/libllmozlib2.dylib' |\
sed -n -e '/.*\.eh/d' -e 's/^single module    \(.*\)/\1/p'|\
grep '\(^__Z.\{1,2\}8LLMozLib\)\|\(^__Z.\{1,2\}31LLEmbeddedBrowserWindowObserver\)' >llmozlib2.exp

