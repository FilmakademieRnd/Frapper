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
//! \file "PoemAnalyserNode.h"
//! \brief Header file for PoemAnalyserNode class.
//!
//! \author     Diana Arellano <diana.arellano@filmakademie.de>
//! \version    1.0
//! \date       07.02.2013 (last updated)
//!
//! \description
//!		The purpose of this node is to automatically analyze and tag a new poem on the fly. It uses the data files obtained by the WDAL (Whissell Dictionary of Affect in Language)
//!		It receives a text file with no marks/tags (the poem), and the output is an entry in the XML file with all the poems tagged.
//!		It should be noted that this node is used only once when a new poem needs to be added.
//!

#ifndef POEMANALYSERNODE_H
#define POEMANALYSERNODE_H



#include "Node.h"
#include <QtWidgets>
#include <QtCore/QFile>
#include <QtCore/QDir>

// OGRE
#include <Ogre.h>
#if (OGRE_PLATFORM  == OGRE_PLATFORM_WIN32)
#include <windows.h>
#endif

namespace PoemAnalyserNode {
using namespace Frapper;

//!
//! Class for a node that is used for debugging purposes.
//!
class PoemAnalyserNode : public Node
{

    Q_OBJECT
    ADD_INSTANCE_COUNTER

public: // constructors and destructors

    //!
    //! Constructor of the PoemAnalyserNode class.
    //!
    //! \param name The name for the new node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.
    //!
    PoemAnalyserNode ( const QString &name, ParameterGroup *parameterRoot );

    //!
    //! Destructor of the PoemAnalyserNode class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~PoemAnalyserNode ();



private slots:

	void readDATFile();
	void readFORFile();
	void readTextFile();

	void reloadFORFile();
	void reloadDATFile();
	void reloadTextFile();

	void poemAnalysis();	

private:

	ParameterGroup *m_wordAnalysisGroup;
	ParameterGroup *m_poemAnalysisGroup;
	ParameterGroup *m_poemTextGroup;
	NumberParameter *m_buttonParameter;

	int m_regularSpeedValue;
	int m_regularPitchValue;

private:


	

	QString lineAnalysisAndTagging( QString poemLine, ParameterGroup *poemState );
	QString linePostProcessing(QString line);

	QString setLineTagging( QString lState, QString line ); 
	QString setPoemTagging( QString state);
	QString setLineProsodyTagging( QString poemLine, QString lState, float lValue, ParameterGroup* poemState);
	QString setPitchTagging( QString poemLine, QString lState, float wValue );
		
	QTableWidget*	getLineStates(QString line, QTableWidget* table);
	QTableWidget *	getLinePredominantState( QTableWidget * tStates );
	ParameterGroup* getPoemState();
	ParameterGroup* getLineTaggingState( QTableWidget * tStates, ParameterGroup * poemState );
	char			getLastCharacter( QString line );
	char			getFirstCharacter( QString lineTmp );

	void checkNegationInLines( QString line );
	void writePoemInFile( QString poemXML );
	int transformStringToNumber(QString state);
	QString transformNumberToString(int number);
	bool wordIsOk(QString word);

	QFile * fileManagement(QString filename, QString mode);

public:



};

} // namespace PoemAnalyserNode 

#endif
