#!/bin/sh

# Run this to set up the build system: configure, makefiles, etc.
set -e

srcdir=$(dirname $0)
test -n "$srcdir" && cd "$srcdir"

echo "Updating build configure files for Ultimarc-linux, please wait..."

autoreconf -isf
