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
#include "ParticleAffector.h"
#include "KDTree.h"


#include <glm/glm.hpp>
#include <memory>
#include <vector>

struct ClosestPointOnTriangle
{
	ClosestPointOnTriangle(const glm::vec3& position, const glm::vec3& normal, unsigned int triangleId) : m_position(position), m_normal(normal), m_triangleId(triangleId) {};
	glm::vec3 m_position;
	glm::vec3 m_normal;
	unsigned int m_triangleId;
};

class MeshAffector;
typedef std::shared_ptr<MeshAffector> MeshAffectorPtr;
typedef std::vector<MeshAffector> MeshAffectorArray;

class ParticleSystem;
typedef std::shared_ptr<ParticleSystem> ParticleSystemPtr;

class MeshAffector : public ParticleAffector
{
public:
    MeshAffector(const ParticleSystemPtr& particleSystem);
    ~MeshAffector();

public:
	void setStrength(float strength);
	float getStrength();
	void setVertices(const Vec3ArrayPtr& vertices);
	void setNormals(const Vec3ArrayPtr& normals);
	void setFaces(const u16Vec3ArrayPtr& faces);

	void initialize();
	ClosestPointOnTriangle findClosestPointOnMesh(const glm::vec3& position);
	virtual void applyForces();

private:
	void generateTriangleLookupTable();
	std::vector<unsigned int> findTrianglesWithVertex(unsigned int vertexId);
	//ClosestPointOnTriangle findClosestPointOnMesh(const glm::vec3& position);
	glm::vec3 findClosestPointOnTriangle(const glm::vec3& position, unsigned int triangleId);
	glm::vec3 calculateNormal(ClosestPointOnTriangle closestPointOnTriangle);
	void applyForces_singlethreaded();
	void applyForces_multithreaded();

protected:
	float m_strength;
	Vec3ArrayPtr m_vertices;
	Vec3ArrayPtr m_normals;
	u16Vec3ArrayPtr m_faces;
	KDTree m_kdTree;
	std::vector<std::vector<unsigned int>> m_triangleLookupTable;
};

