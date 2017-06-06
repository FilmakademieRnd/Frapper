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

#include "PlaneAffector.h"
#include "ParticleSystem.h"

PlaneAffector::PlaneAffector(const ParticleSystemPtr& particleSystem) :
	ParticleAffector(particleSystem),
	m_strength(5.0),
	m_topLeft(glm::vec2(-1.0, 1.0)),
	m_bottomRight(glm::vec2(1.0, -1.0))
{
	//if (particleSystem) {
	//	const Vec3ArrayPtr& positions = particleSystem->getPositions();
	//	m_initalPositions = *positions;
	//}
}

PlaneAffector::~PlaneAffector()
{
}

void PlaneAffector::applyForces()
{
	ParticleSystemPtr particleSystem = m_particleSystem.lock();
	if (!particleSystem)
		return;

	//const Vec3ArrayPtr& positions = particleSystem->getPositions();
	//const Vec3ArrayPtr& velocities = particleSystem->getVelocities();
	//const Vec3ArrayPtr& forces = particleSystem->getForces();

	const ParticleArrayPtr& particles = particleSystem->getParticles();
	unsigned int numberOfParticles = particles->size();

	for (int i = 0; i < numberOfParticles; ++i) {
		if (particles->at(i).lifetime > 350.0)
			continue;
		
		const glm::vec3& currentPosition = particles->at(i).position;
		glm::vec3 onPlanePosition = glm::vec3(currentPosition.x, currentPosition.y, 0.0);
		
		if (currentPosition.x < m_topLeft.x)
			onPlanePosition.x = m_topLeft.x;
		else if (currentPosition.x > m_bottomRight.x)
			onPlanePosition.x = m_bottomRight.x;

		if (currentPosition.y > m_topLeft.y)
			onPlanePosition.y = m_topLeft.y;
		else if (currentPosition.y < m_bottomRight.y)
			onPlanePosition.y = m_bottomRight.y;

		glm::vec3 distanceVector =  onPlanePosition - currentPosition;
		float distance = glm::length(distanceVector);
		if (distance < 0.05)
			continue;
		float forceMagnitude = m_strength / (distance * distance);
		if (forceMagnitude > 50)
				forceMagnitude = 50;
		particles->at(i).force += glm::normalize(distanceVector) * forceMagnitude;
	}
}
