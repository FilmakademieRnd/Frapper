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

#include "ParticleAffector.h"

#include <glm/glm.hpp>
#include <memory>

class PointAffector;
typedef std::shared_ptr<PointAffector> PointAffectorPtr;

class ParticleSystem;
typedef std::shared_ptr<ParticleSystem> ParticleSystemPtr;

class PointAffector : public ParticleAffector
{
public:
    PointAffector(const ParticleSystemPtr& particleSystem);
    ~PointAffector();

public:
	void setPosition(const glm::vec3& position);
	const glm::vec3& getPosition();
	void setStrength(float strength);
	float getStrength();
	void setCutoffRadius(float cutoffRadius);
	float getCutoffRadius();
	
	void applyForces();

private:
	glm::vec3 m_position;
	float m_strength;
	float m_cutoffRadius;
};

