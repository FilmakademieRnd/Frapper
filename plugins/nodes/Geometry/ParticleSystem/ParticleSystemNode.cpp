/*
-----------------------------------------------------------------------------
This source file is part of FRAPPER
research.animationsinstitut.de
sourceforge.net/projects/frapper

Copyright (c) 2008-2016 Filmakademie Baden-Wuerttemberg, Institute of Animation 

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
//! \brief Implementation file for ParticleSystemNode class.
//!
//! \author     Nils Zweiling <n.zweiling@unexpected.de>
//! \version    1.0
//! \date       07.03.2013 (last updated)
//!

#include "ParticleSystemNode.h"

namespace ParticleSystemNode {
using namespace Frapper;

INIT_INSTANCE_COUNTER(ParticleSystemNode)
Q_DECLARE_METATYPE(QVector<float>);
///
/// Constructors and Destructors
///

//!
//! Constructor of the ParticleSystemNode class.
//!
//! \param name The name to give the new mesh node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
ParticleSystemNode::ParticleSystemNode ( const QString &name, ParameterGroup *parameterRoot ) :
	GeometryNode(name, parameterRoot)   	    
{	 
	m_vertexBuffersGroup = new ParameterGroup("VertexBuffersGroup");
	Parameter* materialParameter = new Parameter("mat", Parameter::Type::T_String, QVariant(QString("ParticleSystemMaterial")));
	Parameter* materialGroupParameter = new Parameter("matGroup", Parameter::Type::T_String, QVariant(QString("ParticleSystemNode")));
	Parameter* renderOperationParameter = new Parameter("op", Parameter::Type::T_String, QVariant(QString("triangles")));
	m_vertexBufferParameter = new NumberParameter("pos", Parameter::T_Float, 0.0f);
	m_colorBufferParameter = new NumberParameter("col", Parameter::T_Float, 0.0f);
	m_normalBufferParameter = new NumberParameter("norm", Parameter::T_Float, 0.0f);
	m_texCoordBufferParameter = new NumberParameter("uv", Parameter::T_Float, 0.0f);
	m_vertexBuffersGroup->addParameter(materialParameter);		
	m_vertexBuffersGroup->addParameter(materialGroupParameter);		
	m_vertexBuffersGroup->addParameter(renderOperationParameter);		
	m_vertexBuffersGroup->addParameter(m_vertexBufferParameter);		
	m_vertexBuffersGroup->addParameter(m_colorBufferParameter);
	m_vertexBuffersGroup->addParameter(m_normalBufferParameter);
	m_vertexBuffersGroup->addParameter(m_texCoordBufferParameter);

	createManualObject();
	setupChangeFunctions();
	setupProcessingFunctions();
	initializeAndStartSimulationTimer();
	initializeParticleSystemVariables();

	setupParticleSystem();
	INC_INSTANCE_COUNTER
}

//!
//! Destructor of the ParticleSystemNode class.
//!
ParticleSystemNode::~ParticleSystemNode ()
{
	delete m_vertexBuffersGroup;
	if (m_manualObjContainer)
		delete m_manualObjContainer;

	if (m_manualObject) {
		Ogre::SceneManager *sceneManager = OgreManager::getSceneManager();
		sceneManager->destroyManualObject(m_manualObject);
	}
	DEC_INSTANCE_COUNTER;
}

void ParticleSystemNode::createManualObject ()
{
    Ogre::SceneManager *sceneManager = OgreManager::getSceneManager();
    if (!sceneManager) {
        Log::error("Could not obtain OGRE scene manager.", "PointCloudViewerNode::loadMesh");
        return;
    }

    m_sceneNode = OgreManager::createSceneNode(m_name);
    if (!m_sceneNode) {
        Log::error(QString("Scene node for node \"%1\" could not be created.").arg(m_name), "PointCloudViewerNode::createManualObject");
        return;
    }
    setValue(m_outputGeometryName, m_sceneNode, true);

	m_manualObject = sceneManager->createManualObject((m_name + "_ParticleSystemObject").toStdString());
    m_sceneNode->attachObject(m_manualObject);

    m_manualObjContainer = new OgreContainer(m_manualObject);
    m_manualObject->setUserAny(Ogre::Any(m_manualObjContainer));
	
    updateStatistics();
}

void ParticleSystemNode::updateManualObject ()
{	
	QVector<float> vertices;
	QVector<float> colors;
	QVector<float> normals;
	QVector<float> texCoords;

	m_manualObject->clear();

	m_manualObject->begin("ParticleSystemMaterial", Ogre::RenderOperation::OT_TRIANGLE_LIST, "ParticleSystemNode");

	const ParticleArrayPtr& particles = m_particleSystem->getParticles();
	unsigned int numberOfParticles = particles->size();

	const int resSize = numberOfParticles*4*3;
	vertices.reserve(resSize);
	colors.reserve(resSize);
	normals.reserve(resSize);
	texCoords.reserve(resSize*6);

	for (int i = 0; i < numberOfParticles; ++i) {
		const Particle& particle = particles->at(i);
		QList<glm::vec3> quadVertices = createQuad(particle.position, 0.1); 
		for(int j = 0; j < quadVertices.size(); ++j) {
			glm::vec3 vertex = quadVertices[j];
			for(int k = 0; k < 3; ++k) {
				vertices.append(particle.position[k]);
				colors.append(0.0);
				normals.append(particle.orientation[k]);
			}

			texCoords.append(0.0);
			texCoords.append(0.0);
			texCoords.append(particle.size);
			texCoords.append(1.0);
			texCoords.append(0.0);
			texCoords.append(particle.size);
			texCoords.append(0.0);
			texCoords.append(1.0);
			texCoords.append(particle.size);
			texCoords.append(1.0);
			texCoords.append(0.0);
			texCoords.append(particle.size);
			texCoords.append(0.0);
			texCoords.append(1.0);
			texCoords.append(particle.size);
			texCoords.append(1.0);
			texCoords.append(1.0);
			texCoords.append(particle.size);
		}
	}
	m_manualObject->end();

	m_vertexBufferParameter->setSize(vertices.size());
	m_colorBufferParameter->setSize(colors.size());
	m_normalBufferParameter->setSize(normals.size());
	m_texCoordBufferParameter->setSize(texCoords.size());
	m_vertexBufferParameter->setValue(QVariant::fromValue(vertices));
	m_colorBufferParameter->setValue(QVariant::fromValue(colors));
	m_normalBufferParameter->setValue(QVariant::fromValue(normals));
	m_texCoordBufferParameter->setValue(QVariant::fromValue(texCoords));

	m_manualObjContainer->updateVertexBuffer(m_vertexBuffersGroup);
}

QList<glm::vec3> ParticleSystemNode::createQuad (const glm::vec3& position, float size)
{
	glm::vec3 v0 = glm::vec3(-0.5, 0.5, 0.0) * size + position;
	glm::vec3 v1 = glm::vec3(0.5, 0.5, 0.0) * size + position;
	glm::vec3 v2 = glm::vec3(-0.5, -0.5, 0.0) * size + position;
	glm::vec3 v3 = glm::vec3(0.5, -0.5, 0.0) * size + position;

	QList<glm::vec3> vertices;
	vertices.append(v0);
	vertices.append(v1);
	vertices.append(v2);
	vertices.append(v1);
	vertices.append(v2);
	vertices.append(v3);
	
	return vertices;
}

//
// ParticleSystem
//
void ParticleSystemNode::initializeParticleSystemVariables()
{
	m_particleSystemSceneNode = 0;
	m_isEnabled = getBoolValue("Enabled");
	m_numberOfParticles = getIntValue("Number Of Particles");
	m_particleMeshName = "Disc.mesh";
	m_instanceManager = 0;
	m_minParticleSize = getBoolValue("Min Particle Size");
	m_maxParticleSize = getFloatValue("Max Particle Size");
}

void ParticleSystemNode::initializeAndStartSimulationTimer()
{
	QTimer* particleSimulationTimer = new QTimer(this);
	connect(particleSimulationTimer, SIGNAL(timeout()), this, SLOT(updateParticleSystem()));
	particleSimulationTimer->start(10);
}

void ParticleSystemNode::setupChangeFunctions()
{
	// Particle System
	setChangeFunction("Enabled", SLOT(changeParticleSystemParameters()));
	setChangeFunction("Number Of Particles", SLOT(changeParticleSystemParameters()));
	setChangeFunction("Min Particle Size", SLOT(changeParticleSystemParameters()));
	setChangeFunction("Max Particle Size", SLOT(changeParticleSystemParameters()));

	// Mouth
	//setChangeFunction("Speak", SLOT(changeMouth()));

	// Mouth Emitter
	setChangeFunction("Mouth Emitter Enabled", SLOT(changeMouthEmitter()));
	setChangeFunction("Initial Particle Speed", SLOT(changeMouthEmitter()));

	// Mesh Affector
	setChangeFunction("Mesh Affector Enabled", SLOT(changeMeshAffector()));
	//setChangeFunction("Mesh Force", SLOT(changeMeshAffector()));

	// Inter Particle Affector
	setChangeFunction("Inter Particle Affector Enabled", SLOT(changeInterParticleAffector()));

	// Mouth Affector
	setChangeFunction("Mouth Affector Enabled", SLOT(changeMouthAffector()));
	//setChangeFunction("Mouth Force", SLOT(changeMouthAffector()));

	// Eye Affectors
	setChangeFunction("Left Eye Enabled", SLOT(changeEyeAffectors()));
	setChangeFunction("Left Eye Force", SLOT(changeEyeAffectors()));
	setChangeFunction("Right Eye Enabled", SLOT(changeEyeAffectors()));
	setChangeFunction("Right Eye Force", SLOT(changeEyeAffectors()));

	// Axis Affectors
	setChangeFunction("Axis Left Eye Enabled", SLOT(changeAxisAffectors()));
	setChangeFunction("Axis Left Eye Force", SLOT(changeAxisAffectors()));
	setChangeFunction("Axis Right Eye Enabled", SLOT(changeAxisAffectors()));
	setChangeFunction("Axis Right Eye Force", SLOT(changeAxisAffectors()));
	setChangeFunction("Axis Mouth Enabled", SLOT(changeAxisAffectors()));
	setChangeFunction("Axis Mouth Force", SLOT(changeAxisAffectors()));
}

void ParticleSystemNode::setupProcessingFunctions()
{
	// Mouth
	setProcessingFunction("Speak", SLOT(changeMouth()));

	// Mesh Affector
	setProcessingFunction("Mesh Force", SLOT(changeMeshAffector()));

	// Mouth Affector
	setProcessingFunction("Mouth Force", SLOT(changeMouthAffector()));

	// Inter Particle Affector
	setProcessingFunction("Inter Particle Force", SLOT(changeInterParticleAffector()));
}

void ParticleSystemNode::setupParticleSystem()
{
	m_particleSystem.reset();
	m_particleSystem = ParticleSystemPtr(new ParticleSystem());

	m_randomEmitter = RandomEmitterPtr(new RandomEmitter(m_particleSystem));
	m_randomEmitter->setNumberOfParticles(m_numberOfParticles);
	m_randomEmitter->setMinSize(m_minParticleSize);
	m_randomEmitter->setMinSize(m_maxParticleSize);
	m_particleSystem->addEmitter(m_randomEmitter);

	m_pointEmitter = PointEmitterPtr(new PointEmitter(m_particleSystem));
	m_pointEmitter->setEnabled(false);
	m_pointEmitter->setPosition(glm::vec3(0.0, -1.06, 0.39));
	m_pointEmitter->setInitialSpeed(getFloatValue("Initial Particle Speed"));
	m_particleSystem->addEmitter(m_pointEmitter);

	m_meshAffector = MeshAffectorPtr(new MeshAffector(m_particleSystem));
	Vec3ArrayPtr vertices(new Vec3Array());
	Vec3ArrayPtr normals(new Vec3Array());
	u16Vec3ArrayPtr faces(new u16Vec3Array());
	createAffectorMesh("FaceMesh2.mesh", vertices, normals, faces);	
	m_meshAffector->setVertices(vertices);
	m_meshAffector->setNormals(normals);
	m_meshAffector->setFaces(faces);
	m_meshAffector->initialize();
	m_meshAffector->setStrength(getFloatValue("Mesh Force"));
	m_particleSystem->addAffector(m_meshAffector);

	m_interParticleAffector = InterParticleAffectorPtr(new InterParticleAffector(m_particleSystem));
	m_interParticleAffector->setStrength(getFloatValue("Inter Particle Force"));
	m_particleSystem->addAffector(m_interParticleAffector);

	m_pointAffectorRightEye = PointAffectorPtr(new PointAffector(m_particleSystem));
	m_pointAffectorRightEye->setPosition(glm::vec3(-0.75, 0.38, 0.2));
	m_pointAffectorRightEye->setCutoffRadius(0.7);
	m_pointAffectorRightEye->setStrength(getFloatValue("Right Eye Force"));
	m_pointAffectorRightEye->setEnabled(getBoolValue("Right Eye Enabled"));
	m_particleSystem->addAffector(m_pointAffectorRightEye);

	m_axisAffectorRightEye = AxisAffectorPtr(new AxisAffector(m_particleSystem));
	m_axisAffectorRightEye->setPosition(glm::vec3(-0.75, 0.38, 0.2));
	m_axisAffectorRightEye->setDirection(glm::vec3(0.0, 0.0, 1.0));
	m_axisAffectorRightEye->setCutoffRadius(1.0);
	m_axisAffectorRightEye->setStrength(getFloatValue("Axis Right Eye Force"));
	m_axisAffectorRightEye->setEnabled(getBoolValue("Axis Right Eye Enabled"));
	m_particleSystem->addAffector(m_axisAffectorRightEye);

	m_pointAffectorLeftEye = PointAffectorPtr(new PointAffector(m_particleSystem));
	m_pointAffectorLeftEye->setPosition(glm::vec3(0.75, 0.38, 0.2));
	m_pointAffectorLeftEye->setCutoffRadius(0.7);
	m_pointAffectorLeftEye->setStrength(getFloatValue("Left Eye Force"));
	m_pointAffectorLeftEye->setEnabled(getBoolValue("Left Eye Enabled"));
	m_particleSystem->addAffector(m_pointAffectorLeftEye);

	m_axisAffectorLeftEye = AxisAffectorPtr(new AxisAffector(m_particleSystem));
	m_axisAffectorLeftEye->setPosition(glm::vec3(0.75, 0.38, 0.2));
	m_axisAffectorLeftEye->setDirection(glm::vec3(0.0, 0.0, 1.0));
	m_axisAffectorLeftEye->setCutoffRadius(1.0);
	m_axisAffectorLeftEye->setStrength(getFloatValue("Axis Left Eye Force"));
	m_axisAffectorLeftEye->setEnabled(getBoolValue("Axis Left Eye Enabled"));
	m_particleSystem->addAffector(m_axisAffectorLeftEye);

	m_pointAffectorMouth = PointAffectorPtr(new PointAffector(m_particleSystem));
	m_pointAffectorMouth->setPosition(glm::vec3(0.0, -1.06, 0.39));
	m_pointAffectorMouth->setCutoffRadius(0.7);
	m_pointAffectorMouth->setStrength(100.0);
	m_particleSystem->addAffector(m_pointAffectorMouth);
}

void ParticleSystemNode::createAffectorMesh(const Ogre::String& meshName, const Vec3ArrayPtr& vertexArray, const Vec3ArrayPtr& normalArray, const u16Vec3ArrayPtr& faceArray)
{
	Ogre::Entity* entity = OgreManager::getSceneManager()->createEntity(meshName);
	size_t vertexCount;
	size_t indexCount;
	Ogre::Vector3* vertices;
	unsigned long* indices;
	Ogre::Vector3* normals;
	getMeshInformation(entity->getMesh(), vertexCount, vertices, indexCount, indices, normals);

	for (int i = 0; i < vertexCount; ++i) {
		const Ogre::Vector3& vertex = vertices[i];
		vertexArray->push_back(glm::vec3(vertex.z, vertex.y, vertex.x));
	}

	for (int i = 0; i < vertexCount; ++i) {
		const Ogre::Vector3& normal = normals[i];
		normalArray->push_back(glm::vec3(normal.z, normal.y, normal.x));
	}

	unsigned int faceCount = vertexCount;
	for (int i = 0; i < faceCount; ++i) {
		unsigned int index = i*3;
		faceArray->push_back(glm::u16vec3(indices[index], indices[index+1], indices[index+2]));
	}

	delete[] vertices;
	delete[] normals;
	delete[] indices;

	OgreManager::getSceneManager()->destroyEntity(entity);
}

void ParticleSystemNode::getMeshInformation(const Ogre::MeshPtr& mesh,
	size_t &vertex_count,
	Ogre::Vector3* &vertices,
	size_t &index_count,
	unsigned long* &indices,
	Ogre::Vector3* &normals,
	const Ogre::Vector3 &position,
	const Ogre::Quaternion &orient,
	const Ogre::Vector3 &scale)
{
	bool added_shared = false;
	size_t current_offset = 0;
	size_t shared_offset = 0;
	size_t next_offset = 0;
	size_t index_offset = 0;

	vertex_count = index_count = 0;

	// Calculate how many vertices and indices we're going to need
	for ( unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i)
	{
		Ogre::SubMesh* submesh = mesh->getSubMesh(i);
		// We only need to add the shared vertices once
		if(submesh->useSharedVertices)
		{
			if( !added_shared )
			{
				vertex_count += mesh->sharedVertexData->vertexCount;
				added_shared = true;
			}
		}
		else
		{
			vertex_count += submesh->vertexData->vertexCount;
		}
		// Add the indices
		index_count += submesh->indexData->indexCount;
	}

	// Allocate space for the vertices and indices
	vertices = new Ogre::Vector3[vertex_count];
	normals = new Ogre::Vector3[vertex_count];
	indices = new unsigned long[index_count];

	added_shared = false;

	// Run through the submeshes again, adding the data into the arrays
	for (unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i)
	{
		Ogre::SubMesh* submesh = mesh->getSubMesh(i);

		Ogre::VertexData* vertex_data = submesh->useSharedVertices ? mesh->sharedVertexData : submesh->vertexData;

		if ((!submesh->useSharedVertices) || (submesh->useSharedVertices && !added_shared))
		{
			if(submesh->useSharedVertices)
			{
				added_shared = true;
				shared_offset = current_offset;
			}

			const Ogre::VertexElement* posElem =
				vertex_data->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION);

			const Ogre::VertexElement* normElem =
				vertex_data->vertexDeclaration->findElementBySemantic(Ogre::VES_NORMAL);

			Ogre::HardwareVertexBufferSharedPtr vbuf =
				vertex_data->vertexBufferBinding->getBuffer(posElem->getSource());

			unsigned char* vertex =
				static_cast<unsigned char*>(vbuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));

			// There is _no_ baseVertexPointerToElement() which takes an Ogre::Real or a double
			//  as second argument. So make it float, to avoid trouble when Ogre::Real will
			//  be comiled/typedefed as double:
			//Ogre::Real* pReal;
			float* pReal;

			for( size_t j = 0; j < vertex_data->vertexCount; ++j, vertex += vbuf->getVertexSize())
			{
				posElem->baseVertexPointerToElement(vertex, &pReal);
				Ogre::Vector3 pt(pReal[0], pReal[1], pReal[2]);
				vertices[current_offset + j] = (orient * (pt * scale)) + position;
				
				normElem->baseVertexPointerToElement(vertex, &pReal);				
				Ogre::Vector3 norm(pReal[0], pReal[1], pReal[2]);
				normals[current_offset + j] = norm; 
			}

			vbuf->unlock();
			next_offset += vertex_data->vertexCount;
		}

		Ogre::IndexData* index_data = submesh->indexData;
		size_t numTris = index_data->indexCount / 3;
		Ogre::HardwareIndexBufferSharedPtr ibuf = index_data->indexBuffer;

		bool use32bitindexes = (ibuf->getType() == Ogre::HardwareIndexBuffer::IT_32BIT);

		unsigned long* pLong = static_cast<unsigned long*>(ibuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
		unsigned short* pShort = reinterpret_cast<unsigned short*>(pLong);

		size_t offset = (submesh->useSharedVertices)? shared_offset : current_offset;

		if ( use32bitindexes )
		{
			for ( size_t k = 0; k < numTris*3; ++k)
			{
				indices[index_offset++] = pLong[k] + static_cast<unsigned long>(offset);
			}
		}
		else
		{
			for ( size_t k = 0; k < numTris*3; ++k)
			{
				indices[index_offset++] = static_cast<unsigned long>(pShort[k]) +
					static_cast<unsigned long>(offset);
			}
		}

		ibuf->unlock();
		current_offset = next_offset;
	}
}

void ParticleSystemNode::updateParticleSystem()
{
	if (!m_particleSystem)
		return;

	m_particleSystem->update();
	updateManualObject();
	triggerRedraw();
}

void ParticleSystemNode::changeParticleSystemParameters()
{
	m_numberOfParticles = (unsigned int) getIntValue("Number Of Particles");
	bool isEnabled = getBoolValue("Enabled");
	if (!m_isEnabled && isEnabled) {
		m_particleSystem->removeAllParticles();
		m_randomEmitter->setNumberOfParticles(m_numberOfParticles);
		m_randomEmitter->reset();
	}
	m_isEnabled = isEnabled;

	float minParticleSize = getFloatValue("Min Particle Size");
	m_randomEmitter->setMinSize(minParticleSize);
	float maxParticleSize = getFloatValue("Max Particle Size");
	m_randomEmitter->setMaxSize(maxParticleSize);
}

void ParticleSystemNode::changeMouth()
{
	if (!m_pointAffectorMouth || !m_pointEmitter)
		return;
	
	bool enabled = getBoolValue("Speak");
	m_pointAffectorMouth->setEnabled(enabled);
	m_pointEmitter->setEnabled(enabled);
}

void ParticleSystemNode::changeMouthEmitter()
{
	if (!m_pointEmitter)
		return;

	bool isEnabled = getBoolValue("Mouth Emitter Enabled");
	m_pointEmitter->setEnabled(isEnabled);

	float speed = getFloatValue("Initial Particle Speed");
	m_pointEmitter->setInitialSpeed(speed);
}

void ParticleSystemNode::changeMeshAffector()
{
	if (!m_meshAffector)
		return;

	bool isEnabled = getBoolValue("Mesh Affector Enabled");
	m_meshAffector->setEnabled(isEnabled);

	float meshForce = getFloatValue("Mesh Force");
	m_meshAffector->setStrength(meshForce);
}

void ParticleSystemNode::changeInterParticleAffector()
{
	if (!m_interParticleAffector)
		return;

	bool isEnabled = getBoolValue("Inter Particle Affector Enabled");
	m_interParticleAffector->setEnabled(isEnabled);

	float interParticleForce = getFloatValue("Inter Particle Force");
	m_interParticleAffector->setStrength(interParticleForce);
}

void ParticleSystemNode::changeMouthAffector()
{
	if (!m_pointAffectorMouth)
		return;
	
	bool isEnabled = getBoolValue("Mouth Affector Enabled");
	m_pointAffectorMouth->setEnabled(isEnabled);

	float mouthForce = getFloatValue("Mouth Force");
	m_pointAffectorMouth->setStrength(mouthForce);
}

void ParticleSystemNode::changeEyeAffectors()
{
	bool enabled = getBoolValue("Left Eye Enabled");
	m_pointAffectorLeftEye->setStrength(enabled);
	float leftEyeForce = getFloatValue("Left Eye Force");
	m_pointAffectorLeftEye->setStrength(leftEyeForce);
	enabled = getBoolValue("Right Eye Enabled");
	m_pointAffectorRightEye->setStrength(enabled);
	float rightEyeForce = getFloatValue("Right Eye Force");
	m_pointAffectorRightEye->setStrength(rightEyeForce);
}

void ParticleSystemNode::changeAxisAffectors()
{
	bool enabled = getBoolValue("Axis Left Eye Enabled");
	m_axisAffectorLeftEye->setEnabled(enabled);
	float force = getFloatValue("Axis Left Eye Force");
	m_axisAffectorLeftEye->setStrength(force);
	
	enabled = getBoolValue("Axis Right Eye Enabled");
	m_axisAffectorRightEye->setEnabled(enabled);
	force = getFloatValue("Axis Right Eye Force");
	m_axisAffectorRightEye->setStrength(force);
	
	force = getFloatValue("Axis Mouth Force");
	//m_axisAffectorMouth->setStrength(force);
}

} // namespace ParticleSystemNode
