# C++ Orbit Propagator

OrbProp is a simple wrapping program for the orbitTools SGP4 library which allows to generate satellite trajectory data easily and in batch. Based on the C++ NORAD SGP4/SDP4 Implementation developed by Michael F. Henry, this tool will generate a single CSV file for each TLE set found in an input file. CSV files contain the satellite position and velocity in ECI (x, y, z), their projection in the surface (latitude and longitude) and a timestamp for each data point.

## How to build

A Makefile is provided in order to build the sources easily. Note that the user needs to have GNU C++ compiler (`g++`) and GNU Make installed to build the sources. OrbProp sources will only compile and run in Linux machines; Windows or Mac support is not provided. In order to build the sources, the following targets are provided:

    make all        # Will build the sources.
    make clean      # Removes binary and objects folder.
    make cleanall   # Removes binary, objects folder and the default propagations folder.

## Usage

OrbProp expects at least a TLE file (or a collection of TLE's in a single file.) In order to configure the propagation, the following arguments are accepted:

* `-t <File path>`: Path to the Two-Line Elements collection file (default: `./collection.tle`).
* `-o <Path to folder>`: Path to the results folder. If it doesn't exist, it'll be created. (default: `./propagations/`)
* `-s <UNIX time>`: Orbit propagation start time (default: current timestamp).
* `-e <UNIX time>`: Orbit propagation end time (default: current timestamp + 1000 minutes).
* `-d <integer>`: Positive amount of seconds between each propagation point (default: 1 minute).
* `-v`: Verbose; will output all data points as it generates them.
* `-h`: Shows this help.
