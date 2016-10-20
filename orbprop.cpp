/***********************************************************************************************//**
 *  \brief      Orbit propagator.
 *  \details    A tool to generate satellite orbit propagation data based on NORAD's SGP4 algorithm
 *              and publicly available libraries.
 *  \author     Carles Araguz, carles.araguz@upc.edu
 *  \version    0.1
 *  \date       09-sep-2016
 *  \copyright  GNU Public License (v3). This files are part of an on-going non-commercial research
 *              project at NanoSat Lab (http://nanosatlab.upc.edu) of the Technical University of
 *              Catalonia - UPC BarcelonaTech. Third-party libraries used in this framework might be
 *              subject to different copyright conditions.
 **************************************************************************************************/


/*** INCLUDE SECTION ******************************************************************************/
#include "orbprop.hpp"

using namespace std;

/*** CONSTANTS ************************************************************************************/



void printHelp(void)
{
    /*  OPTION      VALUE       DESCRIPTION:
     *  -t          folder path Path to the TLE folder.
     *  -o          folder paht Path to the results folder.
     *  -s          UNIX time   Propagation start.
     *  -e          UNIX time   Propagation end.
     *  -d          integer     A positive integer representing the amount of seconds of resolution (i.e. propagation step).
     *  -h          (none)      Shows the help menu.
     *  -v          (none)      Verbose: outputs all data points as it generates them.
     */
    cout << "List of possible arguments:" << endl;
    cout << DBG_WHITEB "OPTION   VALUE                 DESCRIPTION" DBG_NOCOLOR << endl;
    cout << DBG_REDD   "  -t     " DBG_YELLOWD "Path to TLE folder    " DBG_NOCOLOR "Path to a folder containing (only) Two-Line Elements collection files." << endl;
    cout << DBG_REDD   "  -o     " DBG_YELLOWD "Path to folder        " DBG_NOCOLOR "Path to the results folder (if it doesn't exist, it'll be created)." << endl;
    cout << DBG_REDD   "  -s     " DBG_YELLOWD "UNIX time             " DBG_NOCOLOR "Orbit propagation start time." << endl;
    cout << DBG_REDD   "  -e     " DBG_YELLOWD "UNIX time             " DBG_NOCOLOR "Orbit propagation end time." << endl;
    cout << DBG_REDD   "  -d     " DBG_YELLOWD "integer               " DBG_NOCOLOR "Positive amount of seconds between each propagation point." << endl;
    cout << DBG_REDD   "  -v     " DBG_YELLOWD "(none)                " DBG_NOCOLOR "Verbose; will output all data points as it generates them." << endl;
    cout << DBG_REDD   "  -h     " DBG_YELLOWD "(none)                " DBG_NOCOLOR "Shows this help." << endl;
}

void printHeader(bool first)
{
    if(first) {
        cout << "┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┳━━━━━━━━━━━━━━━━━━━━━━━━━┳━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┳━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓" << endl;
    } else {
        cout << "┢━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━╈━━━━━━━━━━━━━━━━━━━━━━━━━╈━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━╈━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┪" << endl;
    }
    cout << "┃ Time stamp                       ┃         LAT         LON ┃         ECI X         ECI Y         ECI Z ┃      vel X      vel Y      vel Z ┃" << endl;
    cout << "┡━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━╇━━━━━━━━━━━━━━━━━━━━━━━━━╇━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━╇━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┩" << endl;
}

void printFooter(void)
{
    cout << "└──────────────────────────────────┴─────────────────────────┴───────────────────────────────────────────┴──────────────────────────────────┘" << endl;
}

int main(int argc, char **argv)
{
    /* C-like variable declaration is chosen here for documentation and code cleanliness purposes.*/
    int    prop_n_points;       /* Number of propagation points.                                  */
    time_t prop_time_start;     /* In seconds                                                     */
    time_t prop_time_end;       /* In seconds                                                     */
    time_t prop_time_step;      /* In seconds                                                     */
    time_t prop_time_curr;      /* Simulation's current time.                                     */
    time_t tle_time;            /* Time reference in the TLE file.                                */
    string input_path;          /* Input path where TLE files are located.                        */
    string tle_file_path;       /* The TLE file name/path.                                        */
    string output_path_root;    /* Path folder for the resulting files.                           */
    string output_path;         /* Output path for the resulting files.                           */
    DIR * tle_directory;        /* TLE directory.                                                 */
    struct dirent * dir_entry;  /* File within the TLE directory.                                 */
    FILE * tle_file;            /* TLE input file.                                                */
    FILE * conf_file;           /* Program configuration file.                                    */
    FILE * output_file;         /* Resulting output file.                                         */
    char file_line[200];        /* One single line from an open file.                             */
    int line_count = 0;         /* Line counter (debug purposes).                                 */
    string tle_line1;           /* Line 1 in the TLE file.                                        */
    string tle_line2;           /* Line 2 in the TLE file.                                        */
    string tle_line3;           /* Line 3 in the TLE file.                                        */
    struct tm *tmp;             /* Time struct (debug purposes).                                  */
    char time_formated[21];     /* Time in the format "yyyy-mm-dd hh:mm:ss"                       */
    double julian_days;         /* Time in Julian days (debug purposes).                          */
    bool verbose = false;       /* Whether to print all data points as they are generated.        */
    set<string> norad_ids;      /* The TLE ID's that will be propagated.                          */
    char id_str[6];             /* A single NORAD ID scanned from the config. file.               */

    /* Setup the default configuration values: */
    prop_time_start = time(NULL);
    prop_time_end   = prop_time_start + 60000;
    prop_time_step  = 60;
    prop_n_points   = -1;
    tmp = localtime(&prop_time_start);
    strftime(time_formated, 21, "%Y-%m-%d_%H%M%S", tmp);
    output_path_root = "propagations/" + string(time_formated);
    input_path = "tle_collections";

    /* Configuration based on program arguments: ------------------------------------------------ */
    if(argc > 1) {
        /*  OPTION      VALUE           DESCRIPTION:
         *  -t          folder path     Path to the TLE folder.
         *  -o          folder paht     Path to the results folder.
         *  -s          UNIX time       Propagation start.
         *  -e          UNIX time       Propagation end.
         *  -d          integer         A positive integer representing the amount of seconds of resolution (i.e. propagation step).
         *  -h          (none)          Shows the help menu.
         *  -v          (none)          Verbose: outputs all data points as it generates them.
         */
        int arg_iterator = 1;
        while(arg_iterator < argc)
        {
            string str = string(argv[arg_iterator]);
            if(str == "-t" && (arg_iterator + 1) < argc) {
                input_path = string(argv[arg_iterator + 1]);
                arg_iterator++;
            } else if(str == "-o" && (arg_iterator + 1) < argc) {
                output_path_root = string(argv[arg_iterator + 1]);
                arg_iterator++;
            } else if(str == "-p" && (arg_iterator + 1) < argc) {
                if((prop_n_points = strtol(argv[arg_iterator + 1], NULL, 10)) <= 0)
                {
                    cerr << DBG_REDD "Wrong argument value: \'-p " << string(argv[arg_iterator + 1]) << "\'" DBG_NOCOLOR << endl;
                    cerr << DBG_REDD "The number of propagation points should be a positive integer." DBG_NOCOLOR << endl;
                    printHelp();
                    return -1;
                }
                arg_iterator++;
            } else if(str == "-s" && (arg_iterator + 1) < argc) {
                if((prop_time_start = strtol(argv[arg_iterator + 1], NULL, 10)) <= 0)
                {
                    cerr << DBG_REDD "Wrong argument value: \'-s " << string(argv[arg_iterator + 1]) << "\'" DBG_NOCOLOR << endl;
                    cerr << DBG_REDD "Propagation start should be a UNIX time" DBG_NOCOLOR << endl;
                    printHelp();
                    return -1;
                }
                arg_iterator++;
            } else if(str == "-e" && (arg_iterator + 1) < argc) {
                if((prop_time_end = strtol(argv[arg_iterator + 1], NULL, 10)) <= 0)
                {
                    cerr << DBG_REDD "Wrong argument value: \'-e " << string(argv[arg_iterator + 1]) << "\'" DBG_NOCOLOR << endl;
                    cerr << DBG_REDD "Propagation end should be a UNIX time" DBG_NOCOLOR << endl;
                    printHelp();
                    return -1;
                }
                arg_iterator++;
            } else if(str == "-d" && (arg_iterator + 1) < argc) {
                if((prop_time_step = strtol(argv[arg_iterator + 1], NULL, 10)) <= 0)
                {
                    cerr << DBG_REDD "Wrong argument value: \'-d " << string(argv[arg_iterator + 1]) << "\'" DBG_NOCOLOR << endl;
                    cerr << DBG_REDD "Propagation steps should be represented with positive integers (in seconds)" DBG_NOCOLOR << endl;
                    printHelp();
                    return -1;
                }
                arg_iterator++;
            } else if(str == "-v") {
                verbose = true;
            } else {
                cout << "Unknown argument: \'" << str << "\'" << endl;
                printHelp();
                return -1;
            }
            arg_iterator++;
        }
    }

    if(prop_n_points > 0) {
        prop_time_end = prop_time_start + prop_time_step * prop_n_points;
    } else {
        prop_n_points = ceil((prop_time_end - prop_time_start) / prop_time_step);
    }

    tmp = gmtime(&prop_time_start);
    strftime(time_formated, 21, "%Y-%m-%d %T", tmp);
    julian_days = tmp->tm_yday + tmp->tm_hour/24.0 + tmp->tm_min/(24.0*60.0) + tmp->tm_sec/(24.0*60.0*60.0);
    cout << "  T(start): " << prop_time_start << " (" << time_formated << " UTC, Julian date: " << (1900 + tmp->tm_year) << ", " << julian_days << ")" << endl;

    tmp = gmtime(&prop_time_end);
    strftime(time_formated, 21, "%Y-%m-%d %T", tmp);
    julian_days = tmp->tm_yday + tmp->tm_hour/24.0 + tmp->tm_min/(24.0*60.0) + tmp->tm_sec/(24.0*60.0*60.0);
    cout << "  T(end)  : " << prop_time_end << " (" << time_formated << " UTC, Julian date: " << (1900 + tmp->tm_year) << ", " << julian_days << ")" << endl;
    cout << "  T(step) : " << prop_time_step << " seconds (" << (prop_time_step/60.0) <<" min.)" << endl;
    cout << "  Span    : " << ((prop_time_end - prop_time_start) / 3600.0) << " hours (" << ((prop_time_end - prop_time_start) / 3600.0)/24.0 << " days)." << endl;
    cout << "  Output  : " << prop_n_points << " points." << endl << endl;

    if(prop_time_start > prop_time_end) {
        cerr << DBG_REDD "  ERROR: Start time is after end time." DBG_NOCOLOR << endl;
        return -1;
    }

    if( ((prop_time_end - prop_time_start) / 3600.0) > 48.0 ||
        ((prop_time_end - prop_time_start) / prop_time_step) > 1e4) {
        cerr << DBG_REDD "  WARNING: With the given setup, the propagation will probably take a long time to compute." DBG_NOCOLOR << endl;
    }

    /* Configuration based on external file: ---------------------------------------------------- */
    if((conf_file = fopen(CONF_FILE_PATH, "r")) != NULL) {
        while(fgets(file_line, 200, conf_file) != NULL) {
            line_count++;
            if(sscanf(file_line, "%[0123456789]", id_str) > 0) {
                cout << "  NORAD Id: " << id_str << endl;
                norad_ids.insert(string(id_str));
            } else {
                /* Dirty trick to get rid of line-feed character. */
                if(sscanf(file_line, "%s", id_str) > 0) {
                    cerr << DBG_REDD "  WARNING: Malformed configuration parameter in line " << line_count << ": \"" << id_str << "\"" << endl;
                } /* else --> Empty line. */
            }
        }
        fclose(conf_file);
        if(norad_ids.size() > 0) {
            cout << "  " << norad_ids.size() << " NORAD identifiers/orbits will be propagated." << endl << endl;
        } else {
            cerr << DBG_REDD "  WARNING: No NORAD identifiers have been set. This program will end now." DBG_NOCOLOR << endl;
            return -1;
        }
    } else {
        cerr << DBG_REDD "  ERROR: Configuration file (" CONF_FILE_PATH ") could not be found/opened." DBG_NOCOLOR << endl;
        return -1;
    }


    /* Internal set-up: ------------------------------------------------------------------------- */
    char sat_name[25];
    string sat_identifier;
    line_count = 0;

    /* Create results folder: */
    system(string("mkdir -p " + output_path_root).c_str()); /* Linux/Bash-specific. Will not work in
                                                             * Windows or non-Bash environments.
                                                             */


    if((tle_directory = opendir(input_path.c_str())) != NULL) {
        while((dir_entry = readdir(tle_directory)) != NULL) {
            if(strcmp(dir_entry->d_name, ".") && strcmp(dir_entry->d_name, "..")) {
                string full_path = input_path + "/" + string(dir_entry->d_name);
                if((tle_file = fopen(full_path.c_str(), "r")) != NULL) {

                    /* Will look for TLE ID's and iterate files: -------------------------------- */

                    while(fgets(file_line, 80, tle_file) != NULL) {
                        if(sscanf(file_line, "%24[^\n\t\r]", sat_name) < 1) {
                            cerr << DBG_REDD "Can't read satellite name, will try to continue." DBG_NOCOLOR << endl;
                        } else {
                            tle_line1 = string(file_line);
                        }
                        line_count++;
                        if(fgets(file_line, 80, tle_file) != NULL) {
                            tle_line2 = string(file_line);
                        } else {
                            break;
                        }
                        line_count++;
                        if(fgets(file_line, 80, tle_file) != NULL) {
                            tle_line3 = string(file_line);
                        } else {
                            break;
                        }
                        line_count++;

                        /* At this point, three lines have been read: check that they are well formed. */
                        if(tle_line2.substr(2, 5) == tle_line3.substr(2, 5)) {
                            sat_identifier = tle_line2.substr(2, 5);
                            /* Check whether this TLE has to be propagated or not: */
                            if(norad_ids.find(sat_identifier) != norad_ids.end()) {
                                /* This ID will no longer be found (in case of repeated satellites in different TLE files). */
                                norad_ids.erase(norad_ids.find(sat_identifier));
                                /* Create/open file: */
                                output_path = output_path_root + "/" + sat_identifier + ".prop";
                                if((output_file = fopen(output_path.c_str(), "w+")) == NULL) {
                                    cerr << DBG_REDD "Unable to open file " << output_path << DBG_NOCOLOR << endl;
                                    continue;
                                }
                            } else {
                                /* This TLE will be skipped. */
                                continue;
                            }
                        } else {
                            cerr << DBG_REDD "Malformed TLE file. Error appears in lines " << (line_count - 3) << " to " << line_count << "." DBG_NOCOLOR << endl;
                            break;
                        }


                        /* New cTLE object: */
                        Zeptomoby::OrbitTools::cTle tle_data(tle_line1, tle_line2, tle_line3);

                        /* Create an Orbit object using the satellite TLE object. */
                        Zeptomoby::OrbitTools::cOrbit orbit(tle_data);

                        Zeptomoby::OrbitTools::cEciTime satellite = orbit.PositionEci(0.0);
                        Zeptomoby::OrbitTools::cGeo proj_earth = Zeptomoby::OrbitTools::cGeo(satellite, satellite.Date());

                        /* :: Warning ::
                        *  ToTime() uses mktime to represent the time. This function represents the
                        *  time in its LOCAL form.
                        */
                        tle_time = satellite.Date().ToTime();
                        time_t current_prop_time_start = prop_time_start - tle_time;
                        time_t current_prop_time_end   = prop_time_end - tle_time;

                        cout << sat_identifier << " (" << output_path << "): " << sat_name << endl;

                        if(verbose) {
                            printHeader(true);
                        }

                        /* Write CSV headers to the output file: */
                        time_t current_local_time = time(NULL);
                        tmp = localtime(&current_local_time);
                        strftime(time_formated, 21, "%Y-%m-%d %T", tmp);
                        fprintf(output_file, "File generation time,%s\n", time_formated);
                        fprintf(output_file, "Time (start),%lu\n", prop_time_start);
                        fprintf(output_file, "Time (end),%lu\n", prop_time_end);
                        fprintf(output_file, "Time (step),%lu\n", prop_time_step);
                        fprintf(output_file, "Points,%d\n", prop_n_points);
                        fprintf(output_file, "Time,Timestamp,Latitude,Longitude,x,y,z,vx,vy,vz\n");

                        int step_count = 1;
                        /* Iterate through time as defined in the input arguments: */
                        for(int i = current_prop_time_start; i <= current_prop_time_end; i += prop_time_step)
                        {
                            if(verbose && !((++step_count) % 50)) {
                                printHeader(false);
                            }
                            try {
                                satellite = orbit.PositionEci(i/60.0);
                                prop_time_curr = satellite.Date().ToTime();
                                proj_earth = Zeptomoby::OrbitTools::cGeo(satellite, satellite.Date());
                            } catch (exception& e) {
                                cout << "Exception catched!" << endl;
                                break;
                            }

                            tmp = localtime(&prop_time_curr);
                            strftime(time_formated, 21, "%Y-%m-%d %T", tmp);
                            fprintf(output_file, "%s,%10ld,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f\n",
                            time_formated,
                            prop_time_curr,
                            proj_earth.LatitudeDeg(),
                            (proj_earth.LongitudeDeg() < 180 ? proj_earth.LongitudeDeg() : proj_earth.LongitudeDeg()-360),
                            satellite.Position().m_x,
                            satellite.Position().m_y,
                            satellite.Position().m_z,
                            satellite.Velocity().m_x,
                            satellite.Velocity().m_y,
                            satellite.Velocity().m_z);

                            if(verbose) {
                                printf("│% 10ld (%s) │ % 11.6f % 11.6f │ % 13.6f % 13.6f % 13.6f │ % 10.6f % 10.6f % 10.6f │\n",
                                prop_time_curr,
                                time_formated,
                                proj_earth.LatitudeDeg(),
                                (proj_earth.LongitudeDeg() < 180 ? proj_earth.LongitudeDeg() : proj_earth.LongitudeDeg()-360),
                                satellite.Position().m_x,
                                satellite.Position().m_y,
                                satellite.Position().m_z,
                                satellite.Velocity().m_x,
                                satellite.Velocity().m_y,
                                satellite.Velocity().m_z);
                            }

                        }
                        if(verbose) {
                            printFooter();
                        }
                        fclose(output_file);
                    }
                    fclose(tle_file);

                } else {
                    cerr << DBG_REDD "  ERROR: Could not open the TLE file (" << dir_entry->d_name << ")." DBG_NOCOLOR << endl;
                    continue;
                }
            }
        }
        closedir(tle_directory);
        if(norad_ids.size() > 0) {
            cout << "TLE file scanning has finished. However, the following NORAD ID's could not be found:";
            for (set<string>::iterator it = norad_ids.begin(); it != norad_ids.end(); ++it) {
                cout << " " << *it;
            }
            cout << endl;
        } else {
            cout << "Done!" << endl;
        }
    } else {
        cerr << DBG_REDD "  ERROR: Unable to open TLE collection directory: " << input_path << DBG_NOCOLOR << endl;
    }

    return 0;
}
