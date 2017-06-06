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


/**************************************************************************************
**                                                                                   **
**                                      ssao_ps                                      **
**                                                                                   **
***************************************************************************************/
#version 400 core


int NUM_BASE_SAMPLES = 14;

vec3 RAND_SAMPLES[14] = vec3[14]
(
	vec3( 1.0,  0.0,  0.0),
	vec3(-1.0,  0.0,  0.0),
	vec3( 0.0,  1.0,  0.0),
	vec3( 0.0, -1.0,  0.0),
	vec3( 0.0,  0.0,  1.0),
	vec3( 0.0,  0.0, -1.0),
	normalize(vec3( 1.0,  1.0,  1.0)),
	normalize(vec3(-1.0,  1.0,  1.0)),
	normalize(vec3( 1.0, -1.0,  1.0)),
	normalize(vec3( 1.0,  1.0, -1.0)),
	normalize(vec3(-1.0, -1.0,  1.0)),
	normalize(vec3(-1.0,  1.0, -1.0)),
	normalize(vec3( 1.0, -1.0, -1.0)),
	normalize(vec3(-1.0, -1.0, -1.0))
);


in  vec2 vsUv0;
in  vec3 vsRay;
in  vec2 vsRandUV;

out vec4 color;

uniform float farPlane;
uniform float radius;
uniform float contrast;
uniform mat4 ptMat;

uniform sampler2D inputMap0;
uniform sampler2D inputMap1;
uniform sampler2D randMap;


void main()
{	
    // random normal lookup from a texture and expand to [-1..1]
	// randMap = 1024 * 1024
	// mit .xyz * 2.0 - 1.0 werden die komponenten des Vektors auch einen Wertebereich zwischen -1 und 1 skaliert 
    vec3 randN = normalize(texture(randMap, vsRandUV).xyz); // float2(texCoord.x * 24, texCoord.y * 10)).xyz * 2.0 - 1.0;
    float depth = texture(inputMap1, vsUv0).x;
	
    // "ray" will be distorted slightly due to interpolation
    // it should be normalized here
    vec3 viewPos = vsRay * depth;

    // by computing Z manually, we lose some accuracy under extreme angles
    // considering this is just for bias, this loss is acceptable
    vec3 viewNorm = texture(inputMap0, vsUv0).xyz;	
	
	// accumulated occlusion factor
    float occ = 0.0;
    for (int i = 0; i < NUM_BASE_SAMPLES; i++)
    {
        // reflected direction to move in for the sphere
        // (based on random samples and a random texture sample)
        // bias the random direction away from the normal
        // this tends to minimize self occlusion
        vec3 randomDir = reflect(RAND_SAMPLES[i], randN) + viewNorm;

        // move new view-space position back into texture space
        // #define RADIUS 0.2125
        vec4 nuv = ptMat * vec4(viewPos.xyz + randomDir * radius, 1.0);
        nuv.xy /= nuv.w;

        // compute occlusion based on the (scaled) Z difference
        float zd = clamp(farPlane * (depth - texture(inputMap1, nuv.xy).x), 0.0, 1.0);
        
		// this is a sample occlusion function, you can always play with
        // other ones, like 1.0 / (1.0 + zd * zd) and stuff
        occ += clamp(pow(1.0 - zd, contrast) + zd, 0.0, 1.0);
        //occ += 1.0 / (1.0 + zd*zd);
    }

	color = vec4(vec3(occ/NUM_BASE_SAMPLES), 1.0);
}