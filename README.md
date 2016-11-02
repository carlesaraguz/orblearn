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

Example:

    $ ./orbprop -t tle_collections -s $(date +%s) -e $(($(date +%s)+3600)) -d 30
    $ ./orbprop -t tle_collections -p 1440

# C++/Octave Satellite Filter

satfilt is a simple program that allows to filter up to num_groups satellites with the 6 fundamental orbit parameters criterion.
This tool will generate a CSV file with all TLE found in the input and one file with the filtered satellites information for each parameter
(this files are presented in a way to be easily added to the OrbProp config file).

## Usage

### C++

satfit expects at least a collection of TLE's in a single file. In order to configure the filtering, the following arguments are accepted:

 *  `-t <Path to TLE folder>`: Path to the Two-Line Elements collection file (**default**: `./tle_collections`).
 *  `-o <Path to the output folder>`: Path to the results folder. If it doesn't exist, it'll be created. (**default**: `./database/`) .
 *  `-v`: Verbose; will output all satellite information as it generate it.
 *  `-h`: Shows this help.

### Octave

satfilt expects an "output.db" file with the format of the C++ outtput. In order to configure the filtering, the following arguments are accepted:

 * `path`: Path where it will find the output.db file and where it will store the resulting .db file.
 * `n_param`: Parameter that is expected to filter:
 ** 2  - inclination (degrees)
 ** 3  - eccentricity
 ** 4  - period (minutes)
 ** 5  - perigee (degrees)
 ** 6  - longitude of the ascending node (degrees)
 ** 7  - mean anomaly (degrees)
 ** -1 - all of them 
 * `max_val and min_val`: Are the maximum and minimum values for the parameter selected. (If all the parameteres are being filtered or they are out of range, they will be computed automatically)
 * `num_groups`: Number of satellites that you desire to achieve after the filtering (it may be less). If its value is different from -1, the group list will be ignored. 
 * `groups_lims`: Array with the limits a the group as an array. E.g.: `[[15,30];[45,65];...;[329, 354]]` The program will not controle if they are disjount groups or not. If there is a number of groups specified, this option will be ignored.

Example:

    $ ./satfilt -t tle_collections 
    $ octave --no-gui
    $ >> num_groups = 10;
    $ >> satfilt('./database/<date>/', 5, 15, 250, 5, -1);


## How to build

A Makefile is provided in order to build the sources easily. Note that the user needs to have GNU C++ compiler (`g++`) and GNU Make installed to build the sources. satfilt sources will only compile and run in Linux machines; Windows or Mac support is not provided. In order to build the sources, the following targets are provided:

    make all        # Will build all the sources.	
    make orbprop    # Will build the Orbit Propagator only.
    make satfilt    # Will build the Satellite Filter only.
    make clean      # Removes binary and objects folder.
    make cleanall   # Removes binary, objects folder and the default propagations folder.
