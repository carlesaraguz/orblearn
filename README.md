# C++ Orbit Propagator

OrbProp is a simple wrapping program for the orbitTools SGP4 library which allows to generate satellite trajectory data easily and in batch. Based on the C++ NORAD SGP4/SDP4 Implementation developed by Michael F. Henry, this tool will generate a single CSV file for each TLE set found in an input file. CSV files contain the satellite position and velocity in ECI (x, y, z), their projection in the surface (latitude and longitude) and a timestamp for each data point.

## How to build

A Makefile is provided in order to build the sources easily. Note that the user needs to have GNU C++ compiler (`g++`) and GNU Make installed to build the sources. OrbProp sources will only compile and run in Linux machines; Windows or Mac support is not provided. In order to build the sources, the following targets are provided:

    make all        # Will build the sources.
    make clean      # Removes binary and objects folder.
    make cleanall   # Removes binary, objects folder and the default propagations folder.

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

Example:

    $ ./orbprop -t tle_collections -s $(date +%s) -e $(($(date +%s)+3600)) -d 30
    $ ./orbprop -t tle_collections -p 1440

# C++/Octave Satellite filter

satfilt is a simple program that allows to filter up to num_groups satellites with the 6 fundamental orbit parameters criterion.
This tool will generate a CSV file with all TLE found in the input and one file with the filtered satellites information for each parameter
(this files are presented in a way to be easily added to the OrbProp config file).

## How to build

A Makefile is provided in order to build the sources easily. Note that the user needs to have GNU C++ compiler (`g++`) and GNU Make installed to build the sources. satfilt sources will only compile and run in Linux machines; Windows or Mac support is not provided. In order to build the sources, the following targets are provided:

    make all        # Will build the sources.
    make clean      # Removes binary and objects folder.
    make cleanall   # Removes binary, objects folder and the default propagations folder.

## Usage

satfit expects at least a collection of TLE's in a single file. In order to configure the filtering, the following arguments are accepted:

 *  `-t <Path to TLE folder>`: Path to the Two-Line Elements collection file (**default**: `./tle_collections`).
 *  `-o <Path to the output folder>`: Path to the results folder. If it doesn't exist, it'll be created. (**default**: `./database/`) .
 *  `-v`: Verbose; will output all satellite information as it generate it.
 *  `-h`: Shows this help.

Example:

    $ ./satfilt -t tle_collections 
    $ octave --no-gui
    $ num_groups = 10;
    $ >> satfilt('./database/<date>/', num_groups);
