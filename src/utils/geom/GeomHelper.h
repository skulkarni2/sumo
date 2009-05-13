/****************************************************************************/
/// @file    GeomHelper.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// Some geometrical helpers
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GeomHelper_h
#define GeomHelper_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "Position2D.h"
#include "Position2DVector.h"
#include <utils/common/UtilExceptions.h>


#ifndef PI
#define PI 3.1415926535897932384626433832795
#endif

// ===========================================================================
// class definitions
// ===========================================================================
/**
 *
 */
class GeomHelper {
public:
    static bool intersects(SUMOReal x1b, SUMOReal y1b, SUMOReal x1e, SUMOReal y1e,
                           SUMOReal x2b, SUMOReal y2b, SUMOReal x2e, SUMOReal y2e);

    static bool intersects(const Position2D &p11, const Position2D &p12,
                           const Position2D &p21, const Position2D &p22);

    /// Returns the distance between both points
    static SUMOReal distance(const Position2D &p1, const Position2D &p2);
    /*
        static Position2DVector::const_iterator
            find_intersecting_line(const Position2D &p1, const Position2D &p2,
            const Position2DVector &poly, Position2DVector::const_iterator beg);
    */
    /*
    static Position2D intersection_position(const Position2D &p1,
        const Position2D &p2, const Position2DVector &poly,
        Position2DVector::const_iterator at);
        */
    static Position2D intersection_position(const Position2D &p11,
                                            const Position2D &p12, const Position2D &p21, const Position2D &p22);

//    static bool isWithin(const Position2DVector &poly, const Position2D &p);
    static SUMOReal Angle2D(SUMOReal x1, SUMOReal y1, SUMOReal x2, SUMOReal y2);
    /*
        static Position2D position_at_length_position(
            const Position2DVector &poly, SUMOReal pos);
            */
    /*
        static Position2D position_at_length_position(const Position2D &p1,
            const Position2D &p2, SUMOReal pos);
    */

    static Position2D interpolate(const Position2D &p1,
                                  const Position2D &p2, SUMOReal length);

    static Position2D extrapolate_first(const Position2D &p1,
                                        const Position2D &p2, SUMOReal length);

    static Position2D extrapolate_second(const Position2D &p1,
                                         const Position2D &p2, SUMOReal length);

    static SUMOReal nearest_position_on_line_to_point(
        const Position2D &l1, const Position2D &l2,
        const Position2D &p);

    /** by Damian Coventry */
    static SUMOReal Magnitude(const Position2D &Point1,
                              const Position2D &Point2);
    /** by Damian Coventry */
    static SUMOReal DistancePointLine(const Position2D &Point,
                                      const Position2D &LineStart, const Position2D &LineEnd);

    /**
     * Return the distance from point to line as well as the intersection point.
     * If intersection does not lie within the line segment, the  start or end point of the segment is returned
     */
    static SUMOReal closestDistancePointLine(const Position2D &Point,
            const Position2D &LineStart, const Position2D &LineEnd,
            Position2D& outIntersection);

    static Position2D transfer_to_side(Position2D &p,
                                       const Position2D &lineBeg, const Position2D &lineEnd,
                                       SUMOReal amount);


    static Position2D crossPoint(const Boundary &b,
                                 const Position2DVector &v);

    static std::pair<SUMOReal, SUMOReal> getNormal90D_CW(SUMOReal x1, SUMOReal y1,
            SUMOReal x2, SUMOReal y2, SUMOReal length, SUMOReal wanted_offset) throw(InvalidArgument);

    static std::pair<SUMOReal, SUMOReal> getNormal90D_CW(const Position2D &beg,
            const Position2D &end, SUMOReal length, SUMOReal wanted_offset);

    static std::pair<SUMOReal, SUMOReal> getNormal90D_CW(const Position2D &beg,
            const Position2D &end, SUMOReal wanted_offset);

    /** @brief Returns the distance of second angle from first angle counter-clockwise
     * @param[in] angle1 The first angle
     * @param[in] angle2 The second angle
     * @return Angle (counter-clockwise) starting from first to second angle
     */
    static SUMOReal getCCWAngleDiff(SUMOReal angle1, SUMOReal angle2) throw();


    /** @brief Returns the distance of second angle from first angle clockwise
     * @param[in] angle1 The first angle
     * @param[in] angle2 The second angle
     * @return Angle (clockwise) starting from first to second angle
     */
    static SUMOReal getCWAngleDiff(SUMOReal angle1, SUMOReal angle2) throw();


    /** @brief Returns the minimum distance (clockwise/counter-clockwise) between both angles
     * @param[in] angle1 The first angle
     * @param[in] angle2 The second angle
     * @return The minimum distance between both angles
     */
    static SUMOReal getMinAngleDiff(SUMOReal angle1, SUMOReal angle2) throw();


    /** @brief Returns the maximum distance (clockwise/counter-clockwise) between both angles
     * @param[in] angle1 The first angle
     * @param[in] angle2 The second angle
     * @return The maximum distance between both angles
     */
    static SUMOReal getMaxAngleDiff(SUMOReal angle1, SUMOReal angle2) throw();


    /*
        static void sortAsPolyCWByAngle(Position2DVector &p);

        class as_poly_cw_sorter {
        public:
            /// constructor
            explicit as_poly_cw_sorter(Position2D center);

        public:
            /// comparing operation
            int operator() (const Position2D &p1, const Position2D &p2) const;

        private:
            /// the edge to compute the relative angle of
            Position2D myCenter;

        };
    */


};


#endif

/****************************************************************************/

