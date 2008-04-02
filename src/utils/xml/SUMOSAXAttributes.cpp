/****************************************************************************/
/// @file    SUMOSAXAttributes.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 30 Mar 2007
/// @version $Id: SUMOSAXAttributes.h 5002 2008-02-01 13:46:21Z dkrajzew $
///
// Encapsulated SAX-Attributes
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include "SUMOSAXAttributes.h"
#include <utils/common/MsgHandler.h>
#include <iostream>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
bool 
SUMOSAXAttributes::setIDFromAttribues(const char * objecttype, 
                                      std::string &id, bool report) const throw()
{
    id = "";
    if(hasAttribute(SUMO_ATTR_ID)) {
        id = getString(SUMO_ATTR_ID);
    }
    if(id=="") {
        if(report) {
            MsgHandler::getErrorInstance()->inform(string("Missing id of a '") + string(objecttype) + string("'-object."));
        }
        return false;
    }
    return true;
}


/****************************************************************************/
