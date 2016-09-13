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


/*** INCLUDE SECTION ********************************************************************************/
#include "orbprop.hpp"

using namespace std;

/*** CONSTANTS **************************************************************************************/
// #define ERR_ANGLE 0.001
// #define MIN_VISION_ANGLE 0.0

void printHelp(void)
{
    /*  OPTION      VALUE       DESCRIPTION:
     *  -t          file path   Path to the TLE file.
     *  -s          UNIX time   Propagation start.
     *  -e          UNIX time   Propagation end.
     *  -d          integer     A positive integer representing the amount of seconds of resolution (i.e. propagation step).
     *  -v          (none)      Verbose: outputs all data points as it generates them.
     */
    cout << "List of possible arguments:" << endl;
    cout << "OPTION   VALUE             DESCRIPTION" << endl;
    cout << "  -t     File path         Path to the Two-Line Elements collection file." << endl;
    cout << "  -o     Path to folder    Path to the results folder (if it doesn't exist, it'll be created)." << endl;
    cout << "  -s     UNIX time         Orbit propagation start time." << endl;
    cout << "  -e     UNIX time         Orbit propagation end time." << endl;
    cout << "  -d     integer           Positive amount of seconds between each propagation point." << endl;
    cout << "  -v     (none)            Verbose; will output all data points as it generates them." << endl;
    cout << "  -h     (none)            Shows this help." << endl;
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
    int    prop_n_points;       // Number of propagation points.
    time_t prop_time_start;     // In seconds
    time_t prop_time_end;       // In seconds
    time_t prop_time_step;      // In seconds
    time_t prop_time_curr;      // Simulation's current time.
    time_t tle_time;            // Time reference in the TLE file.
    string tle_file_path;  // The TLE file name/path.
    string output_path_root; // Path folder for the resulting files.
    string output_path;    // Output path for the resulting files.
    FILE * tle_file;            // TLE input file.
    FILE * output_file;         // Resulting output file.
    char tle_line[80];          // One single line from the TLE file (auxiliary variable).
    string tle_line1;           // Line 1 in the TLE file.
    string tle_line2;           // Line 2 in the TLE file.
    string tle_line3;           // Line 3 in the TLE file.
    struct tm *tmp;             // Time struct (debug purposes).
    char time_formated[21];     // Time in the format "yyyy-mm-dd hh:mm:ss"
    double julian_days;         // Time in Julian days (debug purposes).
    bool verbose = false;       // Whether to print all data points as they are generated.

    /* Setup the default configuration values: */
    prop_time_start = time(NULL);
    prop_time_end   = prop_time_start + 60000;
    prop_time_step  = 60;
    prop_n_points   = -1;
    tle_file_path   = "collection.tle";
    tmp = gmtime(&prop_time_start);
    strftime(time_formated, 21, "%Y-%m-%d_%H%M%S", tmp);
    output_path_root = "propagations/" + string(time_formated);

    if(argc > 1) {
        /*  OPTION      VALUE           DESCRIPTION:
         *  -t          file path       Path to the TLE file.
         *  -o          folder path     Path to the results folder.
         *  -s          UNIX time       Propagation start.
         *  -e          UNIX time       Propagation end.
         *  -d          integer         A positive integer representing the amount of seconds of resolution (i.e. propagation step).
         *  -p          integer         A positive integer representing the amount of data points to be generated (end time will be ignored).
         *  -v          (none)          Verbose: outputs all data points as it generates them.
         */
        int arg_iterator = 1;
        while(arg_iterator < argc)
        {
            string str = string(argv[arg_iterator]);
            if(str == "-t" && (arg_iterator + 1) < argc) {
                tle_file_path = string(argv[arg_iterator + 1]);
                arg_iterator++;
            } else if(str == "-o" && (arg_iterator + 1) < argc) {
                output_path_root = string(argv[arg_iterator + 1]);
                arg_iterator++;
            } else if(str == "-p" && (arg_iterator + 1) < argc) {
                if((prop_n_points = strtol(argv[arg_iterator + 1], NULL, 10)) <= 0)
                {
                    cerr << "Wrong argument value: \'-p " << string(argv[arg_iterator + 1]) << "\'" << endl;
                    cerr << "The number of propagation points should be a positive integer." << endl;
                    printHelp();
                    return -1;
                }
                arg_iterator++;
            } else if(str == "-s" && (arg_iterator + 1) < argc) {
                if((prop_time_start = strtol(argv[arg_iterator + 1], NULL, 10)) <= 0)
                {
                    cerr << "Wrong argument value: \'-s " << string(argv[arg_iterator + 1]) << "\'" << endl;
                    cerr << "Propagation start should be a UNIX time" << endl;
                    printHelp();
                    return -1;
                }
                arg_iterator++;
            } else if(str == "-e" && (arg_iterator + 1) < argc) {
                if((prop_time_end = strtol(argv[arg_iterator + 1], NULL, 10)) <= 0)
                {
                    cerr << "Wrong argument value: \'-e " << string(argv[arg_iterator + 1]) << "\'" << endl;
                    cerr << "Propagation end should be a UNIX time" << endl;
                    printHelp();
                    return -1;
                }
                arg_iterator++;
            } else if(str == "-d" && (arg_iterator + 1) < argc) {
                if((prop_time_step = strtol(argv[arg_iterator + 1], NULL, 10)) <= 0)
                {
                    cerr << "Wrong argument value: \'-d " << string(argv[arg_iterator + 1]) << "\'" << endl;
                    cerr << "Propagation steps should be represented with positive integers (in seconds)" << endl;
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
    cout << "  T(start): " << prop_time_start << " (" << time_formated << ", Julian date: " << (1900 + tmp->tm_year) << ", " << julian_days << ")" << endl;

    tmp = gmtime(&prop_time_end);
    strftime(time_formated, 21, "%Y-%m-%d %T", tmp);
    julian_days = tmp->tm_yday + tmp->tm_hour/24.0 + tmp->tm_min/(24.0*60.0) + tmp->tm_sec/(24.0*60.0*60.0);
    cout << "  T(end)  : " << prop_time_end << " (" << time_formated << ", Julian date: " << (1900 + tmp->tm_year) << ", " << julian_days << ")" << endl;
    cout << "  T(step) : " << prop_time_step << " seconds (" << (prop_time_step/60.0) <<" min.)" << endl;
    cout << "  Span    : " << ((prop_time_end - prop_time_start) / 3600.0) << " hours." << endl;
    cout << "  Output  : " << prop_n_points << " points." << endl << endl;

    if(prop_time_start > prop_time_end) {
        cerr << "  ERROR: Start time is after end time." << endl;
        return -1;
    }

    if( ((prop_time_end - prop_time_start) / 3600.0) > 48.0 ||
        ((prop_time_end - prop_time_start) / prop_time_step) > 1e4) {

        cerr << "  WARNING: With the given setup, the propagation will probably take a long time to compute." << endl;
    }

    tle_file = fopen(tle_file_path.c_str(), "r");
    if(tle_file == NULL)
    {
        cerr << "  ERROR: Could not open the TLE file (" << tle_file_path << ")." << endl;
        return -1;
    }

    /* Define ground station location on Earth (Barcelona) */
    Zeptomoby::OrbitTools::cSite ground_station(41.388497, 2.111882, 0);
    char sat_name[25];
    string sat_identifier;
    int line_count = 0;

    /* Create results folder: */
    system(string("mkdir -p " + output_path_root).c_str()); /* Linux-specific. Will not work in Windows or non-bash environments. */
    while(fgets(tle_line, 80, tle_file) != NULL)
    {
        if(sscanf(tle_line, "%24[^\n\t\r]", sat_name) < 1) {
            cerr << "Can't read satellite name, will try to continue." << endl;
        }
        line_count++;
        if(fgets(tle_line, 80, tle_file) != NULL) {
            tle_line2 = string(tle_line);
        } else {
            break;
        }
        line_count++;
        if(fgets(tle_line, 80, tle_file) != NULL) {
            tle_line3 = string(tle_line);
        } else {
            break;
        }
        line_count++;

        /* At this point, three lines have been read: check that they are well formed. */
        if(tle_line2.substr(2, 5) == tle_line3.substr(2, 5)) {
            sat_identifier = tle_line2.substr(2, 5);
            /* Create/open file: */
            output_path = output_path_root + "/" + sat_identifier + ".prop";
            if((output_file = fopen(output_path.c_str(), "w+")) == NULL) {
                cerr << "Unable to open file " << output_path << endl;
                continue;
            }
        } else {
            cerr << "Malformed TLE file. Error appears in lines " << (line_count - 3) << " to " << line_count << "." << endl;
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

        /* Get the location of the satellite from the Orbit object. The ECI information is placed
         * into satellite.
         */
        Zeptomoby::OrbitTools::cTopo topo_look = ground_station.GetLookAngle(satellite);

        cout << sat_identifier << " (" << output_path << "): " << sat_name << endl;

        if(verbose) {
            printHeader(true);
        }

        /* Write CSV headers to the output file: */
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
    cout << "Done!" << endl;

    return 0;
}
