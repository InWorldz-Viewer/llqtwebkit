#!/bin/sh

# turn on verbose debugging output for parabuild logs.
set -x

if [ -z "$AUTOBUILD" ] ; then 
    AUTOBUILD="$(which autobuild)"
fi

autobuild_installed ()
{
    if [ -z "$AUTOBUILD" ] || [ ! -x "$AUTOBUILD" ] ; then
		if [ -z "$helper" ] ; then
			helper=.
		fi
		for AUTOBUILD in `which autobuild` "$helper/../autobuild/bin/autobuild" ; do
			if [ -x "$AUTOBUILD" ] ; then
				break
			fi
		done
    fi

    echo "located autobuild tool: '$AUTOBUILD'"
}

# at this point we should know where everything is, so make errors fatal
set -e

# this fail function will either be provided by the parabuild buildscripts or
# not exist.  either way it's a fatal error
autobuild_installed || fail

# *HACK - bash doesn't know how to pass real pathnames to native windows python
if [ "$OSTYPE" == 'cygwin' ] ; then
	AUTOBUILD="$(cygpath -u $AUTOBUILD.cmd)"
fi

# load autbuild provided shell functions and variables
eval "$("$AUTOBUILD" source_environment)"

if [ -z "$build_log" ] ; then
    build_log='build.log'
fi

"$AUTOBUILD" install 2>&1 | tee -a "$build_log"
"$AUTOBUILD" build --use-cwd 2>&1 | tee -a "$build_log"
"$AUTOBUILD" package 2>&1 | tee -a "$build_log"

INSTALLABLE_PACKAGE_FILENAME="$(ls -1 llqtwebkit-*-$AUTOBUILD_PLATFORM-$(date +%Y%m%d)*.tar.bz2)"

if "$build_legacy_package" ; then
    # repackage_legacy is defined in the branch independent BuildParams defaults
    "$repackage_legacy" "$INSTALLABLE_PACKAGE_FILENAME"
fi

upload_item installer "$INSTALLABLE_PACKAGE_FILENAME" binary/octet-stream

INSTALLABLE_PACKAGE_MD5="$(calc_md5 "$INSTALLABLE_PACKAGE_FILENAME")"
PACKAGE_DST="$S3PUT_URL""$S3PREFIX""repo/$repo/rev/$revision/arch/$arch/installer/$(basename "$INSTALLABLE_PACKAGE_FILENAME")"
echo "{'md5':'$INSTALLABLE_PACKAGE_MD5', 'url':'$PACKAGE_DST'}" > "output.json"

upload_item installer "output.json" text/plain

pass

