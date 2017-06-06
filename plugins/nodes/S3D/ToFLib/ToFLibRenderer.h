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
//! \file "ToFLibRenderer.h"
//! \brief Header file for ToFLibRenderer class.
//!
//! \author		Michael Bussler <michael.bussler@filmakademie.de>
//! \author     Simon Spielmann <simon.spielmann@filmakademie.de>
//! \version	1.0
//! \date		26.03.2014 (last updated)
//!
//!
#ifndef ToFLibRenderer_H
#define ToFLibRenderer_H

#include "OgreRenderQueueListener.h"

#include "common/fusion_interface.hxx"
#include "ToFLibNode.h"
#include <GL/GL.h>


namespace ToFLibNode {

	using namespace Ogre;

	//!
	//! Class representing ToFLib
	//!
	class ToFLibRenderer : public RenderQueueListener
	{

	public: /// constructors and destructors

		//!
		//! Constructor of the ToFLibRenderer class.
		//!
		//! \param name The name to give the new mesh node.
		//! \param parameterRoot The main Parameter Group of this Node
		//!
		ToFLibRenderer () :
			m_algo(NULL),
			m_leftdata(NULL),
			m_rightdata(NULL),
			m_tofdata(NULL),
			m_outdata(NULL)
		{};

		//!
		//! Destructor of the ToFLibRenderer class.
		//!
		//! Defined virtual to guarantee that the destructor of a derived class
		//! will be called if the instance of the derived class is saved in a
		//! variable of its parent class type.
		//!
		virtual ~ToFLibRenderer ()
		{};

		void setImageData( const unsigned char* left, const unsigned char* right, float* tofdata, float* outdata )
		{
			m_leftdata = left;
			m_rightdata = right;
			m_tofdata = tofdata;
			m_outdata = outdata;
		};

		void setAlgo( oxo::FusionAlgorithm<>* algo ) 
		{
			m_algo = algo;
		}

	protected:

		virtual void renderQueueStarted(uint8 queueGroupId, const String& invocation, 
			bool& skipThisInvocation) { }
		virtual void renderQueueEnded(uint8 queueGroupId, const String& invocation, 
			bool& repeatThisInvocation)
		{
			// Set wanted render queue here - make sure there are - make sure that something is on
			// this queue - else you will never pass this if.
			if (queueGroupId != RENDER_QUEUE_MAIN) 
				return;

			//// save attribs
			glPushAttrib(GL_ALL_ATTRIB_BITS);

			//// call native rendering function
			if(m_algo){
				m_algo->compute(m_leftdata, m_rightdata, m_tofdata, m_outdata);
				delete m_algo;
			}

			//// restore original state
			glPopAttrib();
		}

	private: //data

	protected:

		const unsigned char* m_leftdata;
		const unsigned char* m_rightdata;
		float* m_tofdata;
		float* m_outdata;

		oxo::FusionAlgorithm<>* m_algo;
	};


} // namespace ToFLibRenderer

#endif
