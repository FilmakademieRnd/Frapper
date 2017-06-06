#version 400 core

in vec4 vertexPosition;
out vec4 fragColor;

uniform sampler2D scene;
uniform sampler2D tfm;

uniform float sigmaT = 3.0f;

struct lic_t
{ 
    vec2 p; 
    vec2 t;
    float w;
    float dw;
};

void step(inout lic_t s)
{
    vec2 t = texture(tfm, s.p).xy;
    if (dot(t, s.t) < 0.0) t = -t;
    s.t = t;

    s.dw = (abs(t.x) > abs(t.y))? 
        abs((fract(s.p.x) - 0.5 - sign(t.x)) / t.x) : 
        abs((fract(s.p.y) - 0.5 - sign(t.y)) / t.y);

    s.p += t * s.dw / vec2(textureSize(scene, 0));
    s.w += s.dw;
}

void main (void)
{
	vec2 uv = vertexPosition.xy / vertexPosition.w * 0.5 + vec2(0.5);
	
	float twoSigma2 = 2.0f * sigmaT * sigmaT;
    float halfWidth = 2.0f * sigmaT;

    vec3 c = texture(scene, uv).xyz;
    float w = 1.0f;

    lic_t a, b;
    a.p = b.p = uv;
    a.t = texture(tfm, uv).xy / vec2(textureSize(scene, 0));
    b.t = -a.t;
    a.w = b.w = 0.0f;

    while (a.w < halfWidth) {
        step(a);
        float k = a.dw * exp(-a.w * a.w / twoSigma2);
        c += k * texture(scene, a.p).xyz;
        w += k;
    }
    while (b.w < halfWidth) {
        step(b);
        float k = b.dw * exp(-b.w * b.w / twoSigma2);
        c += k * texture(scene, b.p).xyz;
        w += k;
    }
    
    vec3 dst = c / w;
	
	fragColor = vec4(dst, 1);
}