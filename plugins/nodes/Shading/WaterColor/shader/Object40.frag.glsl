//Author: Soeren Pirk
//Date  : 19.11.2010

#version 400 core


uniform vec3 lightPos;
uniform vec3 camPos;


in vec4 VertColor;
in vec4 VertPosition;
in vec3 VertNormal;

void main()
{
	vec3 N = normalize(VertNormal);
    vec3 L = normalize(lightPos - VertPosition.xyz);
	vec3 V = normalize(camPos - VertPosition.xyz);
	float NdotV = max(0.0, dot(N, V)); 
	
	float d = max(0.0, dot(N, L));

	gl_FragData[0] = vec4(0, 0, 1.0, 1.0); // ID
	gl_FragData[1] = vec4(d,d,d,1);	// Lightning
	gl_FragData[2] = vec4(1,1,1,1);	// Shadow
	gl_FragData[3] = vec4(NdotV,NdotV,NdotV,1); // Silhouette	
}
