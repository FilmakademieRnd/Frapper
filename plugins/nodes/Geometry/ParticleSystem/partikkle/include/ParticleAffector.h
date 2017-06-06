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

#include "Partikkle.h"

#include <glm/glm.hpp>
#include <memory>
#include <vector>

class ParticleAffector;
typedef std::shared_ptr<ParticleAffector> ParticleAffectorPtr;
typedef std::vector<ParticleAffectorPtr> ParticleAffectorArray;

class ParticleSystem;
typedef std::shared_ptr<ParticleSystem> ParticleSystemPtr;

class ParticleAffector
{
public:
    ParticleAffector(const ParticleSystemPtr& particleSystem);
    ~ParticleAffector();

public:
	void setEnabled(bool isEnabled);

	void apply();
	virtual void applyForces() = 0;

protected:
	std::weak_ptr<ParticleSystem> m_particleSystem;

private:
	bool m_isEnabled;
};

