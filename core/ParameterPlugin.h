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
http://www.gnu.org/copyleft/lesser.txt.
-----------------------------------------------------------------------------
*/

//!
//! \file "ParameterPlugin.h"
//! \brief Implementation file for vectorfieldclass.
//!
//! \author     Sebastian Bach <sebastian.bach@filmakademie.de>
//! \version    1.0
//! \date       22.12.2009 (last updated)
//!


#ifndef PARAMETERPLUGIN_H
#define PARAMETERPLUGIN_H

#include "FrapperPrerequisites.h"
#include "Parameter.h"
#include "NumberParameter.h"
#include "Key.h"

namespace Frapper {

class FRAPPER_CORE_EXPORT ParameterPlugin : public Parameter
{
    Q_OBJECT
public: // constructors and destructors : public Parameter


    //!
    //! Constructor of the ParameterPlugin class.
    //!
    //! \param name The name of the parameter.
    //! \param type The type of the parameter's widget.
    //! \param falgs The flags of the parameter.
    //! \param value The parameter's value.
    //!
    ParameterPlugin ( const QString &name, QString type, QMap<QString, QVariant> flags, const QVariant &value );

    //!
    //! Constructor of the ParameterPlugin class
    //! Overload without flags
    //!
    //! \param name The name of the parameter.
    //! \param type The type of the parameter's widget.
    //! \param value The parameter's value.
    //!
    ParameterPlugin ( const QString &name, QString type, const QVariant &value );

    //!
    //! Constructor of the ParameterPlugin class
    //! Overload without flags of call
    //!
    //! \param name The name of the parameter.
    //! \param value The parameter's value.
    //!
    ParameterPlugin ( const QString &name, const QVariant &value );

    //!
    //! Copy constructor
    //!
    //! \param parameter The parameter to copy
    //!
    ParameterPlugin ( ParameterPlugin * parameter );

    //!
    //! Destructor of the ParameterPlugin class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~ParameterPlugin ();

public: // functions

    //!
    //! Sets the flags, replaces existing flags
    //!
    //! \param flags QMap of flags
    //!
    void setFlags(QMap<QString, QVariant> flags);

    //!
    //! Adds a flag, overwrites the flag if existing
    //!
    //! \param key The name of the flag
    //! \param value The value of the flag
    //!
    void addFlag(QString key, QVariant value);

    //!
    //! returns all flags
    //!
    //! \return A QMap of all flags
    //!
    QMap<QString, QVariant> getFlags();

    //!
    //! returns the value of a certain flag
    //!
    //! \param flag The name of the flag
    //! \return the Value of the flag
    //!
    QVariant getFlag(QString flag);

    //!
    //! returns the identification of the widget
    //!
    //! \return The name of the associated plugin widget
    //!
    QString getCall();

    //!
    //! sets the identification of the widget
    //!
    //! \param call The name of the associated widget
    //!
    void setCall(QString call);

    //!
    //! forces the associated widget to redraw
    //!
    void triggerWidgetRedraw();

    //																					//
    //																					//
    //	the following functions are still in developement and not intended for use		//
    //																					//
    //																					//

    //!
    //! copies the value from another parameter to this parameter
    //!
    //! \param parameter Pointer of the source parameter
    //!
    void copyParameterValue(Parameter * parameter);

    //!
    //! copies the value from a nuumber parameter to this parameter
    //!
    //! \param numberParameter Pointer of the source NumberParameter
    //!
    void copyNumberParameterValue(NumberParameter * numberParameter);

    //!
    //! Sets the value of a interger to the integer value of a flag
    //!
    //! \param flag Name of the flag
    //! \param number Pointer to the number variable to be set
    //! \return True if number was changed
    //!
    bool getIntegerFlag(QString flag, int &number);

    //!
    //! Sets the value of a float to the float value of a flag
    //!
    //! \param flag Name of the flag
    //! \param number Pointer to the number variable to be set
    //! \return True if number was changed
    //!
    bool getFloatFlag(QString flag, float &number);

    //!
    //! Sets the value of a QString to the QString value of a flag
    //!
    //! \param flag Name of the flag
    //! \param string Pointer to the string variable to be set
    //! \return True if string was changed
    //!
    bool getStringFlag(QString flag, QString &string);

    //!
    //! Sets the value of a bool to the bool value of a flag
    //!
    //! \param flag Name of the flag
    //! \param boolean Pointer to the boolean variable to be set
    //! \return True if boolean variabe was changed
    //!
    bool getBoolFlag(QString flag, bool &boolean);

    //!
    //! Returns the type of a flag
    //!
    //! \param flag Nam of the flag
    //! \return Type of the flag
    //!
    QVariant::Type getFlagType(QString flag);

    //!
    //! Returns the type of the parameters value
    //!
    //! \return Type of the parameters value
    //!
    QVariant::Type getParameterType();

    //!
    //! Transforms a List of floats into a list of QVariants
    //!
    //! \param list QList of float values
    //! \return QList of QVariant values
    //!
    QList<QVariant> * TransformFloatToVariantList(QList<float> * list);

    //!
    //! Transforms a List of QVariants into a list of floats
    //!
    //! \param list QList of QVariant values
    //! \return QList of float values
    //!
    QList<float> * TransformVariantToFloatList(QList<QVariant> * list);

private: // data

    //!
    //! Holds the identification of the associated plugin widget
    //!
    QString m_call;

    //!
    //! Holds the flags
    //!
    QMap<QString, QVariant> m_flags;

signals:

    //!
    //! is emmited to force the widget´s redraw
    //!
    void forceRedraw();
};

} // end namespace Frapper

#endif