#include "MS_E2_ZS_CollectorOverLanes.h"
//#include <utils/common/DOubleVector.h>

using namespace std;

MS_E2_ZS_CollectorOverLanes::MS_E2_ZS_CollectorOverLanes(
        std::string id, MSLane* lane, MSUnit::Meters startPos,
        //MSUnit::Meters detLength, const LaneContinuations &laneContinuations,
        MSUnit::Seconds haltingTimeThreshold,
        MSUnit::MetersPerSecond haltingSpeedThreshold,
        MSUnit::Meters jamDistThreshold,
        MSUnit::Seconds deleteDataAfterSeconds)
        : startPosM(startPos),
        deleteDataAfterSecondsM(deleteDataAfterSeconds),
        haltingTimeThresholdM(haltingSpeedThreshold),
        haltingSpeedThresholdM(haltingSpeedThreshold),
        jamDistThresholdM(jamDistThreshold),
        myID(id), myStartLaneID(lane->id())
{
}

void
MS_E2_ZS_CollectorOverLanes::init(
        MSLane *lane,
        MSUnit::Meters detLength,
        const LaneContinuations &laneContinuations)
{
    if(startPosM==0) {
        startPosM = 0.1;
    }
    double length = lane->length() - startPosM - 0.1;
    double dlength = detLength;
    if(length>dlength) {
        length = dlength;
    }
    myLengths.push_back(length);
    myLaneCombinations.push_back(LaneVector());
    myLaneCombinations[0].push_back(lane);
    myDetectorCombinations.push_back(DetectorVector());
    MS_E2_ZS_Collector *c =
        buildCollector(0, 0, lane, startPosM, length);
    myDetectorCombinations[0].push_back(c);
    myAlreadyBuild[lane] = c;
    extendTo(detLength, laneContinuations);

//    myTmpArray.reserve(myLengths.size());
}


MS_E2_ZS_CollectorOverLanes::~MS_E2_ZS_CollectorOverLanes( void )
{
}


void
MS_E2_ZS_CollectorOverLanes::extendTo(
        double length,
        const LaneContinuations &laneContinuations)
{
 cout << "LaneCont-size:" << laneContinuations.size() << endl;
 cout << myID << endl;
 for(LaneContinuations::const_iterator i=laneContinuations.begin(); i!=laneContinuations.end(); i++) {
     cout << (*i).first << ':';
     for(std::vector<std::string>::const_iterator j=(*i).second.begin(); j!=(*i).second.end(); j++) {
	 cout << (*j) << ", ";
     }
     cout << endl;
 }
    bool done = false;
	    cout << "1" << endl;
    while(!done) {
        done = true;
        LengthVector::iterator leni = myLengths.begin();
        LaneVectorVector::iterator lanei = myLaneCombinations.begin();
        DetectorVectorVector::iterator deti = myDetectorCombinations.begin();
        size_t c = 0;
        for(; leni!=myLengths.end(); leni++, lanei++, deti++) {
            if((*leni)<length) {
                done = false;
                // copy current values
                LaneVector lv = *lanei;
                DetectorVector dv = *deti;
                double clength = *leni;
		    assert(lv.size()>0);
		    assert(dv.size()>0);
                // erase previous elements
		    assert(leni!=myLengths.end());
                myLengths.erase(leni);
                myLaneCombinations.erase(lanei);
                myDetectorCombinations.erase(deti);
                // get the lane to look before
                MSLane *toExtend = lv[lv.size()-1];
		    cout << toExtend->id() << endl;
                // and her predecessors
                LaneContinuations::const_iterator conts =
                    laneContinuations.find(toExtend->id());
                assert(conts!=laneContinuations.end());
                const std::vector<std::string> &predeccessors =
                    (*conts).second;
                // go through the predeccessors and extend the detector
                for(std::vector<std::string>::const_iterator i=predeccessors.begin(); i!=predeccessors.end(); i++) {
                    // get the lane
                    MSLane *l = MSLane::dictionary(*i);
                    // compute detector length
                    double lanelen = length - clength;
                    if(lanelen>l->length()) {
                        lanelen = l->length() - 0.2;
                    }
                    // build new info
                    LaneVector nlv = lv;
                    nlv.push_back(l);
                    DetectorVector ndv = dv;
                    MS_E2_ZS_Collector *coll = 0;
                    if(myAlreadyBuild.find(l)==myAlreadyBuild.end()) {
                        coll = buildCollector(0, 0, l, 0.1, lanelen);
                    } else {
                        coll = myAlreadyBuild.find(l)->second;
                    }
                    ndv.push_back(coll);
                    // store new info
                    myLaneCombinations.push_back(nlv);
                    myDetectorCombinations.push_back(ndv);
                    myLengths.push_back(clength+lanelen);
                }
                // restart
                leni = myLengths.end() - 1;
            }
        }
    }
	    cout << "2" << endl;
}


MS_E2_ZS_Collector *
MS_E2_ZS_CollectorOverLanes::buildCollector(size_t c, size_t r, MSLane *l,
                                            double start, double end)
{
    string id = makeID(l->id(), c, r);
    return new MS_E2_ZS_Collector(id,
        l, start, end, haltingTimeThresholdM,
        haltingSpeedThresholdM, jamDistThresholdM, deleteDataAfterSecondsM);
}


double
MS_E2_ZS_CollectorOverLanes::getCurrent( MS_E2_ZS_Collector::DetType type )
{
    switch(type) {
    case MS_E2_ZS_Collector::DENSITY:
    case MS_E2_ZS_Collector::MAX_JAM_LENGTH_IN_VEHICLES:
    case MS_E2_ZS_Collector::MAX_JAM_LENGTH_IN_METERS:
    case MS_E2_ZS_Collector::JAM_LENGTH_SUM_IN_VEHICLES:
    case MS_E2_ZS_Collector::JAM_LENGTH_SUM_IN_METERS:
    case MS_E2_ZS_Collector::QUEUE_LENGTH_AHEAD_OF_TRAFFIC_LIGHTS_IN_VEHICLES:
    case MS_E2_ZS_Collector::QUEUE_LENGTH_AHEAD_OF_TRAFFIC_LIGHTS_IN_METERS:
    case MS_E2_ZS_Collector::N_VEHICLES:
    case MS_E2_ZS_Collector::OCCUPANCY_DEGREE:
    case MS_E2_ZS_Collector::SPACE_MEAN_SPEED:
    case MS_E2_ZS_Collector::CURRENT_HALTING_DURATION_SUM_PER_VEHICLE:
    default:
        double myMax = 0;
        for(DetectorVectorVector::iterator i=myDetectorCombinations.begin(); i!=myDetectorCombinations.end(); i++) {
            double value = 0;
            for(DetectorVector::iterator j=(*i).begin(); j!=(*i).end(); j++) {
                value += (*j)->getCurrent(type);
            }
            if(myMax<value) {
                myMax = value;
            }
        }
        return myMax;
    }
    return -1;
}


double
MS_E2_ZS_CollectorOverLanes::getAggregate( MS_E2_ZS_Collector::DetType type,
                                          MSUnit::Seconds lastNSeconds )
{
    switch(type) {
    case MS_E2_ZS_Collector::DENSITY:
    case MS_E2_ZS_Collector::MAX_JAM_LENGTH_IN_VEHICLES:
    case MS_E2_ZS_Collector::MAX_JAM_LENGTH_IN_METERS:
    case MS_E2_ZS_Collector::JAM_LENGTH_SUM_IN_VEHICLES:
    case MS_E2_ZS_Collector::JAM_LENGTH_SUM_IN_METERS:
    case MS_E2_ZS_Collector::QUEUE_LENGTH_AHEAD_OF_TRAFFIC_LIGHTS_IN_VEHICLES:
    case MS_E2_ZS_Collector::QUEUE_LENGTH_AHEAD_OF_TRAFFIC_LIGHTS_IN_METERS:
    case MS_E2_ZS_Collector::N_VEHICLES:
    case MS_E2_ZS_Collector::OCCUPANCY_DEGREE:
    case MS_E2_ZS_Collector::SPACE_MEAN_SPEED:
    case MS_E2_ZS_Collector::CURRENT_HALTING_DURATION_SUM_PER_VEHICLE:
    default:
        double myMax = 0;
        for(DetectorVectorVector::iterator i=myDetectorCombinations.begin(); i!=myDetectorCombinations.end(); i++) {
            double value = 0;
            for(DetectorVector::iterator j=(*i).begin(); j!=(*i).end(); j++) {
                value += (*j)->getAggregate(type, lastNSeconds);
            }
            if(value>myMax) {
                myMax = value;
            }
        }
        return myMax;
    }
    return -1;
}


bool
MS_E2_ZS_CollectorOverLanes::hasDetector( MS_E2_ZS_Collector::DetType type )
{
    return myDetectorCombinations[0][0]->hasDetector(type);
}


void
MS_E2_ZS_CollectorOverLanes::addDetector( MS_E2_ZS_Collector::DetType type,
                                         std::string detId)
{
    size_t c = 0;
    for(DetectorVectorVector::iterator i=myDetectorCombinations.begin(); i!=myDetectorCombinations.end(); i++) {
        size_t r = 0;
        for(DetectorVector::iterator j=(*i).begin(); j!=(*i).end(); j++) {
            (*j)->addDetector(type, makeID(detId, c, r));
            r++;
        }
        c++;
    }
}


std::string
MS_E2_ZS_CollectorOverLanes::getXMLOutput( MSUnit::IntSteps lastNTimesteps )
{
    std::string result;
    MSUnit::Seconds lastNSeconds =
        MSUnit::getInstance()->getSeconds( lastNTimesteps );
    return result;
}


std::string
MS_E2_ZS_CollectorOverLanes::getXMLDetectorInfoStart( void ) const
{
    std::string
        detectorInfo("<detector type=\"E2_ZS_Collector\" id=\"" + myID +
                "\" startlane=\"" +
                myStartLaneID + "\" startpos=\"" +
                toString(startPosM) + "\" length=\"" +
                toString(endPosM - startPosM) +
                "\" >\n");
    return detectorInfo;
}


size_t bla = 0;

std::string
MS_E2_ZS_CollectorOverLanes::makeID( const std::string &baseID ,
                                    size_t c, size_t r ) const
{
 cout << "Halllo1 bla" << endl;
string ret =  baseID + toString<size_t>(bla++);/*string("[") + toString<size_t>(c)
		      + string("][") + toString<size_t>(r) + string("]");
*/
 cout << "Halllo2 bla" << endl;
 return ret;
}


const std::string &
MS_E2_ZS_CollectorOverLanes::getId() const
{
    return myID;
}


const std::string &
MS_E2_ZS_CollectorOverLanes::getStartLaneID() const
{
    return myStartLaneID;
}


void
MS_E2_ZS_CollectorOverLanes::resetQueueLengthAheadOfTrafficLights( void )
{
    for(DetectorVectorVector::iterator i=myDetectorCombinations.begin(); i!=myDetectorCombinations.end(); i++) {
        for(DetectorVector::iterator j=(*i).begin(); j!=(*i).end(); j++) {
            (*j)->resetQueueLengthAheadOfTrafficLights();
        }
    }
}
