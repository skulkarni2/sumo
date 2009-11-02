/****************************************************************************/
/// @file    MSEdgeWeightsStorage.h
/// @author  Daniel Krajzewicz
/// @date    02.11.2009
/// @version $Id: MSEdgeWeightsStorage.h 7442 2009-07-15 19:14:55Z dkrajzew $
///
// A storage for edge travel times and efforts
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
#ifndef MSEdgeWeightsStorage_h
#define MSEdgeWeightsStorage_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/common/SUMOTime.h>
#include <utils/common/ValueTimeLine.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSEdge;
class MSVehicle;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSEdgeWeightsStorage
 * @brief A storage for edge travel times and efforts
 */
class MSEdgeWeightsStorage {
public:
    /// @brief Constructor
    MSEdgeWeightsStorage() throw();


    /// @brief Destructor
    ~MSEdgeWeightsStorage() throw();


    /** @brief Returns a travel time for an edge and time if stored
     * @param[in] e The edge for which the travel time shall be retrieved
     * @param[in] v Unused
     * @param[in] t The time for which the travel time shall be retrieved
     * @param[in] value The value if the requested edge/time is described
     * @return Whether the requested edge/time is described
     */
    bool retrieveExistingTravelTime(const MSEdge * const e, const MSVehicle * const v, 
        SUMOTime t, SUMOReal &value) const throw();


    /** @brief Returns an effort for an edge and time if stored
     * @param[in] e The edge for which the effort shall be retrieved
     * @param[in] v Unused
     * @param[in] t The time for which the effort shall be retrieved
     * @param[in] value The value if the requested edge/time is described
     * @return Whether the requested edge/time is described
     */
    bool retrieveExistingEffort(const MSEdge * const e, const MSVehicle * const v, 
        SUMOTime t, SUMOReal &value) const throw();


    /** @brief Adds a travel time information for an edge and a time span
     * @param[in] e The described edge
     * @param[in] begin The begin of the described time span
     * @param[in] end The end of the described time span
     * @param[in] value The travel time value for this edge and time span
     */
    void addTravelTime(const MSEdge * const e, SUMOTime begin, SUMOTime end, SUMOReal value) throw();


    /** @brief Adds an effort information for an edge and a time span
     * @param[in] e The described edge
     * @param[in] begin The begin of the described time span
     * @param[in] end The end of the described time span
     * @param[in] value Theeffort value for this edge and time span
     */
    void addEffort(const MSEdge * const e, SUMOTime begin, SUMOTime end, SUMOReal value) throw();


private:
    /// @brief A map of edge->time->travel time
    std::map<MSEdge*, ValueTimeLine<SUMOReal> > myTravelTimes;

    /// @brief A map of edge->time->effort
    std::map<MSEdge*, ValueTimeLine<SUMOReal> > myEfforts;


private:
    /// @brief Invalidated copy constructor.
    MSEdgeWeightsStorage(const MSEdgeWeightsStorage&);

    /// @brief Invalidated assignment operator.
    MSEdgeWeightsStorage& operator=(const MSEdgeWeightsStorage&);


};


#endif

/****************************************************************************/
