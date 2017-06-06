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

#version 400

//#define DEBUG_COLOR

// in
#ifdef DEBUG_COLOR
in vec4 debugColor;
#endif
in vec3 gs_tangent;
in vec3 gs_viewDir;
in float gs_strandPosition;

//out
out vec4 fragColor;

// parameter
uniform vec4 light_colors[96];
uniform float light_powers[96];
uniform vec4 light_directions[96];

uniform vec3 rootColor;
uniform vec3 tipColor;
uniform float reflectWidth;
uniform float scatterWidth;
uniform float transmitWidth;
uniform float lightPower;
uniform float lightNumber;
uniform float colorGradientWidth;
uniform float colorGradientShift;

uniform vec3 _AM_c_R;
uniform vec3 _AM_c_TT;
uniform vec3 _AM_c_TRT;
uniform vec3 _AM_d;

uniform float _AM_s_R;
uniform float _AM_s_TT;
uniform float _AM_s_TRT;


void main()
{
	fragColor = vec4(0.0, 0.0, 0.0, 1.0);

	vec3 newViewDir = normalize(gs_viewDir);
	vec3 newTangent = normalize(gs_tangent);
	float viewDepSpec = dot(newViewDir, newTangent);

	float lightVec, theta_h, theta_h_, asinLightVec, cosPhi_i;
	vec3 reflectionColor, transRefTransColor, transTransColor, diffColor, lightDir;

	float strandPosition = clamp(abs((gs_strandPosition-colorGradientShift)*colorGradientWidth), 0.0, 1.0);

	for (int i=0; i<lightNumber; i++) 
	{
		lightDir = light_directions[i].xyz;
		lightVec = dot(lightDir, newTangent);
		asinLightVec = asin(lightVec);
		theta_h = asinLightVec + asin(viewDepSpec);
		theta_h_ = asinLightVec + asin(-viewDepSpec); // for transmittion component
		cosPhi_i = max(dot(normalize(lightDir - lightVec * newTangent), normalize(newViewDir - viewDepSpec * newTangent)), 0.0);
	
		// R (reflection) component 
		reflectionColor = clamp(_AM_c_R * cosPhi_i * pow( cos(theta_h - _AM_s_R) , reflectWidth ), 0.0, 1.0); 
		
		// TRT component
		transRefTransColor = clamp(_AM_c_TRT * cosPhi_i * pow( cos(theta_h - _AM_s_TRT) , scatterWidth ), 0.0, 1.0); 
	
		// TT component
		transTransColor = clamp(_AM_c_TT * (1.0 - cosPhi_i) * pow( cos(theta_h_ - _AM_s_TT) , transmitWidth ), 0.0, 1.0);

		// diffuse component
		diffColor = clamp(_AM_d * sqrt(min(1.0, lightVec * lightVec)) + (strandPosition * tipColor + (1.0 - strandPosition) * rootColor), 0.0, 1.0);
		
		fragColor.xyz += (diffColor + reflectionColor + transTransColor + transRefTransColor) * light_colors[i].xyz * light_powers[i];
	}
	fragColor *= lightPower;
	
	// clamp result between 0.0 and 1.0
	//clamp(gs_strandPosition, 0.0, 1.0 );
	
#ifdef DEBUG_COLOR
	fragColor = debugColor;
#endif
}