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

#include "MeshAffector.h"
#include "ParticleSystem.h"

#include <QList>
#include <QFuture>
#include <QtConcurrent/QtConcurrentRun>

MeshAffector::MeshAffector(const ParticleSystemPtr& particleSystem) :
	ParticleAffector(particleSystem),
	m_strength(5.0)
{
}

MeshAffector::~MeshAffector()
{
}

void MeshAffector::setStrength(float strength)
{
	m_strength = strength;
}

float MeshAffector::getStrength()
{
	return m_strength;
}

void MeshAffector::setVertices(const Vec3ArrayPtr& vertices)
{
	m_vertices = vertices;
}

void MeshAffector::setNormals(const Vec3ArrayPtr& normals)
{
	m_normals = normals;
}

void MeshAffector::setFaces(const u16Vec3ArrayPtr& faces)
{
	m_faces = faces;
}

void MeshAffector::initialize()
{
	m_kdTree.buildFromPositions(m_vertices);
	generateTriangleLookupTable();
}

void MeshAffector::generateTriangleLookupTable()
{
	m_triangleLookupTable.resize(m_vertices->size());
	for (int i = 0; i < m_faces->size(); ++i) {
		const glm::u16vec3& face = m_faces->at(i);
		for (int j = 0; j < 3; ++j)
			m_triangleLookupTable.at(face[j]).push_back(i);
	}
}

std::vector<unsigned int> MeshAffector::findTrianglesWithVertex(unsigned int vertexId)
{
	if (vertexId < m_triangleLookupTable.size())
		return m_triangleLookupTable.at(vertexId);
	else
		return std::vector<unsigned int>();
}

ClosestPointOnTriangle MeshAffector::findClosestPointOnMesh(const glm::vec3& position)
{
	unsigned int vertexId = m_kdTree.findNearestNeighbor(position)->getId();
	std::vector<unsigned int> triangleIds = findTrianglesWithVertex(vertexId);
	glm::vec3 normal = m_normals->at(vertexId);

	float closestDistance = FLT_MAX;
	glm::vec3 closestPointOnMesh;
	unsigned int triangleId = 0;
	for (int i = 0; i < triangleIds.size(); ++i) {
		triangleId = triangleIds.at(i);
		glm::vec3 closestPointOnTriangle = findClosestPointOnTriangle(position, triangleId);
		glm::vec3 distanceVector(closestPointOnTriangle - position);
		float distance = glm::length(distanceVector);
		if (distance < closestDistance) {
			closestDistance = distance;
			closestPointOnMesh = closestPointOnTriangle;
		}
	}

	// the calculated normal should always point to the outside of the mesh
	glm::vec3 distanceVector = glm::normalize(position - closestPointOnMesh);
	if (glm::dot(normal, distanceVector) < 0)
		distanceVector *= -1.0;

	ClosestPointOnTriangle closestPoint(closestPointOnMesh, normal, triangleId);
	return closestPoint;
}

glm::vec3 MeshAffector::findClosestPointOnTriangle(const glm::vec3& position, unsigned int triangleId)
{	
	const glm::u16vec3& face = m_faces->at(triangleId);
	const glm::vec3& vertex0 = m_vertices->at(face[0]);
	const glm::vec3& vertex1 = m_vertices->at(face[1]);
	const glm::vec3& vertex2 = m_vertices->at(face[2]);
	const glm::vec3& baseVector0 = vertex1 - vertex0;
	const glm::vec3& baseVector1 = vertex2 - vertex0;
	const glm::vec3& baseVector2 = glm::cross(baseVector0, baseVector1);
	
	glm::vec3 distanceVector = vertex0;
	glm::mat4 triangleSpaceMatrix(glm::vec4(baseVector0, 0.0), glm::vec4(baseVector1, 0.0), glm::vec4(baseVector2, 0.0), glm::vec4(distanceVector, 1.0));
	glm::vec4 positionInTriangleSpace = glm::inverse(triangleSpaceMatrix) * glm::vec4(position, 1.0);
	positionInTriangleSpace.z = 0.0;
	glm::vec3 closestPointOnPlane = glm::vec3(triangleSpaceMatrix * positionInTriangleSpace);


	float vectorSum = positionInTriangleSpace.x + positionInTriangleSpace.y;
	if (positionInTriangleSpace.x < 0.0 ||
		positionInTriangleSpace.y < 0.0 || 
		vectorSum > 1.0) {

		std::vector<glm::vec3> closestPoints;

		glm::vec3 vertex0ToClosestPoint = closestPointOnPlane - vertex0;
		
		float baseVector0Length = glm::length(baseVector0);
		float projectionOnBaseVector0 = glm::dot(glm::normalize(vertex0ToClosestPoint), baseVector0);
		if (projectionOnBaseVector0 <= baseVector0Length) {
			glm::vec3 closestPointOnTriangle = vertex0 + baseVector0 * projectionOnBaseVector0;
			closestPoints.push_back(closestPointOnTriangle);
		}

		float baseVector1Length = glm::length(baseVector1);
		float projectionOnBaseVector1 = glm::dot(glm::normalize(vertex0ToClosestPoint), baseVector1);
		if (projectionOnBaseVector1 <= baseVector1Length) {
			glm::vec3 closestPointOnTriangle = vertex0 + baseVector1 * projectionOnBaseVector1;
			closestPoints.push_back(closestPointOnTriangle);

		}

		glm::vec3 vertex1ToClosestPoint = closestPointOnPlane - vertex1;
		
		glm::vec3 vertex1ToVertex2 = vertex2 - vertex1;
		float projectionOnVertex1ToVertex2 = glm::dot(glm::normalize(vertex1ToClosestPoint), vertex1ToVertex2);
		if (projectionOnVertex1ToVertex2 <= glm::length(vertex1ToVertex2)) {
			glm::vec3 closestPointOnTriangle = vertex1 + vertex1ToVertex2 * projectionOnVertex1ToVertex2;
			closestPoints.push_back(closestPointOnTriangle);
		}

		float closestDistance = FLT_MAX;
		glm::vec3 closestPointOnTriangle;
		for (int i = 0; i < closestPoints.size(); ++i) {
			float currentDistance = glm::length(closestPoints.at(i) - position);
			if (currentDistance < closestDistance) {
				closestDistance = currentDistance;
				closestPointOnPlane = closestPoints.at(i);
			}
		}
	}

	
	return glm::vec3(closestPointOnPlane);
}

extern void applyForces_thread(MeshAffector* meshAffector, const ParticleArrayPtr& particles, unsigned int startId, unsigned int endId)
{
	for (int i = startId; i < endId; ++i) {
		
		const glm::vec3& currentPosition = particles->at(i).position;
		
		ClosestPointOnTriangle closestPointOnMesh = meshAffector->findClosestPointOnMesh(currentPosition);
		glm::vec3 distanceVector =  closestPointOnMesh.m_position - currentPosition;
		glm::vec3 normalizedDistanceVector = glm::normalize(distanceVector);

		particles->at(i).orientation = closestPointOnMesh.m_normal;

		float squaredDistance = glm::dot(distanceVector, distanceVector);
		if (squaredDistance < (0.05 * 0.05))
			continue;
		
		float forceMagnitude = meshAffector->getStrength() / squaredDistance;
		if (forceMagnitude > 50)
				forceMagnitude = 50;
		
		particles->at(i).force +=  normalizedDistanceVector * forceMagnitude;
	}
}

void MeshAffector::applyForces()
{
	//applyForces_singlethreaded();
	applyForces_multithreaded();
}

void MeshAffector::applyForces_singlethreaded()
{
	ParticleSystemPtr particleSystem = m_particleSystem.lock();
	if (!particleSystem)
		return;
	
	const ParticleArrayPtr& particles = particleSystem->getParticles();
	unsigned int numberOfParticles = particles->size();

		for (int i = 0; i < numberOfParticles; ++i) {
		
		const glm::vec3& currentPosition = particles->at(i).position;
		
		ClosestPointOnTriangle closestPointOnMesh = findClosestPointOnMesh(currentPosition);
		glm::vec3 distanceVector =  closestPointOnMesh.m_position - currentPosition;
		glm::vec3 normalizedDistanceVector = glm::normalize(distanceVector);

		particles->at(i).orientation = closestPointOnMesh.m_normal;

		float squaredDistance = glm::dot(distanceVector, distanceVector);
		if (squaredDistance < (0.05 * 0.05))
			continue;
		
		float forceMagnitude = m_strength / squaredDistance;
		if (forceMagnitude > 50)
				forceMagnitude = 50;
		
		particles->at(i).force +=  normalizedDistanceVector * forceMagnitude;
	}
}

void MeshAffector::applyForces_multithreaded()
{
	ParticleSystemPtr particleSystem = m_particleSystem.lock();
	if (!particleSystem)
		return;

	const ParticleArrayPtr& particles = particleSystem->getParticles();
	unsigned int numberOfParticles = particles->size();

	QList<QFuture<void>> futures;
	static const unsigned int NUMBER_OF_THREADS = 4;
	unsigned int particlesPerThread = numberOfParticles / NUMBER_OF_THREADS;
	unsigned int particleMod = numberOfParticles % NUMBER_OF_THREADS; 
	unsigned int startIndex = 0;
	for (int i = 0; i < NUMBER_OF_THREADS; ++i) {
		unsigned int endIndex = startIndex + particlesPerThread;
		if (i < particleMod)
			endIndex++;

		QFuture<void> future = QtConcurrent::run(applyForces_thread, this, particles, startIndex, endIndex);
		futures.append(future);
		startIndex = endIndex;
		
	}

	for (int i = 0; i < futures.size(); ++i) {
		QFuture<void> future = futures.at(0); 
		future.waitForFinished();
	}
}
