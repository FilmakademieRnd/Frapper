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
//! \file "ParticleSystemLogicNode.h"
//! \brief Header file for ParticleSystemLogicNode class.
//!
//! \author     Diana Arellano
//! \version    1.0
//! \date       04.03.2013 (last updated)
//!

#ifndef PARTICLESYSTEMLOGICNODE_H
#define PARTICLESYSTEMLOGICNODE_H

#include "MotionDataNode.h"
#include "InstanceCounterMacros.h"
#include "helper.h"

#include "Node.h"
#include "FrapperPrerequisites.h"

#include <QPointF>
#include <QtCore/QTime>
#include <QtCore/QTimer>


namespace ParticleSystemLogicNode {
using namespace Frapper;

//!
//! Class representing nodes for head animation logic in a 3D scene.
//!
class ParticleSystemLogicNode : public Node
{

    Q_OBJECT

public: // constructors and destructors

    //!
    //! Constructor of the ParticleSystemLogicNode class.
    //!
    //! \param name The name for the new node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.
    //!
    ParticleSystemLogicNode ( const QString &name, ParameterGroup *parameterRoot );

    //!
    //! Destructor of the BorAnimationNode class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~ParticleSystemLogicNode ();


private: //methods
   
	void	createVisemeExpressionParameter(QString item, int numberKeys);
	void	writetoText();
	QString	vowelMapping(float visemeIndex);

	//!
	//! For the particle simulation
	//!
	void startParticleSimulation();
	void endParticleSimulation();
	void speakParticleSimulation(bool visemeIsOpen, qreal mouthForce);

private slots: //
	
	//void createText();
	void extractAnimationInfo();
	void getVisemeTimeandTag();
	void getExpressionTimeandTag();
	void getExpressionsFromText();
	void updateFadingOutTimer();
	void updateFadingInTimer();


signals:

	void emitEndReading();
	

private: // data

	ParameterGroup	*parameterRoot; // pointer to this Parameter Root
	ParameterGroup	*m_expressions_visemes;

	QTimer			*m_timer;
	QTime			m_currentTimestamp;
	QTime			m_startTimestamp;
	QStringList		listvisemes;
	QStringList		listexpressions;
	QStringList		listvisemes_expressions;
	QStringList		framevisemes;
	QStringList		frameexpressions;
	QStringList		timeline;

	QTimer *m_foutTimer;
	QTimer *m_finTimer;
	

protected: // data

	Parameter *m_visemeTagParameter;
	Parameter *m_visemeIndexParameter;
	Parameter *m_emotionTagParameter;
	Parameter *m_readyState;
	Parameter *m_activeState;
	Parameter *m_outputText;	
	

};

}


#endif
