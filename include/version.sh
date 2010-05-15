#!/bin/sh
#
# Build version string and increment Services build number.
#

if [ $# -lt 2 ] ; then
	echo "Syntax: $0 <version.log> <version.h>"
	exit 1
fi
# Grab version information from the version control file.
CTRL="$1"
if [ -f $CTRL ] ; then
	. $CTRL
else
	echo "Error: Unable to find control file: $CTRL"
	exit 0
fi

VERSION="${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}${VERSION_EXTRA} (${VERSION_BUILD})"
VERSIONDOTTED="${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}${VERSION_EXTRA}.${VERSION_BUILD}"

VERSIONH="$2"
if [ -f $VERSIONH ] ; then
	BUILD=`fgrep '#define BUILD' $VERSIONH | cut -f2 -d\"`
	BUILD=`expr $BUILD + 1 2>/dev/null`
else
	BUILD=1
fi
if [ ! "$BUILD" ] ; then
	BUILD=1
fi
cat >$VERSIONH <<EOF
/* Version information for Services.
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and CREDITS for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 * This file is auto-generated by version.sh
 *
 */

 #ifndef VERSION_H
 #define VERSION_H

#define VERSION_MAJOR	$VERSION_MAJOR
#define VERSION_MINOR	$VERSION_MINOR
#define VERSION_PATCH	$VERSION_PATCH
#define VERSION_EXTRA	"$VERSION_EXTRA"
#define VERSION_BUILD	$VERSION_BUILD

#define BUILD	"$BUILD"
#define VERSION_STRING "$VERSION"
#define VERSION_STRING_DOTTED "$VERSIONDOTTED"

#endif

EOF

