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

#include "PointEmitter.h"
#include "ParticleSystem.h"

PointEmitter::PointEmitter(const ParticleSystemPtr& particleSystem) :
	ParticleEmitter(particleSystem),
	m_numberOfParticles(1),
	m_initialSpeed(2.0),
	m_lifetime(400.0)
{	
}

PointEmitter::~PointEmitter()
{
}


void PointEmitter::setNumberOfParticles(unsigned int numberOfParticles)
{
	m_numberOfParticles = numberOfParticles;
}

void PointEmitter::setPosition(const glm::vec3& position)
{
	m_position = position;
}

void PointEmitter::setInitialSpeed(float initialSpeed)
{
	m_initialSpeed = initialSpeed;
}

void PointEmitter::setLifetime(float lifetime)
{
	m_lifetime = lifetime;
}

void PointEmitter::process()
{
	ParticleSystemPtr particleSystem = m_particleSystem.lock();
	if (!particleSystem)
		return;

	const ParticleArrayPtr& particles = particleSystem->getParticles();
	for (int i = 0; i < m_numberOfParticles; ++i) {
		float positionOffsetX = (float) rand() / RAND_MAX;
		float positionOffsetY = (float) rand() / RAND_MAX;
		float velocityOffsetX = (float) rand() / RAND_MAX;
		float velocityOffsetY = (float) rand() / RAND_MAX;

		Particle particle;
		particle.position = m_position + glm::vec3(positionOffsetX * 0.6, positionOffsetY * 0.6, 0.0);
		particle.velocity = glm::vec3((velocityOffsetX - 0.5) * 30.0, (velocityOffsetY - 0.5) * 30.0, m_initialSpeed);
		particle.lifetime = m_lifetime;
		particles->push_back(particle);
	}
}
