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
    /*  OPTION      VALUE           DESCRIPTION:
     *  -t          folder path     Path to the TLE folder.
     *  -o          folder path     Path to the results folder.
     *  -C          (none)          TLE collections will be looked for in the `current` folder.
     *  -H          (none)          TLE collections will be looked for in the `historic` folder.
     *  -s          UNIX time       Propagation start.
     *  -e          UNIX time       Propagation end.
     *  -d          integer         A positive integer representing the amount of seconds of
     *                              resolution (i.e. propagation step).
     *  -j          integer         Number of threads with which to perform the propagation.
     *  -h          (none)          Shows the help menu.
     *  -v          (none)          Verbose: outputs all data points as it generates them.
     */
    cout << "List of possible arguments:" << endl;
    cout << DBG_WHITEB "OPTION   VALUE                 DESCRIPTION" DBG_NOCOLOR << endl;
    cout << DBG_REDD   "  -t     " DBG_YELLOWD "Path to TLE folder    " DBG_NOCOLOR "Path to a folder containing (only) Two-Line Elements collection files." << endl;
    cout << DBG_REDD   "  -C     " DBG_YELLOWD "(none)                " DBG_NOCOLOR "TLE collections will be looked for in <path>/current (default)." << endl;
    cout << DBG_REDD   "  -H     " DBG_YELLOWD "(none)                " DBG_NOCOLOR "TLE collections will be looked for in <path>/historic." << endl;
    cout << DBG_REDD   "  -o     " DBG_YELLOWD "Path to folder        " DBG_NOCOLOR "Path to the results folder (if it doesn't exist, it'll be created)." << endl;
    cout << DBG_REDD   "  -s     " DBG_YELLOWD "UNIX time             " DBG_NOCOLOR "Orbit propagation start time." << endl;
    cout << DBG_REDD   "  -e     " DBG_YELLOWD "UNIX time             " DBG_NOCOLOR "Orbit propagation end time." << endl;
    cout << DBG_REDD   "  -d     " DBG_YELLOWD "integer               " DBG_NOCOLOR "Positive amount of seconds between each propagation point." << endl;
    cout << DBG_REDD   "  -j     " DBG_YELLOWD "integer               " DBG_NOCOLOR "Number of threads with which to perform the propagation." << endl;
    cout << DBG_REDD   "  -v     " DBG_YELLOWD "(none)                " DBG_NOCOLOR "Verbose; will output all data points as it generates them." << endl;
    cout << DBG_REDD   "  -h     " DBG_YELLOWD "(none)                " DBG_NOCOLOR "Shows this help." << endl;
}


int main(int argc, char **argv)
{
    /* C-like variable declaration is chosen here for documentation and code cleanliness purposes.*/
    int    prop_n_points;       /* Number of propagation points.                                  */
    time_t prop_time_start;     /* In seconds                                                     */
    time_t prop_time_end;       /* In seconds                                                     */
    time_t prop_time_step;      /* In seconds                                                     */
    string input_path;          /* Input path where TLE files are located.                        */
    string output_path_root;    /* Path folder for the resulting files.                           */
    string output_path;         /* Output path for the resulting files.                           */
    vector<string> tle_files;   /* A vector of TLE file paths (<*>/<*>/<*>.txt)                   */
    FILE * tle_file;            /* TLE input file.                                                */
    FILE * conf_file;           /* Program configuration file.                                    */
    char file_line[200];        /* One single line from an open file.                             */
    int line_count = 0;         /* Line counter (debug purposes).                                 */
    string tle_line1;           /* Line 1 in the TLE file.                                        */
    string tle_line2;           /* Line 2 in the TLE file.                                        */
    string tle_line3;           /* Line 3 in the TLE file.                                        */
    struct tm *tmp;             /* Time struct (debug purposes).                                  */
    char time_formated[21];     /* Time in the format "yyyy-mm-dd hh:mm:ss"                       */
    double julian_days;         /* Time in Julian days (debug purposes).                          */
    bool verbose = false;       /* Whether to print all data points as they are generated.        */
    unordered_map<int, TLEHistoricSet> tle_data; /* TLE data for each NORAD ID and with
                                                  * historical records.
                                                  */
    int norad_id;               /* A NORAD ID scanned from the config. file.                      */
    char str_aux[500];          /* An auxiliary char pointer. Debug purposes.                     */

    /* Setup the default configuration values: */
    prop_time_start = time(NULL);
    prop_time_end   = prop_time_start + 60000;
    prop_time_step  = 60;
    prop_n_points   = -1;
    tmp = localtime(&prop_time_start);
    strftime(time_formated, 21, "%Y-%m-%d_%H%M%S", tmp);
    output_path_root = "propagations/" + string(time_formated);
    input_path = "tle_collections/current";

    /* Configuration based on program arguments: ------------------------------------------------ */
    if(argc > 1) {
        /*  OPTION      VALUE           DESCRIPTION:
         *  -t          folder path     Path to the TLE folder.
         *  -o          folder path     Path to the results folder.
         *  -c          (none)          TLE collections will be looked for in the `current` folder.
         *  -H          (none)          TLE collections will be looked for in the `historic` folder.
         *  -s          UNIX time       Propagation start.
         *  -e          UNIX time       Propagation end.
         *  -d          integer         A positive integer representing the amount of seconds of
         *                              resolution (i.e. propagation step).
         *  -j          integer         Number of threads with which to perform the propagation.
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
            } else if(str == "-C") {
                input_path = "tle_collections/current";
            } else if(str == "-H") {
                input_path = "tle_collections/historic";
            } else if(str == "-j") {
                cerr << DBG_REDD "Multi-threaded propagation is still not supported." DBG_NOCOLOR << endl;
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

    if( ((prop_time_end - prop_time_start) / 3600.0) > (24.0 * 60.0) ||
        ((prop_time_end - prop_time_start) / prop_time_step) > 1e6) {
        cerr << DBG_REDD "  WARNING: With the given setup, the propagation will probably take a long time to compute." DBG_NOCOLOR << endl;
    }

    /* Configuration based on external file: ---------------------------------------------------- */
    if((conf_file = fopen(CONF_FILE_PATH, "r")) != NULL) {
        while(fgets(file_line, 200, conf_file) != NULL) {
            line_count++;
            if(sscanf(file_line, "%d", &norad_id) > 0) {
                tle_data.insert({norad_id, TLEHistoricSet(norad_id)});
            } else {
                if(sscanf(file_line, "%*[ #] %s", str_aux) > 0) { /* Commented line. Will be skipped*/
                    /* No nothing. */
                } else if(sscanf(file_line, "%s", str_aux) > 0) { /* Dirty trick to get rid of line-feed character. */
                    cerr << DBG_REDD "  WARNING: Malformed configuration parameter in line " << line_count << ": \"" << str_aux << "\"" DBG_NOCOLOR << endl;
                } /* else --> Empty line. Do nothing. */
            }
        }
        fclose(conf_file);
        if(tle_data.size() > 0) {
            cout << "  " << tle_data.size() << " NORAD identifiers/orbits will be propagated." << endl;
        } else {
            cerr << DBG_REDD "  WARNING: No NORAD identifiers have been set. This program will end now." DBG_NOCOLOR << endl;
            return -1;
        }
    } else {
        cerr << DBG_REDD "  ERROR: Configuration file (" CONF_FILE_PATH ") could not be found/opened." DBG_NOCOLOR << endl;
        return -1;
    }

    /* Load TLE data from files: ---------------------------------------------------------------- */
    struct linux_dirent {
        long           d_ino;
        off_t          d_off;
        unsigned short d_reclen;
        char           d_name[];
    };

    int tle_directory, tle_sub_directory;
    int nread, nread_sub;
    int bpos, bpos_sub;
    char dents_buf[2048], dents_buf_sub[20148], d_type;
    struct linux_dirent *d;

    if((tle_directory = open(input_path.c_str(), O_RDONLY | O_DIRECTORY)) == -1) {
        cout << DBG_REDD "  ERROR: Opening the TLE directory failed. Aborting." DBG_NOCOLOR << endl;
        exit(-1);
    }

    while(1) {
        nread = syscall(SYS_getdents, tle_directory, dents_buf, 2048);
        if(nread == -1) {
            cout << DBG_REDD "  ERROR: TLE directory scan failed. Aborting." DBG_NOCOLOR << endl;
            exit(-1);
        } else if(nread == 0) {
            break;
        }
        for(bpos = 0; bpos < nread;) {
            d = (struct linux_dirent *)(dents_buf + bpos);
            d_type = *(dents_buf + bpos + d->d_reclen - 1);
            bpos += d->d_reclen;
            switch(d_type) {
                case DT_DIR:
                    if(strcmp(d->d_name, ".") && strcmp(d->d_name, "..")) {
                        string subdir_path = input_path + "/" + string(d->d_name);
                        if((tle_sub_directory = open(subdir_path.c_str(), O_RDONLY | O_DIRECTORY)) == -1) {
                            cout << DBG_REDD "  ERROR: Opening the TLE sub-directory (" << subdir_path << ") failed. Aborting." DBG_NOCOLOR << endl;
                            exit(-1);
                        }
                        while(1) {
                            nread_sub = syscall(SYS_getdents, tle_sub_directory, dents_buf_sub, 2048);
                            if(nread_sub == -1) {
                                cout << DBG_REDD "  ERROR: TLE sub-directory scan failed. Aborting." DBG_NOCOLOR << endl;
                                exit(-1);
                            } else if(nread_sub == 0) {
                                break;
                            }
                            for(bpos_sub = 0; bpos_sub < nread_sub;) {
                                d = (struct linux_dirent *)(dents_buf_sub + bpos_sub);
                                d_type = *(dents_buf_sub + bpos_sub + d->d_reclen - 1);
                                bpos_sub += d->d_reclen;
                                if(d_type == DT_REG) {
                                    tle_files.push_back(subdir_path + "/" + string(d->d_name));
                                }
                            }
                        }
                    }
                    break;
                case DT_REG:
                    tle_files.push_back(input_path + "/" + string(d->d_name));
                    break;
                default:
                    /* Ignored entries. */
                    break;
            }
        }
    }
    cout << "  " << tle_files.size() << " TLE files have been found." << endl;

    char sat_name[25];
    int sat_identifier;
    line_count = 0;
    for(vector<string>::const_iterator f = tle_files.begin(); f != tle_files.end(); f++) {
        if((tle_file = fopen((*f).c_str(), "r")) != NULL) {
            /* Will look for TLE ID's and iterate files: -------------------------------- */
            while(fgets(file_line, 80, tle_file) != NULL) {
                if(sscanf(file_line, "%24[^\n\t\r]", sat_name) < 1) {
                    cerr << DBG_REDD "  WARNING: Can't read satellite name, will try to continue." DBG_NOCOLOR << endl;
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
                    sat_identifier = stoi(tle_line2.substr(2, 5), NULL);
                    /* Check whether this TLE has to be propagated or not: */
                    unordered_map<int, TLEHistoricSet>::iterator tle_data_it = tle_data.find(sat_identifier);
                    if(tle_data_it != tle_data.end()) {
                        TLEHistoricSet tlehs = tle_data_it->second;
                        tlehs.addTLE(Zeptomoby::OrbitTools::cTle(tle_line1, tle_line2, tle_line3));
                        tle_data_it->second = tlehs;
                    }
                } else {
                    cerr << DBG_REDD "  Malformed TLE file. Error appears in lines " << (line_count - 3) << " to " << line_count << "." DBG_NOCOLOR << endl;
                    break;
                }
            }
            fclose(tle_file);

        } else {
            cerr << DBG_REDD "  ERROR: Could not open the TLE file (" << (*f) << ")." DBG_NOCOLOR << endl;
            continue;
        }
    }
    cout << endl;

    /* Perform the propagations: ---------------------------------------------------------------- */
    /* -- Create results folder: */
    system(string("mkdir -p " + output_path_root).c_str()); /* Linux/Bash-specific. */
    /* -- Propagate each individual orbit: */
    for(auto t = tle_data.begin(); t != tle_data.end(); t++) {
        try {
            t->second.propagate(output_path_root, prop_time_start, prop_time_end, prop_time_step, prop_n_points, verbose);
        } catch(exception& e) {
            // cerr << DBG_REDD "  Propagation of " << t->first << " throwed an EXCEPTION: " << e.what() << DBG_NOCOLOR << endl;
        }
    }

    cout << "  Done." << endl;

    exit(1);

#if 0















    /* Internal set-up: ------------------------------------------------------------------------- */
    char sat_name[25];
    int sat_identifier;
    line_count = 0;



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
                            sat_identifier = stoi(tle_line2.substr(2, 5), NULL);
                            /* Check whether this TLE has to be propagated or not: */
                            if(norad_ids.find(sat_identifier) != norad_ids.end()) {
                                /* This ID will no longer be found (in case of repeated satellites in different TLE files). */
                                norad_ids.erase(norad_ids.find(sat_identifier));

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

sat_name
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
            for (set<int>::iterator it = norad_ids.begin(); it != norad_ids.end(); ++it) {
                cout << " " << *it;
            }
            cout << endl;
        } else {
            cout << "Done!" << endl;
        }
    } else {
        cerr << DBG_REDD "  ERROR: Unable to open TLE collection directory: " << input_path << DBG_NOCOLOR << endl;
    }
#endif
    return 0;
}
