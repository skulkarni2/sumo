#ifndef MSDETECTORCONTAINERWRAPPER_H
#define MSDETECTORCONTAINERWRAPPER_H

/**
 * @file   MSDetectorContainerWrapper.h
 * @author Christian Roessel
 * @date   Started Fri Sep 26 19:11:26 2003
 * @version $Id$
 * @brief
 *
 *
 */

/* Copyright (C) 2003 by German Aerospace Center (http://www.dlr.de) */

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//

#include "MSDetectorContainerWrapperBase.h"
#include "MSVehicle.h"
#include "MSPredicates.h"
#include <algorithm>
#include <functional>
#include <list>

template< class WrappedContainer >
struct MSDetectorContainerWrapper : public MSDetectorContainerWrapperBase
{
    typedef typename WrappedContainer::const_iterator ContainerConstIt;
    typedef WrappedContainer InnerContainer;

    void enterDetectorByMove( MSVehicle* veh )
        {
//             typedef typename WrappedContainer::value_type ContainerItem;
//             containerM.push_front( ContainerItem( veh ) );
            enterDetectorByEmitOrLaneChange( veh );
        }
 
    void enterDetectorByEmitOrLaneChange( MSVehicle* veh )
        {
            typedef typename WrappedContainer::value_type ContainerItem;
            typedef typename WrappedContainer::iterator ContainerIt;
            typedef typename Predicate::PosGreaterC< ContainerItem >
                PosGreaterPredicate;
            ContainerIt insertIt =
                std::find_if( containerM.begin(), containerM.end(),
                              std::bind2nd(
                                  PosGreaterPredicate(), veh->pos() ) );
            containerM.insert( insertIt, ContainerItem( veh ) );
        }

    void leaveDetectorByMove( MSVehicle* veh )
        {
            typedef typename WrappedContainer::value_type ContainerItem;
            typedef typename WrappedContainer::iterator ContainerIt;
            typedef typename Predicate::VehEqualsC< ContainerItem >
                ErasePredicate;
            ContainerIt eraseIt =
                std::find_if( containerM.begin(), containerM.end(),
                              std::bind2nd(
                                  ErasePredicate(), veh ) );
            assert(containerM.size()>0);
            assert(eraseIt!=containerM.end());
            containerM.erase( eraseIt );        
        }

    void leaveDetectorByLaneChange( MSVehicle* veh )
        {
//             typedef typename WrappedContainer::value_type ContainerItem;
//             typedef typename WrappedContainer::iterator ContainerIt;
//             typedef typename Predicate::VehEqualsC< ContainerItem >
//                 ErasePredicate;
//             ContainerIt eraseIt =
//                 std::find_if( containerM.begin(), containerM.end(),
//                               std::bind2nd(
//                                   ErasePredicate(), veh ) );
//             assert(containerM.size()>0);
//             assert(eraseIt!=containerM.end());
//             containerM.erase( eraseIt );
            leaveDetectorByMove( veh );
        }


    MSDetectorContainerWrapper(
        const MSDetectorOccupancyCorrection& occupancyCorrection )
        : MSDetectorContainerWrapperBase( occupancyCorrection ),
          containerM()
        {}

    virtual ~MSDetectorContainerWrapper( void )
        {
            containerM.clear();
        }

    WrappedContainer containerM;
};

namespace DetectorContainer
{
    typedef MSDetectorContainerWrapper<
        std::list< MSVehicle* > > Vehicles;
}

#endif // MSDETECTORCONTAINERWRAPPER_H

// Local Variables:
// mode:C++
// End:
