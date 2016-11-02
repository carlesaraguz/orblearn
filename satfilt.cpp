/*************************************************************************************************
 *  \brief      Satellite filter through determinant parameters.
 *  \details    A tool to generate a database of satellites with relevant parameters.
 *  \author     Borja Rodríguez (borjabrg12@gmail.com)
 *  \version    0.1
 *  \date       27-oct-2017
 *  \copyright  GNU Public License (v3). This files are part of an on-going non-commercial research
 *              project at NanoSat Lab (http://nanosatlab.upc.edu) of the Technical University of
 *              Catalonia - UPC BarcelonaTech. Third-party libraries used in this framework might be
 *              subject to different copyright conditions.
 **************************************************************************************************/

 /*** INCLUDE SECTION ******************************************************************************/
#include "satfilt.hpp"

using namespace std;

/*** CONSTANTS ************************************************************************************/

#define DBG_REDB        "\x1b[31;1m"
#define DBG_REDD        "\x1b[31m"
#define DBG_GREENB      "\x1b[32;1m"
#define DBG_GREEND      "\x1b[32m"
#define DBG_BLUE        "\x1b[34;1m"
#define DBG_YELLOWB     "\x1b[33;1m"
#define DBG_YELLOWD     "\x1b[33m"
#define DBG_WHITEB      "\x1b[37;1m"
#define DBG_GREY        "\x1b[30;1m"
#define DBG_NOCOLOR     "\x1b[0m"

void printHelp(void)
{
    /*  OPTION      VALUE       DESCRIPTION:
     *  -t          folder path Path to the TLE folder.
     *  -o          folder paht Path to the results folder.
     *  -v          (none)      Verbose: outputs all data points as it generates them.
     *  -h          (none)      Shows the help menu.
     */
    cout << "List of possible arguments:" << endl;
    cout << DBG_WHITEB "OPTION   VALUE                 DESCRIPTION" DBG_NOCOLOR << endl;
    cout << DBG_REDD   "  -t     " DBG_YELLOWD "Path to TLE folder    " DBG_NOCOLOR "Path to a folder containing (only) Two-Line Elements collection files." << endl;
    cout << DBG_REDD   "  -o     " DBG_YELLOWD "Path to folder        " DBG_NOCOLOR "Path to the results folder (if it doesn't exist, it'll be created)." << endl;
    cout << DBG_REDD   "  -v     " DBG_YELLOWD "(none)                " DBG_NOCOLOR "Verbose; will output all data points as it generates them." << endl;
    cout << DBG_REDD   "  -h     " DBG_YELLOWD "(none)                " DBG_NOCOLOR "Shows this help." << endl;
}

void printHeader(bool first)
{
    if(first) {
        cout << "┏━━━━━━━━┳━━━━━━━━┳━━━━━━━━━━━┳━━━━━━━━━━━┳━━━━━━━━━━━┳━━━━━━━━━━━━┳━━━━━━━━━━━┓" << endl;
    } else {
        cout << "┢━━━━━━━╈━━━━━━━━╈━━━━━━━━━━━╈━━━━━━━━━━━╈━━━━━━━━━━━╈━━━━━━━━━━━━╈━━━━━━━━━━━┪" << endl;
    }
    cout << "┃   ID   ┃  inc   ┃    ecc    ┃     T     ┃    per    ┃  long_a_n  ┃   m_an    ┃" << endl;
    cout << "╇━━━━━━━━╇━━━━━━━━┃━━━━━━━━━━━┃━━━━━━━━━━━┃━━━━━━━━━━━┃━━━━━━━━━━━━┃━━━━━━━━━━━┃" << endl;
}

void printFooter(void)
{
    cout << "╇━━━━━━━━╇━━━━━━━━┃━━━━━━━━━━━┃━━━━━━━━━━━┃━━━━━━━━━━━┃━━━━━━━━━━━━┃━━━━━━━━━━━┃" << endl;
}

int main(int argc, char **argv){

    /* C-like variable declaration is chosen here for documentation and code cleanliness purposes.*/
    string input_path;          /* Input path where TLE files are located.                        */
    string tle_file_path;       /* The TLE file name/path.                                        */
    string output_path_root;    /* Path folder for the resulting files.                           */
    string output_path;         /* Output path for the resulting files.                           */
    DIR * tle_directory;        /* TLE directory.                                                 */
    struct dirent * dir_entry;  /* File within the TLE directory.                                 */
    FILE * tle_file;            /* TLE input file.                                                */
    FILE * output_file;         /* Resulting output file.                                         */
    char file_line[200];        /* One single line from an open file.                             */
    int line_count = 0;         /* Line counter (debug purposes).                                 */
    string tle_line1;           /* Line 1 in the TLE file.                                        */
    string tle_line2;           /* Line 2 in the TLE file.                                        */
    string tle_line3;           /* Line 3 in the TLE file.                                        */
    time_t time_start;          /* In seconds                                                     */
    char time_formated[21];     /* Time in the format "yyyy-mm-dd hh:mm:ss"                       */
    struct tm *tmp;             /* Time struct (debug purposes).                                  */
    bool verbose = false;       /* Whether to print all data points as they are generated.        */


    /* Internal set-up: ------------------------------------------------------------------------- */
    char sat_name[25];          /* Name of the satellite                                          */
    double norad;               /* Line 3 in the TLE file.                                        */
    double inc;                 /* Inclination degrees.                                           */
    double ecc;                 /* Eccentricity of the /(no units)                                */
    double period;              /* Period of one orbit (minutes)                                  */
    double perig;               /* Argument of the perigee (degrees)                              */
    double long_a_n;            /* Longitude of the ascending node (degrees)                      */
    double m_an;                /* Mean anomaly (degrees)                                         */

    /* Setup the default configuration values: */
    time_start = time(NULL);
    tmp = localtime(&time_start);
    strftime(time_formated, 21, "%Y-%m-%d_%H%M%S", tmp);
    output_path_root = "database/" + string(time_formated);
    input_path = "tle_collections";

    /* Configuration based on program arguments: ------------------------------------------------ */
    if(argc >= 1)
    {
        /*  OPTION      VALUE       DESCRIPTION:
         *  -t          folder path Path to the TLE folder.
         *  -o          folder paht Path to the results folder.
         *  -v          (none)      Verbose: outputs all data points as it generates them.
         *  -h          (none)      Shows the help menu.
         */
        int arg_iterator = 1;
        while(arg_iterator < argc)
        {
            string str = string(argv[arg_iterator]);
            if(str == "-t" && (arg_iterator + 1) < argc)
            {
                input_path = string(argv[arg_iterator + 1]);
                arg_iterator++;
            }
            else if(str == "-o" && (arg_iterator + 1) < argc)
            {
                output_path_root = string(argv[arg_iterator + 1]) + "/" + string(time_formated);
                arg_iterator++;
            }
            else if(str == "-v")
            {
                verbose = true;
            }
            else
            {
                cout << "Unknown argument: \'" << str << "\'" << endl;
                printHelp();
                return -1;
            }
            arg_iterator++;
        }

        /* Create results folder: */
        system(string("mkdir -p " + output_path_root).c_str());     /* Linux/Bash-specific.       */

        /* Creating output file */
        output_path = output_path_root + "/output" + ".db";
        if((output_file = fopen(output_path.c_str(), "w+")) == NULL) {
            cerr << DBG_REDD "Unable to open file " << output_path << DBG_NOCOLOR << endl;
        }

        /* Write CSV headers to the output file. */
        fprintf(output_file, "NORAD's ID, inc, ecc, period, perig, long_a_n, m_an \n");

        if(verbose){
            printHeader(true);
        }

        /* Entering in the tle_directory. */
        if((tle_directory = opendir(input_path.c_str())) != NULL) {

            while((dir_entry = readdir(tle_directory)) != NULL) {
                if(strcmp(dir_entry->d_name, ".") && strcmp(dir_entry->d_name, "..")) {
                    string full_path = input_path + "/" + string(dir_entry->d_name);
                    if((tle_file = fopen(full_path.c_str(), "r")) != NULL) {
                       while(fgets(file_line, 80, tle_file) != NULL) {
                            if(sscanf(file_line, "%24[^\n\t\r]", sat_name) < 1) {
                                cerr << "Can't read satellite name, will try to continue."  << endl;
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

                            /* Getting satellite information. */
                            norad = stod(tle_line2.substr(3,5));
                            inc = stod(tle_line3.substr(9,8));
                            ecc = stod(tle_line3.substr(26,7)) / 10000000;
                            period = (1 / stod(tle_line3.substr(52,11))) * 24 * 60;
                            perig = stod(tle_line3.substr(34,8));
                            long_a_n = stod(tle_line3.substr(17,8));
                            m_an = stod(tle_line3.substr(43,8));

                            /*Writting CSV file. */
                            fprintf(output_file, "%10.0f,", norad);
                            fprintf(output_file, "%.3f,%.6f,%.3f,%.3f,%.3f,%.3f \n",
                            inc, ecc, period, perig, long_a_n, m_an);

                            if(verbose){
                                printf("┃ %6.0f ┃ %6.3f ┃ %9.6f ┃ %9.3f ┃ %9.3f ┃  %9.3f ┃ %9.3f ┃ \n",
                                norad, inc, ecc, period, perig, long_a_n, m_an);
                            }
                        }
                    }
                }
            }

            if(verbose){
                printFooter();
            }

        }

    }
}
