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
//! \file "NumberParameter.h"
//! \brief Header file for NumberParameter class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \author		Simon Spielmann <sspielma@filmakademie.de>
//! \version    1.0
//! \date       05.03.2013 (last updated)
//!

#ifndef NUMBERPARAMETER_H
#define NUMBERPARAMETER_H

#include "FrapperPrerequisites.h"
#include "Parameter.h"
#include "Key.h"
#include <QtCore/QPair>
#include <QPainter>
#include <Helper.h>

namespace Frapper {


//!
//! Class representing parameters for numeric values.
//!
class FRAPPER_CORE_EXPORT NumberParameter : public Parameter
{

	Q_OBJECT

public: // nested enumerations

    //!
    //! Nested enumeration for the different input methods for editing number
    //! parameters.
    //!
    enum InputMethod {
        IM_SpinBox = 0,
        IM_Slider,
        IM_SliderPlusSpinBox,

        IM_NumInputMethods
    };

public: // static functions

    //!
    //! Decodes the given input method string to an input method value.
    //!
    //! \param inputMethodString The input method string to decode.
    //! \return An input method corresponding to the given input method string.
    //!
    static InputMethod decodeInputMethod ( const QString &inputMethodString );

public: // constructors and destructors

    //!
    //! Constructor of the NumberParameter class.
    //!
    //! \param name The name of the parameter.
    //! \param type The type of the parameter's value(s).
    //! \param value The parameter's value.
    //! \param animated The parameter's animation status.
    //!
    NumberParameter ( const QString &name, Type type, const QVariant &value );

    //!
    //! Copy constructor of the NumberParameter class.
    //!
    //! \param parameter The parameter to copy.
    //! \param node The parent node.
    //!
    NumberParameter ( const NumberParameter &parameter, Node* node = 0  );

    //!
    //! Destructor of the FilenameParameter class.
    //!
    virtual ~NumberParameter ();

public: // functions

    //!
    //! Creates an exact copy of the parameter.
    //!
    //! \return An exact copy of the parameter.
    //!
    virtual AbstractParameter * clone ();

    //!
    //! Sets whether the parameter's value has changed.
    //!
    //! \param dirty The new value for the parameter's dirty flag.
    //!
    virtual void setDirty ( bool dirty );

    //!
    //! Sets whether the controls for editing the parameter or parameter group
    //! in the UI should be enabled.
    //!
    //! \param enabled The value for the parameter's enabled flag.
    //!
    virtual void setEnabled ( const bool enabled, const bool propagate /*= true*/ );

    //!
    //! Returns the desired input method of numeric values of the parameter.
    //!
    //! \return The input method for numeric values of the parameter.
    //!
    InputMethod getInputMethod () const;

    //!
    //! Sets the desired input method of numeric values of the parameter.
    //!
    //! \param inputMethod The input method for numeric values of the parameter.
    //!
    void setInputMethod ( InputMethod inputMethod );

    //!
    //! Returns the lower limit for numeric values of the parameter.
    //!
    //! \return The lower limit for numeric values of the parameter.
    //!
    QVariant getMinValue () const;

    //!
    //! Sets the lower limit for numeric values of the parameter.
    //!
    //! \param minValue The lower limit for numeric values of the parameter.
    //!
    void setMinValue (const QVariant &minValue );
	
    //!
    //! Returns the upper limit for numeric values of the parameter.
    //!
    //! \return The upper limit for numeric values of the parameter.
    //!
    QVariant getMaxValue () const;

    //!
    //! Returns the absolute upper limit for numeric values of the parameter.
    //!
    //! \return The absolute upper limit for numeric values of the parameter.
    //!
    float getAbsMaxValue () const;

    //!
    //! Sets the upper limit for numeric values of the parameter.
    //!
    //! \param maxValue The upper limit for numeric values of the parameter.
    //!
    void setMaxValue ( const QVariant &maxValue );

    //!
    //! Returns the step size numeric values of the parameter.
    //!
    //! \return The step size for numeric values of the parameter.
    //!
    QVariant getStepSize () const;

    //!
    //! Sets the step size numeric values of the parameter.
    //!
    //! \param stepSize The step size for numeric values of the parameter.
    //!
    void setStepSize ( const QVariant &stepSize );

	//!
    //! Returns the step size numeric values of the parameter.
    //!
    //! \return The step size for numeric values of the parameter.
    //!
    float getTimeStepSize () const;

    //!
    //! Sets the step size numeric values of the parameter.
    //!
    //! \param stepSize The step size for numeric values of the parameter.
    //!
    void setTimeStepSize ( const float timeStepSize );

    //!
    //! Returns the unit of the parameter.
    //!
    //! \return The unit of the parameter (an empty string if no unit has been set).
    //!
    QString getUnit () const;

	//!
    //! Sets the unit scale for the parameter.
    //!
    //! \param scale The new unit scale.
    //!
    void setUnitScale ( const float scale );

	//!
    //! Returns the unit scale for the parameter.
    //!
    //! \return The unit scale for the parameter.
    //!
    float getUnitScale ( ) const;

	//!
	//! Calculate the limits for numeric values of the parameter.
	//!
	void calculateExtrema ();

    //!
    //! Sets the unit for the parameter.
    //!
    //! \param unit The unit for the parameter.
    //!
    void setUnit ( const QString &unit );

    //!
    //! Sets the current time for the parameter.
    //!
    //! \param timeValue The current time for the parameter.
    //!
    void setTime ( const QVariant &timeValue );

    //!
    //! Scales all values from the parameter
    //!
    //! \param The scale factor.
    //!
    void scaleValues( const float &scaleFactor);


public: // animation functions

    //!
    //! Returns whether the parameter contains keys.
    //!
    //! \return True if the parameter contains keys, otherwise False.
    //!
    bool isAnimated () const;

	//!
	//! Adds the given parameter to the given time in the list of keys.
	//!
	//! \param key The key to add to the list
	//!
	void addKey ( Key &key );

    //!
    //! Adds the actuel numberparameter value to the given time in the list of keys.
    //!
    //! \param time The time index to add the value to the list.
    //!
	void addKey ( const int time, const Key::KeyType type = Key::KT_Linear );

    //!
    //! Adds the given parameter to the back of the list of keys.
    //!
    //! \param value The value to add to the list.
    //!
    void addKey ( const QVariant &value, const Key::KeyType type = Key::KT_Linear );

    //!
    //! Adds the given parameter to the back of the list of keys.
    //!
    //! \param value The value to add to the list
    //!
    void addKey ( float value, const Key::KeyType type = Key::KT_Linear );

    //!
    //! Adds the given parameter to the given time in the list of keys, without sorting.
    //!
    //! \param key The key to add to the list
    //!
    void addKeyPresorted ( Key &key );

    //!
    //! Sets the given list in the given steps in the list of keys.
    //!
    //! \param keys The value list to add to the key list.
    //!
    void addKeys ( const QList<float> *keys, const Key::KeyType type = Key::KT_Linear );

    //!
    //! Returns the number of keys in the list.
    //!
    //! \return The number of keys in the list.
    //!
    int numKeys () const;

    //!
    //! Returns whether the list of keys is empty.
    //!
    //! \return True if the list of keys is empty, otherwise False.
    //!
    bool isEmpty () const;

	//!
	//! Removes the given key.
	//!
	//! \param key The pointer to the key to be removed.
	//!
	void removeKey (const Key &key );

    //!
    //! Removes the key at the given time.
    //!
    //! \param time The time of the key to remove.
    //!
    void removeKey ( const float time );

    //!
    //! Removes the key at the given index.
    //!
    //! \param index The index for the key to remove.
    //!
    void removeKey ( const int index );

    //!
    //! Removes the keys at the given range.
    //!
    //! \param minTime The start time for the keys to remove.
    //! \param maxTime The end time for the keys to remove.
    //!
    void removeKeys ( const float minTime, const float maxTime );

    //!
    //! Removes the keys at the given range.
    //!
    //! \param minIndex The start time for the keys to remove.
    //! \param maxIndex The end time for the keys to remove.
    //!
    void removeKeys ( const int minIndex, const int maxIndex );

    //!
    //! Removes all keys.
    //!
    void clearKeys ( );

    //!
    //! Returns the key at the given time.
    //!
    //! \param time The time to the key.
    //! \return The key at the given time.
    //!
    const Key &getKey ( const float time );

    //!
    //! Returns the key at the given index.
    //!
    //! \param index The index to the Key.
    //! \return The key at the given index.
    //!
    const Key &getKey ( const unsigned int &index );

    //!
    //! Returns a pointer to the list of keys.
    //!
    //! \return A pointer to the list of keys.
    //!
    const QList<Key> &getKeys () const;

    //!
    //! Returns the size of the list of keys.
    //!
    //! \return The size of the list of keys.
    //!
    unsigned int getKeysSize () const;

    //!
    //! Returns the value of the key at the given time.
    //!
    //! \param time The time to the Key.
    //! \return The value at the given time.
    //!
    QVariant getKeyValuePos ( const float time ) ;

    //!
    //! Returns the value of the key at the given index.
    //!
    //! \param index The index to the Key.
    //! \return The value at the given index.
    //!
    QVariant getKeyValueIndex ( const unsigned int index ) const;

	//!
    //! Returns the interpolated value of the key at the given time.
    //!
    //! \param time The time inbetween the Keys.
    //! \return The interpolated value at the given time.
    //!
    QVariant getKeyValueTime ( const float time );

	//!
    //! Returns the time position of the last key.
    //!
    //! \return The time position of the last key.
    //!
	float getLastKeyPos () const;

    //!
    //! Returns the parameter's value while optionally triggering the
    //! evaluation chain.
    //!
    //! \param triggerEvaluation Flag to control whether to trigger the evaluation chain.
    //! \return The parameter's value.
    //!
    QVariant getValue ( const bool triggerEvaluation = false );

signals: // signals

	//!
	//! Signal that is emitted when the range of the parameter (min/max) was changed.
	//!
	void rangeChanged();

protected: // functions

	//!
	//! Enables timeline affection;
	//!
	void enableTimelineAffection();

	//!
    //! Returns the linear interpolated value of the key at the given time.
    //!
    //! \param time The time of the key.
    //! \return The interpolated value of the key at the given time.
    //!
    QVariant getKeyValueInterpol ( const QVariant &time );

    //!
    //! Returns the sorted index from keylist by evaluating the time value.
    //!
    //! \param time The time of the Key.
    //! \return A list iterator to the index in list.
    //!
    inline QList<Key>::iterator findIndex ( const float time )
	{
		return qLowerBound(m_keys.begin(), m_keys.end(), Key(time, 0.0f), lessThan);
	}

    //!
    //! Returns the sorted index from keylist by evaluating the time value of the given key.
    //!
    //! \param key The Key to add.
    //! \return A list iterator to the index in list.
    //!
    inline QList<Key>::iterator findIndex ( const Key &key ) 
	{
		return qLowerBound(m_keys.begin(), m_keys.end(), key, lessThan);
	}

private: // helper functions

	//!
	//! Sets the lower and upper limit for numeric values of the parameter.
	//!
	//! \param value The limit for numeric values of the parameter.
	//!
	inline void setBounds (const QVariant &value)
	{
		if (value.toFloat() > m_maxValue.toFloat())
			m_maxValue = value;
		else if (value.toFloat() < m_minValue.toFloat())
			m_minValue = value;
	}

    //!
    //! Less than definition Function for comparing unique icon Key
    //!
    //! \param k1 The first key for comparison.
    //! \param k2 The second key for comparison.
    //! \return A boolean as result from comparison.
    //!
    static inline bool lessThan ( const Key &k1, const Key &k2 )
	{
		return k1.index < k2.index;
	}

	//!
	//! Returns the interpolated value of the key at the given time.
	//!
	//! \param time The time of the key.
	//! \return The interpolated value of the key at the given time.
	//!
	inline float interpolateLinear ( const float lowerX,  const float lowerY,
									 const float upperX,  const float upperY,							
									 const float inBetween ) const
	{
		const float s1 = 1.0f - (upperX - inBetween) / (upperX - lowerX);
		return lowerY * (1.0f - s1) + upperY * s1;
	}

	//!
	//! Returns the bezier interpolated value of the key at the given time.
	//!
	//! \param time The time of the key.
	//! \return The interpolated value of the key at the given time.
	//!
	inline float interpolateBezier ( const float lowerX, const float lowerY, const float upperX, const float upperY,
									 const float lowerTanX, const float lowerTanY, const float upperTanX, const float upperTanY,
									 const float inBetween ) const
	{
		const float s1 = Helpers::InterpolationH::ApproximateCubicBezierParameter(inBetween, lowerX, lowerTanX, 2.0f*upperX - upperTanX, upperX);
		const float s2 = s1*s1;
		const float s3 = s2*s1;
		const float newUpperTanY = 2.0f*upperY - upperTanY;

		return	s3*(upperY + 3.0f*(lowerTanY - newUpperTanY) - lowerY) + 
				3.0f*s2*(lowerY - 2.0f*lowerTanY + newUpperTanY) +
				3.0f*s1*(lowerTanY - lowerY) + lowerY;
	}

private: // data

    //!
    //! The input method for numeric values of the parameter.
    //!
    InputMethod m_inputMethod;

	//!
    //! True if the parameter is affected by the timeline.
    //!
	bool m_timeAffected;

	//!
    //! The unit scale of the numeric values.
    //!
    float m_unitScale;

	//!
    //! The time step size for numeric values of the parameter.
    //!
	float m_timeStepSize;

	//!
    //! The step size for numeric values of the parameter.
    //!
    QVariant m_stepSize;

    //!
    //! The lower limit for numeric values of the parameter.
    //!
    QVariant m_minValue;

    //!
    //! The upper limit for numeric values of the parameter.
    //!
    QVariant m_maxValue;

    //!
    //! The parameter's unit (an empty string if no unit has been set).
    //!
    QString m_unit;

    //!
    //! The list of keys for animating the parameter's numeric value.
    //!
    QList<Key> m_keys;
};

} // end namespace Frapper

#endif
