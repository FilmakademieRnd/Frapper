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

#include "InterParticleAffector.h"
#include "ParticleSystem.h"

InterParticleAffector::InterParticleAffector(const ParticleSystemPtr& particleSystem) :
	ParticleAffector(particleSystem),
	m_strength(0.5)
{	
}

InterParticleAffector::~InterParticleAffector()
{
}

void InterParticleAffector::setStrength(float strength)
{
	m_strength = strength;
}

void InterParticleAffector::applyForces()
{
	ParticleSystemPtr particleSystem = m_particleSystem.lock();
	if (!particleSystem)
		return;

	//const Vec3ArrayPtr& positions = particleSystem->getPositions();
	//const Vec3ArrayPtr& velocities = particleSystem->getVelocities();
	//const Vec3ArrayPtr& forces = particleSystem->getForces();
	

	const ParticleArrayPtr& particles = particleSystem->getParticles();
	unsigned int numberOfParticles = particles->size();
	
	Vec3ArrayPtr positions = Vec3ArrayPtr(new Vec3Array());
	for (int i = 0; i < particles->size(); ++i) {
		positions->push_back(particles->at(i).position);
	}

	m_spatialHashTable.buildFromPositions(positions);

	for (int i = 0; i < numberOfParticles; ++i) {
		if (particles->at(i).lifetime > 0.0)
			continue;
		const glm::vec3& particlePosition = particles->at(i).position; 
		const std::vector<unsigned int>& collidingParticleIds = m_spatialHashTable.getVertexIdsOfCurrentCell(particlePosition);
		for (int j = 0; j < collidingParticleIds.size(); ++j) {
			if (particles->at(i).lifetime > 50)
				continue;
			unsigned int particleId = collidingParticleIds.at(j);
			const glm::vec3& collidingParticlePosition = particles->at(particleId).position;
			glm::vec3 distanceVector = particlePosition - collidingParticlePosition;
			float squaredDistance = glm::dot(distanceVector, distanceVector);
			if (squaredDistance == 0.0 || squaredDistance < 0.01)
				continue;
			float forceMagnitude = m_strength / squaredDistance;
			if (forceMagnitude > 200)
				forceMagnitude = 200;
			glm::vec3 debug = glm::normalize(distanceVector);
			particles->at(i).force += glm::normalize(distanceVector) * forceMagnitude;
		}
	}

	//for (int i = 0; i < numberOfParticles; ++i) {
	//	if (particles->at(i).lifetime > 0.0)
	//		continue;
	//	const glm::vec3& particlePosition = particles->at(i).position; 
	//	for (int j = 0; j < numberOfParticles; ++j) {
	//		if (particles->at(i).lifetime > 50)
	//			continue;
	//		glm::vec3 distanceVector = particlePosition - particles->at(j).position;
	//		float distance = glm::length(distanceVector);
	//		if (distance == 0.0 || distance > 0.1)
	//			continue;
	//		float forceMagnitude = m_strength / (distance * distance);
	//		if (forceMagnitude > 200)
	//			forceMagnitude = 200;
	//		glm::vec3 debug = glm::normalize(distanceVector);
	//		particles->at(i).force += glm::normalize(distanceVector) * forceMagnitude;
	//	}
	//}
}
