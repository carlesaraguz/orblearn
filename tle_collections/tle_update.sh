#!/bin/bash

TLECPATH=.
HISTPATH=$TLECPATH/historic/`date +%Y-%m-%d_%H%M%S`
CURRPATH=$TLECPATH/current

mkdir -p $HISTPATH
mkdir -p $CURRPATH

wget -B http://celestrak.com/NORAD/elements/ -i $TLECPATH/tle_update_list -q --no-cache --directory-prefix=$HISTPATH
wget -B http://celestrak.com/NORAD/elements/ -i $TLECPATH/tle_update_list -N -q --no-cache --progress=bar --show-progress --directory-prefix=$CURRPATH
