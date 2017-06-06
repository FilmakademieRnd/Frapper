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

#include "ParticleSystem.h"

#include "Partikkle.h"

ParticleSystem::ParticleSystem() : 
	m_mass(1.0),
	m_dampingConstant(0.03),
	m_timeStep(0.01),
	m_particles(ParticleArrayPtr(new ParticleArray()))
{
}

ParticleSystem::~ParticleSystem()
{
}

void ParticleSystem::setTimeStep(float timeStep)
{
	m_timeStep = timeStep;
}

float ParticleSystem::getTimeStep()
{
	return m_timeStep;
}

const ParticleArrayPtr& ParticleSystem::getParticles()
{
	return m_particles;
}

void ParticleSystem::addEmitter(const ParticleEmitterPtr& particleEmitter)
{
	m_emitters.push_back(particleEmitter);
}

void ParticleSystem::addAffector(const ParticleAffectorPtr& particleAffector)
{
	m_affectors.push_back(particleAffector);
}

void ParticleSystem::removeAllParticles()
{
	m_particles->clear();
}

void ParticleSystem::update()
{
	processEmitters();
	processAffectors();
	decrementLifetime();
	integrate();
}

void ParticleSystem::processEmitters()
{
	for (int i = 0; i < m_emitters.size(); ++i) {
		m_emitters[i]->emitParticles();
	}
}

void ParticleSystem::processAffectors()
{
	for (int i = 0; i < m_affectors.size(); ++i) {
		m_affectors[i]->apply();
	}
}

void ParticleSystem::decrementLifetime()
{
	for (int i = 0; i < m_particles->size(); ++i) {
		Particle& particle = m_particles->at(i);
		if (particle.lifetime < 0.0)
			continue;
		else if ((particle.lifetime - 1.0) < 0.0)
			m_particles->erase(m_particles->begin() + i);
		else
			particle.lifetime -= 1.0;
	}
}

void ParticleSystem::integrate()
{
	for (int i = 0; i < m_particles->size(); ++i) {
		glm::vec3& position = m_particles->at(i).position;
		glm::vec3& force = m_particles->at(i).force;
		glm::vec3& velocity = m_particles->at(i).velocity;
		velocity += force * (1.0f / m_mass) * m_timeStep + velocity * -m_dampingConstant;
		position += velocity * m_timeStep;
		force.x = 0.0f;
		force.y = 0.0f;
		force.z = 0.0f;
	}
}
