#version 400 core
#extension GL_EXT_geometry_shader4 : enable

layout(points, invocations = 2) in;
layout(triangle_strip, max_vertices = 64) out;

in vec4 vertPosition[];
out vec2 geomTexture;

uniform sampler2D inputMap;
uniform float seedingThreshold;
uniform int numberSteps;
uniform int stepSize;
uniform int minSteps;
uniform float strokeContourCurvature;
uniform int strokeThickness;

vec2 tex(vec2 texCoord)
{
	vec2 v = texture2D(inputMap, texCoord).xy;
	vec2 res = vec2(-v.y, v.x);
	return res;
}

vec2 getGradient(vec4 pos, float ux, float uy)
{
	vec2 texCoord = pos.xy / pos.w * 0.5 + vec2(0.5);
	
	vec2 v1 = tex(texCoord).xy;
	vec2 d1 = stepSize * normalize(v1);
	d1 = vec2(d1.x * ux, d1.y * uy);
	
	vec2 v2 = tex(texCoord + 0.5 * d1).xy;
	vec2 d2 = stepSize * normalize(v2);
	d2 = vec2(d2.x * ux, d2.y * uy);
	
	vec2 v3 = tex(texCoord + 0.5 * d2).xy;
	vec2 d3 = stepSize * normalize(v3);
	d3 = vec2(d3.x * ux, d3.y * uy);
	
	vec2 v4 = tex(texCoord + d3);
	return (1.0f / 6.0f) * (v1 + 2.0 * v2 + 2.0 * v3 + v4);
}

void emit(vec4 pos, vec2 tex)
{
	gl_Position = pos;
	geomTexture = tex;
	EmitVertex();
}

void main()
{
	int MAX_VERTS = 32;
	if(gl_MaxGeometryOutputVertices < MAX_VERTS)
		MAX_VERTS = gl_MaxGeometryOutputVertices; // check for lower hardware limits

	ivec2 size = textureSize(inputMap, 0);
	float ux = 1.0f / float(size.x);
	float uy = 1.0f / float(size.y);
	
	int iterations = min(MAX_VERTS, numberSteps);
	vec4 candidatePositionsFwd[32];
	vec4 candidatePositionsBwd[32];
	int lineLengthFwd = 0;
	int lineLengthBwd = 0;

	vec4 p = vertPosition[0];
	
	vec2 g0 = getGradient(p, ux, uy);
	if(length(g0) < seedingThreshold)
		return;
	
	candidatePositionsFwd[0] = p;
	++lineLengthFwd;
	candidatePositionsBwd[0] = p;
	++lineLengthBwd;
	
	
	vec2 g0n = normalize(g0) * stepSize;
	vec4 delta = vec4(g0n.x * ux, g0n.y * uy, 0, 0);
	
	// forward (invocation 0)
	for(int i = 0; i < iterations; ++i)
	{
		vec2 gradient = getGradient(p, ux, uy);
		
		if(length(gradient) < seedingThreshold)
			break;
			
		gradient = normalize(gradient) * stepSize;
		
		// check for sign flips
		if(dot(delta.xy, gradient) < 0.0f)
			gradient = -gradient;
		
		delta = strokeContourCurvature * delta + (1.0f - strokeContourCurvature) * vec4(gradient.x * ux, gradient.y * uy, 0, 0);
		p += delta;
			
		candidatePositionsFwd[lineLengthFwd] = p;
		++lineLengthFwd;
	}
	
	p = vertPosition[0]; // reset seed position
	g0n = normalize(g0) * stepSize * -1;
	delta = vec4(g0n.x * ux, g0n.y * uy, 0, 0);
	
	// backward (invocation 1)
	for(int i = 0; i < iterations; ++i)
	{
		vec2 gradient = getGradient(p, ux, uy);
		
		if(length(gradient) < seedingThreshold)
			break;
			
		gradient = normalize(gradient) * stepSize * -1;
		
		// check for sign flips
		if(dot(delta.xy, gradient) < 0.0f)
			gradient = -gradient;
		
		delta = strokeContourCurvature * delta + (1.0f - strokeContourCurvature) * vec4(gradient.x * ux, gradient.y * uy, 0, 0);
		p += delta;
			
		candidatePositionsBwd[lineLengthBwd] = p;
		++lineLengthBwd;
	}
	
	int overallLength = lineLengthFwd + lineLengthBwd;
	if(overallLength < minSteps)
		return;
	
	float delta_x = 1.0f / (overallLength - 2);
	float t_x = 0.0f;
		
	// start at backward, outputting geometry when invocation is 0
	if(gl_InvocationID == 0)
	{
		if(lineLengthBwd <= 1)
			return; // nothing to do in this invocation
			
			
		for(int i = lineLengthBwd - 1; i > 0; --i)
		{
				vec2 p0 = candidatePositionsBwd[i].xy;
				vec2 p1 = candidatePositionsBwd[i - 1].xy;
				
				vec2 ortho = normalize(vec2(-(p1.y - p0.y), p1.x - p0.x));
				vec4 ortho4 = vec4(ortho.x * ux, ortho.y * uy, 0, 0) * 0.5 * strokeThickness;
				
				vec4 v0 = candidatePositionsBwd[i] - ortho4;
				vec4 v1 = candidatePositionsBwd[i] + ortho4;
				
				vec2 t0 = vec2(t_x, 0.0f);
				vec2 t1 = vec2(t_x, 1.0f);

				emit(v0, t0);				
				emit(v1, t1);
				
				t_x += delta_x;
		}
		// connect to seed
		vec2 p0 = candidatePositionsFwd[0].xy;
		vec2 p1 = candidatePositionsFwd[1].xy;
		
		vec2 ortho = normalize(vec2(-(p1.y - p0.y), p1.x - p0.x));
		vec4 ortho4 = vec4(ortho.x * ux, ortho.y * uy, 0, 0) * 0.5 * strokeThickness;
		
		vec4 v0 = candidatePositionsBwd[0] - ortho4;
		vec4 v1 = candidatePositionsBwd[0] + ortho4;
		
		vec2 t0 = vec2(t_x, 0.0f);
		vec2 t1 = vec2(t_x, 1.0f);
		
		emit(v0, t0);				
		emit(v1, t1);
		
		EndPrimitive();
		
	}
	else if(gl_InvocationID == 1) // forward (invocation is 1)
	{
		t_x = (lineLengthBwd - 1) * delta_x;
	
		if(lineLengthFwd <= 1)
			return; // nothing to do in this invocation
			
		// connect to seed
		vec2 p0 = candidatePositionsFwd[0].xy;
		vec2 p1 = candidatePositionsFwd[1].xy;
		
		vec2 ortho = normalize(vec2(-(p1.y - p0.y), p1.x - p0.x));
		vec4 ortho4 = vec4(ortho.x * ux, ortho.y * uy, 0, 0) * 0.5 * strokeThickness;
		
		vec4 v0 = candidatePositionsFwd[0] - ortho4;
		vec4 v1 = candidatePositionsFwd[0] + ortho4;
		
		vec2 t0 = vec2(t_x, 0.0f);
		vec2 t1 = vec2(t_x, 1.0f);
		
		emit(v0, t0);				
		emit(v1, t1);
		
		t_x += delta_x;
		
		for(int i = 1; i < lineLengthFwd; ++i)
		{
			vec2 p0 = candidatePositionsFwd[i-1].xy;
			vec2 p1 = candidatePositionsFwd[i].xy;
			
			vec2 ortho = normalize(vec2(-(p1.y - p0.y), p1.x - p0.x));
			vec4 ortho4 = vec4(ortho.x * ux, ortho.y * uy, 0, 0) * 0.5 * strokeThickness;
			
			vec4 v0 = candidatePositionsFwd[i] - ortho4;
			vec4 v1 = candidatePositionsFwd[i] + ortho4;
			
			vec2 t0 = vec2(t_x, 0.0f);
			vec2 t1 = vec2(t_x, 1.0f);
			
			emit(v0, t0);				
			emit(v1, t1);
			
			t_x += delta_x;
		}
		EndPrimitive();
	}
}