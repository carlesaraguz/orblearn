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

#ifndef __INCL_ORBPROP_
#define __INCL_ORBPROP_

/*** INCLUDE SECTION ******************************************************************************/
/* Standard C++11 libraries: */
#include <iostream>
#include <fstream>
#include <string.h>
#include <dirent.h>
#include <stdio.h>
#include <map>
#include <ctime>
#include <cstdlib>

/*** GLOBAL CONSTANTS *****************************************************************************/

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


#endif
