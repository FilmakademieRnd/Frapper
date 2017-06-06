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
//! \file "PreComputationNode.cpp"
//! \brief Header file for PreComputationNode class.
//!
//! \author     Stefan Seibert <stefan.seibert@filmakademie.de>
//! \version    1.0
//! \date       10.02.2014 (created)
//!

#ifndef PRECOMPUTATIONNODE_H
#define PRECOMPUTATIONNODE_H

#include <PluginManager.h>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QDir>
#include <QProcess>
#include <QTextStream>
#include "PreCalcProgressDialog.h"
#include "Log.h"
#include "Node.h"

namespace PreComputationNode {
using namespace Frapper;

//!
//! Base class for all render nodes.
//!
class PreComputationNode : public Node
{

private:

	class PreComputationProcess : public QProcess
	{

	public:

		PreComputationProcess();

		~PreComputationProcess();

		QString getType();

		void setType(QString type );

//signals:

		//void readyReadStandardOutput();
		//
		//void finished( int exitCode, QProcess::ExitStatus exitStatus );
		//
		//void error ( QProcess::ProcessError error );

	private:

		QString m_type;
	};

	Q_OBJECT

public: // constructors and destructors

    //!
    //! Constructor of the PreComputationNode class.
    //!
    //! \param name The name for the new node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.
    //!
	PreComputationNode ( const QString &name, Frapper::ParameterGroup *parameterRoot );

	//!
    //! Destructor of the PreComputationNode class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~PreComputationNode ();

private slots: 

	//! Initiates the PreComputation Steps
	void calculate ();

	//! Resets the PreComputation 
	void reset();

	//! Writes the Process Standard Output to the Frapper Output Log
	Q_INVOKABLE void readProcessStandardOutput();

	//! Is received after one of the calculation processes has finished
	Q_INVOKABLE void isFinished ( int exitCode, QProcess::ExitStatus exitStatus );

	//! Is received after one of the calculation processes has an error
	Q_INVOKABLE void hasError( QProcess::ProcessError errorStatus );


private: // functions

	//! Adds one calculation process to the pipeline
	void addCalculationProcess();

private: // data
	int									m_errorCounter;
	float								m_doneCounter;
	float								m_progressCounter;
	int									m_calculationCounter;
	bool								m_hasCalculated;
	bool								m_calculationStopper;
	QList<PreComputationProcess*>		m_processList;
	QStringList*						m_wrpList;
	QStringList*						m_resultList;
	NumberParameter*					m_threadNumber;
	FilenameParameter*					m_flowDirectory;
	FilenameParameter*					m_featDirectory;
	FilenameParameter*					m_wrpDirectory;
	FilenameParameter*					m_imageDirectory;
	FilenameParameter*					m_featureTemplateName;
	FilenameParameter*					m_flowTemplateName;
	PreCalcProgressDialog*				m_lpd;
	QMutex*								m_errorMutex;
	QMutex*								m_finishMutex;
	QMutex*								m_messageMutex;
};

} // end namespace

#endif