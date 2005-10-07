/***************************************************************************
                          NetElements.cpp
                             -------------------
    project              : SUMO
    begin                : Mar, 2003
    copyright            : (C) 2003 by DLR/IVF http://ivf.dlr.de/
    author               : Markus Hartinger
    email                : Markus.Hartinger@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.21  2005/10/07 11:48:01  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.20  2005/09/23 06:13:19  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.19  2005/09/15 12:27:08  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.18  2005/07/12 12:55:28  dkrajzew
// build number output added
//
// Revision 1.17  2005/05/04 09:33:43  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.16  2005/02/17 10:33:29  dkrajzew
// code beautifying;
// Linux building patched;
// warnings removed;
// new configuration usage within guisim
//
// Revision 1.15  2004/12/20 13:15:58  dkrajzew
// options output corrected
//
// Revision 1.14  2004/12/16 12:17:29  dkrajzew
// a further network prune option added
//
// Revision 1.13  2004/11/23 10:43:29  dkrajzew
// debugging
//
// Revision 1.12  2004/07/02 09:50:54  dkrajzew
// options changes applied
//
// Revision 1.11  2004/02/06 08:54:28  dkrajzew
// _INC_MALLOC definition removed (does not work on MSVC7.0)
//
// Revision 1.10  2003/12/04 12:51:26  dkrajzew
// documentation added; possibility to use actuated and agentbased junctions added; usage of street types patched
//
// Revision 1.9  2003/10/28 08:35:01  dkrajzew
// random number specification options added
//
// Revision 1.8  2003/09/05 14:40:42  dkrajzew
// options for network building are now commoly set within NBNetBuilder
//
// Revision 1.7  2003/08/21 13:00:57  dkrajzew
// random networks debugging
//
// Revision 1.6  2003/07/30 09:30:11  dkrajzew
// parameter to specify the duration of the red phase of a traffic light added
//
// Revision 1.5  2003/07/22 15:17:42  dkrajzew
// traffic light duration may now be changed on command line
//
// Revision 1.4  2003/07/21 11:05:30  dkrajzew
// patched some bugs found in first real-life execution
//
// Revision 1.3  2003/07/18 12:35:04  dkrajzew
// removed some warnings
//
// Revision 1.2  2003/07/17 12:14:21  dkrajzew
// NBLoader moved to netimport; NBNetBuilder performs now all the building steps
//
// Revision 1.1  2003/07/16 15:41:17  dkrajzew
// network generator added
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <netbuild/NBNetBuilder.h>
#include <netgen/NGNet.h>
#include <netgen/NGRandomNet.h>
#include <netbuild/NBTypeCont.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/Option.h>
#include <utils/options/OptionsSubSys.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/SystemFrame.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/RandHelper.h>
#include <utils/common/DevHelper.h>
#include <utils/common/ToString.h>
#include "netgen_help.h"
#include "netgen_build.h"
#include "sumo_version.h"


/* =========================================================================
 * debugging definitions (MSVC++ only)
 * ======================================================================= */
#ifdef _DEBUG
   #define _CRTDBG_MAP_ALLOC // include Microsoft memory leak detection procedures
//   #define _INC_MALLOC         // exclude standard memory alloc procedures
#ifdef WIN32
//   #include <utils/dev/MemDiff.h>
#endif

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG
#endif


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
bool
checkOptions(OptionsCont &oc)
{
    // check whether the output is valid and can be build
    if(!oc.isSet("o")) {
        MsgHandler::getErrorInstance()->inform("No output specified.");
        return false;
    }
    std::ofstream tst(oc.getString("o").c_str());
    if(!tst.good()) {
        MsgHandler::getErrorInstance()->inform(
            string("The output file '") + oc.getString("o")
            + string("' can not be build."));
        return false;
    }
    // check whether exactly one type of a network to build was wished
    int no = 0;
    if(oc.getBool("s")) no++;
    if(oc.getBool("g")) no++;
    if(oc.getBool("r")) no++;
    if(no==0) {
        MsgHandler::getErrorInstance()->inform(
            "You have to specify the type of network to generate.");
        return false;
    }
    if(no>1) {
        MsgHandler::getErrorInstance()->inform(
            "You may specify only one type of network to generate at once.");
        return false;
    }
    // check whether the junction type to use is properly set
    if(!oc.isDefault("default-junction-type")) {
        string type = oc.getString("default-junction-type");
        if(type!="traffic_light"&&type!="priority"&&type!="actuated"&&type!="agentbased") {
            MsgHandler::getErrorInstance()->inform(
                "Only the following junction types are known: traffic_light, priority, actuated, agentbased");
            return false;
        }
    }
    //
    return true;
}


/** build and retrieve the options (settings) */
void
fillOptions(OptionsCont &oc)
{
    // register the file i/o options
    oc.doRegister("random-net", 'r', new Option_Bool(false));
    oc.doRegister("spider-net", 's', new Option_Bool(false));
    oc.doRegister("grid-net", 'g', new Option_Bool(false));
    oc.doRegister("output", 'o', new Option_FileName());
    oc.doRegister("configuration-file", 'c', new Option_FileName());
    oc.addSynonyme("random-net", "random");
    oc.addSynonyme("spider-net", "spider");
    oc.addSynonyme("grid-net", "grid");
    oc.addSynonyme("output-file", "output");
    oc.addSynonyme("configuration-file", "configuration");
    // register random-net options
    oc.doRegister("rand-max-distance", new Option_Float(250));
    oc.doRegister("rand-min-distance", new Option_Float(100));
    oc.doRegister("rand-min-angle", new Option_Float((SUMOReal) (45.0/180.0*PI)));
    oc.doRegister("rand-num-tries", new Option_Integer(50));
    oc.doRegister("rand-connectivity", new Option_Float((SUMOReal) 0.95));
    oc.doRegister("rand-neighbor-dist1", new Option_Float(0));
    oc.doRegister("rand-neighbor-dist2", new Option_Float(0));
    oc.doRegister("rand-neighbor-dist3", new Option_Float(10));
    oc.doRegister("rand-neighbor-dist4", new Option_Float(10));
    oc.doRegister("rand-neighbor-dist5", new Option_Float(2));
    oc.doRegister("rand-neighbor-dist6", new Option_Float(1));
    oc.doRegister("rand-iterations", new Option_Integer(2000));
    oc.addSynonyme("rand-max-distance", "max-dist");
    oc.addSynonyme("rand-min-distance", "min-dist");
    oc.addSynonyme("rand-min-angle", "min-angle");
    oc.addSynonyme("rand-num-tries", "num-tries");
    oc.addSynonyme("rand-connectivity", "connectivity");
    oc.addSynonyme("rand-neighbor-dist1", "dist1");
    oc.addSynonyme("rand-neighbor-dist2", "dist2");
    oc.addSynonyme("rand-neighbor-dist3", "dist3");
    oc.addSynonyme("rand-neighbor-dist4", "dist4");
    oc.addSynonyme("rand-neighbor-dist5", "dist5");
    oc.addSynonyme("rand-neighbor-dist6", "dist6");
    oc.addSynonyme("rand-iterations", "iterations");
    // register spider-net options
    oc.doRegister("spider-arm-number", new Option_Integer(13));
    oc.doRegister("spider-circle-number", new Option_Integer(20));
    oc.doRegister("spider-space-rad", new Option_Float(100));
    oc.addSynonyme("spider-arm-number", "arms");
    oc.addSynonyme("spider-circle-number", "circles");
    oc.addSynonyme("spider-space-rad", "radius");
//    oc.doRegister("spider-override-priority-center", new Option_Bool(false));
    // register grid-net options
    oc.doRegister("grid-x-number", new Option_Integer(5));
    oc.doRegister("grid-y-number", new Option_Integer(5));
    oc.doRegister("grid-x-length", new Option_Float(100));
    oc.doRegister("grid-y-length", new Option_Float(100));
    oc.doRegister("grid-number", new Option_Integer(5));
    oc.doRegister("grid-length", new Option_Float(100));
    oc.addSynonyme("grid-x-number", "x-no");
    oc.addSynonyme("grid-y-number", "y-no");
    oc.addSynonyme("grid-x-length", "x-length");
    oc.addSynonyme("grid-y-length", "y-length");
    oc.addSynonyme("grid-length", "length");
    oc.addSynonyme("grid-number", "number");
    // register building options
    oc.doRegister("default-junction-type", 'j', new Option_String("priority"));
    oc.addSynonyme("default-junction-type", "junctions");
    // add netbuilding options
    NBNetBuilder::insertNetBuildOptions(oc);
    // add rand and dev options
    DevHelper::insertDevOptions(oc);
    RandHelper::insertRandOptions(oc);
}


TNGNet *
buildNetwork(NBNetBuilder &nb)
{
    TNGNet *net = new TNGNet(nb);
    OptionsCont &oc = OptionsSubSys::getOptions();
    // spider-net
    if(oc.getBool("s")) {
        net->CreateSpiderWeb(
            oc.getInt("arms"),
            oc.getInt("circles"),
            oc.getFloat("radius"));
        return net;
    }
    // grid-net
    if(oc.getBool("g")) {
        int xNo = oc.getInt("x-no");
        int yNo = oc.getInt("y-no");
        SUMOReal xLength = oc.getFloat("x-length");
        SUMOReal yLength = oc.getFloat("y-length");
        if(oc.isDefault("x-no")&&!oc.isDefault("number")) {
            xNo = oc.getInt("number");
        }
        if(oc.isDefault("y-no")&&!oc.isDefault("number")) {
            yNo = oc.getInt("number");
        }
        if(oc.isDefault("x-length")&&!oc.isDefault("length")) {
            xLength = oc.getFloat("length");
        }
        if(oc.isDefault("y-length")&&!oc.isDefault("length")) {
            yLength = oc.getFloat("length");
        }
        net->CreateChequerBoard(xNo, yNo, xLength, yLength);
        return net;
    }
    // random net
    TNGRandomNet RandomNet(net);
    RandomNet.SetMaxDistance(oc.getFloat("rand-max-distance"));
    RandomNet.SetMinDistance(oc.getFloat("rand-min-distance"));
    RandomNet.SetMinLinkAngle(oc.getFloat("min-angle"));
    RandomNet.SetNumTries(oc.getInt("num-tries"));
    RandomNet.SetConnectivity(oc.getFloat("connectivity"));
    RandomNet.NeighbourDistribution.Add(1, oc.getFloat("dist1"));
    RandomNet.NeighbourDistribution.Add(2, oc.getFloat("dist2"));
    RandomNet.NeighbourDistribution.Add(3, oc.getFloat("dist3"));
    RandomNet.NeighbourDistribution.Add(4, oc.getFloat("dist4"));
    RandomNet.NeighbourDistribution.Add(5, oc.getFloat("dist5"));
    RandomNet.NeighbourDistribution.Add(6, oc.getFloat("dist6"));
    RandomNet.CreateNet(oc.getInt("iterations"));
    return net;
}



int
main(int argc, char **argv)
{
#ifdef _DEBUG
#ifdef WIN32
    //CMemDiff state1;
    // uncomment next line and insert the context of an undeleted
    //  allocation to break within it (MSVC++ only)
    // _CrtSetBreakAlloc(434490);
#endif
#endif
    int ret = 0;
    try {
        // initialise the application system (messaging, xml, options)
        int init_ret = SystemFrame::init(false, argc, argv,
            fillOptions, checkOptions, help);
        if(init_ret==-1) {
            cout << "SUMO netgen" << endl;
            cout << " Version " << version << endl;
            cout << " Build #" << NEXT_BUILD_NUMBER << endl;
            SystemFrame::close();
            return 0;
        } else if(init_ret!=0) {
            throw ProcessError();
        }
        // initialise the (default) types
        OptionsCont &oc = OptionsSubSys::getOptions();
        NBNetBuilder nb;
        nb.getTypeCont().setDefaults(
            oc.getString("T"), oc.getInt("L"), oc.getFloat("S"),
            oc.getInt("P"));
        // build the netgen-network description
        TNGNet *net = buildNetwork(nb);
        // ... and we have to do this...
        oc.resetWritable();
        oc.set("no-node-removal", true);
        // transfer to the netbuilding structures
        NBNetBuilder::preCheckOptions(oc);
        net->toNB();
        delete net;
        nb.buildLoaded();
    } catch (...) {
        MsgHandler::getMessageInstance()->inform(
            "Quitting (building failed).");
        ret = 1;
    }
    SystemFrame::close();
    if(ret==0) {
        WRITE_MESSAGE("Success.");
    }
    return ret;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


