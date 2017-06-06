#version 400 core

in vec4 vertexPosition;
out vec4 fragColor;

uniform sampler2D lic;

uniform float lightSourceAngle = 0.0f;
uniform float kSpecular = 2.0f;
uniform float kShininess = 1.0f;

const vec2 size = vec2(2.0,0.0);
const ivec3 off = ivec3(-1,0,1);

vec3 normal(vec2 uv)
{
	float s11 = texture(lic, uv).x;
	float s01 = textureOffset(lic, uv, off.xy).x;
    float s21 = textureOffset(lic, uv, off.zy).x;
    float s10 = textureOffset(lic, uv, off.yx).x;
    float s12 = textureOffset(lic, uv, off.yz).x;
	
	vec3 va = normalize(vec3(size.xy, s21 - s01));
    vec3 vb = normalize(vec3(size.yx, s12 - s10));
    return cross(va,vb);	
}

vec2 rotate(vec2 v, float angle)
{
	vec2 r;
	float rads = radians(angle);
	r.x = v.x * cos(rads)  - v.y * sin(rads);
	r.y = v.x * -sin(rads) + v.y * cos(rads);
	return r;
}

void main (void)
{
	vec2 uv = vertexPosition.xy / vertexPosition.w * 0.5 + vec2(0.5);
	vec3 N = normalize(normal(uv));
	
	vec2 dirXZ = vec2(0, 1);
	vec2 rot = rotate(dirXZ, lightSourceAngle);
	
	vec3 L = normalize(vec3(rot.x, rot.y, 0));
	
	float x = kSpecular * pow(dot(N,L), kShininess);
	x = max(0, x);

	fragColor = vec4(x,x,x,1);
}