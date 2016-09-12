/***********************************************************************************************//**
 *  \brief      Orbit propagator.
 *  \details
 *  \author     Carles Araguz, carles.araguz@upc.edu
 *  \version    0.1
 *  \date       09-sep-2016
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
// Math library:
// #include "uMath.h"


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
