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

#include "RandomEmitter.h"
#include "Partikkle.h"
#include "ParticleSystem.h"

RandomEmitter::RandomEmitter(const ParticleSystemPtr& particleSystem) :
	ParticleEmitter(particleSystem),
	m_numberOfParticles(1000),
	m_minSize(0.1),
	m_maxSize(0.2),
	m_topLeft(glm::vec2(-1, 1)),
	m_bottomRight(glm::vec2(1, -1))
{
	setOneTimeEmitter(true);
}

RandomEmitter::~RandomEmitter()
{
}

void RandomEmitter::setNumberOfParticles(unsigned int numberOfParticles)
{
	m_numberOfParticles = numberOfParticles;
}

void RandomEmitter::setMinSize(float size)
{
	m_minSize = size;
}

void RandomEmitter::setMaxSize(float size)
{
	m_maxSize = size;
}

void RandomEmitter::setTopLeft(const glm::vec2& topLeft)
{
	m_topLeft = topLeft;
}

void RandomEmitter::setBottomRight(const glm::vec2& bottomRight)
{
	m_bottomRight = bottomRight;
}

void RandomEmitter::process()
{
	ParticleSystemPtr particleSystem = m_particleSystem.lock();
	if (!particleSystem)
		return;

	const ParticleArrayPtr& particles = particleSystem->getParticles();
	for (int i = 0; i < m_numberOfParticles; ++i) {
		float x = (float) rand() / RAND_MAX;
		float y = (float) rand() / RAND_MAX;
		float size = (float) rand() / RAND_MAX;
		size = m_minSize + size * m_maxSize;
		Particle particle;
		particle.position = glm::vec3(x - 0.5f, y - 0.5f, 1.0) * 2.0f;
		particle.size = size;
		particles->push_back(particle);
	}
}
