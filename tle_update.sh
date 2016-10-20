#!/bin/bash

wget -B http://celestrak.com/NORAD/elements/ -i tle_update_list --no-cache --progress=bar --show-progress --directory-prefix=tle_collections
