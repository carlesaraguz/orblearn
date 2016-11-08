# C++ Orbit Propagator

OrbProp is a simple wrapping program for the orbitTools SGP4 library which allows to generate satellite trajectory data easily and in batch. Based on the C++ NORAD SGP4/SDP4 Implementation developed by Michael F. Henry, this tool will generate a single CSV file for each TLE set found in an input file. CSV files contain the satellite position and velocity in ECI (x, y, z), their projection in the surface (latitude and longitude) and a timestamp for each data point.

## Usage

OrbProp expects at least a TLE file (or a collection of TLE's in a single file.) In order to configure the propagation, the following arguments are accepted:

* `-t <Path to TLE folder>`: Path to the Two-Line Elements collection file (**default**: `./collection.tle`).
* `-o <Path to output folder>`: Path to the results folder. If it doesn't exist, it'll be created. (**default**: `./propagations/`)
* `-s <UNIX time>`: Orbit propagation start time (**default**: current timestamp).
* `-e <UNIX time>`: Orbit propagation end time (**default**: current timestamp + 1000 minutes).
* `-p <integer>`: Number of propagation points. If set, the end time will be ignored.
* `-d <integer>`: Positive amount of seconds between each propagation point (**default**: 1 minute).
* `-v`: Verbose; will output all data points as it generates them.
* `-h`: Shows this help.

## How to build

A Makefile is provided in order to build the sources easily. Note that the user needs to have GNU C++ compiler (`g++`) and GNU Make installed to build the sources. satfilt sources will only compile and run in Linux machines; Windows or Mac support is not provided. In order to build the sources, the following targets are provided:

    make all        # Will build all the sources.	
    make orbprop    # Will build the Orbit Propagator only.
    make clean      # Removes binary and objects folder.
    make cleanall   # Removes binary, objects folder, the default propagations folder and the plots folder.

Example:

    $ ./orbprop -t tle_collections -s $(date +%s) -e $(($(date +%s)+3600)) -d 30
    $ ./orbprop -t tle_collections -p 1440

# Octave Read Satellite

This tool will generate an object sat with all satellites in the input files with their fundamental orbit parameters.

## Usage

read_sat expects at least a collection of TLE's in a single file. In order to configure the filtering, the following arguments are accepted.

 *  `curr_path`: Path to the Two-Line Elements collection file (at the moment).
 *  `hit_path`: Path to the Two-Line Elements collection file (historical data).

## How to build

No necessary build is needed for the usage of this tool. (Octave must be installed)

Example:

    $ octave --no-gui
    $ >> curr = './tle_collections/current/';
    $ >> hist  = './tle_collections/historic/';
    $ >> sat = read_sat(curr, hist);

# Octave Param Statistics

This tool will generate the statistics of the different 6 fundamental orbital parametres through the time.

## Usage

paramStatistics expects the path of the current TLE of the satellites and a path of the historical data of the satellites. (This tool requires read_sat).
paramStatistics will generate 6 plots (one for each parameter) with the mean, variance, maximum and minimum values of the difference between these paremetes. We can also see the mean value (through the satellites) of this graphics.

satfilt expects an "output.db" file with the format of the C++ outtput. In order to configure the filtering, the following arguments are accepted:

 *  `curr_path`: Path to the Two-Line Elements collection file (at the moment).
 *  `hit_path`: Path to the Two-Line Elements collection file (historical data).

## How to build

No necessary build is needed for the usage of this tool. (Octave must be installed). However, using the same Makefile we can delete all plots.

    make cleanall   # Removes binary, objects folder, the default propagations folder and the plots folder.

Example:

    $ octave --no-gui
    $ >> curr = './tle_collections/current/';
    $ >> hist  = './tle_collections/historic/';
    $ >> paramStatistics(curr, hist);





