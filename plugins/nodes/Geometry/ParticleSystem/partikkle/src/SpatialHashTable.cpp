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

#include "SpatialHashTable.h"

SpatialHashTable::SpatialHashTable() : 
	m_gridCellSize(0.1),
	m_hashTableSize(299)
{	
	m_hashTable.resize(m_hashTableSize);
}

SpatialHashTable::~SpatialHashTable()
{
}

void SpatialHashTable::setGridCellSize(float size)
{
	m_gridCellSize = size;
}

float SpatialHashTable::getGridCellSize()
{
	return m_gridCellSize;
}

void SpatialHashTable::setHashTableSize(unsigned int size)
{
	m_hashTableSize = size;
	m_hashTable.resize(m_hashTableSize);
}

unsigned int SpatialHashTable::getHashTableSize()
{
	return m_hashTableSize;
}

const std::vector<unsigned int>& SpatialHashTable::getVertexIdsOfCurrentCell(const glm::vec3& position)
{
	unsigned int hashId = calculateHashTableId(position);
	std::vector<unsigned int>& entry = m_hashTable.at(hashId);
	return entry;
}

void SpatialHashTable::buildFromPositions(const Vec3ArrayPtr& positions)
{
	clearHashTableEntries();
	for (int i = 0; i < positions->size(); ++i) {
		const glm::vec3& position = positions->at(i);
		unsigned int hashId = calculateHashTableId(position);
		std::vector<unsigned int>& entry = m_hashTable.at(hashId);
		entry.push_back(i);
	}
}

void SpatialHashTable::clearHashTableEntries()
{
	for (int i = 0; i < m_hashTable.size(); ++i) {
		std::vector<unsigned int>& entry = m_hashTable.at(i);
		entry.clear();
	}
}

unsigned int SpatialHashTable::calculateHashTableId(const glm::vec3& position)
{
	int i = (int) floor(position.x);
	int j = (int) floor(position.y);
	int k = (int) floor(position.z);

	unsigned int hash = ((i * 73856093) ^ (j * 19349663) ^ (k * 83492791)) % m_hashTableSize;
	return hash;
}
