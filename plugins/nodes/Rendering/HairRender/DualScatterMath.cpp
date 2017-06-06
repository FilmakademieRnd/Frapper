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
//! \file "DualScatterMath.cpp"
//! \brief Implementation file for DualScatterMath class.
//!
//! \author     Simon Spielmann <sspielma@filmakademie.de>
//! \version    1.0
//! \date       07.02.2011 (last updated)
//!

#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>
#include "DualScatterMath.h"

#define INTEGRAL_PRECISION 512

namespace HairRenderNode {

DualScatterMath::DualScatterMath(const unsigned int tableSize /*= 128*/) :
	m_tableSize(tableSize),
	m_eta(1.55f),
	m_sigma_a(2.f),
	m_a(0.89f),
	m_alpha_R(GToR(-8.f)),
	m_alpha_TT(-0.5f*m_alpha_R),
	m_alpha_TRT(-1.5f*m_alpha_R),
	m_beta_R(GToR(8.f)),
	m_beta_TT(0.5f*m_beta_R),
	m_beta_TRT(2.f*m_beta_R),
	m_k_g(2.f),
	m_w_c(GToR(15.f)),
	m_dEta(0.3f),
	m_dh_M(0.5),
	m_C_R(1.f, 0.f, 0.f),
	m_C_TT(0.f, 1.f, 0.f),
	m_C_TRT(0.f, 0.f, 1.f),
	m_I_R(1.f),
	m_I_TT(1.f),
	m_I_TRT(1.f),
	m_I_G(1.f)
{
	m_A_b = (Ogre::ColourValue*) malloc(m_tableSize*sizeof(Ogre::ColourValue));
	m_delta_b = (Ogre::ColourValue*) malloc(m_tableSize*sizeof(Ogre::ColourValue));
	m_sigma_b = (Ogre::ColourValue*) malloc(m_tableSize*sizeof(Ogre::ColourValue));
	m_N_gR = (float*) malloc(m_tableSize*sizeof(float));
	m_N_gTT = (float*) malloc(m_tableSize*sizeof(float));
	m_N_gTRT = (float*) malloc(m_tableSize*sizeof(float));
}

DualScatterMath::~DualScatterMath()
{
	free(m_A_b);
	free(m_delta_b);
	free(m_sigma_b);
	free(m_N_gR);
	free(m_N_gTT);
	free(m_N_gTRT); 
}

unsigned int DualScatterMath::getTablesize() const
{
	return m_tableSize;
}

inline float DualScatterMath::GToR(const float grad)
{
	return grad/180.f*M_PI;
}

float DualScatterMath::g(float x, float variance)
{
	return exp(-x*x/(2.f*variance))/sqrt(2.f*M_PI*variance);
}

//!
//! Computes the integral using the Simpson rule.
//!
//!
//! \param f The pointer to the function f(x, c).
//! \param a The intervall start value
//! \param b The intervall end value
//!
inline float DualScatterMath::S(funcPtr f, const float a, const float b, int n, const float c)
{
	float s, dx, x;
	
	// if n is odd - add +1 interval to make it even
    if ((n/2)*2 != n) n = n+1;
    s = 0.f;
    dx = (b-a)/static_cast<float>(n);
    
	for (unsigned int i=2; i<=n-1; i=i+2)
    {
        x = a + static_cast<float>(i)*dx;
        s = s + 2.f*(this->*f)(x, c) + 4.f*(this->*f)(x + dx, c);
    }

    s = (s + (this->*f)(a, c) + (this->*f)(b, c) + 4.f*(this->*f)(a + dx, c) )*dx/3.f;
    
	return s;
}

inline float DualScatterMath::M_X(const float alpha, const float beta, const float theta_h)
{ 
	return g(theta_h - alpha, beta*beta);
}

inline float DualScatterMath::SM_X(const float alpha, const float beta, const float a, const float b)
{
	return S(&DualScatterMath::g, a-alpha, b-alpha, INTEGRAL_PRECISION, beta*beta);
}

inline float DualScatterMath::N_R(const float phi, float empty)
{
	return cos(phi/2.f);
}

inline float DualScatterMath::N_TT(const float phi, const float gamma)
{
	return g(M_PI - phi, gamma*gamma);
}

inline float DualScatterMath::N_TRT(const float phi, const float gamma)
{
	// G = random angle between 30° und 45° in 0.1° steps
	const float G = GToR(static_cast<float>(rand() % 150 + 300)/10.f);

	return N_R(phi) + m_I_G*g(G - phi, gamma*gamma);
}

// integrals

inline Ogre::ColourValue DualScatterMath::f_R(const float theta, const float I, const float phi, Ogre::ColourValue &colour)
{
	return I*colour*N_R(phi)*M_X(m_alpha_R, m_beta_R, theta);
}

inline Ogre::ColourValue DualScatterMath::f_TT(const float theta, const float I, const float phi, Ogre::ColourValue &colour)
{
	return I*colour*N_TT(phi, m_gamma_TT)*M_X(m_alpha_TT, m_beta_TT, theta);
}

inline Ogre::ColourValue DualScatterMath::f_TRT(const float theta, const float I, const float phi, Ogre::ColourValue &colour)
{
	return I*colour*N_TRT(phi, m_gamma_G)*M_X(m_alpha_TRT, m_beta_TRT, theta);
}

inline Ogre::ColourValue DualScatterMath::f_s(const float theta, const float phi, const float I_R, const float I_TT, const float I_TRT, const float I_G, Ogre::ColourValue &colour_R, Ogre::ColourValue &colour_TT, Ogre::ColourValue &colour_TRT)
{
	return (
		f_R(theta, I_R, phi, colour_R) +
		f_TT(theta, I_TT, phi, colour_TT) +
		f_TRT(theta, I_TRT, phi, colour_TRT) ) / cos(phi);
}

inline Ogre::ColourValue DualScatterMath::alpha_fb(const float phi, const float from, const float to)
{
	const Ogre::ColourValue SfR   = SM_X(m_alpha_R, m_beta_R, from, to) * m_C_R * m_I_R * N_R(phi);
	const Ogre::ColourValue SfTT  = SM_X(m_alpha_TT, m_beta_TT, from, to) * m_C_TT * m_I_TT * N_TT(phi, m_gamma_TT);
	const Ogre::ColourValue SfTRT = SM_X(m_alpha_TRT, m_beta_TRT, from, to) * m_C_TRT * m_I_TRT * N_TRT(phi, m_gamma_G);

	return (SfR*m_alpha_R + SfTT*m_alpha_TT + SfTT*m_alpha_TT) / (SfR + SfTT + SfTRT);
}

inline Ogre::ColourValue DualScatterMath::beta_fb(const float phi, const float from, const float to)
{
	const Ogre::ColourValue SfR   = SM_X(m_alpha_R, m_beta_R, from, to) * m_C_R * m_I_R * N_R(phi);
	const Ogre::ColourValue SfTT  = SM_X(m_alpha_TT, m_beta_TT, from, to) * m_C_TT * m_I_TT * N_TT(phi, m_gamma_TT);
	const Ogre::ColourValue SfTRT = SM_X(m_alpha_TRT, m_beta_TRT, from, to) * m_C_TRT * m_I_TRT * N_TRT(phi, m_gamma_G);

	return (SfR*m_beta_R + SfTT*m_beta_TT + SfTT*m_beta_TT) / (SfR + SfTT + SfTRT);
}

inline Ogre::ColourValue DualScatterMath::SX_f_s(const float theta, const float from, const float to)
{
	const float G = GToR(static_cast<float>(rand() % 150 + 300)/10.f);

	const float SN_R   =				S(&DualScatterMath::N_R,      from,        to, INTEGRAL_PRECISION, 0.f);
	const float SN_TT  =				S(&DualScatterMath::g, M_PI - from, M_PI - to, INTEGRAL_PRECISION, m_gamma_TT*m_gamma_TT);
	const float SN_TRT = SN_R + m_I_G * S(&DualScatterMath::g,    G - from,    G - to, INTEGRAL_PRECISION, m_gamma_G*m_gamma_G);

	const Ogre::ColourValue fs_R   = SN_R   * m_C_R   * m_I_R   * M_X(m_alpha_R, m_beta_R, theta);
	const Ogre::ColourValue fs_TT  = SN_TT  * m_C_TT  * m_I_TT  * M_X(m_alpha_TT, m_beta_TT, theta);
	const Ogre::ColourValue fs_TRT = SN_TRT * m_C_TRT * m_I_TRT * M_X(m_alpha_TRT, m_beta_TRT, theta);

	const float cosT = cos(theta);

	return (fs_R + fs_TT + fs_TRT)/(cosT*cosT);
}

inline Ogre::ColourValue DualScatterMath::a_fb(const float theta, const float from, const float to)
{
	const Ogre::ColourValue SIn = SX_f_s(theta, -M_PI_2, M_PI_2);
	
	// investigate the three components!!
	return (
		SX_f_s(SIn.r, from, to) +
		SX_f_s(SIn.g, from, to) +
		SX_f_s(SIn.b, from, to) ) * (cos(theta)/M_PI);
}


inline Ogre::ColourValue DualScatterMath::A_b(const float theta)
{
	// investigate what are the values of omega f and omega b??
	const float omega_f = 1.f;
	const float omega_b = 1.f;

	const Ogre::ColourValue a_f = a_fb(theta, 0.f, omega_f);
	const Ogre::ColourValue a_b = a_fb(theta, 0.f, omega_b);
	const Ogre::ColourValue a_f2 = a_f*a_f;
	const Ogre::ColourValue a_b3 = a_b*a_b*a_b;
	const Ogre::ColourValue n = (Ogre::ColourValue(1.f, 1.f, 1.f, 1.f) - a_f2); 

	return (a_b*a_f2)/n + (a_b3*a_f2)/(n*n);
}

inline Ogre::ColourValue DualScatterMath::delta_b(const float theta)
{
	// investigate the values of omega f and omega b??
	const float omega_f = 1.f;
	const float omega_b = 1.f;

	const Ogre::ColourValue a_f = a_fb(theta, 0.f, omega_f);
	const Ogre::ColourValue a_b = a_fb(theta, 0.f, omega_b);
	const Ogre::ColourValue a_f2 = a_f*a_f;
	const Ogre::ColourValue a_b2 = a_b*a_b;
	const Ogre::ColourValue n = Ogre::ColourValue(1.f,1.f,1.f,1.f) - a_f2;

	// investigate integral bounds!!
	const float alpha_fFrom = 0.f;
	const float alpha_fTo = M_PI;
	const float alpha_bFrom = M_PI;
	const float alpha_bTo = M_PI*2.f;

	Ogre::ColourValue alpha_f;// = alpha_fb(Ogre::ColourValue(1.f,1.f,1.f,1.f) - (2.f*a_b2)/(n*n), alpha_bFrom, alpha_bTo);
	Ogre::ColourValue alpha_b;// = alpha_fb((2.f*n*n + 4.f*a_f2*a_b2)/(n*n*n), alpha_fFrom, alpha_fTo);
	
	return alpha_b + alpha_f;
}

inline Ogre::ColourValue DualScatterMath::sigma_b(const float theta)
{
	return Ogre::ColourValue();
}

float* DualScatterMath::fillN_gR()
{
	const float n = 2.f/M_PI;
	for (unsigned int i=0; i<m_tableSize; ++i)
		m_N_gR[i] = S(&DualScatterMath::N_R, M_PI_2, M_PI, INTEGRAL_PRECISION, 0.f)*n;
	return m_N_gR;
}

float* DualScatterMath::fillN_gTT()
{
	const float n = 2.f/M_PI;
	const float gammaTTSquare = m_gamma_TT*m_gamma_TT;
	for (unsigned int i=0; i<m_tableSize; ++i)
		m_N_gTT[i] = S(&DualScatterMath::g, M_PI_2, 0.f, INTEGRAL_PRECISION, gammaTTSquare)*n;
	return m_N_gTT;
}

float* DualScatterMath::fillN_gTRT()
{
	const float n = 2.f/M_PI;
	// investigate local G or const global G?
	const float G = GToR(static_cast<float>(rand() % 150 + 300)/10.f);

	const float gammaGSquare = m_gamma_G*m_gamma_G;
	for (unsigned int i=0; i<m_tableSize; ++i)
		m_N_gTRT[i] = S(&DualScatterMath::g, G - M_PI_2, G - M_PI, INTEGRAL_PRECISION, gammaGSquare)*n;
	return m_N_gTT;
}

Ogre::ColourValue* DualScatterMath::fill_A_b(const float from, const float to)
{
	float theta = 0.f;
	const float stepsize = (to - from)/m_tableSize;
	for (unsigned int i=0; i<m_tableSize; ++i, theta+=stepsize)
		m_A_b[i] = A_b(theta);
	return m_A_b;
}

Ogre::ColourValue* DualScatterMath::fill_delta_b(const float from, const float to)
{
	float theta = 0.f;
	const float stepsize = (to - from)/m_tableSize;
	for (unsigned int i=0; i<m_tableSize; ++i, theta+=stepsize)
		m_delta_b[i] = delta_b(theta);
	return m_delta_b;
}

Ogre::ColourValue* DualScatterMath::fill_sigma_b(const float from, const float to)
{
	float theta = 0.f;
	const float stepsize = (to - from)/m_tableSize;
	for (unsigned int i=0; i<m_tableSize; ++i, theta+=stepsize)
		m_sigma_b[i] = sigma_b(theta);
	return m_sigma_b;
}

} // namespace HairRenderNode 
