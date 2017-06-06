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
//! \brief Implementation file for PreComputationNode class.
//!
//! \author     Stefan Seibert <stefan.seibert@filmakademie.de>
//! \version    1.0
//! \date       10.02.2014 (created)
//!

#include "PreComputationNode.h"
#include <qeventloop.h>

Q_DECLARE_METATYPE(Slot *);

namespace PreComputationNode {
using namespace Frapper;



PreComputationNode::PreComputationProcess::PreComputationProcess() : 
QProcess(),
m_type("")
{
}

PreComputationNode::PreComputationProcess::~PreComputationProcess()
{
}

QString PreComputationNode::PreComputationProcess::getType()
{
	return m_type;
}

void PreComputationNode::PreComputationProcess::setType(QString type )
{
	m_type = type;
}



///
/// Constructors and Destructors
///
 
//!
//! Constructor of the PreComputationNode class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
PreComputationNode::PreComputationNode ( const QString &name, Frapper::ParameterGroup *parameterRoot ) :
	Frapper::Node(name, parameterRoot),
	m_threadNumber(0),
	m_flowDirectory(0),
	m_featDirectory(0),
	m_wrpDirectory(0),
	m_imageDirectory(0),
	m_featureTemplateName(0),
	m_flowTemplateName(0),
	m_calculationCounter(),
	m_hasCalculated(false),
	m_errorCounter(0),
	m_doneCounter(0.f),
	m_progressCounter(0.f),
	m_calculationStopper(false)
{
	setCommandFunction("Start Calculation",	 SLOT(calculate()));

	qRegisterMetaType<PreComputationProcess::ExitStatus>("PreComputationProcess::ExitStatus");
	qRegisterMetaType<PreComputationProcess::ProcessError>("PreComputationProcess::ProcessError");

	m_flowDirectory = getFilenameParameter("Setup > Flow Output Directory");
	m_flowDirectory->setValue(QVariant(""));
	m_featDirectory = getFilenameParameter("Setup > Features Output Directory");
	m_featDirectory->setValue(QVariant(""));
	m_wrpDirectory = getFilenameParameter("Setup > Wrp Files Working Directory");
	m_wrpDirectory->setValue(QVariant(""));
	m_imageDirectory = getFilenameParameter("Setup > Images Input Directory");
	m_imageDirectory->setValue(QVariant(""));
	m_featureTemplateName = getFilenameParameter("Setup > Feature Template File");
	m_flowTemplateName = getFilenameParameter("Setup > Flow Template File");
	m_threadNumber = getNumberParameter("Setup > Thread Number");

	m_processList = QList<PreComputationProcess*>();
	m_wrpList = new QStringList();
	m_resultList = new QStringList();
	m_lpd = new PreCalcProgressDialog();
	QPushButton* cancelButton = m_lpd->getCancelButton();
	m_lpd->connect(cancelButton, SIGNAL(clicked()), this, SLOT(reset()));
	m_lpd->setWindowTitle(("Processing PreComputations... (0% are done)"));
	m_lpd->setWindowModality(Qt::ApplicationModal);
	m_lpd->getProgressBar()->setRange( 0, 100 );
	m_lpd->getProgressBar()->setValue(0);

	m_errorMutex = new QMutex();
	m_finishMutex = new QMutex();
	m_messageMutex = new QMutex();
}


//!
//! Destructor of the PreComputationNode class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
PreComputationNode::~PreComputationNode ()
{
	foreach(PreComputationProcess* proc, m_processList) {
		proc->kill();
		delete proc;
	}
	m_wrpList->clear();
	m_resultList->clear();
	delete m_wrpList;
	delete m_resultList;
	delete m_errorMutex;
	delete m_finishMutex;
	delete m_messageMutex;
	delete m_lpd;
}

void PreComputationNode::calculate ()
{
	if(m_hasCalculated) {
		this->reset();
	}
	else {
		m_hasCalculated = true;
	}
	// Get all informations that are needed
	QString imageString = m_imageDirectory->getValue().toString();
	QDir imageInFolder = QDir(imageString);

	QString featString = m_featDirectory->getValue().toString();
	QDir featFolder = QDir(featString);

	QString flowString = m_flowDirectory->getValue().toString();
	QDir flowFolder = QDir(flowString);

	QString wrpString = m_wrpDirectory->getValue().toString();
	QDir wrpFolder = QDir(wrpString);

	QString featTemplateString = m_featureTemplateName->getValue().toString();
	QString flowTemplateString = m_flowTemplateName->getValue().toString();

	QFile featTemplate = featTemplateString;
	QFile flowTemplate = flowTemplateString;

	const int numberOfLeadingZeros = 6;
	const int numberOfUsedThreads = m_threadNumber->getValue().toInt();

	if(imageInFolder.exists() && featFolder.exists() && flowFolder.exists() && wrpFolder.exists() && featTemplate.exists() && flowTemplate.exists()) {

		Log::info("====== PreComputation started! ======", "PreComputationNode");

		QStringList images = imageInFolder.entryList();
		images.removeOne(".");
		images.removeOne("..");

		// creating optical flow calculation files
		if(images.length() > 1 && getValue("Setup > Calculate Optical Flow").toBool()) {
			for(int i = 0; i < images.length()-1; i++) {

				QString inputImage_BaseName = QFileInfo(images.at(i)).baseName();
				QString flowWorkflow;

				if (flowTemplate.open(QFile::ReadOnly | QFile::Text)) {
					QTextStream in(&flowTemplate);
					while (!in.atEnd())
						flowWorkflow = in.readAll();
					flowTemplate.close();
				}

				flowWorkflow = flowWorkflow.replace("${inFile1}", imageInFolder.filePath(images.at(i)));
				flowWorkflow = flowWorkflow.replace("${inFile2}", imageInFolder.filePath(images.at(i+1)));
				flowWorkflow = flowWorkflow.replace("${outFlow}", flowFolder.filePath(inputImage_BaseName + ".flo"));
				flowWorkflow = flowWorkflow.replace("${outFlowImage}", flowFolder.filePath(inputImage_BaseName + ".png"));

				QString wrp_flowFileName = wrpFolder.filePath("of_" + QVariant(i).toString().rightJustified(numberOfLeadingZeros, '0') + ".wrp");
				Log::info("Writing File: " + wrp_flowFileName, "PreComputationNode");
				QFile wrpFile = wrp_flowFileName;

				if (!wrpFile.open(QFile::WriteOnly)) {
					m_errorCounter++;
					Log::error(("PreComputation Error! - Cannot write to wrp file: " + wrpFile.fileName()), "PreComputationNode");
				}
				else {
					QTextStream out(&wrpFile);
					out << flowWorkflow;
					wrpFile.close();
				}

				m_wrpList->append( wrp_flowFileName );
				m_resultList->append( flowFolder.filePath(inputImage_BaseName + ".flo") );
				m_doneCounter = m_doneCounter + 25.f;
			}
		}

		// creating feature calculation files
		if(getValue("Setup > Calculate Features").toBool()) {
			for(int i = 0; i < images.length(); i++) {

				QString inputImage_BaseName = QFileInfo(images.at(i)).baseName();
				QString featWorkflow;

				if (featTemplate.open(QFile::ReadOnly | QFile::Text)) {
					QTextStream in(&featTemplate);
					while (!in.atEnd())
						featWorkflow = in.readAll();
					featTemplate.close();
				}

				featWorkflow = featWorkflow.replace("${inFile}", imageInFolder.filePath(images.at(i)));
				featWorkflow = featWorkflow.replace("${outFile}", featFolder.filePath( inputImage_BaseName + ".cimg" ));

				QString wrp_featFileName = wrpFolder.filePath("feat_" + QVariant(i).toString().rightJustified(numberOfLeadingZeros, '0') + ".wrp");
				Log::info("Writing File: " + wrp_featFileName, "PreComputationNode");
				QFile wrpFile = wrp_featFileName;

				if (!wrpFile.open(QFile::WriteOnly)) {
					m_errorCounter++;
					Log::error(("PreComputation Error! - Cannot write to wrp file: " + wrpFile.fileName()), "PreComputationNode");
				}
				else {
					QTextStream out(&wrpFile);
					out << featWorkflow;
					wrpFile.close();
				}

				m_wrpList->append( wrp_featFileName );
				m_resultList->append( featFolder.filePath(inputImage_BaseName + ".cimg") );
				m_doneCounter = m_doneCounter + 1.f;
			}
		}

		//start calculating that much files like threads are allowed
		m_lpd->show();
		for(int i = 0; i < numberOfUsedThreads; i++) {
			this->addCalculationProcess();
		}

	}
	else {
		m_errorCounter++;
		Log::error("PreComputation Error! - One or more of the given Folders or Files are missing!", "PreComputationNode");
	}
}

void PreComputationNode::addCalculationProcess() {
	if(m_calculationStopper == false) {
		const int numberOfUsedThreads = m_threadNumber->getValue().toInt();
		if(m_calculationCounter < m_resultList->length() && m_processList.length() < numberOfUsedThreads) {
			QString wrpPath = m_wrpList->at(m_calculationCounter);
			QDir resultPath = m_resultList->at(m_calculationCounter);
			if( (resultPath.exists() && getValue("Setup > Overwrite existing Files").toBool()) || !resultPath.exists() ) {

				PreComputationProcess* newProcess = new PreComputationProcess();
				QString typeInfo = QFileInfo(m_resultList->at(m_calculationCounter)).suffix();
				newProcess->setType(typeInfo);
				m_processList.append(newProcess);
				connect(newProcess, SIGNAL(readyReadStandardOutput()),				this, SLOT(readProcessStandardOutput()),				Qt::DirectConnection);
				connect(newProcess, SIGNAL(finished(int, QProcess::ExitStatus)),	this, SLOT(isFinished( int, QProcess::ExitStatus )),	Qt::DirectConnection);
				connect(newProcess, SIGNAL(error(QProcess::ProcessError)),			this, SLOT(hasError( QProcess::ProcessError )),			Qt::DirectConnection);
				QString program = "tuchulcha-run.exe";
				QStringList arguments = QStringList();
				arguments.append("-nf");
				arguments.append(wrpPath);
				newProcess->setProcessChannelMode(PreComputationProcess::MergedChannels);
				Log::info("process created on " + wrpPath, "PreComputationNode");
				newProcess->start(program, arguments);
			}
		}
		m_calculationCounter++;
		if(m_calculationCounter >= m_resultList->length() && m_processList.isEmpty()) {
			m_lpd->hide();
			if(m_errorCounter == 0)
				Log::info("====== PreComputation finished without errors! ======", "PreComputationNode");
			else
				Log::error("====== PreComputation had some errors, check log! ======", "PreComputationNode");
		}
	}
}

void PreComputationNode::isFinished ( int exitCode, QProcess::ExitStatus exitStatus ) {
	if(m_calculationStopper == false) {
		m_finishMutex->lock();
		PreComputationProcess* myProcess = dynamic_cast<PreComputationProcess*>(sender());
		m_processList.removeOne(myProcess);
		QString exitString;
		QString type = myProcess->getType();
		switch(exitStatus) {
		case QProcess::ExitStatus::CrashExit:
			exitString = "crash exit!";
			m_errorCounter++;
			break;
		case QProcess::ExitStatus::NormalExit:
			exitString = "normal exit!";
			break;
		default:
			exitString = "unknown exit!";
			break;
		}
		float progress = 100.f / m_doneCounter;
		if(type == "flo") {
			m_progressCounter = m_progressCounter + (progress*25.f);
		}
		else {
			m_progressCounter = m_progressCounter + progress;
		}
		Log::info("process finished with exit code: " + exitString, "PreComputationNode");
		QVariant varProg = QVariant(m_progressCounter);
		QVariant varProg2 = QVariant(varProg.toInt());
		m_lpd->getProgressBar()->setValue(varProg.toInt());
		QString title = "Processing PreComputations... " + varProg2.toString() + "% are done)";
		m_lpd->setWindowTitle(title);
		this->addCalculationProcess();
		m_finishMutex->unlock();
	}
}

void PreComputationNode::hasError( QProcess::ProcessError errorStatus ) {
	if(m_calculationStopper == false) {
		m_errorMutex->lock();
		PreComputationProcess* myProcess = dynamic_cast<PreComputationProcess*>(sender());
		m_processList.removeOne(myProcess);
		QString errorString;
		QString type = myProcess->getType();
		m_errorCounter++;
		switch(errorStatus) {
		case QProcess::ProcessError::Crashed:
			errorString = "crashed!";
			break;
		case QProcess::ProcessError::FailedToStart:
			errorString = "failed to start!";
			break;
		case QProcess::ProcessError::ReadError:
			errorString = "read error!";
			break;
		case QProcess::ProcessError::Timedout:
			errorString = "timed out!";
			break;
		case QProcess::ProcessError::WriteError:
			errorString = "write error!";
			break;
		default:
			errorString = "unknown error!";
			break;
		}
		float progress = 100.f / m_doneCounter;
		if(type == "flo") {
			m_progressCounter = m_progressCounter + (progress*25.f);
		}
		else {
			m_progressCounter = m_progressCounter + progress;
		}
		Log::error("process has error: " + errorString, "PreComputationNode");
		QVariant varProg = QVariant(m_progressCounter);
		QVariant varProg2 = QVariant(varProg.toInt());
		m_lpd->getProgressBar()->setValue( varProg.toInt() );
		QString title = "Processing PreComputations... " + varProg2.toString() + "% are done)";
		m_lpd->setWindowTitle(title);
		this->addCalculationProcess();
		m_errorMutex->unlock();
	}
}

void PreComputationNode::readProcessStandardOutput() {
	if(m_calculationStopper == false) {
		m_messageMutex->lock();
		PreComputationProcess* myProcess = dynamic_cast<PreComputationProcess*>(sender());
		Log::info(myProcess->readAllStandardOutput(), "PreComputationNode");
		m_messageMutex->unlock();
	}
}

void PreComputationNode::reset ()
{
	m_calculationStopper = true;
	foreach(PreComputationProcess* proc, m_processList) {
		proc->kill();
		delete proc;
	}
	m_errorCounter = 0;
	m_doneCounter = 0.f;
	m_hasCalculated = false;
	m_calculationCounter = 0;
	m_progressCounter = 0.f;
	m_processList.clear();
	m_wrpList->clear();
	m_resultList->clear();
	m_lpd->hide();
	m_lpd->setWindowTitle(("Processing PreComputations... (0% are done)"));
	m_lpd->setWindowModality(Qt::ApplicationModal);
	m_lpd->getProgressBar()->setRange( 0, 100 );
	m_lpd->getProgressBar()->setValue(0);
	m_calculationStopper = false;
}

} // end namespace

