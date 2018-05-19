//
//  AttractionMapper.cpp
//  project4
//
//  Created by Akaash Venkat on 3/10/17.
//  Copyright © 2017 Akaash Venkat. All rights reserved.
//

#include "provided.h"
#include "MyMap.h"
#include <string>
#include <iostream>
#include <algorithm>
#include <cctype>
using namespace std;

class AttractionMapperImpl
{
    //AtractionMapperImpl public interface provided by spec
public:
	AttractionMapperImpl();
	~AttractionMapperImpl();
	void init(const MapLoader& ml);
	bool getGeoCoord(string attraction, GeoCoord& gc) const;
    
    //AttractionMapperImpl private interface
private:
    MyMap<string, GeoCoord> mAttractionMap;
    string lowercase(const string& attraction) const;
};

//AttractionMapperImpl constructor (empty)
AttractionMapperImpl::AttractionMapperImpl() {}

//AttractionMapperImpl destructor (empty)
AttractionMapperImpl::~AttractionMapperImpl() {}

//AttractionMapperImpl init method: constructs efficient data structure based on parameter data
void AttractionMapperImpl::init(const MapLoader& ml)
{
    //repeatedly for each segment available
    for (int i = 0; i < ml.getNumSegments(); i++)
    {
        StreetSegment temp;
        ml.getSegment(i, temp);
        
        //repeatedly for each attraction in each segment
        for (int j = 0; j < temp.attractions.size(); j++)
        {
            //call MyMap's associate method to associate an attraction's name to its geocoordinates
            mAttractionMap.associate(lowercase(temp.attractions[j].name), temp.attractions[j].geocoordinates);
        }
    }
}

//AttractionMapperImpl getGeoCoord method: finds GeoCoord associated with parameter attraction name
bool AttractionMapperImpl::getGeoCoord(string attraction, GeoCoord& gc) const
{
    const GeoCoord* temp = mAttractionMap.find(lowercase(attraction));
    
    //return false if temp equals nullptr
    if (temp == nullptr)
        return false;
    
    //otherwise, update parameter 'gc' and return true
    else
    {
        gc = *temp;
        return true;
    }
}

//lowercase method: returns lowercase version of input string
string AttractionMapperImpl::lowercase(const string& attraction) const
{
    string modifiedString = "";
    for (int i = 0; i < attraction.size(); i++)
    {
        //adds lowercase version of character to modified string, if character is a letter
        if (attraction[i] >= 'A' && attraction[i] <= 'Z')
            modifiedString += tolower(attraction[i]);
        
        //otherwise adds the character to modified string
        else
            modifiedString += attraction[i];
    }
    
    //returns modified string
    return modifiedString;
}

//******************** AttractionMapper functions *****************************

// These functions simply delegate to AttractionMapperImpl's functions.
// You probably don't want to change any of this code.

AttractionMapper::AttractionMapper()
{
	m_impl = new AttractionMapperImpl;
}

AttractionMapper::~AttractionMapper()
{
	delete m_impl;
}

void AttractionMapper::init(const MapLoader& ml)
{
	m_impl->init(ml);
}

bool AttractionMapper::getGeoCoord(string attraction, GeoCoord& gc) const
{
	return m_impl->getGeoCoord(attraction, gc);
}
