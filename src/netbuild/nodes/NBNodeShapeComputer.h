/****************************************************************************/
/// @file    NBNodeShapeComputer.h
/// @author  Daniel Krajzewicz
/// @date    2004-01-12
/// @version $Id$
///
// This class computes shapes of junctions
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
#ifndef NBNodeShapeComputer_h
#define NBNodeShapeComputer_h
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <fstream>
#include <utils/geom/Position2DVector.h>


// ===========================================================================
// class definitions
// ===========================================================================
class NBNode;
class NBEdge;


// ===========================================================================
// class declarations
// ===========================================================================
/**
 * @class NBNodeShapeComputer
 * brief This class computes shapes of junctions
 */
class NBNodeShapeComputer
{
public:
    /// Constructor
    NBNodeShapeComputer(const NBNode &node, std::ofstream * const out);

    /// Destructor
    ~NBNodeShapeComputer();

    /// Computes the shape of the assigned junction
    Position2DVector compute();

private:
    /// Adds internal geometry information
    void addInternalGeometry();

    Position2DVector computeContinuationNodeShape(bool simpleContinuation);
    Position2DVector computeNodeShapeByCrosses();
    void replaceLastChecking(Position2DVector &g, bool decenter,
        Position2DVector counter, size_t counterLanes, SUMOReal counterDist,
        int laneDiff);
    void replaceFirstChecking(Position2DVector &g, bool decenter,
        Position2DVector counter, size_t counterLanes, SUMOReal counterDist,
        int laneDiff);

private:
    /// The node to compute the geometry for
    const NBNode &myNode;

    /// The stream to write the node shape pois to
    std::ofstream * const myNodeShapePOIOut;

};


#endif

/****************************************************************************/

