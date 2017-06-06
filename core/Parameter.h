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
//! \file "Parameter.h"
//! \brief Header file for Parameter class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \author     Simon Spielmann <sspielma@filmakademie.de>
//! \version    1.0
//! \date       12.04.2010 (last updated)
//!

#ifndef PARAMETER_H
#define PARAMETER_H

#include "FrapperPrerequisites.h"
#include "AbstractParameter.h"
#include "Connection.h"
#include <QtCore/QString>
#include <QtCore/QVariant>
#include <QtCore/QVariantList>
#include <QtXml/QDomElement>
#include <QtCore/QStringList>
#include <QtCore/QMutex>
#include "InstanceCounterMacros.h"

// OGRE
#include <OgreVector3.h>
#include <OgreTexture.h>
#include <OgreSceneNode.h>

#if (OGRE_PLATFORM  == OGRE_PLATFORM_WIN32)
#include <windows.h>
#endif


// Declare Ogre Metatypes to be used with QVariant
Q_DECLARE_METATYPE(Ogre::Vector3)
Q_DECLARE_METATYPE(Ogre::TexturePtr)
Q_DECLARE_METATYPE(Ogre::SceneNode*)

namespace Frapper {

    //!
    //! Forward declaration for parameter group.
    //!
    class ParameterGroup;

    //!
    //! Class representing a parameter of a Node.
    //!
    class FRAPPER_CORE_EXPORT Parameter : public AbstractParameter
    {
		friend class SceneModel;
        Q_OBJECT
            ADD_INSTANCE_COUNTER

    public: // type definitions

        //!
        //! Type definition for the size of a parameter, specifying how many values
        //! of the given type the parameter can store.
        //!
        typedef unsigned int Size;

    public: // nested enumerations

        //!
        //! Nested enumeration for the different types of parameters.
        //!
        enum Type {
            T_Unknown = -1,
            T_Bool,
            T_Int,
            T_UnsignedInt,
            T_Float,
            T_String,
            T_Filename,
			T_Directory,
            T_Color,
            T_Enumeration,
            T_TextInfo,
            T_Command,
            T_Geometry,
            T_Light,
            T_Camera,
            T_Image,
            T_Group,
            T_PlugIn,
            T_Generic,
			T_Label,

            T_NumTypes
        };

        //!
        //! Nested enumeration for special types of multiplicity.
        //!
        enum Multiplicity {
            M_Invalid = -1,
            M_OneOrMore
        };

        //!
        //! Nested enumeration for the different types of pins to represent
        //! parameters.
        //!
        enum PinType {
            PT_None = -1,
            PT_Input,
            PT_Output
        };

    public: // static functions

        //!
        //! Returns the name of the given parameter type.
        //!
        //! \param type The type for which to return the name.
        //! \return The name of the given parameter type.
        //!
        static QString getTypeName ( Type type );

        //!
        //! Returns the color of the given parameter type.
        //!
        //! \param type The type for which to return the color.
        //! \return The color of the given parameter type.
        //!
        static QColor getTypeColor ( Type type );

        //!
        //! Returns the parameter type corresponding to the given name.
        //!
        //! \param typeName The name of a parameter type.
        //! \return The parameter type corresponding to the given name.
        //!
        static Type getTypeByName ( const QString &typeName );

        //!
        //! Returns the default value for parameters of the given type.
        //!
        //! \param type The parameter type for which to return the default value.
        //! \return The default value for parameters of the given type.
        //!
        static QVariant getDefaultValue ( Type type );

        //!
        //! Converts the given string to a color value.
        //! The string should contain comma-separated integer values for the red,
        //! green and blue channels respectively.
        //!
        //! \param color The string to convert to a color.
        //! \return A color value corresponding to the given string, or Black when an error occured during conversion.
        //!
        static QColor decodeIntColor ( const QString &color );

        //!
        //! Converts the given string to a color value.
        //! The string should contain comma-separated single-precision floating
        //! point values for the red, green and blue channels respectively.
        //!
        //! \param color The string to convert to a color.
        //! \return A color value corresponding to the given string, or Black when an error occured during conversion.
        //!
        static QColor decodeDoubleColor ( const QString &color );

        //!
        //! Converts the given string to a size value.
        //!
        //! \param size The string to convert to a size value.
        //! \return A size value corresponding to the given string.
        //!
        static Size decodeSize ( const QString &size );

        //!
        //! Converts the given string to a multiplicity value.
        //!
        //! \param multiplicity The string to convert to a multiplicity value.
        //! \return A multiplicity value corresponding to the given string.
        //!
        static int decodeMultiplicity ( const QString &multiplicity );

        //!
        //! Converts the given string to a pin type value.
        //!
        //! \param pinTypeString The string to convert to a pin type value.
        //! \return A pin type value corresponding to the given string.
        //!
        static PinType decodePinType ( const QString &pinTypeString );

        //!
        //! Creates a parameter with the given name and type.
        //!
        //! Depending on the given type an instance of a Parameter-derived class
        //! may be returned, e.g. a NumberParameter object if type equals T_Float.
        //!
        //! \param name The name to use for the new parameter.
        //! \param type The type to use for the new parameter.
        //! \param defaultValue The default value to use for the new parameter.
        //! \return A new parameter with the given name and type.
        //!
        static Parameter * create ( const QString &name, Type type, QVariant defaultValue = QVariant() );

        //!
        //! Creates a parameter from the given XML element.
        //!
        //! Depending on the type described in the XML element an instance of a
        //! Parameter-derived class may be returned, e.g. a NumberParameter object
        //! if the type equals T_Float.
        //!
        //! \param element The element in a DOM tree containing information about the parameter to create.
        //! \return A new parameter corresponding to the given XML element, or 0 if the parameter description is invalid.
        //!
        static Parameter * create ( const QDomElement &element );

        //!
        //! Creates a copy of the given parameter.
        //!
        //! \param parameter The parameter to clone.
        //! \return A new parameter with data corresponding to the given parameter.
        //!
        static Parameter * clone ( const Parameter &parameter );

        //!
        //! Extracts the name of first group found in the given parameter path.
        //!
        //! \param path The parameter path to extract the first group name from.
        //! \return The name of the first group found in the given path, or an empty string if the path is a simple parameter name.
        //!
        static QString extractFirstGroupName ( QString *path );

        //!
        //! Creates an image parameter with the given name.
        //!
        //! \param name The name to use for the parameter.
        //! \param texturePointer The texture pointer to use as the parameter's value.
        //! \return A new image parameter with the given name.
        //!
        static Parameter * createImageParameter ( const QString &name, Ogre::TexturePtr texturePointer = Ogre::TexturePtr() );

        //!
        //! Creates a group parameter with the given name.
        //!
        //! \param name The name to use for the parameter.
        //! \param parameterGroup The parameter group pointer to be contained in the parameter.
        //! \return A new group parameter with the given name.
        //!
        static Parameter * createGroupParameter ( const QString &name, ParameterGroup *parameterGroup = 0 );

    public: // static data

        //!
        //! The string that separates parts in parameter paths.
        //!
        static const QString PathSeparator;

        //!
        //! The string that identifies enumeration separators.
        //!
        static const QString EnumerationSeparator;

    public: // constructors and destructors

        //!
        //! Constructor of the Parameter class.
        //!
        //! \param name The name of the parameter.
        //! \param type The type of the parameter's value(s).
        //! \param value The parameter's value.
        //!
        Parameter ( const QString &name, Type type, const QVariant &value );

        //!
        //! Copy constructor of the Parameter class.
        //!
        //! \param parameter The parameter to copy.
        //!
        Parameter ( const Parameter &parameter, Node* node = 0 );

        //!
        //! Destructor of the Parameter class.
        //!
        virtual ~Parameter ();

    public: // functions

		//!
		//! Creates an exact copy of the parameter.
		//!
		//! \return An exact copy of the parameter.
		//!
		virtual AbstractParameter *clone ();

        //!
        //! Returns whether the parameter object derived from this class is a
        //! parameter group.
        //!
        //! \return True if the parameter object is a parameter group, otherwise False.
        //!
		virtual bool isGroup () const;

        //!
        //! Returns the parameter's type.
        //!
        //! \return The parameter's type.
        //!
        Type getType () const;

        //!
        //! Returns the parameter's size specifying how many values of the given
        //! type it stores.
        //!
        //! \return The parameter's size value.
        //!
        Size getSize () const;

        //!
        //! Sets the parameter's size specifying how many values of the given type
        //! it stores.
        //!
        //! \param size The parameter's size value.
        //!
        void setSize ( Size size );

        //!
        //! Returns the parameter's multiplicity specifying how many parameters of
        //! the given type can be connected to it.
        //!
        //! \return The parameter's multiplicity value.
        //!
        int getMultiplicity () const;

        //!
        //! Sets the parameter's multiplicity specifying how many parameters of the
        //! given type can be connected to it.
        //!
        //! \param multiplicity The parameter's multiplicity value.
        //!
        void setMultiplicity ( int multiplicity );

        //!
        //! Returns the parameter's number of connections.
        //!
        //! \return The parameter's number of connections.
        //!
        int getNumberOfConnections () const;

        //!
        //! Returns the parameter's value while optionally triggering the
        //! evaluation chain.
        //!
        //! \param triggerEvaluation Flag to control whether to trigger the evaluation chain.
        //! \return The parameter's value.
        //!
        virtual QVariant getValue ( const bool triggerEvaluation = false );

		//!
		//! Returns the parameter's value list while optionally triggering the
		//! evaluation chain.
		//!
		//! \param Trigger evaluation.
		//! \param triggerEvaluation Flag to control whether to trigger the evaluation chain.
		//! \return The parameter's value list.
		//!
		virtual const QVariantList getValues ( bool triggerEvaluation  = false );

        //!
        //! Returns the parameter's value list. Which contains the values of all connected
        //! parameters.
        //!
        //! \return The parameter's value list.
        //!
        virtual const QVariantList &getValueList (bool triggerEvaluation = false);

        //!
        //! Sets the parameter's value to the given value.
        //!
        //! \param value The new value for the parameter.
        //!
        void setValue ( const QVariant &value, bool triggerDirtying = false );

		//!
		//! Sets the parameter's value to the given value list.
		//!
		//! \param valueList The new value list for the parameter.
		//!
		void setValues ( const QVariantList &valueList, bool triggerDirtying = false );

        //!
        //! Sets the parameter's value with the given index to the given value.
        //!
        //! Convenience function that works on a list of values.
        //!
        //! \param index The index of the value to set.
        //! \param value The new value for the parameter.
        //!
        void setValue ( int index, const QVariant &value, bool triggerDirtying = false );

        //!
        //! Returns whether the parameter's current value is the default value.
        //!
        //! \return True if the parameter's current value is the default value, otherwise False.
        //!
        bool hasDefaultValue () const;

        //!
        //! Sets the parameter's default value to the given value.
        //!
        //! \param defaultValue The default value to use for the parameter.
        //!
        void setDefaultValue ( const QVariant &defaultValue );

        //!
        //! Resets the parameter to its default value.
        //!
        virtual void reset ();

        //!
        //! Returns the list of parameters that are affected when the value of this
        //! parameter is changed.
        //!
        //! \return The list of parameters affected by this parameter.
        //!
        const List &getAffectedParameters () const;

        //!
        //! Adds a parameter to the list of parameters that are affected when the value of this
        //! parameter is changed.
        //!
        //! \param affectedParameter Parameter that is affected when the value of this parameter is changed.
        //!
        void addAffectedParameter ( Parameter *affectedParameter );

        //!
        //! Removes a parameter from the list of parameters that are affected when the value of this
        //! parameter is changed.
        //!
        //! \param affectedParameter Parameter that is affected when the value of this parameter is changed.
        //!
        void removeAffectedParameter ( Parameter *affectedParameter );

        //!
        //! Returns the list of parameters that affect this parameter when their value
        //! is changed.
        //!
        //! \return List of parameters that affect this parameter when their value is changed.
        //!
        const List &getAffectingParameters () const;

        //!
        //! Adds a parameter to the list of parameters that are affected when the value of this
        //! parameter is changed.
        //!
        //! \param affectingParameter Parameter that affects this parameter when its value is changed.
        //!
        void addAffectingParameter ( Parameter *affectingParameter );

        //!
        //! Removes a parameter from the list of parameters that are affected when the value of this
        //! parameter is changed.
        //!
        //! \param affectingParameter Parameter that affects this parameter when its value is changed.
        //!
        void removeAffectingParameter ( Parameter *affectingParameter );

        //!
        //! Returns the description of the parameter.
        //!
        //! \return The description of the parameter.
        //!
        const QByteArray &getDescription () const;

        //!
        //! Sets the description for the parameter.
        //!
        //! \param description The description for the parameter.
        //!
        void setDescription ( const QString &description );

        //!
        //! Returns the type of pin that should be created to represent the
        //! parameter.
        //!
        //! \return the type of pin that should be created to represent the parameter.
        //!
        PinType getPinType () const;

        //!
        //! Sets the type of pin that should be created to represent the parameter.
        //!
        //! \param pinType The type of pin that should be created to represent the parameter.
        //!
        void setPinType ( PinType pinType );

        //!
        //! Returns all connections currently connected to this parameter.
        //!
        //! \return All connections currently connected to this parameter.
        //!
        const Connection::Map &getConnectionMap () const;

        //!
        //! Adds the given connection to the list of connections.
        //!
        //! \param connection The connection to add to the list of connections.
        //!
        void addConnection ( Connection *connection );

        //!
        //! Removes the given connection from the list of connections.
        //!
        //! \param id The ID of the connection to remove from the list of connections.
        //!
        void removeConnection ( Connection::ID id );

        //!
        //! Returns whether the parameter is connected to at least one other
        //! parameter.
        //!
        //! \return True if the parameter is connected to another parameter, otherwise False.
        //!
        bool isConnected () const;

		//!
		//! Returns the parameter connected to this parameter via the connection with the given id.
		//!
		//! \return The parameter connected to this parameter via the connection with the given id.
		//!
        Parameter * getConnectedParameterById (const unsigned int connectionID = 1) const;

		//!
		//! Returns the parameter connected to this parameter via the connection with the given position.
		//!
		//! \return The parameter connected to this parameter via the connection with the given position.
		//!
		Parameter * getConnectedParameter (const unsigned int position = 0) const;

        //!
        //! Returns whether the parameter's value has changed.
        //!
        //! \return True if the parameter's value has changed, otherwise False.
        //!
        bool isDirty ();

        //!
        //! Sets whether the parameter's value has changed.
        //!
        //! \param dirty The new value for the parameter's dirty flag.
        //!
        virtual void setDirty ( bool dirty );

        //!
        //! Returns the auxiliary dirty flag.
        //!
        //! \return The auxiliary dirty flag.
        //!
        bool isAuxDirty ();

        //!
        //! Sets the auxiliary dirty flag.
        //!
        //! \param dirty The new value for the parameter auxiliary dirty flag.
        //!
        virtual void setAuxDirty ( bool dirty );

        //!
        //! Returns the visibility for this parameter
        //!
        //! \return The visibility of the parameter.
        //!
        bool isVisible () const;

        //!
        //! Sets the visibility for this parameter
        //!
        //! \param visible The visibility of the parameter.
        //!
        void setVisible ( bool visible );

        //!
        //! Returns whether the parameter can not be modified using a control in
        //! the UI.
        //!
        //! \return True if the parameter is read-only, otherwise False.
        //!
        bool isReadOnly () const;

        //!
        //! Sets whether the parameter can not be modified using a control in
        //! the UI.
        //!
        //! \param readOnly The value for the parameter's read-only flag.
        //!
        void setReadOnly ( bool readOnly );

        //!
        //! Sets whether the parameter is self-evaluating.
        //!
        //! \param selfEvaluating True if the parameter is self-evaluating.
        //!
        void setSelfEvaluating ( bool selfEvaluating );

        //! Returns wether the parameter is self-evaluating.
        //!
        //! \return True if the parameter is self-evaluating.
        //!
        bool isSelfEvaluating () const;

        //!
        //! Sets the dirty flag for all parameters that are connected with and 
        //! affected by this parameter.
        //!
        void propagateDirty (bool setFirstTrue = true);

        //!
        //! Sets the aux dirty flag for all parameters that are affecting
        //! this parameter..
        //!
        void propagateAuxDirty ();

        //!
        //! Propagates the evaluation of nodes.
        //!
        //! \param callingNode The node calling this function.
        //!
        void propagateEvaluation ();

        //!
        //! Sets the function to call when the parameter's value changes.
        //!
        //! Should only be called after the parameter has been added to a parameter
        //! group of a node so that the m_node member is already set.
        //!
        //! \param changeFunction The name of the function to call when the parameter's value changes.
        //!
        void setChangeFunction ( const char *changeFunction );

        //!
        //! Sets the function to use for computing the parameter's value.
        //!
        //! Should only be called after the parameter has been added to a parameter
        //! group of a node so that the m_node member is already set.
        //!
        //! \param processingFunction The name of the function to use for computing the parameter's value.
        //!
        void setProcessingFunction ( const char *processingFunction );

        //!
        //! Sets the function to use for computing auxiliary value dependant things.
        //!
        //! Should only be called after the parameter has been added to a parameter
        //! group of a node so that the m_node member is already set.
        //!
        //! \param processingFunction The name of the function to use for computing the parameter's value.
        //!
        void setAuxProcessingFunction ( const char *auxProcessingFunction );

        //!
        //! Sets the function to use for executing the command represented by the
        //! parameter.
        //!
        //! Should only be called after the parameter has been added to a parameter
        //! group of a node so that the m_node member is already set.
        //!
        //! \param commandFunction The function to use for executing the command represented by the parameter.
        //!
        void setCommandFunction ( const char *commandFunction );

		//!
		//! Sets the function which should be called on parameter's connection.
		//!
		//!
		//! \param onCreateConnectionFunction The function called on parameter's connection.
		//!
		void setOnCreateConnectionFunction ( const char *onCreateConnectionFunction );
		
		//!
		//! Sets the function which should be called on parameter's connection is established.
		//!
		//!
		//! \param onConnectFunction The function called on parameter's connection is established.
		//!
		void setOnConnectFunction ( const char *onConnectedFunction );

		//!
		//! Sets the function which should be called on parameter's disconnection.
		//!
		//!
		//! \param onDisconnectFunction The function called on parameter's disconnection.
		//!
		void setOnDisconnectFunction ( const char *onDisconnectFunction );

        //!
        //! Emits the commandExecutionRequested signal to notify connected objects
        //! that the command represented by the parameter should be executed.
        //!
        void executeCommand ();

		//!
		//! Emits the changeExecutionRequested signal to notify connected objects
		//! that the change function of the parameter should be executed.
		//!
		void executeChange ();

        //!
        //! Returns the parameter's value as a character string.
        //!
        //! \return The parameter's value as a character string.
        //!
        QString getValueString () const;

		//!
        //! Returns the signature of the parameters change function.
        //!
        //! \return The signature of the parameters change function.
        //!
		const QByteArray &getChangeFunction() const;

		//!
        //! Returns the signature of the parameters processing function.
        //!
        //! \return The signature of the parameters processing function.
        //!
		const QByteArray &getProcessingFunction() const;

		//!
        //! Returns the signature of the parameters aux processing function.
        //!
        //! \return The signature of the parameters aux processing function.
        //!
		const QByteArray &getAuxProcessingFunction() const;

		//!
		//! Returns the parameter as string of the format NodeName.ParameterName
		//!
		//! \return The node and name of the parameter
		//!
		const QString toString() const;

signals: // signals

        //!
        //! Signal that is emitted when the value of the parameter becomes dirty.
        //!
        void dirtied ();

        //!
        //! Signal that is emitted when the parameter's value has changed.
        //!
        void valueChanged ();

        //!
        //! Signal that is emitted when the parameter's value with the given index
        //! has changed.
        //!
        //! \param index The index of the value in a list of values.
        //!
        void valueChanged ( int index );

        //!
        //! Signal that is emitted when the value of the parameter should be
        //! processed.
        //!
        void processingRequested ();

        //!
        //! Signal that is emitted when auxiliary changes in combination with the
        //! value happen.
        //!
        void auxProcessingRequested ();

        //!
        //! Signal that is emitted when a command parameter is triggered.
        //!
        void commandExecutionRequested ();

		//!
        //! Signal that is emitted when the change function should be triggered
        //! processed.
        //!
        void changeExecutionRequested ();

		//!
        //! Signal that is emitted when a connection is created.
        //!
		void connectionCreated(Parameter *);

		//!
        //! Signal that is emitted when a connection is established.
        //!
		void connectionEstablished(int id = -1);

		//!
        //! Signal that is emitted when a connection is destroyed.
        //!
		void connectionDestroyed(int id = -1);


    protected: // data

		//!
        //! Flag that states whether the parameter's value has changed.
        //!
        bool m_dirty;

        //!
        //! Auxiliary dirty flag.
        //!
        bool m_auxDirty;

        //!
        //! Flag that states whether the parameter's should be visible
        //! in e.g. ParameterPanel or not.
        //!
        bool m_visible;

        //!
        //! Flag that states whether the parameter's value can not be changed using
        //! an input control in the UI.
        //!
        bool m_readOnly;

        //!
        //! Flag that states whether the parameter is self-evaluating.
        //!
        bool m_selfEvaluating;

        //!
        //! Mutex for threaded programming.
        //!
        QMutex m_mutex;

        //!
        //! The type of the parameter's value.
        //!
        Type m_type;

		//!
        //! The type of pin that should be created to represent the parameter.
        //!
        PinType m_pinType;

        //!
        //! The parameter's size specifying how many values of the given type it
        //! stores.
        //!
        int m_size;

        //!
        //! The parameter's multiplicity specifying how many parameters of the
        //! given type can be connected to it.
        //!
        int m_multiplicity;

        //!
        //! The parameter's default value.
        //!
        QVariant m_defaultValue;

        //!
        //! The parameter's value.
        //!
        QVariant m_value;

        //!
        //! The parameter's value.
        //!
        QVariantList m_valueList;

        //!
        //! A description of the parameter.
        //!
        QByteArray m_description;

		//!
		//! The Name of the function to call when the parameter's value changes.
		//!
		QByteArray m_changeFunction;

		//!
		//! The Name of the function to call when the parameter is processed.
		//!
		QByteArray m_processingFunction;

		//!
		//! The Name of the function to call when the aux parameter is processed.
		//!
		QByteArray m_auxProcessingFunction;

        //!
        //! The map of connections that are connected to this connector with
        //! connection IDs as keys.
        //!
        Connection::Map m_connectionMap;

        //!
        //! A list of parameter names that are affected when the value of this
        //! parameter is changed.
        //!
        List m_affectedParameters;

        //!
        //! A list of parameter names that affect this parameter.
        //!
        List m_affectingParameters;
    };

} // end namespace Frapper

#endif
