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
//! \file "DualScatterMath.h"
//! \brief Header file for DualScatterMath class.
//!
//! \author     Simon Spielmann <sspielma@filmakademie.de>
//! \version    1.0
//! \date       07.02.2011 (last updated)
//!

#ifndef DUALSCATTERMATH_H
#define DUALSCATTERMATH_H

// OGRE
#include <Ogre.h>
#if (OGRE_PLATFORM  == OGRE_PLATFORM_WIN32)
#include <windows.h>
#endif

namespace HairRenderNode {

class DualScatterMath
{
public:
	DualScatterMath(const unsigned int tableSize = 128);
	~DualScatterMath();

public:
	unsigned int getTablesize() const;

private:
	typedef float (DualScatterMath::*funcPtr) (float, float);

private:
	inline float GToR(const float grad);
	
	float g(float x, float variance);

	//!
	//! Computes the integral using the Simpson rule.
	//!
	//!
	//! \param f The pointer to the function f(x).
	//! \param a The intervall start value
	//! \param b The intervall end value
	//!
	inline float S(funcPtr f, const float a, const float b, int n, const float c);

	inline float M_X(const float alpha, const float beta, const float theta);

	inline float SM_X(const float alpha, const float beta, const float a, const float b);

	inline float N_R(const float phi, float empty = 0);

	inline float N_TT(const float phi, const float gamma);

	inline float N_TRT(const float phi, const float gamma);

	inline Ogre::ColourValue f_R(const float theta, const float I, const float phi, Ogre::ColourValue &colour);

	inline Ogre::ColourValue f_TT(const float theta, const float I, const float phi, Ogre::ColourValue &colour);

	inline Ogre::ColourValue f_TRT(const float theta, const float I, const float phi, Ogre::ColourValue &colour);

	inline Ogre::ColourValue DualScatterMath::f_s(const float theta, const float phi, const float I_R, const float I_TT, const float I_TRT, const float I_G, Ogre::ColourValue &colour_R, Ogre::ColourValue &colour_TT, Ogre::ColourValue &colour_TRT);

	inline Ogre::ColourValue alpha_fb(const float phi, const float from, const float to);

	inline Ogre::ColourValue beta_fb(const float phi, const float from, const float to);

	inline Ogre::ColourValue SX_f_s(const float theta, const float from, const float to);

	inline Ogre::ColourValue a_fb(const float theta, const float from, const float to);

	inline Ogre::ColourValue A_b(const float theta);

	inline Ogre::ColourValue delta_b(const float theta);

	inline Ogre::ColourValue sigma_b(const float theta);

	float* fillN_gR();
	float* fillN_gTT();
	float* fillN_gTRT();

	Ogre::ColourValue* fill_A_b(const float from, const float to);
	Ogre::ColourValue* fill_delta_b(const float from, const float to);
	Ogre::ColourValue* fill_sigma_b(const float from, const float to);

private:
	// hair fibre
	float m_eta;
	float m_sigma_a;
	float m_a;
	
	// hair colors and intensities
	Ogre::ColourValue m_C_R;
	Ogre::ColourValue m_C_TT;
	Ogre::ColourValue m_C_TRT;
	float m_I_R;
	float m_I_TT;
	float m_I_TRT;
	float m_I_G;
	
	// surface
	float m_alpha_R;
	float m_alpha_TT;
	float m_alpha_TRT;
	float m_beta_R;
	float m_beta_TT;
	float m_beta_TRT;
	float m_gamma_TT;
	float m_gamma_G;

	// highlights
	float m_k_g;
	float m_w_c;
	float m_dEta;
	float m_dh_M;


	Ogre::ColourValue *m_A_b;
	Ogre::ColourValue *m_delta_b;
	Ogre::ColourValue *m_sigma_b;
	float *m_N_gR;
	float *m_N_gTT;
	float *m_N_gTRT;
	unsigned int m_tableSize;
};

} // namespace HairRenderNode 

#endif
