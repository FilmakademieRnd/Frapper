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

#include "KDTreeNode.h"
#include <algorithm>

static KDTreeVertexListSorter vertexListSorter;

KDTreeNode::KDTreeNode() :
	m_splitAxis(0),
	m_currentSubTree(LEFT)
{
	m_subTrees[LEFT] = 0;
	m_subTrees[RIGHT] = 0;
}

KDTreeNode::~KDTreeNode()
{
	if (m_subTrees[LEFT])
		delete m_subTrees[LEFT];

	if (m_subTrees[RIGHT])
		delete m_subTrees[RIGHT];
}

KDTreeVertex* KDTreeNode::getMedianVertex()
{
	return m_medianVertex;
}

KDTreeNode* KDTreeNode::getLeftSubTree()
{
	return m_subTrees[LEFT];
}
	
KDTreeNode* KDTreeNode::getRightSubTree()
{
	return m_subTrees[RIGHT];
}
	
void KDTreeNode::buildSubTree(KDTreeVertexArray vertexArray, unsigned int depth)
{
	m_splitAxis = depth % 3;
	unsigned int medianIndex = vertexArray.size() / 2;
		
	vertexListSorter.setSortAxis(m_splitAxis);
	sortVertexArray(vertexArray);
	m_medianVertex = vertexArray.at(medianIndex);

	KDTreeVertexArray leftVertexArray(vertexArray.begin(), vertexArray.begin() + medianIndex);
	if (!leftVertexArray.empty()) {
		m_subTrees[LEFT] = new KDTreeNode();
		m_subTrees[LEFT]->buildSubTree(leftVertexArray, depth + 1);
	}

	KDTreeVertexArray rightVertexArray(vertexArray.begin() + medianIndex + 1, vertexArray.end());
	if (!rightVertexArray.empty()) {
		m_subTrees[RIGHT] = new KDTreeNode();
		m_subTrees[RIGHT]->buildSubTree(rightVertexArray, depth + 1);
	}
}

void KDTreeNode::sortVertexArray(KDTreeVertexArray& vertexArray)
{
	std::sort(vertexArray.begin(), vertexArray.end(), vertexListSorter);
}

KDTreeVertex* KDTreeNode::findNearestNeighbor(const glm::vec3& position)
{
	KDTreeVertex* nearestNeighbor = 0;
	if (!m_subTrees[LEFT] && !m_subTrees[RIGHT]) {
		nearestNeighbor = m_medianVertex;
		return nearestNeighbor;
	}
			
	if (m_subTrees[LEFT] && position[m_splitAxis] <= m_medianVertex->getPosition()[m_splitAxis]) {
		m_currentSubTree = LEFT;
		nearestNeighbor = m_subTrees[LEFT]->findNearestNeighbor(position);
	}
	else if (m_subTrees[RIGHT] && position[m_splitAxis] > m_medianVertex->getPosition()[m_splitAxis]) {
		m_currentSubTree = RIGHT;
		nearestNeighbor = m_subTrees[RIGHT]->findNearestNeighbor(position);
	}
	else
		nearestNeighbor = m_medianVertex;

	glm::vec3 distanceVector = nearestNeighbor->getPosition() - position;
	float squaredDistanceToCurrentBest = glm::dot(distanceVector, distanceVector);
	distanceVector = m_medianVertex->getPosition() - position;
	float squaredDistanceToCurrentNode = glm::dot(distanceVector, distanceVector);
	
	if (squaredDistanceToCurrentNode < squaredDistanceToCurrentBest) {
		nearestNeighbor = m_medianVertex;
		squaredDistanceToCurrentBest = squaredDistanceToCurrentNode;
	}

	float distanceToSplitAxis = abs(m_medianVertex->getPosition()[m_splitAxis] - position[m_splitAxis]);
	float squaredDistanceToSplitAxis = distanceToSplitAxis * distanceToSplitAxis;

	if (squaredDistanceToCurrentBest > squaredDistanceToSplitAxis) {
		unsigned int oppositeSubTree = (m_currentSubTree + 1) % 2;
		if (m_subTrees[oppositeSubTree]) {
			KDTreeVertex* newNearestNeighbor = m_subTrees[oppositeSubTree]->findNearestNeighbor(position);
			glm::vec3 distanceVector = newNearestNeighbor->getPosition() - position;
			float squaredDistanceToOppositeSubTreesBest = glm::dot(distanceVector, distanceVector);
			if (squaredDistanceToOppositeSubTreesBest < squaredDistanceToCurrentBest)
				nearestNeighbor = newNearestNeighbor;
		}
	}			
	
	return nearestNeighbor;
}
