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
//! \file "Node.h"
//! \brief Header file for Node class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \author		Simon Spielmann <sspielma@filmakademie.de>
//! \version    1.0
//! \date       24.03.2010 (last updated)
//!

#ifndef NODE_H
#define NODE_H

#include "FrapperPrerequisites.h"
#include "ParameterGroup.h"
#include "Parameter.h"
#include "GenericParameter.h"
#include "NumberParameter.h"
#include "FilenameParameter.h"
#include "EnumerationParameter.h"
#include "SceneNodeParameter.h"
#include "Connection.h"
#include "InstanceCounterMacros.h"
#include <QtCore/QObject>
#include <QtCore/QThread>
#include <QtCore/QStringList>
#include "OgreVector3.h"

namespace Frapper {

//!
//! Abstract base class for all nodes.
//!
//! <div align="center"><b>Inheritance Diagram</b></div>
//!
//! \dot
//!   digraph Node_ {
//!     node [fontname="FreeSans",fontsize="10",shape="box",height=0.2,width=0.4,fontname="FreeSans",color="black",style="filled",fillcolor="white"];
//!     edge [fontname="FreeSans",fontsize="10",labelfontname="FreeSans",labelfontsize="10",dir=back,style="solid",color="midnightblue"];
//!
//!     Node_ [label="Node",fillcolor="grey75"];
//!     QObject -> Node_;
//!     QObject [label="QObject",fontcolor="white",fillcolor="#66b036",URL="http://doc.qtsoftware.com/4.5/qobject.html"];
//!   }
//! \enddot
//! <div><center>[<a href="graph_legend.html">legend</a>]</center></div>
//!
class FRAPPER_CORE_EXPORT Node : public QThread
{

    Q_OBJECT

public:
#pragma data_seg(".shared")
    static int s_numberOfInstances;
    static int s_totalNumberOfInstances;
#pragma data_seg()

public: // constructors and destructors

    //!
    //! Constructor of the Node class.
    //!
    //! \param name The name for the new node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.
    //!
    Node ( const QString &name, ParameterGroup *parameterRoot );

    //!
    //! Destructor of the Node class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~Node ();


public: // functions

    //!
    //! Execute function for threading.
    //!
    virtual void run ( const QString &parameterName );

    //########### obsolete - just for performance testing
    //!
    //! Processes the node's input data to generate the data for the parameter
    //! with the given name.
    //!
    //! \todo Stefan: <span style="color: red;">Remove</span> function and use processing functions instead (Parameter::setProcessingFunction).
    //!
    //! \param parameterName The name of the parameter whose data should be generated.
    //! \return True if generating the data succeeded, otherwise False.
    //!
    virtual bool process ( const QString &parameterName );

    bool getTypeUnknown () const;

    //!
    //! Returns the actual timeline intex.
    //!
    //! \return The actual timeline intex.
    //!
	NumberParameter *getTimeParameter () const;

    //!
    //! Returns the name of the node.
    //! The name must be unique in the node network so that each node object can be uniquely identified by its name.
    //!
    //! \return The name of the node.
    //!
    const QString &getName () const;

    //!
    //! Sets the name of the node.
    //! The name must be unique in the node network so that each node object can be uniquely identified by its name.
    //!
    //! \param name The new name for the node.
    //!
    void setName ( const QString &name );

    //!
    //! Returns the name of the node's type.
    //!
    //! \return The name of the node's type.
    //!
    const QString &getTypeName () const;

    //!
    //! Set the name of the node's type.
    //!
    //! This function should normally only be called once directly after the
    //! node has been created by NodeFactory::createNode.
    //!
    //! \param typeName The name of the node's type.
    //! \see NodeFactory::createNode
    //!
    void setTypeName ( const QString &typeName );

    //!
    //! Returns whether a plugin for the node's type is available.
    //!
    //! \return True if a plugin for the node's type is available, otherwise False.
    //!
    bool isTypeUnknown () const;

    //!
    //! Sets the flag that indicates whether a plugin for the node's type is
    //! available.
    //!
    //! \param typeUnknown Flag to indicate whether a plugin for the node's type is available.
    //!
    void setTypeUnknown ( bool typeUnknown = true );

    //!
    //! Returns whether this node should be evaluated in the network.
    //!
    //! \return True if this node should be evaluated in the network, otherwise False.
    //!
    bool isEvaluated () const;

    //!
    //! Returns whether this node is currently selected.
    //!
    //! \return True if this node is currently selected, otherwise False.
    //!
    bool isSelected () const;

    //!
    //! Sets whether this node is currently selected.
    //!
    //! \param selected The new selected value for the node.
    //!
    virtual void setSelected ( bool selected );

    //!
    //! Returns whether the node is self-evaluating.
    //!
    //! \return True if the node is self-evaluating, otherwise False.
    //!
    bool isSelfEvaluating () const;

    //!
    //! Returns whether the node is saveable.
    //!
    //! \return True if the node is saveable, otherwise False.
    //!
    bool isSaveable () const;

    //!
    //! Sets whether the node is self-evaluating.
    //!
    //! \param selfEvaluating The new value for the self-evaluating flag.
    //!
    void setSelfEvaluating ( bool selfEvaluating );

    //!
    //! Sets whether the node is saveable.
    //!
    //! \param saveable The new value for the saveable flag.
    //!
    void setSaveable ( bool saveable );

    //!
    //! Returns the search text currently set for the node.
    //!
    //! \return The search text currently set for the node.
    //!
    const QString &getSearchText () const;

    //!
    //! Sets the text to use when filtering parameters of the node before
    //! creating editing widgets in a parameter editor panel.
    //!
    //! \param searchText The text to use for filtering parameters.
    //!
    void setSearchText ( const QString &searchText );

public: // parameter group functions

    //!
    //! Returns the root of the tree of parameters and parameter groups for the
    //! node.
    //!
    //! \return The root of the tree of parameters for the node.
    //!
    ParameterGroup * getParameterRoot () const;

    //!
    //! Returns the parameter group with the given name.
    //!
    //! \param name The name of the parameter group to return.
    //! \return The parameter group with the given name.
    //!
    ParameterGroup * getParameterGroup ( const QString &name ) const;

    //!
    //! Iterates over the children of the parameter root and fills the given
    //! lists with input and output parameters and parameter groups containing
    //! input and output parameters.
    //!
    //! \param inputParameters The list to fill with input parameters and parameter groups containing input parameters.
    //! \param outputParameters The list to fill with output parameters and parameter groups containing output parameters.
    //!
    void fillParameterLists ( AbstractParameter::List *inputParameters, AbstractParameter::List *outputParameters );

    //!
    //! Iterates over the children of the parameter root and fills the given
    //! lists with connected input and output parameters and parameter groups
    //! containing connected input and output parameters.
    //!
    //! \param connectedInputParameters The list to fill with connected input parameters and parameter groups containing connected input parameters.
    //! \param connectedOutputParameters The list to fill with connected output parameters and parameter groups containing connected output parameters.
    //! \param unconnectedInputParametersAvailable Flag that returns whether there are unconnected input parameters contained in the node.
    //! \param unconnectedOutputParametersAvailable Flag that returns whether there are unconnected output parameters contained in the node.
    //!
    void fillParameterLists ( AbstractParameter::List *connectedInputParameters, AbstractParameter::List *connectedOutputParameters, bool *unconnectedInputParametersAvailable, bool *unconnectedOutputParametersAvailable );

    //!
    //! Returns a list of parameters contained in the given parameter group
    //! that are of the given pin type and that are connected or not.
    //!
    //! \param pinType Only parameters of this pin type will be returned.
    //! \param connected Flag to control whether to return connected or unconnected parameters.
    //! \param parameterGroup The root of the parameter tree to get parameters from.
    //! \return A list of (connected) parameters of the given pin type contained in the given parameter group.
    //!
    AbstractParameter::List getParameters ( Parameter::PinType pinType, bool connected, ParameterGroup *parameterGroup = 0 );

public: // parameter functions

    //!
    //! Returns whether the node contains a parameter with the given name.
    //!
    //! \param name The name of the parameter to check for existence.
    //! \return True if a parameter with the given name exists in the node, otherwise False.
    //!
    bool hasParameter ( const QString &name ) const;

    //!
    //! Returns the parameter with the given name.
    //!
    //! \param name The name of the parameter to return.
    //! \return The parameter with the given name.
    //!
    Parameter * getParameter ( const QString &name ) const;

	//!
	//! Returns the enumeration parameter with the given name.
	//!
	//! Convenience function that uses getParameter and performs type casting.
	//!
	//! \param name The name of the enumeration parameter to return.
	//! \return The enumeration parameter with the given name.
	//!
	GenericParameter * getGenericParameter ( const QString &name ) const;

    //!
    //! Returns the number parameter with the given name.
    //!
    //! Convenience function that uses getParameter and performs type casting.
    //!
    //! \param name The name of the number parameter to return.
    //! \return The number parameter with the given name.
    //!
    NumberParameter * getNumberParameter ( const QString &name ) const;

    //!
    //! Returns the filename parameter with the given name.
    //!
    //! Convenience function that uses getParameter and performs type casting.
    //!
    //! \param name The name of the filename parameter to return.
    //! \return The filename parameter with the given name.
    //!
    FilenameParameter * getFilenameParameter ( const QString &name ) const;

    //!
    //! Returns the enumeration parameter with the given name.
    //!
    //! Convenience function that uses getParameter and performs type casting.
    //!
    //! \param name The name of the enumeration parameter to return.
    //! \return The enumeration parameter with the given name.
    //!
    EnumerationParameter * getEnumerationParameter ( const QString &name ) const;

    //!
    //! Returns the geometry parameter with the given name.
    //!
    //! Convenience function that uses getParameter and performs type casting.
    //!
    //! \param name The name of the geometry parameter to return.
    //! \return The geometry parameter with the given name.
    //!
    GeometryParameter * getGeometryParameter ( const QString &name ) const;

    //!
    //! Returns the light parameter with the given name.
    //!
    //! Convenience function that uses getParameter and performs type casting.
    //!
    //! \param name The name of the light parameter to return.
    //! \return The light parameter with the given name.
    //!
    LightParameter * getLightParameter ( const QString &name ) const;

    //!
    //! Returns the camera parameter with the given name.
    //!
    //! Convenience function that uses getParameter and performs type casting.
    //!
    //! \param name The name of the camera parameter to return.
    //! \return The camera parameter with the given name.
    //!
    CameraParameter * getCameraParameter ( const QString &name ) const;

    //!
    //! Sets the dirty state of the parameter with the given name to the given
    //! state.
    //!
    //! \param name The name of the parameter to set the dirty state for.
    //! \param dirty The new value for the parameter's dirty state.
    //!
    void setDirty ( const QString &name, bool dirty );

    //!
    //! Sets the dirty state of the node and all child parameters.
    //!
    //! \param dirty The new value for the node's dirty state.
    //!
    void setDirty ( bool dirty );

    //!
    //! Sets the dirty state of the node.
    //!
    //! \param dirty The new value for the node's dirty state.
    //!
    void setNodeDirty ( bool dirty );

    //!
    //! Returns the dirty state of the node.
    //!
    //! \return The value for the node's dirty state.
    //!
    bool isNodeDirty ();

    //!
    //! Removes and deletes the parameter with the given name.
    //!
    //! \param name The name of the parameter to remove and delete.
    //!
    void removeParameter ( const QString &name, bool diveInGroups = true );

    //!
    //! Removes and deletes the given parameter.
    //!
    //! \param parameter The parameter to remove and delete.
    //!
    void removeParameter ( Parameter *parameter, bool diveInGroups = true );

    //!
    //! Adds a affection between two parameters.
    //!
    void addAffection ( const QString &parameterName, const QString &affectedParameterName );

public: // value getter functions

    //!
    //! Returns the value of the parameter with the given name while optionally
    //! triggering the evaluation chain.
    //!
    //! \param name The name of the parameter whose value to return.
    //! \param triggerEvaluation Flag to control whether to trigger the evaluation chain.
    //! \return The value of the parameter with the given name.
    //!
    QVariant getValue ( const QString &name, bool triggerEvaluation = false ) const;

    //!
    //! Convenience function for getting the value of a boolean parameter while
    //! optionally triggering the evaluation chain.
    //!
    //! \param name The name of the parameter whose value to return.
    //! \param triggerEvaluation Flag to control whether to trigger the evaluation chain.
    //! \return The value of the parameter with the given name.
    //!
    bool getBoolValue ( const QString &name, bool triggerEvaluation = false ) const;

    //!
    //! Convenience function for getting the value of an integer parameter
    //! while optionally triggering the evaluation chain.
    //!
    //! \param name The name of the parameter whose value to return.
    //! \param triggerEvaluation Flag to control whether to trigger the evaluation chain.
    //! \return The value of the parameter with the given name.
    //!
    int getIntValue ( const QString &name, bool triggerEvaluation = false ) const;

    //!
    //! Convenience function for getting the value of an unsigned integer
    //! parameter while optionally triggering the evaluation chain.
    //!
    //! \param name The name of the parameter whose value to return.
    //! \param triggerEvaluation Flag to control whether to trigger the evaluation chain.
    //! \return The value of the parameter with the given name.
    //!
    unsigned int getUnsignedIntValue ( const QString &name, bool triggerEvaluation = false );

	//!
	//! Convenience function for getting the value of a 32bit-precision
	//! floating point parameter while optionally triggering the evaluation
	//! chain.
	//!
	//! \param name The name of the parameter whose value to return.
	//! \param triggerEvaluation Flag to control whether to trigger the evaluation chain.
	//! \return The value of the parameter with the given name.
	//!
	float getFloatValue ( const QString &name, bool triggerEvaluation = false ) const;

    //!
    //! Convenience function for getting the value of a double-precision
    //! floating point parameter while optionally triggering the evaluation
    //! chain.
    //!
    //! \param name The name of the parameter whose value to return.
    //! \param triggerEvaluation Flag to control whether to trigger the evaluation chain.
    //! \return The value of the parameter with the given name.
    //!
    double getDoubleValue ( const QString &name, bool triggerEvaluation = false ) const;

    //!
    //! Convenience function for getting the value of a string parameter while
    //! optionally triggering the evaluation chain.
    //!
    //! \param name The name of the parameter whose value to return.
    //! \param triggerEvaluation Flag to control whether to trigger the evaluation chain.
    //! \return The value of the parameter with the given name.
    //!
    QString getStringValue ( const QString &name, bool triggerEvaluation = false ) const;

    //!
    //! Convenience function for getting the value of a color parameter while
    //! optionally triggering the evaluation chain.
    //!
    //! \param name The name of the parameter whose value to return.
    //! \param triggerEvaluation Flag to control whether to trigger the evaluation chain.
    //! \return The value of the parameter with the given name.
    //!
    QColor getColorValue ( const QString &name, bool triggerEvaluation = false ) const;

    //!
    //! Convenience function for getting the value of an OGRE vector parameter
    //! while optionally triggering the evaluation chain.
    //!
    //! \param name The name of the parameter whose value to return.
    //! \param triggerEvaluation Flag to control whether to trigger the evaluation chain.
    //! \return The value of the parameter with the given name.
    //!
    Ogre::Vector3 getVectorValue ( const QString &name, bool triggerEvaluation = false ) const;

    //!
    //! Convenience function for getting the value of a scene node parameter
    //! while optionally triggering the evaluation chain.
    //!
    //! \param name The name of the parameter whose value to return.
    //! \param triggerEvaluation Flag to control whether to trigger the evaluation chain.
    //! \return The value of the parameter with the given name.
    //!
    Ogre::SceneNode * getSceneNodeValue ( const QString &name, bool triggerEvaluation = false ) const;

    //!
    //! Convenience function for getting the value of an OGRE texture parameter
    //! while optionally triggering the evaluation chain.
    //!
    //! \param name The name of the parameter whose value to return.
    //! \param triggerEvaluation Flag to control whether to trigger the evaluation chain.
    //! \return The value of the parameter with the given name.
    //!
    Ogre::TexturePtr getTextureValue ( const QString &name, bool triggerEvaluation = false ) const;

    //!
    //! Convenience function for getting the value of a parameter group
    //! parameter while optionally triggering the evaluation chain.
    //!
    //! \param name The name of the parameter whose value to return.
    //! \param triggerEvaluation Flag to control whether to trigger the evaluation chain.
    //! \return The value of the parameter with the given name.
    //!
    ParameterGroup * getGroupValue ( const QString &name, bool triggerEvaluation = false ) const;

public: // value setter functions


    //!
    //! Sets the value of the parameter with the given name to the given value
    //! while optionally triggering the dirtying chain.
    //!
    //! \param name The name of the parameter to set the value for.
    //! \param value The new value for the parameter with the given name.
    //! \param triggerDirtying Flag to control whether to trigger the dirtying chain.
    //!
    void setValue ( const QString &name, const QVariant &value, bool triggerDirtying = false );

    //!
    //! Convenience function for setting the value of a boolean parameter
    //! while optionally triggering the dirtying chain.
    //!
    //! \param name The name of the parameter to set the value for.
    //! \param value The value to set for the parameter with the given name.
    //! \param triggerDirtying Flag to control whether to trigger the dirtying chain.
    //!
    void setValue ( const QString &name, bool value, bool triggerDirtying = false );

    //!
    //! Convenience function for setting the value of an integer parameter
    //! while optionally triggering the dirtying chain.
    //!
    //! \param name The name of the parameter to set the value for.
    //! \param value The value to set for the parameter with the given name.
    //! \param triggerDirtying Flag to control whether to trigger the dirtying chain.
    //!
    void setValue ( const QString &name, int value, bool triggerDirtying = false );

    //!
    //! Convenience function for setting the value of an unsigned integer
    //! parameter while optionally triggering the dirtying chain.
    //!
    //! \param name The name of the parameter to set the value for.
    //! \param value The value to set for the parameter with the given name.
    //! \param triggerDirtying Flag to control whether to trigger the dirtying chain.
    //!
    void setValue ( const QString &name, unsigned int value, bool triggerDirtying = false );

	//!
	//! Convenience function for setting the value of a double-precision
	//! floating point parameter while optionally triggering the dirtying
	//! chain.
	//!
	//! \param name The name of the parameter to set the value for.
	//! \param value The value to set for the parameter with the given name.
	//! \param triggerDirtying Flag to control whether to trigger the dirtying chain.
	//!
	void setValue ( const QString &name, float value, bool triggerDirtying /* = false */ );

    //!
    //! Convenience function for setting the value of a double-precision
    //! floating point parameter while optionally triggering the dirtying
    //! chain.
    //!
    //! \param name The name of the parameter to set the value for.
    //! \param value The value to set for the parameter with the given name.
    //! \param triggerDirtying Flag to control whether to trigger the dirtying chain.
    //!
    void setValue ( const QString &name, double value, bool triggerDirtying = false );

    //!
    //! Convenience function for setting the value of a string parameter while
    //! optionally triggering the dirtying chain.
    //!
    //! \param name The name of the parameter to set the value for.
    //! \param value The value to set for the parameter with the given name.
    //! \param triggerDirtying Flag to control whether to trigger the dirtying chain.
    //!
    void setValue ( const QString &name, char *value, bool triggerDirtying = false );

    //!
    //! Convenience function for setting the value of a string parameter while
    //! optionally triggering the dirtying chain.
    //!
    //! \param name The name of the parameter to set the value for.
    //! \param value The value to set for the parameter with the given name.
    //! \param triggerDirtying Flag to control whether to trigger the dirtying chain.
    //!
    void setValue ( const QString &name, const char *value, bool triggerDirtying = false );

    //!
    //! Convenience function for setting the value of a string parameter while
    //! optionally triggering the dirtying chain.
    //!
    //! If the parameter with the given name is not a string parameter the
    //! given value will be converted according to the parameter's type.
    //!
    //! \param name The name of the parameter to set the value for.
    //! \param value The value to set for the parameter with the given name.
    //! \param triggerDirtying Flag to control whether to trigger the dirtying chain.
    //!
    void setValue ( const QString &name, const QString &value, bool triggerDirtying = false );

    //!
    //! Convenience function for setting the value of a color parameter while
    //! optionally triggering the dirtying chain.
    //!
    //! \param name The name of the parameter to set the value for.
    //! \param value The value to set for the parameter with the given name.
    //! \param triggerDirtying Flag to control whether to trigger the dirtying chain.
    //!
    void setValue ( const QString &name, const QColor &value, bool triggerDirtying = false );

    //!
    //! Convenience function for setting the value of an OGRE vector parameter
    //! while optionally triggering the dirtying chain.
    //!
    //! \param name The name of the parameter to set the value for.
    //! \param value The value to set for the parameter with the given name.
    //! \param triggerDirtying Flag to control whether to trigger the dirtying chain.
    //!
    void setValue ( const QString &name, const Ogre::Vector3 &value, bool triggerDirtying = false );

    //!
    //! Convenience function for setting the value of a scene node parameter
    //! while optionally triggering the dirtying chain.
    //!
    //! \param name The name of the parameter to set the value for.
    //! \param value The value to set for the parameter with the given name.
    //! \param triggerDirtying Flag to control whether to trigger the dirtying chain.
    //!
    void setValue ( const QString &name, Ogre::SceneNode *value, bool triggerDirtying = false );

    //!
    //! Convenience function for setting the value of an OGRE texture parameter
    //! while optionally triggering the dirtying chain.
    //!
    //! \param name The name of the parameter to set the value for.
    //! \param value The value to set for the parameter with the given name.
    //! \param triggerDirtying Flag to control whether to trigger the dirtying chain.
    //!
    void setValue ( const QString &name, const Ogre::TexturePtr value, bool triggerDirtying = false );

    //!
    //! Convenience function for setting the value of a parameter group
    //! parameter while optionally triggering the dirtying chain.
    //!
    //! \param name The name of the parameter to set the value for.
    //! \param value The value to set for the parameter with the given name.
    //! \param triggerDirtying Flag to control whether to trigger the dirtying chain.
    //!
    void setValue ( const QString &name, ParameterGroup *value, bool triggerDirtying = false );

    //!
    //! Convenience function for setting the enabled flag of a string parameter
    //!
    //! \param name The name of the parameter to set the value for.
    //! \param enabled The enabled flag to set for the parameter with the given name.
    //!
    void setParameterEnabled ( const QString &name, bool enabled );

public: // callback setter functions

    //!
    //! Sets the function to call when a parameter's value changes.
    //!
    //! Convenience function that calls setChangeFunction for the parameter
    //! with the given name.
    //!
    //! \param name The name of the parameter to set the change function for.
    //! \param changeFunction The name of the function to call when the parameter's value changes.
    //!
    void setChangeFunction ( const QString &name, const char *changeFunction );

    //!
    //! Sets the function to use for computing a parameter's value.
    //!
    //! Convenience function that calls setProcessingFunction for the parameter
    //! with the given name.
    //!
    //! \param name The name of the parameter to set the processing function for.
    //! \param processingFunction The name of the function to use for computing the parameter's value.
    //!
    void setProcessingFunction ( const QString &name, const char *processingFunction );

    //!
    //! Sets the auxiliary function to use for computing a parameter's value.
    //!
    //! Convenience function that calls setProcessingFunction for the parameter
    //! with the given name.
    //!
    //! \param name The name of the parameter to set the processing function for.
    //! \param processingFunction The name of the function to use for computing the parameter's value.
    //!
    void setAuxProcessingFunction ( const QString &name, const char *processingFunction );

    //!
    //! Sets the function to use for executing the command represented by a
    //! parameter.
    //!
    //! Convenience function that calls setCommandFunction for the parameter
    //! with the given name.
    //!
    //! \param name The name of the parameter to set the command function for.
    //! \param commandFunction The function to use for executing the command represented by the parameter.
    //!
    void setCommandFunction ( const QString &name, const char *commandFunction );

	//!
	//! Sets the function which should be called on parameter's connection.
	//!
	//!
	//! \parem name The name of the parameter which should be called on parameter's connection.
	//! \param onCreateConnectFunction The function called on parameter's connection.
	//!
	void setOnCreateConnectionFunction ( const QString &name, const char *onCreateConnectFunction );

	//!
	//! Sets the function which should be called on parameter's connection.
	//!
	//!
	//! \parem name The name of the parameter which should be called on parameter's connection.
	//! \param onConnectFunction The function called on parameter's connection.
	//!
	void setOnConnectFunction ( const QString &name, const char *onConnectFunction );

	//!
	//! Sets the function which should be called on parameter's disconnection.
	//!
	//!
	//! \parem name The name of the parameter which should be called on parameter's connection.
	//! \param onDisconnectFunction The function called on parameter's connection.
	//!
	void setOnDisconnectFunction ( const QString &name, const char *onDisconnectFunction );

public: // misc functions

    //!
    //! Notify when something has changed in the node.
    //! (Parameters added/deleted)
    //!
    void notifyChange ();

    //!
    //! Sets up time dependencies for node parameters.
    //!
    //! Can be implemented in derived classes to set up parameter affections
    //! from the given time parameter to specific parameters of the node that
    //! should be evaluated when the scene time represented by the given time
    //! parameter changes.
    //!
    //! The default implementation does nothing.
    //!
    //! \param timeParameter The parameter representing the current scene time.
    //!
	virtual void setUpTimeDependencies ( NumberParameter *timeParameter, NumberParameter *rangeParameter );

    //!
    //! Evaluates the given new connection for the node.
    //!
    //! The default implementation does nothing.
    //!
    //! \param connection The new connection to evaluate.
    //!
    virtual void evaluateConnection ( Connection *connection );

    //!
    //! Signal that is emitted in order to delete a connection (through SceneModel)
    //!
    //! \param name The name of the object.
    //!
    void deleteConnection ( Connection *connection );

    //!
    //! Render a single frame.
    //!
    void renderSingleFrame ();

	//!
	//! Returns true if the nodes internal structure has changed.
	//!
	bool parametersChanged ();

	//!
	//! Forces all panels to reinit.
	//!
	void forcePanelUpdate ();


    public slots: //

        //!
        //! Sets whether this node should be evaluated in the network.
        //!
        //! \param evaluate The new value for eval flag of this node.
        //!
        virtual void setEvaluate ( bool evaluate );

        //!
        //! Executed when scene loading is ready.
        //!
        virtual void loadReady();

        //!
        //! Executed when scene saving begins.
        //!
        virtual void saveStarted();

		//!
		//! Executed when scene saving has finished.
		//!
		virtual void saveFinished();

		//!
        //! Executed when scene loading is begins.
        //!
        virtual void loadStarted();

		//!
		//! Executed when the scene elements loading is ready.
		//!
		virtual void loadSceneElementsReady();

		//!
		//! Execute before redraw is triggered.
		//!
		virtual void onPreRedraw();

signals: //

        //!
        //! Signal that is emitted when current frame has changed.
        //!
        void frameChanged (int);

        //!
        //! Signal that is emitted when something has changed in the node.
        //! (Parameters added/deleted)
        //!
        void nodeChanged ();

        //!
        //! Signal that is emitted when the node's name has changed.
        //!
        void nodeNameChanged (const QString &name);

        //!
        //! Signal that is emitted when the selected state of the node has changed.
        //!
        //! \param selected The new selected state of the node.
        //!
        void selectedChanged ( bool selected );

        //!
        //! Signal that is emitted in order to delete a connection (through SceneModel)
        //!
        //! \param connection A pointer to the connecion.
        //!
        void sendDeleteConnection ( Connection *connection);

		//!
		//! Signal that is emitted in order to send a request to the scene model to create a connection
		//!
		//! The creation of the connection may fail due to incompatible types, different node names etc.
		//!
		//! \param sourceNodeName The name of the source node.
		//! \param sourceParameterName The name of the parameter of the source node to connect.
		//! \param targetNodeName The name of the target node.
		//! \param targetParameterName The name of the parameter of the target node to connect.
		//!
		void requestCreateConnection(QString sourceNodeName, QString sourceParameterName, QString targetNodeName, QString targetParameterName);			  

        //!
        //! Update frame
        //!
        void updateFrame ();

		//!
        //! Foces a reselection of the node to update the parameter panel.
		//!
        void selectDeselectObject ( const QString &name );

protected: // methods

    //!
    //! Creates a unique name from the given object name by adding the Node
    //! number.
    //!
    Ogre::String createUniqueName ( const QString &objectName );

    virtual void finalize(); 

protected: // data

	//!
    //! The dirty bit
    //!
    bool m_dirty;

	//!
    //! Flag that states whether this node should be evaluated in the network.
    //!
    bool m_evaluate;

    //!
    //! Flag that states whether a plugin for the node's type is available.
    //!
    bool m_typeUnknown;

    //!
    //! Flag that states whether this node is currently selected.
    //!
    bool m_selected;

    //!
    //! Flag that states whether the node is self-evaluating.
    //!
    bool m_selfEvaluating;
	
    //!
    //! Flag that states whether the node is saveable.
    //!
    bool m_saveable;

	//!
    //! Flag that states whether the nodes internal structure has changed.
    //!
    bool m_parametersChanged;

	//!
    //! The global time parameter to use when addressing image sequences.
    //!
    NumberParameter *m_timeParameter;

	//!
	//! The global time range parameter to use when addressing image sequences.
	//!
	NumberParameter *m_rangeParameter;

    //!
    //! The name of the node.
    //!
    //! Must be unique in the node network so that each node can be uniquely
    //! identified by its name.
    //!
    QString m_name;

    //!
    //! The type of the node.
    //!
    QString m_typeName;

    //!
    //! The text to use for filtering parameters of this node before creating
    //! editing widgets in a parameter editor panel.
    //!
    QString m_searchText;

private: // data

    //!
    //! The root of the tree of the node's parameters.
    //!
    ParameterGroup *m_parameterRoot;

	//!
	//! The instance number from the creation of the node
	//!
	int m_instanceNumber;
};

} // end namespace Frapper

#endif
