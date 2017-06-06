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

#include "ParticleEmitter.h"
#include "Partikkle.h"

ParticleEmitter::ParticleEmitter(const ParticleSystemPtr& particleSystem) :
	m_particleSystem(particleSystem),
	m_isEnabled(true),
	m_isOneTimeEmitter(false),
	m_hasOneTimeEmitted(false)
{	
}

ParticleEmitter::~ParticleEmitter()
{
}

void ParticleEmitter::setEnabled(bool isEnabled)
{
	m_isEnabled = isEnabled;
}

void ParticleEmitter::setOneTimeEmitter(bool isOneTimeEmitter)
{
	m_isOneTimeEmitter = isOneTimeEmitter;
}

bool ParticleEmitter::isOneTimeEmitter()
{
	return m_isOneTimeEmitter;
}

void ParticleEmitter::reset()
{
	m_hasOneTimeEmitted = false;
}

void ParticleEmitter::emitParticles()
{
	if (!m_isEnabled)
		return;

	if (m_isOneTimeEmitter) {
		if (!m_hasOneTimeEmitted) {
			m_hasOneTimeEmitted = true;
			process();
		}
	}
	else
		process();
}
