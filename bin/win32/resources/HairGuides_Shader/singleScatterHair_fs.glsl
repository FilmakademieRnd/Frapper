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
in vec4 gs_worldPos;   
in float gs_strandPosition;

//out
out vec4 fragColor;

// parameter
uniform vec4 light_colors[16];
uniform vec4 light_directions[16]; 

uniform vec3 rootColor;
uniform vec3 tipColor;
uniform float reflectWidth;
uniform float scatterWidth;
uniform float transmitWidth;
uniform float lightPower;
uniform float lightScale;
uniform float lightNumber;

uniform vec3 _AM_c_R;
uniform vec3 _AM_c_TT;
uniform vec3 _AM_c_TRT;
uniform vec3 _AM_d;

uniform float _AM_s_R;
uniform float _AM_s_TT;
uniform float _AM_s_TRT;

//uniform sampler1D	LightDirs; 		// array of light directions in camera space (lx,ly,lz)
//uniform sampler1D	LightPows; 		// array of light colors - rgb intensities (r,g,b)


void main()
{
	fragColor = vec4(0, 0, 0, 1);
	
	float lightVec, viewDepSpec, theta_h, cosPhi_i;
	vec3 reflectionColor, transRefTransColor, transTransColor, diffColor, lightDir;
	
	for (int i=0; i<lightNumber; i++) 
	{
		//lightDir = texture(LightDirs, i).xyz;
		//lightDir = normalize( vec4(texture(LightDirs, i).xyz, 1.0f) ).xyz;
		lightDir = light_directions[i].xyz;
		lightVec = dot(lightDir, gs_tangent);
		viewDepSpec = dot(gs_viewDir, gs_tangent);
		
		theta_h = asin(lightVec) + asin(viewDepSpec);
		cosPhi_i = dot(normalize(lightDir - lightVec * gs_tangent), normalize(gs_viewDir - viewDepSpec * gs_tangent));
	
		// R (reflection) component 
		reflectionColor = clamp(_AM_c_R * pow( abs(cos(theta_h - _AM_s_R)) , reflectWidth ), 0.0, 1.0); 
		
		// TRT component
		transRefTransColor = clamp(_AM_c_TRT * pow( abs(cos(theta_h - _AM_s_TRT)) , scatterWidth ), 0.0, 1.0); 
	
		// TT component
		transTransColor = clamp(_AM_c_TT * max(0.0, cosPhi_i) * pow( cos(theta_h - _AM_s_TT) , transmitWidth ), 0.0, 1.0);

		// diffuse component
		diffColor = clamp(_AM_d * sqrt(min(1.0, lightVec*lightVec)) + (gs_strandPosition * tipColor + gs_strandPosition * rootColor), 0.0, 1.0);
		
		fragColor.xyz +=	(diffColor + reflectionColor + transTransColor + transRefTransColor) * (light_colors[i].xyz / lightScale) * lightPower;
	}
	
	// clamp result between 0.0 and 1.0
	clamp(fragColor, 0.0, 1.0 );
	
	fragColor.a = 1.0;
	
	// Debug
	//fragColor.xyz =  gs_tangent;
#ifdef DEBUG_COLOR
	fragColor = debugColor;
#endif
}