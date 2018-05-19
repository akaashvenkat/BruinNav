//
//  support.h
//  project4
//
//  Created by Akaash Venkat on 3/12/17.
//  Copyright © 2017 Akaash Venkat. All rights reserved.
//

#ifndef support_h
#define support_h

#include <iostream>
#include <vector>
#include "MyMap.h"
#include "provided.h"
using namespace std;

//operator< method: overloads the < operator for GeoCoord
inline
bool operator<(const GeoCoord& a, const GeoCoord& b)
{
    //returns true if a's latitude < b's latitude
    if (a.latitude < b.latitude)
        return true;
    
    //if a and b's latitudes are equal, compare the longitudes
    else if (a.latitude == b.latitude)
    {
        //returns true if a's longitude < b's longitude
        if (a.longitude < b.longitude)
            return true;
        
        //otherwise returns false
        else
            return false;
    }
    
    //otherwise returns false
    else
        return false;
}

//operator== method: overloads the == operator for GeoCoord
inline
bool operator==(const GeoCoord& a, const GeoCoord& b)
{
    //returns true if both GeoCoord's have the same latitude and longitude
    return (a.latitude == b.latitude) && (a.longitude == b.longitude);
}

//operator== method: overloads the == operator for GeoSegment
inline
bool operator==(const GeoSegment& a, const GeoSegment& b)
{
    //returns true if both GeoCoord's have the same latitude and longitude
    return (a.start == b.start) && (a.end == b.end);
}

//operator== method: overloads the == operator for StreetSegment
inline
bool operator==(StreetSegment& a, StreetSegment& b)
{
    //returns true if both StreetSegments geosegments are equal
    return (a.segment == b.segment);
}

//directionOfLine method
inline
string directionOfLine(GeoSegment segment)
{
    //get angle based on parameter segment
    double angle = angleOfLine(segment);
    
    //set string direction based on angle value
    string direction;
    if (0 <= angle && angle <= 22.5)
        direction = "east";
    if (22.5 < angle && angle <= 67.5)
        direction = "northeast";
    if (67.5 < angle && angle <= 112.5)
        direction = "north";
    if (112.5 < angle && angle <= 157.5)
        direction = "northwest";
    if (157.5 < angle && angle <= 202.5)
        direction = "west";
    if (202.5 < angle && angle <= 247.5)
        direction = "southwest";
    if (247.5 < angle && angle <= 292.5)
        direction = "south";
    if (292.5 < angle && angle <= 337.5)
        direction = "southeast";
    if (337.5 < angle && angle < 360)
        direction = "east";
    
    //return direction
    return direction;
}

struct navigPointer
{
    //initialize necessary values
    GeoCoord *mPoint;
    double mDistance = 0, mAngle = 0, mPriority = 0;
    string mStreetName = "", mCapStreet = "";
    navigPointer *mPrevPoint;
    bool mProcess = false;
    
    //navigPointer constructor
    navigPointer(const SegmentMapper& segMapper, GeoCoord& geoCo, navigPointer* prevnavigPointer = nullptr)
    {
        //create geolocation based on parameter
        mPoint = new GeoCoord(geoCo);
        mPoint->latitude = geoCo.latitude;
        mPoint->longitude = geoCo.longitude;
        
        //set previous point, distance, priority, and street
        mPrevPoint = prevnavigPointer;
        setDistance();
        mPriority = 100000000;
        setStreet(segMapper);
    }
    
    //navigPointer destructor
    ~navigPointer()
    {
        if (mPoint != nullptr)
            delete mPoint;
    }
    
    //sets street segment based on parameter
    void setStreet(const SegmentMapper& segMapper)
    {
        if (mPrevPoint == nullptr)
        {
            vector<StreetSegment> temp = segMapper.getSegments(*mPoint);
            if (!temp.empty())
            {
                mStreetName = temp.at(0).streetName;
                mCapStreet = mStreetName;
                transform(mStreetName.begin(), mStreetName.end(), mStreetName.begin(), ::tolower);
            }
        }
        else
        {
            vector<StreetSegment> segmentVector = segMapper.getSegments(*mPoint);
            for (int i = 0; i < segmentVector.size(); i++)
            {
                StreetSegment segRunner = segmentVector.at(i);
                
                mCapStreet = segRunner.streetName;
                transform(mStreetName.begin(), mStreetName.end(), mStreetName.begin(), ::tolower);
                GeoSegment geoSeg;
                geoSeg.start = *(mPrevPoint->mPoint);
                geoSeg.end = *mPoint;
                mAngle = angleOfLine(geoSeg);
            }
        }
    }
    
    //sets distane based on mPrevPoint value, using distanceEarthMiles function
    void setDistance()
    {
        if (mPrevPoint == nullptr)
            mDistance = 0;
        else
            mDistance = distanceEarthMiles(*mPoint, *(mPrevPoint->mPoint));
    }
    
    
    void setPriority(const GeoCoord& end, navigPointer* navigPointer)
    {
        double distance1 = 0;
        double distance2 = distanceEarthMiles(*mPoint, end);
        
        if (mPrevPoint == nullptr)
            distance1 = 0;
        else
            distance2 = navigPointer->mPriority;
        
        if (distance1 + distance2 + mDistance < mPriority)
        {
            mPriority = distance1 + distance2 + mDistance;
            mPrevPoint = navigPointer;
        }
    }
};

class PriorityCheck
{
public:
    bool operator() (navigPointer* a, navigPointer* b)
    {
        return a->mPriority > b->mPriority;
    }
};

#endif /* support_h */
