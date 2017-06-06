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

#include <iostream>
#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/type_precision.hpp>

#define LOG(logString) std::cout << logString << std::endl

typedef std::vector<glm::u16vec3> u16Vec3Array;
typedef std::shared_ptr<u16Vec3Array> u16Vec3ArrayPtr;
typedef std::vector<glm::vec2> Vec2Array;
typedef std::shared_ptr<Vec2Array> Vec2ArrayPtr;
typedef std::vector<glm::vec3> Vec3Array;
typedef std::shared_ptr<Vec3Array> Vec3ArrayPtr;
typedef std::vector<glm::vec4> Vec4Array;
typedef std::shared_ptr<Vec4Array> Vec4ArrayPtr;

struct Particle;
typedef std::vector<Particle> ParticleArray;
typedef std::shared_ptr<ParticleArray> ParticleArrayPtr;

struct Particle {
public:
	Particle() :
		lifetime(-1.0) {}

public:
	glm::vec3 position;
	glm::vec3 orientation;
	glm::vec3 velocity;
	glm::vec3 force;
	float lifetime;
	float size;
};



