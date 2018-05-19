//
//  MapLoader.cpp
//  project4
//
//  Created by Akaash Venkat on 3/10/17.
//  Copyright © 2017 Akaash Venkat. All rights reserved.
//

#include "provided.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

class MapLoaderImpl
{
    //MapLoaderImpl public interface provided by spec
public:
	MapLoaderImpl();
	~MapLoaderImpl();
	bool load(string mapFile);
	size_t getNumSegments() const;
	bool getSegment(size_t segNum, StreetSegment& seg) const;
    
    //MapLoaderImpl private interface
private:
    size_t mNumSegments = 0;
    vector<StreetSegment> mStreetList;
};

//MapLoaderImpl constructor (empty)
MapLoaderImpl::MapLoaderImpl() {}

//MapLoaderImpl destructor (empty)
MapLoaderImpl::~MapLoaderImpl() {}

//MapLoaderImpl load method: loads input file of map data
bool MapLoaderImpl::load(string mapFile)
{
    
    //return false if file cannot be read
    ifstream infile(mapFile);
    if (!infile)
        return false;
    
    string s;
    int headerCount = 2;
    int attractionCount = 0;
    
    //while parsing through map data line by line
    while (getline(infile, s))
    {
        //create new pointer to StreetSegment and add streetName
        if (headerCount == 2)
        {
            StreetSegment newSegment;
            mStreetList.push_back(newSegment);
            mStreetList.at(mNumSegments).streetName = s;
            headerCount--;
            continue;
        }
        
        //add GeoSegment to pointer
        else if (headerCount == 1)
        {
            //separate line by ',' or ' ', to get 4 coordinates
            string token;
            vector<string> coordString;
            istringstream ss(s);
            
            getline(ss, token,',');
            while(ss.peek() == ' ')
                ss.ignore();
            coordString.push_back(token);
            
            getline(ss, token,' ');
            while(ss.peek() == ' ')
                ss.ignore();
            coordString.push_back(token);
            
            getline(ss, token,',');
            while(ss.peek() == ' ')
                ss.ignore();
            coordString.push_back(token);
            
            getline(ss, token);
            while(ss.peek() == ' ')
                ss.ignore();
            coordString.push_back(token);
            
            //compile the coordinates (in text and number forms) to create GeoSegment
            mStreetList.at(mNumSegments).segment.start.latitudeText = coordString.at(0);
            mStreetList.at(mNumSegments).segment.start.latitude = stod(coordString.at(0));
            mStreetList.at(mNumSegments).segment.start.longitudeText = coordString.at(1);
            mStreetList.at(mNumSegments).segment.start.longitude = stod(coordString.at(1));
            mStreetList.at(mNumSegments).segment.end.latitudeText = coordString.at(2);
            mStreetList.at(mNumSegments).segment.end.latitude = stod(coordString.at(2));
            mStreetList.at(mNumSegments).segment.end.longitudeText = coordString.at(3);
            mStreetList.at(mNumSegments).segment.end.longitude = stod(coordString.at(3));
            
            headerCount--;
            continue;
        }
        
        //set attractionCount to number of attractions in street segment
        else if (headerCount == 0)
        {
            attractionCount = stoi(s);
            headerCount--;
            
            //if there are no (or none left) attractions, reset the street segment reading and increment segment number
            if (attractionCount == 0)
            {
                headerCount = 2;
                mNumSegments++;
            }
            continue;
        }
        
        //otherwise add a vector of attractions on street segment to the pointer
        else
        {
            //separate line by '|', ',', and ' ' to separate attraction info
            string token;
            vector<string> attractionString;
            istringstream ss(s);
            
            getline(ss, token,'|');
            while(ss.peek() == ' ')
                ss.ignore();
            attractionString.push_back(token);
            
            getline(ss, token,',');
            while(ss.peek() == ' ')
                ss.ignore();
            attractionString.push_back(token);
            
            getline(ss, token,' ');
            while(ss.peek() == ' ')
                ss.ignore();
            attractionString.push_back(token);
            
            //update specific attraction from data
            Attraction newAttraction;
            newAttraction.name = attractionString.at(0);
            newAttraction.geocoordinates.latitudeText = attractionString.at(1);
            newAttraction.geocoordinates.latitude = stod(attractionString.at(1));
            newAttraction.geocoordinates.longitudeText = attractionString.at(2);
            newAttraction.geocoordinates.longitude = stod(attractionString.at(2));
            
            //push attraction to pointer's vector of attraction
            mStreetList.at(mNumSegments).attractions.push_back(newAttraction);
            
            attractionCount--;
            
            //if there are no (or none left) attractions, reset the street segment reading and increment segment number
            if (attractionCount == 0)
            {
                headerCount = 2;
                mNumSegments++;
            }
        }
    }
    return true;
}

//MapLoaderImpl getNumSegments method: returns number of segments
size_t MapLoaderImpl::getNumSegments() const
{
    return mNumSegments;
}

//MapLoaderImpl getSegment method: returns true if parameter 'segNum' is valid and then updates parameter 'seg'
bool MapLoaderImpl::getSegment(size_t segNum, StreetSegment &seg) const
{
	if (segNum < mNumSegments)
    {
        seg = mStreetList.at(segNum);
        return true;
    }
    return false;
}

//******************** MapLoader functions ************************************

// These functions simply delegate to MapLoaderImpl's functions.
// You probably don't want to change any of this code.

MapLoader::MapLoader()
{
	m_impl = new MapLoaderImpl;
}

MapLoader::~MapLoader()
{
	delete m_impl;
}

bool MapLoader::load(string mapFile)
{
	return m_impl->load(mapFile);
}

size_t MapLoader::getNumSegments() const
{
	return m_impl->getNumSegments();
}

bool MapLoader::getSegment(size_t segNum, StreetSegment& seg) const
{
   return m_impl->getSegment(segNum, seg);
}
