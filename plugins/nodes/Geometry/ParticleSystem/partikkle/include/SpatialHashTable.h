/*
    Copyright (C) 2012 by Nils Zweiling (n.zweiling@unexpected.de)

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.
*/

#pragma once

#include "Partikkle.h"
#include "SpatialHashTable.h"

#include <glm/glm.hpp>
#include <memory>

typedef std::vector<std::vector<unsigned int>> HashTable;

class SpatialHashTable;
typedef std::shared_ptr<SpatialHashTable> SpatialHashTablePtr;

class SpatialHashTable
{
public:
    SpatialHashTable();
    ~SpatialHashTable();

public:
	void setGridCellSize(float size);
	float getGridCellSize();
	void setHashTableSize(unsigned int size);
	unsigned int getHashTableSize();
	
	const std::vector<unsigned int>& getVertexIdsOfCurrentCell(const glm::vec3& position);
	void buildFromPositions(const Vec3ArrayPtr& positions);

private:
	void clearHashTableEntries();
	unsigned int calculateHashTableId(const glm::vec3& position);

private:
	unsigned int m_gridCellSize;
	unsigned int m_hashTableSize;
	HashTable m_hashTable;

};

