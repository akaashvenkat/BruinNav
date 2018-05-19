//
//  SegmentMapper.cpp
//  project4
//
//  Created by Akaash Venkat on 3/10/17.
//  Copyright © 2017 Akaash Venkat. All rights reserved.
//

#include "provided.h"
#include "MyMap.h"
#include "support.h"
#include <vector>
using namespace std;

class SegmentMapperImpl
{
    //SegmentMapperImpl public interface provided by spec
public:
	SegmentMapperImpl();
	~SegmentMapperImpl();
	void init(const MapLoader& ml);
	vector<StreetSegment> getSegments(const GeoCoord& gc) const;
    
    //SegmentMapperImpl private interface
private:
    MyMap<GeoCoord, vector<StreetSegment>> mStreetSegmentCoord;
};

//SegmentMapperImpl constructor (empty)
SegmentMapperImpl::SegmentMapperImpl() {}

//SegmentMapperImpl destructor (empty)
SegmentMapperImpl::~SegmentMapperImpl() {}

//SegmentMapperImpl init method: constructs efficient data structure based on parameter data
void SegmentMapperImpl::init(const MapLoader& ml)
{
    for (size_t i = 0; i < ml.getNumSegments(); i++)
    {
        StreetSegment street;
        ml.getSegment(i, street);
        
        GeoSegment geoSeg = street.segment;
        GeoCoord start = geoSeg.start;
        GeoCoord end = geoSeg.end;
        
        vector<StreetSegment>* temp = mStreetSegmentCoord.find(start);
        
        //if start GeoCoord has corresponding StreetSegment, push StreetSegment into vector
        if (temp != nullptr)
            temp->push_back(street);
        else
        {
            //create new, empty StreetSegment
            vector<StreetSegment> streetSeg;
            
            //push new StreetSegment into vector, and associate with start GeoCoord
            streetSeg.push_back(street);
            mStreetSegmentCoord.associate(start, streetSeg);
        }

        temp = mStreetSegmentCoord.find(end);
        
        //if end GeoCoord has corresponding StreetSegment, push StreetSegment into vector
        if (temp != nullptr)
            temp->push_back(street);
        else
        {
            //create new, empty StreetSegment
            vector<StreetSegment> streetSeg;
            
            //push new StreetSegment into vector, and associate with end GeoCoord
            streetSeg.push_back(street);
            mStreetSegmentCoord.associate(end, streetSeg);
        }
        
        for (int j = 0; j < street.attractions.size(); j++)
        {
            GeoCoord attractionCoord = street.attractions.at(j).geocoordinates;
            
            temp = mStreetSegmentCoord.find(attractionCoord);
            
            //if attractionCoord has corresponding StreetSegment, push StreetSegment into vector
            if (temp != nullptr)
                temp->push_back(street);
            else
            {
                //create new, empty StreetSegment
                vector<StreetSegment> streetSeg;
                
                //push new StreetSegment into vector, and associate with attractionCoord
                streetSeg.push_back(street);
                mStreetSegmentCoord.associate(attractionCoord, streetSeg);
            }
        }
    }
}

//SegmentMapperImpl getSegments method: returns vector of StreetSegments corresponding to input GeoCoord
vector<StreetSegment> SegmentMapperImpl::getSegments(const GeoCoord& gc) const
{
    //get pointer to vector of StreetSegments, using MyMap's find method
    const vector<StreetSegment>* segment = mStreetSegmentCoord.find(gc);
    
    //if pointer equals nullptr, return empty vector
	if (segment == nullptr)
    {
        vector<StreetSegment> emptyVector;
        return emptyVector;
    }
    
    //return pointer to vector otherwise
    return *segment;
}

//******************** SegmentMapper functions ********************************

// These functions simply delegate to SegmentMapperImpl's functions.
// You probably don't want to change any of this code.

SegmentMapper::SegmentMapper()
{
	m_impl = new SegmentMapperImpl;
}

SegmentMapper::~SegmentMapper()
{
	delete m_impl;
}

void SegmentMapper::init(const MapLoader& ml)
{
	m_impl->init(ml);
}

vector<StreetSegment> SegmentMapper::getSegments(const GeoCoord& gc) const
{
	return m_impl->getSegments(gc);
}
