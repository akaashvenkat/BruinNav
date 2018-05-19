//
//  Navigator.cpp
//  project4
//
//  Created by Akaash Venkat on 3/10/17.
//  Copyright © 2017 Akaash Venkat. All rights reserved.
//

#include "provided.h"
#include "support.h"
#include "MyMap.h"
#include <string>
#include <vector>
#include <stack>
#include <sstream>
#include <iomanip>
#include <queue>
#include <iostream>
using namespace std;

class NavigatorImpl
{
    //NavigatorImpl public interface provided by spec
public:
    NavigatorImpl();
    ~NavigatorImpl();
    bool loadMapData(string mapFile);
    NavResult navigate(string start, string end, vector<NavSegment>& directions) const;
    
    //NavigatorImpl private interface
private:
    MapLoader mLoader;
    AttractionMapper mAttraction;
    SegmentMapper mSegment;
    vector<NavSegment> mResults;
    bool findShortPath(GeoCoord& start, GeoCoord& end, vector<NavSegment>& directions) const;
    bool getParticularStreetSegForAttraction(GeoCoord start, StreetSegment &streetSeg) const;
    void connectSegment(GeoCoord& end, navigPointer* lastStreet, MyMap<GeoCoord, navigPointer*>& road) const;
    void addNavigationSegment(GeoCoord& start, GeoCoord& end, vector<NavSegment>& directions, MyMap<GeoCoord, navigPointer*> *road) const;
};

//NavigatorImpl constructor (empty)
NavigatorImpl::NavigatorImpl() {}

//NavigatorImpl destructor (empty)
NavigatorImpl::~NavigatorImpl() {}

//NavigatorImpl loadMapData method: loads all the data
bool NavigatorImpl::loadMapData(string mapFile)
{
    //returns true if mapFile can be loaded
    if (mLoader.load(mapFile))
    {
        //run AttractionMapper and SegmentMapper init methods
        mAttraction.init(mLoader);
        mSegment.init(mLoader);
        return true;
    }
    
    //return false otherwise
    return false;
}

//NavigatorImpl connectSegment method: associates coordinates with navigPointers
void NavigatorImpl::connectSegment(GeoCoord& endCoord, navigPointer* endStreet, MyMap<GeoCoord, navigPointer*>& path) const
{
    
    //get vector of street segments where parameter coordinate can be found at
    vector<StreetSegment> streetSeg = mSegment.getSegments(endCoord);
    
    //run through vector of street segments
    for (int i = 0; i < streetSeg.size(); i++)
    {
        StreetSegment streetRunner = streetSeg.at(i);
        
        //go through each segment's attractions
        for (int j = 0; j < streetRunner.attractions.size(); j++)
        {
            Attraction attraction = streetRunner.attractions.at(j);
            
            //if the attraction is found at the parameter coordinate, associate the coordinate and new navigPointer
            if (attraction.geocoordinates == endCoord)
            {
                navigPointer* newNav = new navigPointer(mSegment, endCoord, endStreet);
                newNav->setPriority(endCoord, endStreet);
                path.associate(endCoord, newNav);
                return;
            }
        }
    }
}

//NavigatorImpl addNavigationSegment method: adds Navigation Segmetns into parameter vector of NavSegments
void NavigatorImpl::addNavigationSegment(GeoCoord& startCoord, GeoCoord& endCoord, vector<NavSegment>& directions, MyMap<GeoCoord, navigPointer*> *path) const
{
    GeoCoord geoCo = endCoord;
    stack<NavSegment> navStack;
    navigPointer **point = path->find(geoCo);
    
    int indexOfPath = 0;
    double combinationDist = 0;
    
    //repeatedly until previous point is not null
    while ((*point)->mPrevPoint != nullptr)
    {
        navigPointer* tempNav = *point;
        
        //push a PROCEED NavSegment onto navStack
        
        StreetSegment streetSeg;
        streetSeg.segment.start = *(tempNav->mPrevPoint->mPoint);
        streetSeg.segment.end = *(tempNav->mPoint);
        
        //sets streetSegment's street name if previous's previous points to nullptr
        navigPointer* prevPoint = (*point)->mPrevPoint;
        if (prevPoint->mPrevPoint == nullptr)
            streetSeg.streetName = mSegment.getSegments(startCoord).at(0).streetName;
        else
            streetSeg.streetName = mSegment.getSegments(*tempNav->mPoint).at(0).streetName;
        
        string dir = directionOfLine(streetSeg.segment);
        double navDistance;
        
        //adjusts navDistance based on value of indexOfPath
        if (indexOfPath == 1)
            navDistance = round((tempNav->mDistance - combinationDist + 0.001) * 100000.0) / 100000.0;
        else
            navDistance = tempNav->mDistance;
        
        //set combinationDist to modified navDistance if indexOfPath is 0
        if (indexOfPath == 0)
            combinationDist = navDistance;
        
        NavSegment navSeg(dir, streetSeg.streetName, navDistance, streetSeg.segment);
        navStack.push(navSeg);
        
        geoCo = *((*point)->mPrevPoint->mPoint);
        point = path->find(geoCo);
        
        //increment indexOfPath
        indexOfPath++;
    }
    
    int maxStackSize = navStack.size();
    
    //repeatedly until navStack is emptied
    while (!navStack.empty())
    {
        NavSegment navSeg = navStack.top();
        
        if (maxStackSize == directions.size())
            return;
        
        //push back the top navSeg and pop it off navStack
        directions.push_back(navSeg);
        navStack.pop();
        if (!navStack.empty())
        {
            NavSegment next = navStack.top();
            
            //if the street names are different
            if (navSeg.m_streetName != next.m_streetName)
            {
                double angle = angleBetween2Lines(navSeg.m_geoSegment, next.m_geoSegment);
                
                string direction = "";
                
                if (0 <= angle && angle < 180)
                    direction = "left";
                else
                    direction = "right";
        
                string streetName = next.m_streetName;
                
                //push a TURN NavSegment onto navStack
                NavSegment turn(direction, streetName);
                directions.push_back(turn);
            }
        }
    }
}

//NavigatorImpl findShortPath method: returns vector of NavSegments that should get from startCoord to endCoord in shortest way possible
bool NavigatorImpl::findShortPath(GeoCoord& startCoord, GeoCoord& endCoord, vector<NavSegment>& directions) const
{
    MyMap<GeoCoord, navigPointer*> pathToDest;
    priority_queue<navigPointer*, vector<navigPointer*>, PriorityCheck> navQueue;
    navigPointer *navPt;
    
    //create new navigPointer based on startCoord parameter and associate with startoord
    navPt = new navigPointer(mSegment, startCoord, nullptr);
    navPt->setPriority(endCoord, nullptr);
    navQueue.push(navPt);
    pathToDest.associate(startCoord, navPt);
    
    //return false if there are no neighboring streets to the destination
    vector<StreetSegment> neighboringStreets = mSegment.getSegments(endCoord);
    if (neighboringStreets.empty())
        return false;
    
    while (!navQueue.empty())
    {
        navigPointer* currnavigPointer = navQueue.top();
        navQueue.pop();
        
        vector<StreetSegment> navigPointerNeighbors = mSegment.getSegments(*currnavigPointer->mPoint);
        
        //repeatedly through neighbors of current point
        for (int i = 0; i < navigPointerNeighbors.size(); i++)
        {
            StreetSegment streetRunner = navigPointerNeighbors.at(i);
            
            GeoCoord neighboringPoints[2] = {streetRunner.segment.start, streetRunner.segment.end };
            
            //for the start and end segments of the street...
            for (int j = 0; j < 2; j++)
            {
                if (neighboringPoints[j] == *(currnavigPointer->mPoint))
                    continue;
                
                //try finding neighboring navigPointers to set their priority
                navigPointer** neighbornavigPointers = pathToDest.find(neighboringPoints[j]);
                
                //otherwise create a new navigPointer and associate with neighboring point
                if (neighbornavigPointers == nullptr)
                {
                    navigPointer* temp = new navigPointer(mSegment, neighboringPoints[j], currnavigPointer);
                    temp->setPriority(endCoord, currnavigPointer);
                        
                    navQueue.push(temp);
                    pathToDest.associate(neighboringPoints[j], temp);
                }
                else
                    (*neighbornavigPointers)->setPriority(endCoord, currnavigPointer);
            }
        }
        
        currnavigPointer->mProcess = true;
        bool neighboringChecked = true;
        
        //check if surrounding streets around the end has been visited yet
        for (int i = 0; i < neighboringStreets.size(); i++)
        {
            StreetSegment streetRunner = neighboringStreets.at(i);
            
            navigPointer** nav1 = pathToDest.find(streetRunner.segment.start);
            navigPointer** nav2 = pathToDest.find(streetRunner.segment.end);
            if (nav1 == nullptr || nav2 == nullptr)
            {
                neighboringChecked = false;
                break;
            }
            else if ((*nav1)->mProcess != true || (*nav2)->mProcess != true)
            {
                neighboringChecked = false;
                break;
            }
        }
        
        //connects the end coordinate with current navigPointer and adds NavigationSegment if surrounding street has been visited
        if (neighboringChecked)
        {
            connectSegment(endCoord, currnavigPointer, pathToDest);
            addNavigationSegment(startCoord, endCoord, directions, &pathToDest);
            
            return true;
        }
    }
    return false;
}

//NavigatorImpl navigate method: takes in two coordinates and returns step by step navigation
NavResult NavigatorImpl::navigate(string start, string end, vector<NavSegment> &directions) const
{
    directions.clear();
    
    GeoCoord startGeo, endGeo;
    StreetSegment startSeg, endSeg;
    
    //return if parameters are invalid
    if (!mAttraction.getGeoCoord(start, startGeo))
        return NAV_BAD_SOURCE;
    if (!mAttraction.getGeoCoord(end, endGeo))
        return NAV_BAD_DESTINATION;

    //lowercase parameter strings and return success if they are equal
    string lowercaseStart = start, lowercaseEnd = end;
    transform(lowercaseStart.begin(), lowercaseStart.end(), lowercaseStart.begin(), ::tolower);
    transform(lowercaseEnd.begin(), lowercaseEnd.end(), lowercaseEnd.begin(), ::tolower);
    if (lowercaseStart == lowercaseEnd)
        return NAV_SUCCESS;
    
    //call findShortPath to see if navigation is a success
    if (findShortPath(startGeo, endGeo, directions))
        return NAV_SUCCESS;
    
    //otherwise return that no route is possible
    directions.clear();
    return NAV_NO_ROUTE;
}

//NavigatorImpl getParticularStreetSegForAttraction method: gets street segment that parameter coordinate is on
bool NavigatorImpl::getParticularStreetSegForAttraction(GeoCoord start, StreetSegment &streetSeg) const
{
    //find all possible street segments parameter attraction is on
    vector<StreetSegment> nearbySegments = mSegment.getSegments(start);
    
    //repeatedly for all possible street segments
    for (size_t i = 0; i < nearbySegments.size(); i++)
    {
        //repeatedly for all possible attractions on each street segment
        for (int j = 0; j < nearbySegments[i].attractions.size(); j++)
        {
            //get coordinate at each attraction
            GeoCoord coord;
            if (mAttraction.getGeoCoord(nearbySegments[i].attractions[j].name, coord))
            {
                //if there is a coordinate match, updated parameter streetSeg and return true
                if (coord == start)
                {
                    streetSeg = nearbySegments[i];
                    return true;
                }
            }
        }
    }
    //return false otherwise
    return false;
}

//******************** Navigator functions ************************************

// These functions simply delegate to NavigatorImpl's functions.
// You probably don't want to change any of this code.

Navigator::Navigator()
{
    m_impl = new NavigatorImpl;
}

Navigator::~Navigator()
{
    delete m_impl;
}

bool Navigator::loadMapData(string mapFile)
{
    return m_impl->loadMapData(mapFile);
}

NavResult Navigator::navigate(string start, string end, vector<NavSegment>& directions) const
{
    return m_impl->navigate(start, end, directions);
}
