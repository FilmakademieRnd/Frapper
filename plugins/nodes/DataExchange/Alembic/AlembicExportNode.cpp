/*
-----------------------------------------------------------------------------
This source file is part of FRAPPER
research.animationsinstitut.de
sourceforge.net/projects/frapper

Copyright (c) 2008-2014 Filmakademie Baden-Wuerttemberg, Institute of Animation 

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
//! \file "AlembicExportNode.cpp"
//! \brief Implementation file for AlembicExportNode class.
//!
//! \author     Simon Spielmann <sspielma@filmakademie.de>
//! \version    1.0
//! \date       11.04.2013 (last updated)
//!

#include "AlembicExportNode.h"
#include "Log.h"
#include "SceneModel.h"
#include <QtCore/QDateTime>


Q_DECLARE_METATYPE(QVector<float>)

namespace AlembicExportNode {
		using namespace Frapper;

INIT_INSTANCE_COUNTER(AlembicExportNode)

///
/// Constructors and Destructors
///

//!
//! Constructor of the AlembicExportNode class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
AlembicExportNode::AlembicExportNode ( const QString &name, ParameterGroup *parameterRoot ) :
    Node(name, parameterRoot)
{
	Parameter* vertexBuffer = getParameter("VertexBuffer");
	vertexBuffer->setSelfEvaluating(true);

    FilenameParameter *filenameParameter = getFilenameParameter("Alembic File");
	if (filenameParameter) {
        filenameParameter->setType(FilenameParameter::FT_Save);
		//filenameParameter->setChangeFunction(SLOT(saveSolveResultFile()));
	}
    else
        Log::error("Could not obtain solve result filename parameter", "AlembicExportNode::AlembicExportNode");


    // set command function for save parameter
    Parameter *saveParameter = getParameter("Export");
    saveParameter->setCommandFunction(SLOT(saveSolveResultFile()));

    INC_INSTANCE_COUNTER
}

//!
//! Destructor of the AlembicExportNode class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
AlembicExportNode::~AlembicExportNode ()
{
    DEC_INSTANCE_COUNTER
}


///
/// Private Functions
///


//!
//! Creates the COLLADA header with asset information.
//!
//! \param element The COLLADA element to store the data in.
//!
void AlembicExportNode::buildHeader (  )
{
   
}


//!
//! Creates the COLLADA header with asset information.
//!
//! \param list The source list.
//! \param vector The destination vector.
//!
template <class T>
void AlembicExportNode::copyListToVector (const QVector<float> &list, std::vector<T> &vector)
{
	for(int i=0; i<list.size(); i+=3) {
		vector.push_back(T(
			list[i],
			list[i+1],
			list[i+2]) );
	}
}

void AlembicExportNode::createAlembicFile(const AbstractParameter::List& buffers, const QString &path, const unsigned int nbrFrames, const unsigned int fps)
{
	std::vector< C3f > colors;
	std::vector< V3f > positions, normals, texcoords;
	std::vector< Alembic::Util::uint64_t > ids;
	
	const int size = static_cast<NumberParameter*>(buffers.at(0))->getValue().value<QVector<float>>().size();
	for (int i=0; i<size/3; ++i)
		ids.push_back(i);

	if (buffers.size() > 0) {
		copyListToVector<V3f>(
			static_cast<NumberParameter*>(buffers.at(0))->getValue().value<QVector<float>>(),
			positions);
	}
	if (buffers.size() > 1) {
		copyListToVector<C3f>(
			static_cast<NumberParameter*>(buffers.at(1))->getValue().value<QVector<float>>(),
			colors);
	}
	//if (buffers.size() > 2) {
	//	copyListToVector<V3f>(
	//		static_cast<NumberParameter*>(buffers.at(2))->getValue().value<QVector<float>>(),
	//		normals);
	//}

	OArchive archive( Alembic::AbcCoreHDF5::WriteArchive(), path.toStdString() );
    OObject topObj( archive, kTop );

	TimeSampling ts(fps, 0.0);
    Alembic::Util::uint32_t tsidx = topObj.getArchive().addTimeSampling(ts);

	OPoints partsOut( topObj, "alembicFrapperParticles", tsidx );

	OPointsSchema &pSchema = partsOut.getSchema();
    MetaData mdata;
    SetGeometryScope( mdata, kVaryingScope );
    OC3fArrayProperty rgbOut( pSchema, "Cs", tsidx );
	//OV3fArrayProperty normalOut( pSchema, "normal", mdata, tsidx );	// keyword "normal" may be incorrect

	chrono_t iSpf = 1.0 / fps;

	for ( index_t sampIndex = 0; sampIndex < ( index_t )nbrFrames; ++sampIndex )
	{
		//chrono_t sampTime = (( chrono_t )sampIndex) * iSpf;

		// First, write the sample.
		OPointsSchema::Sample &testSample = OPointsSchema::Sample(OPointsSchema::Sample(P3fArraySample( positions ), UInt64ArraySample( ids )));
		pSchema.set( testSample );
		rgbOut.set( C3fArraySample( colors ) );
		//normalOut.set( V3fArraySample( normals ) );
	}

}


///
/// Private Slots
///



//!
//! Saves the result data as COLLADA-formatted file.
//!
//! \return True if the data was successfully written to file, false otherwise.
//!
void AlembicExportNode::saveSolveResultFile ()
{
	FilenameParameter *filenameParameter = getFilenameParameter("Alembic File");
	ParameterGroup *vertexBufferGroup = getParameter("VertexBuffer")->getValue().value<ParameterGroup *>();

	if (!vertexBufferGroup)
		return;
		
	const AbstractParameter::List& buffers = vertexBufferGroup->getParameterList();

	if (filenameParameter ) {
		if (!buffers.empty()) {
			const unsigned int nbrFrames = 1;
			const unsigned int framerate = getUnsignedIntValue("Framerate");

			QString path;
			if (Frapper::SceneModel::getWorkingDirectory().size()>0)
				path = QString(Frapper::SceneModel::getWorkingDirectory()+"/"+filenameParameter->getValueString());		
			else 
				path = filenameParameter->getValueString();

			createAlembicFile(buffers, path, nbrFrames, framerate);
		}
		else
			Log::warning("Input vertex buffer empty, no file created.", "AlembicExportNode::saveSolveResultFile");
	}
}

} // end namespace
