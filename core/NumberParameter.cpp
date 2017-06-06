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
//! \file "NumberParameter.cpp"
//! \brief Implementation file for NumberParameter class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \author     Simon Spielmann <sspielma@filmakademie.de>
//! \version    1.0
//! \date       05.03.2013 (last updated)
//!

#include "NumberParameter.h"
#include "Node.h"

namespace Frapper {

///
/// Public Static Functions
///


//!
//! Decodes the given input method string to an input method value.
//!
//! \param inputMethodString The input method string to decode.
//! \return An input method corresponding to the given input method string.
//!
NumberParameter::InputMethod NumberParameter::decodeInputMethod ( const QString &inputMethodString )
{
    if (inputMethodString == "SliderPlusSpinBox")
        return IM_SliderPlusSpinBox;
    else
        return IM_SpinBox;
}


///
/// Constructors and Destructors
///


//!
//! Constructor of the FilenameParameter class.
//!
//! \param name The name of the parameter.
//! \param type The type of the parameter's value(s).
//! \param value The parameter's value.
//! \param animated The parameter's animation status.
//!
NumberParameter::NumberParameter ( const QString &name, Type type, const QVariant &value) :
Parameter(name, type, value),
m_inputMethod(IM_SpinBox),
m_minValue(type == Parameter::T_UnsignedInt ? 0 : -100),
m_maxValue(100),
m_stepSize(1.0f),
m_timeStepSize(1.0f),
m_unit(""),
m_unitScale(1.0f),
m_timeAffected(false)
{
}


//!
//! Copy constructor of the NumberParameter class.
//!
//! \param parameter The parameter to copy.
//! \param node The parent node.
//!
NumberParameter::NumberParameter ( const NumberParameter &parameter, Node* node /*= 0*/ ) :
Parameter(parameter),
m_inputMethod(parameter.m_inputMethod),
m_minValue(parameter.m_minValue),
m_maxValue(parameter.m_maxValue),
m_stepSize(parameter.m_stepSize),
m_timeStepSize(parameter.m_timeStepSize),
m_unit(parameter.m_unit),
m_unitScale(parameter.m_unitScale),
m_timeAffected(parameter.m_timeAffected)
{
    foreach (Key key, parameter.m_keys)
        m_keys.append(key);

    m_node = node;
    if (node)
        m_node->getTimeParameter()->addAffectedParameter(this);
}


//!
//! Destructor of the FilenameParameter class.
//!
NumberParameter::~NumberParameter ()
{
     m_keys.clear();
}


///
/// Public Functions
///


//!
//! Creates an exact copy of the parameter.
//!
//! \return An exact copy of the parameter.
//!
AbstractParameter * NumberParameter::clone ()
{
    return new NumberParameter(*this);
}


//!
//! Sets whether the parameter's value has changed.
//!
//! \param dirty The new value for the parameter's dirty flag.
//!
void NumberParameter::setDirty ( const bool dirty )
{
    Parameter::setDirty(dirty);

    // check if the parameter is animated and has become dirty
    if (dirty && m_enabled && !m_keys.empty()) {
        // update the value depending on the current time index
        NumberParameter *timeParameter = dynamic_cast<NumberParameter *>(m_node->getTimeParameter());
        if ( timeParameter ){
            setValue(getKeyValueInterpol(timeParameter->getValue()));
			processingRequested();
        }
    }
}

//!
//! Sets whether the controls for editing the parameter or parameter group
//! in the UI should be enabled.
//!
//! \param enabled The value for the parameter's enabled flag.
//!
void NumberParameter::setEnabled ( const bool enabled, const bool propagate /*= true*/ )
{
    AbstractParameter::setEnabled(enabled, propagate);
    if (!enabled) setValue(0.0);
}


//!
//! Returns the desired input method of numeric values of the parameter, e.g. Slider, etc.
//!
//! \return The input method for numeric values of the parameter.
//!
NumberParameter::InputMethod NumberParameter::getInputMethod () const
{
    return m_inputMethod;
}


//!
//! Sets the desired input method of numeric values of the parameter, e.g. Slider, etc.
//!
//! \param stepSize The input method for numeric values of the parameter.
//!
void NumberParameter::setInputMethod ( NumberParameter::InputMethod inputMethod )
{
    m_inputMethod = inputMethod;
}


//!
//! Returns the lower limit for numeric values of the parameter.
//!
//! \return The lower limit for numeric values of the parameter.
//!
QVariant NumberParameter::getMinValue () const
{
    return m_minValue;
}


//!
//! Sets the lower limit for numeric values of the parameter.
//!
//! \param minValue The lower limit for numeric values of the parameter.
//!
void NumberParameter::setMinValue (const QVariant &minValue )
{
	if (minValue.toFloat() < m_maxValue.toFloat()) {
		m_minValue = minValue;
		emit rangeChanged();
	}
}


//!
//! Returns the upper limit for numeric values of the parameter.
//!
//! \return The upper limit for numeric values of the parameter.
//!
QVariant NumberParameter::getMaxValue () const
{
    return m_maxValue;
}


//!
//! Sets the upper limit for numeric values of the parameter.
//!
//! \param maxValue The upper limit for numeric values of the parameter.
//!
void NumberParameter::setMaxValue (const QVariant &maxValue )
{
	if (maxValue.toFloat() > m_minValue.toFloat()) {
		m_maxValue = maxValue;
		emit rangeChanged();
	}
}


//!
//! Returns the absolute upper limit for numeric values of the parameter.
//!
//! \return The absolute upper limit for numeric values of the parameter.
//!
float NumberParameter::getAbsMaxValue () const
{
    if (abs(m_minValue.toFloat()) > m_maxValue.toFloat())
        return abs(m_minValue.toFloat());
    else
        return m_maxValue.toFloat();
}


//!
//! Returns the step size numeric values of the parameter.
//!
//! \return The step size for numeric values of the parameter.
//!
QVariant NumberParameter::getStepSize () const
{
    return m_stepSize;
}


//!
//! Sets the step size numeric values of the parameter.
//!
//! \param stepSize The step size for numeric values of the parameter.
//!
void NumberParameter::setStepSize ( const QVariant &stepSize )
{
    m_stepSize = stepSize;
}


//!
//! Returns the step size numeric values of the parameter.
//!
//! \return The step size for numeric values of the parameter.
//!
float NumberParameter::getTimeStepSize () const
{
    return m_timeStepSize;
}


//!
//! Sets the step size numeric values of the parameter.
//!
//! \param stepSize The step size for numeric values of the parameter.
//!
void NumberParameter::setTimeStepSize ( const float timeStepSize )
{
    m_timeStepSize = timeStepSize;
}


//!
//! Returns the unit of the parameter.
//!
//! \return The unit of the parameter.
//!
QString NumberParameter::getUnit () const
{
    return m_unit;
}


//!
//! Sets the unit for the parameter.
//!
//! \param unit The unit for the parameter.
//!
void NumberParameter::setUnit ( const QString &unit )
{
    m_unit = unit;
}


//!
//! Sets the unit scale for the parameter.
//!
//! \param scale The new unit scale.
//!
void NumberParameter::setUnitScale ( const float scale )
{
	m_unitScale = scale;
}


//!
//! Returns the unit scale for the parameter.
//!
//! \return The unit scale for the parameter.
//!
float NumberParameter::getUnitScale ( ) const
{
	return m_unitScale;
}

//!
//! Calculate the limits for numeric values of the parameter.
//!
void NumberParameter::calculateExtrema ()
{
    m_maxValue = -HUGE_VAL;
    m_minValue = +HUGE_VAL;
    register float keyvalue;

    foreach (const Key &key, m_keys) {
		keyvalue = key.keyValue.toFloat();
        if (keyvalue > m_maxValue.toFloat())
            m_maxValue = keyvalue;
        if (keyvalue < m_minValue.toFloat())
            m_minValue = keyvalue;
    }
}

//!
//! Scales all values from the parameter
//!
//! \param The scale factor.
//!
void NumberParameter::scaleValues( const float &scaleFactor )
{
    foreach (const Key &key, m_keys)
        const_cast<Key&>(key) /= scaleFactor;

    m_value.setValue(m_value.toFloat() / scaleFactor);
    m_minValue.setValue(m_minValue.toFloat() / scaleFactor);
    m_maxValue.setValue(m_maxValue.toFloat() / scaleFactor);
}



///
/// Public Animation Functions
///


//!
//! Returns whether the parameter contains keys.
//!
//! \return True if the parameter contains keys, otherwise False.
//!
bool NumberParameter::isAnimated () const
{
    return !m_keys.isEmpty();
}


//!
//! Adds the given parameter to the given time in the list of keys.
//!
//! \param key The key to add to the list
//!
void NumberParameter::addKey ( Key &key )
{
	key.parent = this;
	const QList<Frapper::Key>::iterator i = findIndex(key.index);
	if(i != m_keys.end()) {	
		if (i->keyValue != key.keyValue) {
			m_keys.insert(i, key);
		}
	} else {
		m_keys.insert(i, key);
	}
	enableTimelineAffection();
	setBounds(key.keyValue);
}


//!
//! Adds the actual number parameter value to the given time in the list of keys.
//!
//! \param time The time index to add the value to the list.
//!
void NumberParameter::addKey ( const int time, const Key::KeyType type /*= Key::KT_Linear*/ )
{
    const float index = time * m_timeStepSize;
	const QList<Frapper::Key>::iterator i = findIndex(index);
	if(i != m_keys.end()) {	
		if (i->keyValue != m_value) {
			m_keys.insert(i, Key(index, m_value, type, this));
		}
	} else {
		m_keys.insert(i, Key(index, m_value, type, this));
	}
	enableTimelineAffection();
}


//!
//! Adds the given parameter to the back of the list of keys.
//!
//! \param value The value to add to the list
//!
void NumberParameter::addKey ( const QVariant &value, const Key::KeyType type /*= Key::KT_Linear*/ )
{
    m_keys.push_back(Key(m_keys.size() * m_timeStepSize, value, type, this));
	
	enableTimelineAffection();
	setBounds(value);
}


//!
//! Adds the given parameter to the back of the list of keys.
//!
//! \param value The value to add to the list
//!
void NumberParameter::addKey ( float value, const Key::KeyType type /*= Key::KT_Linear*/ )
{
    m_keys.push_back(Key((m_keys.size()) * m_timeStepSize, value, type, this));
	
	enableTimelineAffection();
	setBounds(value);
}


//!
//! Adds the given parameter to the given time in the list of keys, without sorting.
//!
//! \param key The key to add to the list
//!
void NumberParameter::addKeyPresorted ( Key &key )
{
	key.parent = this;
    m_keys.push_back(key);
    
	enableTimelineAffection();
	setBounds(key.keyValue);
}


//!
//! Sets the given List in the given steps in the list of keys.
//!
//! \param key The value list to add to the key list
//!
void NumberParameter::addKeys ( const QList<float> *keys, const Key::KeyType type /*= Key::KT_Linear*/ )
{
    m_keys.clear();
    register float step = 0.0;
    
	foreach (const float &value, *keys) {
		m_keys.push_back(Key(step, value, type, this));
        step += m_timeStepSize;
		setBounds(value);
    }
    
	enableTimelineAffection();
}


//!
//! Returns the number of keys in list.
//!
//! \return The number of keys.
//!
int NumberParameter::numKeys () const
{
    return m_keys.size();
}


//!
//! Check for empty list.
//!
//! \return True if list is empty.
//!
bool NumberParameter::isEmpty () const
{
    return m_keys.isEmpty();
}


//!
//! Removes the given key.
//!
//! \param key The pointer to the key to be removed.
//!
void NumberParameter::removeKey ( const Key &key )
{
	QList<Key>::iterator iter = findIndex(key);

	if (iter != m_keys.end())
        m_keys.erase(iter);
    else
        Log::error("Key not in list.", "NumberParameter::removeKey");
}


//!
//! Removes the key at the given time.
//!
//! \param time The time for the key to remove.
//!
void NumberParameter::removeKey ( const float time )
{
    QList<Key>::iterator iter = findIndex(time);

	if (iter->index == time)
        m_keys.erase(iter);
    else
        Log::error("Key not in list.", "NumberParameter::removeKey");
}


//!
//! Removes the key at the given index.
//!
//! \param index The index for the key to remove.
//!
void NumberParameter::removeKey ( const int index )
{
    if (index < m_keys.size()) {
        m_keys.removeAt(index);
    }
    else
        Log::error("List index out of range.", "NumberParameter::removeKey");
}


//!
//! Removes the keys at the given range.
//!
//! \param minTime The start time for the keys to remove.
//! \param maxTime The end time for the keys to remove.
//!
void NumberParameter::removeKeys ( const float minTime, const float maxTime )
{
    QList<Key>::iterator iter_start = findIndex(minTime);
    QList<Key>::iterator iter_end = findIndex(maxTime);

    m_keys.erase(iter_start, iter_end);
}


//!
//! Removes the keys at the given range.
//!
//! \param minIndex The start time for the keys to remove.
//! \param maxIndex The end time for the keys to remove.
//!
void NumberParameter::removeKeys ( const int minIndex, const int maxIndex )
{
    if ((minIndex < m_keys.size()) && (maxIndex < m_keys.size()) && (minIndex <= maxIndex))
        m_keys.erase(m_keys.begin()+minIndex, m_keys.begin()+maxIndex);
    else
        Log::error("Index out of range.", "NumberParameter::removeKeys");
}


//!
//! Removes all keys.
//!
void NumberParameter::clearKeys ( )
{
    m_keys.clear();
}


//!
//! Returns the key at the given time.
//!
//! \param time The time to the key.
//! \return The key at the given time.
//!
const Key &NumberParameter::getKey ( const float time )
{
    QList<Key>::iterator iter = findIndex(time);

    if (iter->index == time)
        return *iter;
    else {
        Log::error("Key not in list.", "NumberParameter::getKey");
        return Key();
    }
}


//!
//! Returns the key at the given index.
//!
//! \param index The index to the Key.
//! \return The key at the given index.
//!
const Key &NumberParameter::getKey ( const unsigned int &index )
{
    if (index < (unsigned int) m_keys.size())
        return m_keys[index];
    else {
        Log::error("List index out of range.", "NumberParameter::getKey");
        return Key();
    }
}


//!
//! Returns a pointer to the list of keys.
//!
//! \return A pointer to the list of keys.
//!
const QList<Key> &NumberParameter::getKeys () const
{
    return m_keys;
}


//!
//! Returns the size of the list of keys.
//!
//! \return The size of the list of keys.
//!
unsigned int NumberParameter::getKeysSize () const
{
    if (m_keys.empty())
        return 0;
    else
        return m_keys.size();
}


//!
//! Returns the value of the key at the given time.
//!
//! \param time The time of the key.
//! \return The value at the given time.
//!
QVariant NumberParameter::getKeyValuePos ( const float time ) 
{
    QList<Key>::iterator iter = findIndex(time);

    if (iter->index == time)
		return iter->keyValue;
    else {
        Log::error("KeyValue not in list.", "NumberParameter::getKeyValue");
        return 0;
    }
}


//!
//! Returns the interpolated value of the key at the given time.
//!
//! \param time The time inbetween the Keys.
//! \return The interpolated value at the given time.
//!
QVariant NumberParameter::getKeyValueTime ( const float time )
{
	return getKeyValueInterpol(time);
}


//!
//! Returns the interpolated value of the key at the given time.
//!
//! \param time The time of the key.
//! \return The interpolated value of the key at the given time.
//!
QVariant NumberParameter::getKeyValueInterpol ( const QVariant &time )
{
	const float pos = time.toFloat() * m_timeStepSize;
	const QList<Key>::iterator iter1 = findIndex(pos);
	const QList<Key>::iterator iter0 = iter1-1;

	if ( iter1 != m_keys.begin() && iter1 != m_keys.end() ) {
		if (m_type == T_Color) {
			switch (iter0->type) {
				// nearest neighbour
			case Key::KT_Step:
				return iter0->keyValue;
				// bezier
			case Key::KT_Bezier: 
				{
					const QColor in0 = iter0->keyValue.value<QColor>();
					const QColor in1 = iter1->keyValue.value<QColor>();

					QColor out;
					out.setRedF(interpolateBezier(
						iter0->index, in0.redF(), iter1->index, in1.redF(),
						iter0->tangentIndex, iter0->tangentValue,
						iter1->tangentIndex, iter1->tangentValue, pos) );
					out.setGreenF(interpolateBezier(
						iter0->index, in0.greenF(), iter1->index, in1.greenF(),
						iter0->tangentIndex, iter0->tangentValue,
						iter1->tangentIndex, iter1->tangentValue, pos) );
					out.setBlueF(interpolateBezier(
						iter0->index, in0.blueF(), iter1->index, in1.blueF(),
						iter0->tangentIndex, iter0->tangentValue,
						iter1->tangentIndex, iter1->tangentValue, pos) );
					out.setAlphaF(interpolateBezier(
						iter0->index, in0.alphaF(), iter1->index, in1.alphaF(),
						iter0->tangentIndex, iter0->tangentValue,
						iter1->tangentIndex, iter1->tangentValue, pos) );
					return QVariant(out);						
				}
				// linear
			default:
				{
					const QColor in0 = iter0->keyValue.value<QColor>();
					const QColor in1 = iter1->keyValue.value<QColor>();

					QColor out;
					out.setRedF(interpolateLinear( iter0->index, in0.redF(),   iter1->index, in1.redF(),   pos));
					out.setGreenF(interpolateLinear( iter0->index, in0.greenF(), iter1->index, in1.greenF(), pos));
					out.setBlueF(interpolateLinear( iter0->index, in0.blueF(),  iter1->index, in1.blueF(),  pos));
					out.setAlphaF(interpolateLinear( iter0->index, in0.alphaF(), iter1->index, in1.alphaF(), pos));
					return QVariant(out);
				}
			}
		}
		else {
			switch (iter0->type) {
				// nearest neighbour
			case Key::KT_Step:
				return iter0->keyValue;
				// bezier
			case Key::KT_Bezier: 
				return QVariant( interpolateBezier(
					iter0->index, iter0->keyValue.toFloat(),
					iter1->index, iter1->keyValue.toFloat(),
					iter0->tangentIndex, iter0->tangentValue,
					iter1->tangentIndex, iter1->tangentValue,
					pos) );
				// linear
			default:
				return QVariant( interpolateLinear( 
					iter0->index, iter0->keyValue.toFloat(),
					iter1->index, iter1->keyValue.toFloat(),
					pos) );
			}
		}
	}
	else if (iter1 == m_keys.end())
		return iter0->keyValue;
	else
		return iter1->keyValue;
}


//!
//! Returns the value of the key at the given index.
//!
//! \param index The index to the Key.
//! \return The value at the given index.
//!
QVariant NumberParameter::getKeyValueIndex ( const unsigned int index ) const
{
    if (index < (unsigned int) m_keys.size())
		return m_keys.at(index).keyValue;
    else {
        Log::error("List index out of range.", "NumberParameter::getKey");
        return 0;
    }
}


//!
//! Returns the time position of the last key.
//!
//! \return The time position of the last key.
//!
float NumberParameter::getLastKeyPos () const
{
	if (m_keys.empty())
		return 0;
	else
		return m_keys.last().index;
}


//!
//! Returns the parameter's value while optionally triggering the
//! evaluation chain.
//!
//! \param Trigger evaluation.
//! \param triggerEvaluation Flag to control whether to trigger the evaluation chain.
//! \return The parameter's value.
//!
QVariant NumberParameter::getValue ( const bool triggerEvaluation /* = false */ )
{
    // optionally trigger the evaluation chain
    if (triggerEvaluation) {
        Parameter::PinType pinType = getPinType();
        if (pinType == Parameter::PT_Output || pinType == Parameter::PT_Input)
            propagateEvaluation();
    }

    return m_value;
}


///
/// Protected Functions
///


//!
//! Enables timeline affection;
//!
void NumberParameter::enableTimelineAffection()
{
	if (!m_timeAffected && m_node) {
		m_node->getTimeParameter()->addAffectedParameter(this);
		m_timeAffected = true;
	}
}


} // end namespace Frapper