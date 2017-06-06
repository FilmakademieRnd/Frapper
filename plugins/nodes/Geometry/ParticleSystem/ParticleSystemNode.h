/*
-----------------------------------------------------------------------------
This source file is part of FRAPPER
research.animationsinstitut.de
sourceforge.net/projects/frapper

Copyright (c) 2008-2012 Filmakademie Baden-Wuerttemberg, Institute of Animation 

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; version 2.1 of the License.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html
-----------------------------------------------------------------------------
*/

//!
//! \file "ParticleSystemNode.h"
//! \brief Header file for ParticleSystemNode class.
//!
//! \author     Nils Zweiling <n.zweiling@unexpected.de>
//! \version    1.0
//! \date       07.03.2013 (last updated)
//!

#ifndef PARTICLESYSTEMNODE_H
#define PARTICLESYSTEMNODE_H

#include "GeometryAnimationNode.h"
#include "OgreContainer.h"
#include "OgreInstanceManager.h"
#include "OgreManualObject.h"

#include <QtCore/QTimer>
//
// Particle System
//
#include "ParticleSystem.h"
#include "PointEmitter.h"
#include "PointAffector.h"
#include "AxisAffector.h"
#include "InterParticleAffector.h"
#include "PlaneAffector.h"
#include "MeshAffector.h"
#include "RandomEmitter.h"


namespace ParticleSystemNode {
using namespace Frapper;

//!
//! Class representing a particle system.
//!
class ParticleSystemNode : public GeometryNode
{

    Q_OBJECT
    ADD_INSTANCE_COUNTER

public: // constructors and destructors

    //!
    //! Constructor of the ParticleSystemNode class.
    //!
    //! \param name The name to give the new mesh node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.
    //!
    ParticleSystemNode ( const QString &name, ParameterGroup *parameterRoot );

    //!
    //! Destructor of the ParticleSystemNode class.
    //!
    ~ParticleSystemNode ();

private: // functions

	void createManualObject ();
	void updateManualObject ();
	QList<glm::vec3> createQuad (const glm::vec3& position, float size);

private: // data

	Ogre::ManualObject* m_manualObject;
    OgreContainer *m_manualObjContainer;

	//
	// ParticleSystem
	//
private:
	void initializeParticleSystemVariables ();
	void initializeAndStartSimulationTimer ();
	void setupChangeFunctions ();
	void setupProcessingFunctions ();
	
	void setupParticleSystem ();
	void createAffectorMesh(const Ogre::String& meshName, const Vec3ArrayPtr& vertices, const Vec3ArrayPtr& normals, const u16Vec3ArrayPtr& faces);
	void getMeshInformation(const Ogre::MeshPtr& mesh,
                        size_t &vertex_count,
                        Ogre::Vector3* &vertices,
                        size_t &index_count,
                        unsigned long* &indices,
						Ogre::Vector3* &normals,
                        const Ogre::Vector3 &position = Ogre::Vector3::ZERO,
                        const Ogre::Quaternion &orient = Ogre::Quaternion::IDENTITY,
                        const Ogre::Vector3 &scale = Ogre::Vector3::UNIT_SCALE);

private slots:
	void updateParticleSystem();
	void changeParticleSystemParameters();
	void changeMouth();
	void changeMouthEmitter();
	void changeMeshAffector();
	void changeMouthAffector();
	void changeEyeAffectors();
	void changeInterParticleAffector();
	void changeAxisAffectors();



private:
	ParameterGroup* m_vertexBuffersGroup;
	NumberParameter* m_vertexBufferParameter;
	NumberParameter* m_colorBufferParameter;
	NumberParameter* m_normalBufferParameter;
	NumberParameter* m_texCoordBufferParameter;

	Ogre::InstanceManager *m_instanceManager;
	Ogre::SceneNode *m_particleSystemSceneNode;
	std::vector<Ogre::InstancedEntity*> m_instancedEntities;
	ParticleSystemPtr m_particleSystem;
	unsigned int m_numberOfParticles;
	Ogre::String m_particleMeshName;

	bool m_isEnabled;
	float m_minParticleSize;
	float m_maxParticleSize;
	float m_interParticleForce;

	RandomEmitterPtr m_randomEmitter;
	PointEmitterPtr m_pointEmitter;
	MeshAffectorPtr m_meshAffector;
	InterParticleAffectorPtr m_interParticleAffector;
	PointAffectorPtr m_pointAffectorRightEye;
	PointAffectorPtr m_pointAffectorLeftEye;
	PointAffectorPtr m_pointAffectorMouth;
	AxisAffectorPtr m_axisAffectorRightEye;
	AxisAffectorPtr m_axisAffectorLeftEye;
	AxisAffectorPtr m_axisAffectorMouth;
};

} // namespace ParticleSystemNode
#endif
