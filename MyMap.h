//
//  MyMap.h
//  project4
//
//  Created by Akaash Venkat on 3/9/17.
//  Copyright © 2017 Akaash Venkat. All rights reserved.
//

#ifndef MyMap_h
#define MyMap_h

#include <algorithm>
#include <iostream>
#include <typeinfo>
#include "support.h"
using namespace std;

template<typename KeyType, typename ValueType>
class MyMap
{
    //MyMap public interface provided by spec
public:
    MyMap();
    ~MyMap();
    void clear();
    int size() const;
    void associate(const KeyType& key, const ValueType& value);
    
    // for a map that can't be modified, return a pointer to const ValueType
    const ValueType* find(const KeyType& key) const;
    
    // for a modifiable map, return a pointer to modifiable ValueType
    ValueType* find(const KeyType& key)
    {
        return const_cast<ValueType*>(const_cast<const MyMap*>(this)->find(key));
    }
    
    // C++11 syntax for preventing copying and assignment
    MyMap(const MyMap&) = delete;
    MyMap& operator=(const MyMap&) = delete;
    
    //MyMap private interface
private:
    struct Node
    {
        KeyType mKey;
        ValueType mValue;
        Node* mRight;
        Node* mLeft;
    };
    Node* mRoot = nullptr;
    int mSize = 0;
    void clear(Node* cur);
};

//MyMap constructor
template<typename KeyType, typename ValueType>
MyMap<KeyType,ValueType>::MyMap() {}

//MyMap destructor: calls clear method
template<typename KeyType, typename ValueType>
MyMap<KeyType,ValueType>::~MyMap()
{
    clear();
}

//MyMap clear method: calls recursive clearTree method
template<typename KeyType, typename ValueType>
void MyMap<KeyType,ValueType>::clear()
{
    clear(mRoot);
}

//MyMap clearTree method: recursively deletes all of tree nodes
template<typename KeyType, typename ValueType>
void MyMap<KeyType,ValueType>::clear(Node* cur)
{
    if (cur == nullptr)
        return;
    clear(cur->mLeft);
    clear(cur->mRight);
    delete cur;
}

//MyMap size method: returns number of elements in tree
template<typename KeyType, typename ValueType>
int MyMap<KeyType,ValueType>::size() const
{
    return mSize;
}

//MyMap associate method: adds new element or modifies existing element
template<typename KeyType, typename ValueType>
void MyMap<KeyType,ValueType>::associate(const KeyType& key, const ValueType& value)
{
    //sets the new node to mRoot if there are 0 elements
    if (size() == 0)
    {
        mRoot = new Node();
        mRoot->mKey = key;
        mRoot->mValue = value;
        mRoot->mRight = nullptr;
        mRoot->mLeft = nullptr;
        mSize++;
        return;
    }
    else
    {
        ValueType* existingVal = find(key);
        //modifies value if existing key is present in binary search tree
        if (existingVal != nullptr)
            *existingVal = value;
        
        else
        {
            //otherwise creates a new node with parameter 'key' and 'value'
            Node* newNode = new Node();
            newNode->mKey = key;
            newNode->mValue = value;
            newNode->mRight = nullptr;
            newNode->mLeft = nullptr;
            
            //adds new node as a leaf to the binary search tree
            Node* tempNode = mRoot;
            for (;;)
            {
                //safety check: don't do anything if matching key present
                if (key == tempNode->mKey)
                    return;
                
                //if input key < current key, move left
                else if (key < tempNode->mKey)
                {
                    //parse through the left side of current node, if possible
                    if (tempNode->mLeft != nullptr)
                        tempNode = tempNode->mLeft;
                    
                    //add the new node to the left of the current node, and increment count
                    else
                    {
                        tempNode->mLeft = newNode;
                        mSize++;
                        return;
                    }
                }
                
                //if input key > current key, move right
                else
                {
                    //parse through right side of current node, if possible
                    if (tempNode->mRight != nullptr)
                        tempNode = tempNode->mRight;
                    
                    //add the new node to the right of the current node, and increment count
                    else
                    {
                        tempNode->mRight = newNode;
                        mSize++;
                        return;
                    }
                }
            }
        }
    }
}

//MyMap find method: returns 'value' corresponding to matching 'key'
template<typename KeyType, typename ValueType>
const ValueType* MyMap<KeyType,ValueType>::find(const KeyType& key) const
{
    Node* pointer = mRoot;
    
    //repeatedly until pointer is not equal to nullptr
    while (pointer != nullptr)
    {
        //return 'value' if 'key' matches
        if (pointer->mKey == key)
            return &(pointer->mValue);
        
        //otherwise move to the left or right
        else if (key < pointer->mKey)
            pointer = pointer->mLeft;
        else
            pointer = pointer->mRight;
    }
    
    //otherwise return nullptr
    return nullptr;
}

#endif /* MyMap_h */
