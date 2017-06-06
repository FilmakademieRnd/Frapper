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

#include <glm/glm.hpp>
#include <memory>

class KDTreeVertex;
typedef std::vector<KDTreeVertex*> KDTreeVertexArray;

class KDTreeVertex
{
public:
	KDTreeVertex(glm::vec3 position, unsigned int id) : m_position(position), m_id(id) {}

public:
	const glm::vec3& getPosition()
	{
		return m_position;
	}

	unsigned int getId()
	{
		return m_id;
	}

private:
	unsigned int m_id;
	glm::vec3 m_position;
};

struct KDTreeVertexListSorter
{
	KDTreeVertexListSorter() :
		m_sortAxisIndex(0)
	{
	}

	void setSortAxis(unsigned int sortAxisIndex)
	{
		m_sortAxisIndex = sortAxisIndex;
	}

	bool operator() (KDTreeVertex* i, KDTreeVertex* j)
	{
		return (i->getPosition()[m_sortAxisIndex]) < (j->getPosition()[m_sortAxisIndex]); 
	}

private:
	unsigned int m_sortAxisIndex;

};

//static KDTreeVertexListSorter vertexListSorter;

class KDTreeNode
{

enum TreeSide {
	LEFT,
	RIGHT
};

public:
	KDTreeNode() ;
	~KDTreeNode();

private:
	void sortVertexArray(KDTreeVertexArray& vertexArray);

public:
	KDTreeVertex* getMedianVertex();
	KDTreeNode* getLeftSubTree();
	KDTreeNode* getRightSubTree();
	void buildSubTree(KDTreeVertexArray vertexArray, unsigned int depth = 0);
	KDTreeVertex* findNearestNeighbor(const glm::vec3& position);

private:
	unsigned int m_splitAxis;
	TreeSide m_currentSubTree;
	KDTreeVertex* m_medianVertex;
	KDTreeNode* m_subTrees[2];

};

