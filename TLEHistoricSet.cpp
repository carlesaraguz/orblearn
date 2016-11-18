/***********************************************************************************************//**
 *  \brief      Orbit propagator: TLE Historic Set.
 *  \details    Helper class that stores TLE data when this is fragmented in pieces.
 *  \author     Carles Araguz, carles.araguz@upc.edu
 *  \version    0.1
 *  \date       03-nov-2016
 *  \copyright  GNU Public License (v3). This files are part of an on-going non-commercial research
 *              project at NanoSat Lab (http://nanosatlab.upc.edu) of the Technical University of
 *              Catalonia - UPC BarcelonaTech. Third-party libraries used in this framework might be
 *              subject to different copyright conditions.
 **************************************************************************************************/

#include "orbprop.hpp"

TLEHistoricSet::TLEHistoricSet(int id)
    : sat_id(id)
{
}

TLEHistoricSet::TLEHistoricSet(int id, string name)
    : sat_id(id), sat_name(name)
{
}

TLEHistoricSet::TLEHistoricSet(int id, string name, cTle tle_init)
    : sat_id(id), sat_name(name)
{
    addTLE(tle_init);
}

bool TLEHistoricSet::addTLE(Zeptomoby::OrbitTools::cTle tle)
{
    std::pair<std::set<Zeptomoby::OrbitTools::cTle>::iterator, bool> ret = data.insert(tle);
    return ret.second;
}

int TLEHistoricSet::getSize(void)
{
    return data.size();
}

void TLEHistoricSet::displayData(void)
{
    int count = 0;
    cout << "Displaying data for " << (sat_name == "" ? "(unknown name)" : sat_name) << ": " << sat_id << endl;
    for(auto i = data.begin(); i != data.end(); i++) {
        cout << (count++) << " (" << sat_id << "): " << i->getTLEtime() << endl;
    }
}

void TLEHistoricSet::propagate(std::string output_path_root, std::time_t prop_time_start,
    std::time_t prop_time_end, std::time_t prop_time_step, int prop_n_points, bool verbose)
{
    std::FILE * output_file;    /* Propagation file. Results will be written here in CSV format.  */
    int prop_step_count = 0;    /* Partial-propagation counter.                                   */
    time_t prop_time_curr;      /* Propagations's current time.                                   */
    time_t tle_time;            /* Time extracted from TLE files.                                 */
    time_t tle_time_next;       /* Time extracted from TLE files (corresponds to next iteration.) */
    struct tm *tmp;             /* Time struct (debug purposes).                                  */
    char time_formated[21];     /* Time in the format "yyyy-mm-dd hh:mm:ss"                       */
    int prop_inner_step_count = 1;  /* Propagation's steps counter.                               */
    int tt;                         /* Internal time iterator for each propagation step.          */
    int tt_remain = 0;              /* The remaining (in seconds) of incomplete steps.            */

    /* Create/open file: */
    std::string output_path = output_path_root + "/" + std::to_string(sat_id) + ".prop";
    if((output_file = fopen(output_path.c_str(), "w+")) == NULL) {
        cerr << DBG_REDD "Unable to open file " << output_path << DBG_NOCOLOR << endl;
        exit(-1);
    } else {
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
    }

    bool initial_TLE_found = false;
    std::set<Zeptomoby::OrbitTools::cTle>::iterator i; // TLE iterator.
    std::set<Zeptomoby::OrbitTools::cTle>::iterator j; // TLE iterator with offset -1.
    for(i = data.begin(); i != data.end(); i++) {
        prop_step_count++;
        /* Create an Orbit object using the satellite TLE object. */
        Zeptomoby::OrbitTools::cOrbit orbit(*i);
        Zeptomoby::OrbitTools::cEciTime satellite = orbit.PositionEci(0.0);
        tle_time = satellite.Date().ToTime();
        tle_time_next = 0;

        if(i == data.begin() && prop_time_start < tle_time) {
            /*  The propagation start time for this set of TLE's is set before the earliest TLE
             *  time.
             */
            printf("  %5d (%s) %-3d [%3.0f%%] " DBG_REDD "error(2)" DBG_NOCOLOR": Earliest TLE time is %ld.\n",
                sat_id, output_path.c_str(), prop_step_count, 0.0, tle_time);
            throw TLEHistoricSetException();
        }
        if(tle_time >= prop_time_end) {
            break;
        }

        j = std::next(i);
        if(j != data.end()) {
            Zeptomoby::OrbitTools::cOrbit orbit_next(*j);
            Zeptomoby::OrbitTools::cEciTime satellite_next = orbit_next.PositionEci(0.0);
            tle_time_next = satellite_next.Date().ToTime();
        }

        /* Loop control: ------------------------------------------------------------------------ */
        if(!initial_TLE_found && j != data.end()) {
            /* This (`i`) TLE is not the last one and the initial TLE is still unknown. */
            if(tle_time_next <= prop_time_start) {
                /* This TLE can be skipped. */
                continue;
            } else {
                /* This (`i`) is the initial TLE. */
                initial_TLE_found = true;
            }
        } else if(!initial_TLE_found && j == data.end()) {
            /*  The initial TLE has not been found, and this is the last one. */
            if(tle_time <= prop_time_start) {
                /* This (`i`) is the initial TLE (and the only one). */
                initial_TLE_found = true;
                tle_time_next = prop_time_end;
            } else {
                /*  This (`i`) is the last TLE in the historic set, but the initial one with which
                 *  the propagation has to start has not been detected yet. This should throw an
                 *  exception because the orbital parameters are unknown for a part of the given
                 *  propagation span.
                 */
                printf("  %5d (%s) %-3d [%3.0f%%] " DBG_REDD "error(1)" DBG_NOCOLOR".\n", sat_id, output_path.c_str(), prop_step_count, 0.0);
                throw TLEHistoricSetException();
            }
        } /* else --> initial_TLE_found == true: */

        /* Configure partial-propagation start and end times: ----------------------------------- */
        time_t current_prop_time_start;
        time_t current_prop_time_end;
        if(prop_time_start >= tle_time && prop_time_start < tle_time_next) {
            current_prop_time_start = (prop_time_start - tle_time);
        } else {
            current_prop_time_start = 0 + tt_remain;
        }

        if(j == data.end() || tle_time_next >= prop_time_end) {
            /* This is the last TLE: */
            current_prop_time_end = (prop_time_end - tle_time);
        } else {
            current_prop_time_end = (tle_time_next - tle_time);
        }

        /* Perform partial-propagation: --------------------------------------------------------- */
        Zeptomoby::OrbitTools::cGeo proj_earth = Zeptomoby::OrbitTools::cGeo(satellite, satellite.Date());

        /* Iterate through time as defined in the input arguments: */
        if(verbose) {
            printHeader(true);
        } else {
            printf("  %5d (%s) %-3d [%3.0f%%] %d pp.\r", sat_id, output_path.c_str(), prop_step_count,
                (100.0 * (tle_time - prop_time_start) / (prop_time_end - prop_time_start)),
                prop_inner_step_count);
            fflush(stdout);
        }

        for(tt = current_prop_time_start; tt <= current_prop_time_end; tt += prop_time_step)
        {
            prop_inner_step_count++;
            if(verbose && !(prop_inner_step_count % 50)) {
                printHeader(false);
            }

            try {
                satellite = orbit.PositionEci(tt / 60.0);
                prop_time_curr = satellite.Date().ToTime();
                proj_earth = Zeptomoby::OrbitTools::cGeo(satellite, satellite.Date());
            } catch(exception& e) {
                printf("  %5d (%s) %-3d [%3.0f%%] " DBG_REDD "error(3)" DBG_NOCOLOR": %s.\n",
                    sat_id, output_path.c_str(), prop_step_count, 0.0, e.what());
                break;
            }

            tmp = localtime(&prop_time_curr);
            strftime(time_formated, 21, "%Y-%m-%d %T", tmp);
            fprintf(output_file, "%s,%10ld,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f\n",
                time_formated, prop_time_curr,
                proj_earth.LatitudeDeg(),
                (proj_earth.LongitudeDeg() < 180 ? proj_earth.LongitudeDeg() : proj_earth.LongitudeDeg()-360),
                satellite.Position().m_x, satellite.Position().m_y, satellite.Position().m_z,
                satellite.Velocity().m_x, satellite.Velocity().m_y, satellite.Velocity().m_z);

            if(verbose) {
                printf("│% 10ld (%s) │ % 11.6f % 11.6f │ % 13.6f % 13.6f % 13.6f │ % 10.6f % 10.6f % 10.6f │\n",
                    prop_time_curr, time_formated,
                    proj_earth.LatitudeDeg(),
                    (proj_earth.LongitudeDeg() < 180 ? proj_earth.LongitudeDeg() : proj_earth.LongitudeDeg()-360),
                    satellite.Position().m_x, satellite.Position().m_y, satellite.Position().m_z,
                    satellite.Velocity().m_x, satellite.Velocity().m_y, satellite.Velocity().m_z);
            } else if(prop_inner_step_count % 512 == 0) {
                printf("  %d (%s) %-3d [%3.0f%%] %d pp.\r", sat_id, output_path.c_str(), prop_step_count,
                    (100.0 * (tle_time - prop_time_start + tt) / (prop_time_end - prop_time_start)),
                    prop_inner_step_count);
                fflush(stdout);
            }
        }
        tt_remain = tt - current_prop_time_end;

        if(verbose) {
            printFooter();
        } else {
            printf("  %5d (%s) %-3d [%3.0f%%] %d pp.\r", sat_id, output_path.c_str(), prop_step_count,
                (100.0 * ( (tle_time - prop_time_start + tt) <= (prop_time_end - prop_time_start) ?
                (tle_time - prop_time_start + tt) : (prop_time_end - prop_time_start) ) /
                (prop_time_end - prop_time_start) ),
                prop_inner_step_count);
            fflush(stdout);
        }
    }
    if(!verbose) {
        printf("\n");
    }
    fclose(output_file);
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
