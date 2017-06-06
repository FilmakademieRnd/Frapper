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

#include "PointAffector.h"
#include "ParticleSystem.h"

PointAffector::PointAffector(const ParticleSystemPtr& particleSystem) :
	ParticleAffector(particleSystem),
	m_strength(1.0),
	m_cutoffRadius(1.0)
{	
}

PointAffector::~PointAffector()
{
}

void PointAffector::setPosition(const glm::vec3& position)
{
	m_position = position; 
}

const glm::vec3& PointAffector::getPosition()
{
	return m_position;
}

void PointAffector::setStrength(float strength)
{
	m_strength = strength;
}

float PointAffector::getStrength()
{
	return m_strength;
}

void PointAffector::setCutoffRadius(float cutoffRadius)
{
	m_cutoffRadius = cutoffRadius; 
}

float PointAffector::getCutoffRadius()
{
	return m_cutoffRadius;
}

void PointAffector::applyForces()
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
		if (particles->at(i).lifetime > 350)
			continue;

		glm::vec3 distanceVector = m_position - particles->at(i).position;
		float distance = glm::length(distanceVector);
		if (distance > m_cutoffRadius)
			continue;

		float forceMagnitude = m_strength / (distance * distance);
		if (forceMagnitude > 300)
			forceMagnitude = 300;

		//particles->at(i).force += glm::vec3(0.0, 0.0, -1.0) * glm::length(distanceVector) * 500.0f;
		particles->at(i).force += glm::normalize(distanceVector) * -forceMagnitude;
	}
}
