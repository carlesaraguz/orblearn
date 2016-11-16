# Orbit Propagator Tools
OrbProp is a simple wrapping program for the [OrbitTools SGP4 library](http://www.zeptomoby.com/satellites/) that allows to generate satellite trajectory data easily and in batch. Based on the C++ NORAD SGP4/SDP4 Implementation developed by Michael F. Henry, this tool will generate one CSV file for each NORAD ID defined in the configuration file (`orbprop.conf`). CSV files contain the satellite position and velocity in ECI (x, y, z), their projection in the surface (latitude and longitude) and a timestamp for each data point.

Orbital parameters can be stored in regular text files, with the same format than those publicly available in [CelesTrak](http://www.celestrak.com). Each text file may include an unlimited number of TLE's (Two-Line Element sets). Its NORAD ID's will be scanned and only the selected ones will be propagated. In addition, OrbProp is designed to allow the propagation of orbits based on historical data.

## How to build
A Makefile is provided to ease the compilation process. Note that the user needs to have GNU C++ compiler (`g++`) and GNU Make installed to build the sources. OrbProp sources will only compile and run in Linux machines; Windows or Mac support is not provided. In order to build the sources, the following targets are provided:

    make all        # Will build the sources.
    make clean      # Removes binary and objects folder.
    make cleanall   # Removes binary, objects folder and the default propagations folder.

## Usage
OrbProp expects, at least, one TLE file (i.e. a collection of TLE's in a single file.) In order to configure the propagation, the following arguments are accepted:

* `-t <Path to TLE folder>`: Path to the Two-Line Elements collection file (**default**: `./tle_collections/current`).
* `-o <Path to output folder>`: Path to the results folder. If it doesn't exist, it'll be created. (**default**: `./propagations/`)
* `-C`: TLE collections will be looked for in the `current` sub-folder.
* `-H`: TLE collections will be looked for in the `historic` sub-folder.
* `-s <UNIX time>`: Orbit propagation start time (**default**: current timestamp).
* `-e <UNIX time>`: Orbit propagation end time (**default**: current timestamp + 1000 minutes).
* `-p <integer>`: Number of propagation points. If set, the end time will be ignored.
* `-d <integer>`: Positive amount of seconds between each propagation point (**default**: 1 minute).
* `-v`: Verbose; will output all data points as it generates them.
* `-h`: Shows this help.

## Gathering TLE data
A script has been left inside the `tle_collections` folder to download TLE data automatically. TLE data will be downloaded from CelesTrak and stored in two different folders: `current` and `historic` (containing the current TLE details and an historic collection, respectively). This script can be called manually inside the directory:

    cd tle_collections      # Enters the directory.
    ./tle_update.sh         # Updates the TLE files.

This script will store historical data in sub-folders `historic/YYYY-MM-DD_HHMMSS`. Instead of manually executing the update script, the user can easily automate this process by configuring a _cron job_ that calls that script periodically. The file `tle_update_list` defines which TLE collections (from [CelesTrak's NORAD Elements](http://celestrak.com/NORAD/elements/)) will be downloaded. Currently, with the provided list in this repo, all TLE collections will be downloaded.


## Configuration file:
OrbProp reads the file `orbprop.conf` at the beginning. This file lists the NORAD ID's that will be propagated. Example:

    # Comments can be written with the hash symbol (#), but are not mandatory.
    # NORAD ID      # Incl.         Eccentr.    Period      Arg. peri.  Long as.n.  Mean anom.
    # ========      # ==========    ==========  ========    ==========  ==========  ===========
    7530            # 101.595600    275.623700  0.001238     41.608600   24.602400  114.866990
    25338           #  98.785000    312.972700  0.001131    137.652700  222.552700  100.998661
    36287           #   1.530000      6.220500  0.000555    226.192900  255.227800  1436.162301
    22825           #  98.778700    265.699300  0.000732    271.771500   88.262800  100.701067
    28890           #  97.849400    110.366800  0.001611    136.766800  223.479400   98.552769
    27607           #  64.554600     95.051100  0.005175    184.159400  175.908600   97.611289
    26958           #  97.531900    278.395800  0.007428      4.441500  355.745400   96.328652
    37846           #  55.656900     78.061100  0.000132    340.323200   19.647400  844.704037
    19548           #  14.854000     10.518400  0.003667    302.174800  359.955900  1436.125778

## Examples:
To propagate from the current time to +3600 seconds (1h) with a 30 second step:

    $ ./orbprop -t my_tle_collections -s $(date +%s) -e $(($(date +%s)+3600)) -d 30

With the previous call, TLE collection files will be looked for in the folder `./my_tle_collections/current`.

The following call, will propagate with a time step of 1440 seconds (24 minutes).

    $ ./orbprop -p 1440

### Comparing updated TLE's with data from the past
The `-H` option can be used when the user wants to compare how inaccurate the SGP4 model is with old/outdated TLE data. In order to do so, two propagations can be performed: one with the historical data and another with the initial TLE data. Bear in mind that propagating backwards is **not** supported. This means that in order for a propagation to be performed, the TLE data must have a date _previous or equal_ to the propagation start time.

    $ ./orbprop -t my_tle_collections/historic/2016-11-16_120000 -o propagations_0 -s $(date --date="Nov 16 2016 12:00:00" +%s)
    $ ./orbprop -t my_tle_collections/historic/                  -o propagations_1 -s $(date --date="Nov 16 2016 12:00:00" +%s)

The first call will start the propagation with the TLE collection files located in the given directory, with a start time in Nov. 16th 2016 at 12h. On the other hand, the second call will propagate with the same setup but scanning through all the `historic` folder. Both calls will generate the same number of output files and each file will have the same number of points. Note that in order for the previous example to work, the folder `2016-11-16_120000` must have TLE collection files where the TLE data is set for a date before November 16th at 12h.
