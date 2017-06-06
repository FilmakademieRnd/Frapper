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

#include "AxisAffector.h"
#include "ParticleSystem.h"

AxisAffector::AxisAffector(const ParticleSystemPtr& particleSystem) :
	ParticleAffector(particleSystem),
	m_direction(glm::vec3(0.0, 0.0, -1.0)),
	m_strength(1.0),
	m_cutoffRadius(1.0)
{	
}

AxisAffector::~AxisAffector()
{
}

void AxisAffector::setPosition(const glm::vec3& position)
{
	m_position = position; 
}

const glm::vec3& AxisAffector::getPosition()
{
	return m_position;
}

void AxisAffector::setDirection(const glm::vec3& direction)
{
	m_direction = glm::normalize(direction); 
}

const glm::vec3& AxisAffector::getDirection()
{
	return m_direction;
}

void AxisAffector::setStrength(float strength)
{
	m_strength = strength;
}

float AxisAffector::getStrength()
{
	return m_strength;
}

void AxisAffector::setCutoffRadius(float cutoffRadius)
{
	m_cutoffRadius = cutoffRadius; 
}

float AxisAffector::getCutoffRadius()
{
	return m_cutoffRadius;
}

void AxisAffector::applyForces()
{
	ParticleSystemPtr particleSystem = m_particleSystem.lock();
	if (!particleSystem)
		return;

	const ParticleArrayPtr& particles = particleSystem->getParticles();
	unsigned int numberOfParticles = particles->size();

	for (int i = 0; i < numberOfParticles; ++i) {
		const Particle& particle = particles->at(i);
		float distance = glm::length(m_position - particle.position);
		if (distance > m_cutoffRadius)
			continue;

		glm::vec3 distanceVector = calculateVectorToAxis(particle.position);
		distance = glm::length(distanceVector);
		
		float forceMagnitude = m_strength / (distance * distance);
		if (forceMagnitude > 300)
			forceMagnitude = 300;

		particles->at(i).force += m_direction * -forceMagnitude;
	}
}

glm::vec3 AxisAffector::calculateVectorToAxis(const glm::vec3& position)
{
	glm::vec3 toParticleVector = position - m_position;
	float projectedLength = glm::dot(toParticleVector, m_direction);
	glm::vec3 projectedPosition = m_direction * projectedLength;
	glm::vec3 distanceVector = projectedPosition - toParticleVector;
	return distanceVector;
}
