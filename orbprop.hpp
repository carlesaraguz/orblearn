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


#ifndef __INCL_ORBPROP_
#define __INCL_ORBPROP_

/*** INCLUDE SECTION ******************************************************************************/
/* Standard C++11 libraries: */
#include <iostream>
#include <ctime>
#include <cstdio>
#include <string>
#include <set>
#include <assert.h>
#include <math.h>
#include <sys/types.h>
#include <dirent.h>

/* Open-source NORAD SGP4 C++ Implementation library (by Michael F. Henry): */
#include "stdafx.h"         /* orbitTools main header file. */
#include "coreLib.h"        /* orbitTools core library.     */
#include "orbitLib.h"       /* orbitTools orbit library.    */


/*** GLOBAL CONSTANTS *****************************************************************************/
#define CONF_FILE_PATH  "orbprop.conf"

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


/*** TYPEDEFS *************************************************************************************/
typedef struct
{
    double lat;
    double lon;
    double eci_x;
    double eci_y;
    double eci_z;
    double vel_x;
    double vel_y;
    double vel_z;
    long long time;
} OrbitPropagation;

/*** GLOBAL VARIABLES *****************************************************************************/

/*** FUNCTIONS ************************************************************************************/

#endif
