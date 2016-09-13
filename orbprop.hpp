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

/*** INCLUDE SECTION ********************************************************************************/
// Standard libraries:
#include <iostream>
#include <ctime>
#include <cstdio>
#include <string>
#include <map>
#include <assert.h>
#include <math.h>
// Open-source NORAD SGP4 orbit propagation library (by Michael F. Henry):
#include "stdafx.h"         // orbitTools main header file.
#include "coreLib.h"        // orbitTools core library.
#include "orbitLib.h"       // orbitTools orbit library.


/*** GLOBAL CONSTANTS *******************************************************************************/

/*** TYPEDEFS ***************************************************************************************/
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

/*** GLOBAL VARIABLES *******************************************************************************/

/*** FUNCTIONS **************************************************************************************/

#endif


// EOF
