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

#include "ParticleEmitter.h"
#include <glm/glm.hpp>
#include <memory>

class PointEmitter;
typedef std::shared_ptr<PointEmitter> PointEmitterPtr;

class ParticleSystem;
typedef std::shared_ptr<ParticleSystem> ParticleSystemPtr;

class PointEmitter : public ParticleEmitter
{
public:
    PointEmitter(const ParticleSystemPtr& particleSystem);
    ~PointEmitter();

	void setNumberOfParticles(unsigned int numberOfParticles);
	void setPosition(const glm::vec3& position);
	void setInitialSpeed(float initialSpeed);
	void setLifetime(float lifetime);

public:
	void process();

private:
	unsigned int m_numberOfParticles;
	glm::vec3 m_position;
	float m_initialSpeed;
	float m_lifetime;
};

