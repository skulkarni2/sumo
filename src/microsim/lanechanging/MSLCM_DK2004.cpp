#include "MSLCM_DK2004.h"
#include <iostream>

using namespace std;

/* =========================================================================
 * some definitions (debugging only)
 * ======================================================================= */
#define DEBUG_OUT cout



std::string id_leader("367719");
std::string id_vehicle("366338");
std::string id_follower("4536");
std::string id_pfollower("1131");
size_t searchedtime = 21900;

//#define GUI_DEBUG

#ifdef GUI_DEBUG
#include <gui/GUIGlobalSelection.h>
#include <guisim/GUIVehicle.h>
#endif


MSLCM_DK2004::MSLCM_DK2004(MSVehicle &v)
    : MSAbstractLaneChangeModel(v),
    myChangeProbability(0),
    myVSafe(0)
{
}

MSLCM_DK2004::~MSLCM_DK2004()
{
    changed();
}

int
MSLCM_DK2004::wantsChangeToRight(MSAbstractLaneChangeModel::MSLCMessager &msgPass,
                                 int blocked,
                                 const std::pair<MSVehicle*, double> &leader,
                                 const std::pair<MSVehicle*, double> &neighLead,
                                 const std::pair<MSVehicle*, double> &neighFollow,
                                 const MSLane &neighLane,
                                 int bestLaneOffset, double bestDist,
                                 double neighDist,
                                 double currentDist,
                                 MSVehicle **lastBlocked)
{
#ifdef GUI_DEBUG
    if(gSelected.isSelected(GLO_VEHICLE, static_cast<GUIVehicle&>(myVehicle).getGlID())) {
        int blb = 0;
    }
#endif
#ifdef ABS_DEBUG
    if(MSNet::globaltime>=MSNet::searchedtime && (myVehicle.id()==MSNet::searched1||myVehicle.id()==MSNet::searched2)) {
        DEBUG_OUT << "bla" << endl;
    }
#endif
    // keep information about being a leader/follower
    int ret = (myState&0x00ffff00);

    if( leader.first!=0
        &&
        (myState&LCA_AMBLOCKINGFOLLOWER_DONTBRAKE)!=0
        &&
        (leader.first->getLaneChangeModel().getState()&LCA_AMBLOCKINGFOLLOWER_DONTBRAKE)!=0) {

        myState &= (0xffffffff-LCA_AMBLOCKINGFOLLOWER_DONTBRAKE);
        if(myVehicle.speed()>0.1) {
            myState |= LCA_AMBACKBLOCKER;
        } else {
            ret |= LCA_AMBACKBLOCKER;
            myVSafe = 0;
        }
    }

    if((*lastBlocked)!=0) {
        double gap = (*lastBlocked)->pos()-(*lastBlocked)->length()-myVehicle.pos();
/*        if(gap>=1.0) {
            double decelGap = gap
                + myVehicle.speed() * MSNet::deltaT() * 2.0
                - MAX((*lastBlocked)->speed() - (*lastBlocked)->decelAbility() * 2.0, 0);
            if(decelGap>0&&(*lastBlocked)->isSafeChange_WithDistance(decelGap, myVehicle, &(*lastBlocked)->getLane())) {
                ret |= LCA_AMBACKBLOCKER;
                double vSafe = myVehicle.vsafe(myVehicle.speed(),
                    myVehicle.decelAbility(), gap-1.0, (*lastBlocked)->speed());
                if(myVSafe>=0) {
                    myVSafe = MIN(myVSafe, vSafe);
                } else {
                    myVSafe = vSafe;
                }
                (*lastBlocked) = 0;
            }
        } else */
        if(gap>0.1) {
            if(myVehicle.speed()<myVehicle.decelAbility()) {
                if((*lastBlocked)->speed()<0.1) {
                    ret |= LCA_AMBACKBLOCKER_STANDING;
                    myVSafe = myVehicle.vsafe(myVehicle.speed(),
                        myVehicle.decelAbility(), gap-0.1, (*lastBlocked)->speed());
                    (*lastBlocked) = 0;
                } else {
                    ret |= LCA_AMBACKBLOCKER;
                    myVSafe = myVehicle.vsafe(myVehicle.speed(),
                        myVehicle.decelAbility(), gap-0.1, (*lastBlocked)->speed());
                    (*lastBlocked) = 0;
                }
            }
        }
    }

    // -------- forced changing
    double rv = /*neighLead.first!=0&&myVehicle.speed()>myVehicle.accelAbility()
        ? neighLead.first->speed()+20.0
        :*/ myVehicle.getLane().maxSpeed() * 5;
    if( bestLaneOffset<0&&currentDistDisallows(currentDist, bestLaneOffset, rv)) {
        informBlocker(msgPass, blocked, LCA_MRIGHT, neighLead, neighFollow);
        if(neighLead.second>0&&neighLead.second>leader.second) {
            myVSafe = myVehicle.vsafe(myVehicle.speed(),
                myVehicle.decelAbility(), neighLead.second, neighLead.first->speed())
                - 0.5;
        }
        return ret|LCA_RIGHT|LCA_URGENT|blocked;
    }
    // left lanes are better...
    if( bestLaneOffset>0&&currentDistDisallows(currentDist, bestLaneOffset, rv)/*&&currentDist!=neighDist*/) {
        return ret;
    }
    // the current lane is better
    if(neighDist<rv&&bestLaneOffset==0) {
        return ret;
    }
    // --------

    // -------- make place on current lane if blocking follower
    if( amBlockingFollowerPlusNB()
        &&
        (currentDistAllows(neighDist, bestLaneOffset, rv)||neighDist>=currentDist) ) {

        return ret|LCA_RIGHT|LCA_URGENT|LCA_KEEP1;
    }
    // --------


    // -------- security checks for krauss
    //  (vsafe fails when gap<0)
    if((blocked&(LCA_BLOCKEDBY_FOLLOWER|LCA_BLOCKEDBY_LEADER))!=0) {
        return ret;
    }
    // --------

    // -------- higher speed
    if(congested( neighLead.first )||predInteraction(leader.first)) {
        return ret;
    }
    double neighLaneVSafe, thisLaneVSafe;
    if ( neighLead.first == 0 ) {
        neighLaneVSafe =
            myVehicle.vsafe(
                myVehicle.speed(), myVehicle.decelAbility(),
                neighLane.length() - myVehicle.pos(), 0);
    } else {
        assert(neighLead.second>=0);
        neighLaneVSafe =
            myVehicle.vsafe(myVehicle.speed(), myVehicle.decelAbility(),
                neighLead.second, neighLead.first->speed());
    }
    if(leader.first==0) {
        thisLaneVSafe =
            myVehicle.vsafe(
                myVehicle.speed(), myVehicle.decelAbility(),
                myVehicle.getLane().length() - myVehicle.pos(), 0);
    } else {
        assert(leader.second>=0);
        thisLaneVSafe =
            myVehicle.vsafe(
                myVehicle.speed(), myVehicle.decelAbility(),
                leader.second, leader.first->speed());
    }

    thisLaneVSafe =
        MIN3(thisLaneVSafe, myVehicle.getLane().maxSpeed(), myVehicle.getVehicleType().maxSpeed());
    neighLaneVSafe =
        MIN3(neighLaneVSafe, neighLane.maxSpeed(), myVehicle.getVehicleType().maxSpeed());
    if(thisLaneVSafe>neighLaneVSafe) {
        // this lane is better
        if(myChangeProbability<0) {
            myChangeProbability /= 2.0;
        }
    } else {
        // right lane is better
        myChangeProbability -= (float)
            ((neighLaneVSafe-thisLaneVSafe) / (myVehicle.getLane().maxSpeed()));
    }
    if(myChangeProbability<-1.0) {
        return ret | LCA_RIGHT|LCA_SPEEDGAIN;
    }
    // --------
    return ret;
}


int
MSLCM_DK2004::wantsChangeToLeft(MSAbstractLaneChangeModel::MSLCMessager &msgPass,
                                int blocked,
                                const std::pair<MSVehicle*, double> &leader,
                                const std::pair<MSVehicle*, double> &neighLead,
                                const std::pair<MSVehicle*, double> &neighFollow,
                                const MSLane &neighLane,
                                int bestLaneOffset, double bestDist,
                                double neighDist,
                                double currentDist,
                                MSVehicle **lastBlocked)
{
#ifdef GUI_DEBUG
    if(gSelected.isSelected(GLO_VEHICLE, static_cast<GUIVehicle&>(myVehicle).getGlID())) {
        int blb = 0;
    }
#endif
#ifdef ABS_DEBUG
    if(MSNet::globaltime>=MSNet::searchedtime && (myVehicle.id()==MSNet::searched1||myVehicle.id()==MSNet::searched2)) {
        DEBUG_OUT << "bla" << endl;
    }
#endif
    // keep information about being a leader/follower
    int ret = (myState&0x00ffff00);

    if( leader.first!=0
        &&
        (myState&LCA_AMBLOCKINGFOLLOWER_DONTBRAKE)!=0
        &&
        (leader.first->getLaneChangeModel().getState()&LCA_AMBLOCKINGFOLLOWER_DONTBRAKE)!=0) {

        myState &= (0xffffffff-LCA_AMBLOCKINGFOLLOWER_DONTBRAKE);
        if(myVehicle.speed()>0.1) {
            myState |= LCA_AMBACKBLOCKER;
        } else {
            ret |= LCA_AMBACKBLOCKER;
            myVSafe = 0;
        }
    }

    if((*lastBlocked)!=0) {
        double gap = (*lastBlocked)->pos()-(*lastBlocked)->length()-myVehicle.pos();
        /*
        if(gap>=1.0) {
            double decelGap = gap
                + myVehicle.speed() * MSNet::deltaT() * 2.0
                - MAX((*lastBlocked)->speed() - (*lastBlocked)->decelAbility() * 2.0, 0);
            if(decelGap>0&&(*lastBlocked)->isSafeChange_WithDistance(decelGap, myVehicle, &(*lastBlocked)->getLane())) {
                ret |= LCA_AMBACKBLOCKER;
                double vSafe = myVehicle.vsafe(myVehicle.speed(),
                    myVehicle.decelAbility(),
                   gap-1.0, (*lastBlocked)->speed());
                myVSafe = MIN(myVSafe, vSafe);
                (*lastBlocked) = 0;
                if(myVSafe>=0) {
                    myVSafe = MIN(myVSafe, vSafe);
                } else {
                    myVSafe = vSafe;
                }
            }
        } else if(gap>0&&myVehicle.speed()<myVehicle.decelAbility()) {
            ret |= LCA_AMBACKBLOCKER;
            (*lastBlocked) = 0;
            myVSafe = 0;
        }*/
        if(gap>0.1) {
            if(myVehicle.speed()<myVehicle.decelAbility()) {
                if((*lastBlocked)->speed()<0.1) {
                    ret |= LCA_AMBACKBLOCKER_STANDING;
                    myVSafe = myVehicle.vsafe(myVehicle.speed(),
                        myVehicle.decelAbility(), gap-0.1, (*lastBlocked)->speed());
                    (*lastBlocked) = 0;
                } else {
                    ret |= LCA_AMBACKBLOCKER;
                    myVSafe = myVehicle.vsafe(myVehicle.speed(),
                        myVehicle.decelAbility(), gap-0.1, (*lastBlocked)->speed());
                    (*lastBlocked) = 0;
                }
            }
        }
    }

    // -------- forced changing
    double lv = /*neighLead.first!=0&&myVehicle.speed()>myVehicle.accelAbility()
        ? neighLead.first->speed()+20.0
        : */myVehicle.getLane().maxSpeed() * 5;
    if( bestLaneOffset>0&&currentDistDisallows(currentDist, bestLaneOffset, lv)) {
        informBlocker(msgPass, blocked, LCA_MLEFT, neighLead, neighFollow);
        if(neighLead.second>0&&neighLead.second>leader.second) {
            myVSafe = myVehicle.vsafe(myVehicle.speed(),
                myVehicle.decelAbility(), neighLead.second, neighLead.first->speed())
                - 0.5;
        }
        return ret|LCA_LEFT|LCA_URGENT|blocked;
    }
    // left lanes are better...
    if( bestLaneOffset<0&&currentDistDisallows(currentDist, bestLaneOffset, lv)/*&&currentDist!=neighDist*/) {
        return ret;
    }
    // the current lane is better
    if( neighDist<lv&&bestLaneOffset==0) {
        return ret;
    }
    // --------

    // -------- make place on current lane if blocking follower
    if( amBlockingFollowerPlusNB()
        &&
        (currentDistAllows(neighDist, bestLaneOffset, lv)||neighDist>=currentDist) ) {

        return ret|LCA_LEFT|LCA_URGENT|LCA_KEEP1;
    }
    // --------

    // -------- security checks for krauss
    //  (vsafe fails when gap<0)
    if((blocked&(LCA_BLOCKEDBY_FOLLOWER|LCA_BLOCKEDBY_LEADER))!=0) {
        return ret;
    }

    // -------- higher speed
    if(congested( neighLead.first )) {
        return ret;
    }
    double neighLaneVSafe, thisLaneVSafe;
    if ( neighLead.first == 0 ) {
        neighLaneVSafe =
            myVehicle.vsafe(
                myVehicle.speed(), myVehicle.decelAbility(),
                neighLane.length() - myVehicle.pos(), 0);
    } else {
        assert(neighLead.second>=0);
        neighLaneVSafe =
            myVehicle.vsafe(myVehicle.speed(), myVehicle.decelAbility(),
                neighLead.second, neighLead.first->speed());
    }
    if(leader.first==0) {
        thisLaneVSafe =
            myVehicle.vsafe(
                myVehicle.speed(), myVehicle.decelAbility(),
                myVehicle.getLane().length() - myVehicle.pos(), 0);
    } else {
        assert(leader.second>=0);
        thisLaneVSafe =
            myVehicle.vsafe(
                myVehicle.speed(), myVehicle.decelAbility(),
                leader.second, leader.first->speed());
    }
    thisLaneVSafe =
        MIN3(thisLaneVSafe, myVehicle.getLane().maxSpeed(), myVehicle.getVehicleType().maxSpeed());
    neighLaneVSafe =
        MIN3(neighLaneVSafe, neighLane.maxSpeed(), myVehicle.getVehicleType().maxSpeed());
    if(thisLaneVSafe>neighLaneVSafe) {
        // this lane is better
        if(myChangeProbability>0) {
            myChangeProbability /= 2.0;
        }
    } else {
        // right lane is better
        myChangeProbability += (float)
            ((neighLaneVSafe-thisLaneVSafe) / (myVehicle.getLane().maxSpeed()));
    }
    if(myChangeProbability>1.0) {
        return ret | LCA_LEFT|LCA_SPEEDGAIN;
    }
    // --------
    return ret;
}


double
MSLCM_DK2004::patchSpeed(double min, double wanted, double max, double vsafe)
{
#ifdef GUI_DEBUG
    if(gSelected.isSelected(GLO_VEHICLE, static_cast<GUIVehicle&>(myVehicle).getGlID())) {
        int blb= 0;
    }
#endif
    double vSafe = myVSafe;
    int state = myState;
    myState = 0;
    myVSafe = -1;
    // just to make sure to be notified about lane chaning end
    if(myVehicle.getLane().edge().nLanes()==1) {
        // remove chaning information if on a road with a single lane
        changed();
    }

    // check whether the vehicle is blocked
    if((state&LCA_URGENT)!=0) {
        if(vSafe>0) {
            return MIN(max, MAX(min, vSafe));
        }
        // check whether the vehicle maybe has to be swapped with one of
        //  the blocking vehicles
        if( (state&LCA_BLOCKEDBY_LEADER)!=0
            &&
            (state&LCA_BLOCKEDBY_FOLLOWER)!=0) {

            return wanted;
        } else {
            if((state&LCA_BLOCKEDBY_LEADER)!=0) {
                // if interacting with leader and not too slow
                return (min+wanted)/2.0;
            }
            if((state&LCA_BLOCKEDBY_FOLLOWER)!=0) {
                return (max+wanted)/2.0;
            }
        }
    }


    // decelerate if being a blocking follower
    //  (and does not have to change lanes)
    if((state&LCA_AMBLOCKINGFOLLOWER)!=0) {
        if(max==myVehicle.accelAbility()&&min==0) {
            return 0;
        }
        if(myVSafe<=0) {
            return (min+wanted)/2.0;
        }
        return MAX(min, MIN(vSafe, wanted));
    }
    if((state&LCA_AMBACKBLOCKER)!=0) {
        if(max<=myVehicle.accelAbility()&&min==0) {
            return MAX(min, MIN(vSafe, wanted));
        }
    }
    if((state&LCA_AMBACKBLOCKER_STANDING)!=0) {
        return MAX(min, MIN(vSafe, wanted));
    }
    // accelerate if being a blocking leader or blocking follower not able to brake
    //  (and does not have to change lanes)
    if((state&LCA_AMBLOCKINGLEADER)!=0) {
        return (max+wanted)/2.0;
    }
    /*
    if((state&LCA_AMBACKBLOCKER)!=0) {
        if(max<=myVehicle.accelAbility()&&min==0) {
            return MAX(min, MIN(vSafe, wanted));
        }
        if(myVSafe>(min+wanted)/2.0) {
            return MIN(vSafe, wanted);
        }
        return (min+wanted)/2.0;
    }
    */
    if((state&LCA_AMBLOCKINGFOLLOWER_DONTBRAKE)!=0) {
        if(max<=myVehicle.accelAbility()&&min==0) {
            return wanted;
        }
        if(myVSafe>(min+wanted)/2.0) {
            return MIN(vSafe, wanted);
        }
        return (min+wanted)/2.0;
    }
    return wanted;
}


void *
MSLCM_DK2004::inform(void *info, MSVehicle *sender)
{
    Info *pinfo = (Info*) info;
    if(pinfo->second==LCA_UNBLOCK) {
        myState &= 0xffff00ff;
        return (void*) true;
    }
    myState &= 0xffffffff;
    myState |= pinfo->second;
    if(pinfo->first>=0) {
        myVSafe = MIN(myVSafe, pinfo->first);
    }
    return (void*) true;
}


void
MSLCM_DK2004::changed()
{
    myChangeProbability = 0;
    myState = 0;
}


void
MSLCM_DK2004::informBlocker(MSAbstractLaneChangeModel::MSLCMessager &msgPass,
                            int &blocked,
                            int dir,
                            const std::pair<MSVehicle*, double> &neighLead,
                            const std::pair<MSVehicle*, double> &neighFollow)
{
#ifdef GUI_DEBUG
    if(gSelected.isSelected(GLO_VEHICLE, static_cast<GUIVehicle&>(myVehicle).getGlID())) {
        int blb = 0;
    }
#endif
    if((blocked&LCA_BLOCKEDBY_FOLLOWER)!=0) {
        assert(neighFollow.first!=0);
        MSVehicle *nv = neighFollow.first;
        double decelGap =
            neighFollow.second
            + myVehicle.speed() * MSNet::deltaT() * 2.0
            - MAX(nv->speed() - nv->decelAbility() * 2.0, 0);
        if(neighFollow.second>0&&decelGap>0&&nv->isSafeChange_WithDistance(decelGap, myVehicle, &nv->getLane())) {
            double vsafe = myVehicle.vsafe(myVehicle.speed(), myVehicle.decelAbility(),
                neighFollow.second, neighFollow.first->speed());
            msgPass.informNeighFollower(
                (void*) &(Info(vsafe, dir|LCA_AMBLOCKINGFOLLOWER)), &myVehicle);
        } else {
            double vsafe = neighFollow.second<=0
                ? 0
                : myVehicle.vsafe(myVehicle.speed(), myVehicle.decelAbility(),
                    neighFollow.second, neighFollow.first->speed());
            msgPass.informNeighFollower(
                (void*) &(Info(vsafe, dir|LCA_AMBLOCKINGFOLLOWER_DONTBRAKE)), &myVehicle);
        }
    }
    if((blocked&LCA_BLOCKEDBY_LEADER)!=0) {
        if(neighLead.first!=0&&neighLead.second>0) {
            msgPass.informNeighLeader(
                (void*) &(Info(0, dir|LCA_AMBLOCKINGLEADER)), &myVehicle);
        }
    }
}


void
MSLCM_DK2004::prepareStep()
{
}


double
MSLCM_DK2004::getProb() const
{
    return myChangeProbability;
}


