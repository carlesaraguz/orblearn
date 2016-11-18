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

#ifndef __TLE_HISTORIC_SET__
#define __TLE_HISTORIC_SET__

struct TLECompare {
    bool operator()(Zeptomoby::OrbitTools::cTle const & ta, Zeptomoby::OrbitTools::cTle const & tb) const
    {
        return ta.getTLEtime() < tb.getTLEtime();
    }
};

class TLEHistoricSet
{
    int sat_id;
    string sat_name;
    std::set<Zeptomoby::OrbitTools::cTle, TLECompare> data;

public:
    TLEHistoricSet(int id);
    TLEHistoricSet(int sat_id, string sat_name);
    TLEHistoricSet(int sat_id, string sat_name, Zeptomoby::OrbitTools::cTle tle_init);
    bool addTLE(Zeptomoby::OrbitTools::cTle tle);
    int getSize(void);
    void displayData(void);
    void propagate(std::string output_path_root, std::time_t prop_time_start,
        std::time_t prop_time_end, std::time_t prop_time_step, int prop_n_points, bool verbose);

};


class TLEHistoricSetException: public exception
{
    virtual const char* what() const throw() {
        return "TLE Historic Set exception: missing orbit details from the past (i.e. start time is before the first TLE time)";
    }
};

/* Forward declaration of helper functions: */
void printHeader(bool first);
void printFooter(void);

#endif /* __TLE_HISTORIC_SET__ */
