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

#version 400 core

//#define LIGHTING


////////////////////
// add. functions //
////////////////////

vec2 btex2D_rg(sampler2D map, vec2 uv, float radius, vec2 offset, float size) 
{
	// this is sometimes too slow and long
	// (3 * 2 + 1) ^ 2 =  7 ^ 2 = 49 samples
	vec2 o = offset * size;
	vec2 sample_vector = vec2(0.0, 0.0);
	int radiusInt = int(radius);
	for (int x = -radiusInt; x <= radiusInt; x++) {
		for (int y = -radiusInt; y <= radiusInt; y++) {
			sample_vector += texture(map, vec2(uv.x + x * o.x, uv.y + y * o.y)).ra;
		}
	}
	return sample_vector / ((radius * 2 + 1) * (radius * 2 + 1));

	// simple 3x3 filter
	//float2 o = offset * size;
	//float2 c = tex2D(map, uv.xy).rg; // center
	//c += tex2D(map, uv.xy - o.xy).rg; // top left
	//c += tex2D(map, uv.xy + o.xy).rg; // bottom right
	//c += tex2D(map, float2(uv.x - o.x, uv.y)).rg; // left
	//c += tex2D(map, float2(uv.x + o.x, uv.y)).rg; // right
	//c += tex2D(map, float2(uv.x, uv.y + o.y)).rg; // bottom
	//c += tex2D(map, float2(uv.x, uv.y - o.y)).rg; // top
	//c += tex2D(map, float2(uv.x - o.x, uv.y + o.y)).rg; // bottom left
	//c += tex2D(map, float2(uv.x + o.x, uv.y - o.y)).rg; // top right
	//return c / 9;
}

float shadow(sampler2D shadowMap, vec4 shadowMapPos, float radius, float vsmEpsilon, vec2 offset, float size) 
{
	vec2 suv = shadowMapPos.xy / shadowMapPos.w;

	// sharp texture read
	//vec2 moments = texture(shadowMap, suv).rg;

	// blurred texture read
	vec2 moments = btex2D_rg(shadowMap, suv, radius, offset, size);

	float litFactor = (shadowMapPos.z <= moments.x ? 1 : 0);

	// standard variance shadow mapping code
	//static const float vsmEpsilon = 0.0001;
	float E_x2 = moments.y;
	float Ex_2 = moments.x * moments.x;
	float variance = min(max(E_x2 - Ex_2, 0.0) + vsmEpsilon, 1.0);
	float m_d = moments.x - shadowMapPos.z;
	float p = variance / (variance + m_d * m_d);

	return smoothstep(0.4, 1, max(litFactor, p));
}


in vec4 gs_WP;	//in float4 iWP : TEXCOORD0,
in vec3 gs_SDir;	//in float3 iSDir : TEXCOORD1,
in vec4 gs_LP;	//in float4 iLP : TEXCOORD2,
#ifdef LIGHTING
in vec3 vsN;	//in float3 iN : TEXCOORD3,
#endif

out float fragColor;		//out half oCol : COLOR,

uniform vec3 lightDif0;
uniform vec4 lightPos0;
uniform vec4 lightAtt0;
uniform vec4 invSMSize;
uniform vec4 spotlightParams;
uniform float vsmEpsilon;
uniform float sampleRadius;
uniform float blurSize;
uniform float lightPower;
uniform sampler2D shadowMap;

void main()
{

	// direction
	vec3 ld0 = normalize(lightPos0.xyz - (lightPos0.w * gs_WP.xyz));

	// attenuation
	//float ila = length(lightPos0.xyz - gs_WP.xyz) / lightAtt0.r;
	float la = 1.0;// - ila * ila;  // quadratic falloff

	// calculate the spotlight effect
	float spot = dot(ld0, normalize(-gs_SDir)); // angle between spotlight dir and actual dir
	spot = clamp((spot - spotlightParams.y) / (spotlightParams.x - spotlightParams.y), 0.0, 1.0);

#ifdef LIGHTING
	float LdotN0 = max(dot(ld0, normalize(vsN)), 0).x;
	float light = la * spot * LdotN0;
#else
	float light = la * spot;
#endif

	if (light == 0) {
		fragColor = 0.0;
	}
	else {
		float outputColor = 
			lightDif0.x * 
			light *
			lightPower *
			shadow(
			// pass in the shadow map
			shadowMap,
			// the calculated shadow position in the shadow map
			gs_LP,
			// radius to blur 
			sampleRadius,
			// vsm epilon
			vsmEpsilon/100,
			// inverse shadow map size so we know how much to move when blurring
			invSMSize.xy,
			// multiplier for blurring
			blurSize);
		fragColor = outputColor;
	}
	
	//fragColor = vec2(0.0, 1.0);
}