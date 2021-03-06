/****************************************************************************/
/// @file    ROVehicle.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A vehicle as used by router
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef ROVehicle_h
#define ROVehicle_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <iostream>
#include <utils/common/SUMOTime.h>
#include <utils/common/RGBColor.h>
#include <utils/common/SUMOVehicleParameter.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/SUMOAbstractRouter.h>


// ===========================================================================
// class declarations
// ===========================================================================
class SUMOVTypeParameter;
class RORouteDef;
class OutputDevice;
class ROEdge;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class ROVehicle
 * @brief A vehicle as used by router
 */
class ROVehicle {
public:
    /** @brief Constructor
     *
     * @param[in] pars Parameter of this vehicle
     * @param[in] route The definition of the route the vehicle shall use
     * @param[in] type The type of the vehicle
     *
     * @todo Why is the vehicle builder given?
     */
    ROVehicle(const SUMOVehicleParameter &pars,
              RORouteDef *route, SUMOVTypeParameter *type) throw();


    /// @brief Destructor
    virtual ~ROVehicle() throw();


    /** @brief Returns the definition of the route the vehicle takes
     *
     * @return The vehicle's route definition
     *
     * @todo Why not return a reference?
     */
    RORouteDef * const getRouteDefinition() const throw() {
        return myRoute;
    }


    /** @brief Returns the type of the vehicle
     *
     * @return The vehicle's type
     *
     * @todo Why not return a reference?
     */
    const SUMOVTypeParameter * const getType() const throw() {
        return myType;
    }


    /** @brief Returns the id of the vehicle
     *
     * @return The id of the vehicle
     */
    const std::string &getID() const throw() {
        return myParameter.id;
    }


    /** @brief Returns the time the vehicle starts at
     *
     * @return The vehicle's depart time
     */
    SUMOTime getDepartureTime() const throw() {
        return myParameter.depart;
    }


    /** @brief Saves the complete vehicle description.
     *
     * Saves the vehicle type if it was not saved before.
     * Saves the vehicle route if it was not saved before.
     * Saves the vehicle itself.
     *
     * @param[in] os The routes - output device to store the vehicle's description into
     * @param[in] altos The route alternatives - output device to store the vehicle's description into
     * @param[in] route !!!describe
     * @exception IOError If something fails (not yet implemented)
     * @todo What is the given route definition?
     */
    void saveAllAsXML(SUMOAbstractRouter<ROEdge,ROVehicle> &router,
                      OutputDevice &os, OutputDevice * const altos, bool withExitTimes) const throw(IOError);


    /** @brief Returns a copy of the vehicle using a new id, departure time and route
     *
     * @param[in] id the new id to use
     * @param[in] depTime The new vehicle's departure time
     * @param[in] newRoute The new vehicle's route
     * @return The new vehicle
     *
     * @todo Is this used? What for if everything is replaced?
     */
    virtual ROVehicle *copy(const std::string &id, unsigned int depTime, RORouteDef *newRoute) throw();


protected:
    /// @brief The vehicle's parameter
    SUMOVehicleParameter myParameter;

    /// @brief The type of the vehicle
    SUMOVTypeParameter *myType;

    /// @brief The route the vehicle takes
    RORouteDef *myRoute;


private:
    /// @brief Invalidated copy constructor
    ROVehicle(const ROVehicle &src);

    /// @brief Invalidated assignment operator
    ROVehicle &operator=(const ROVehicle &src);

};


#endif

/****************************************************************************/

