#version 400 core

in vec4 vertexPosition;
out vec4 fragColor;

uniform sampler2D gauss;

void main (void)
{
	vec2 uv = vertexPosition.xy / vertexPosition.w * 0.5 + vec2(0.5);

    vec3 g = texture(gauss, uv).xyz;

    float lambda1 = 0.5f * (g.y + g.x +
        sqrt(g.y * g.y - 2.0f * g.x * g.y + g.x * g.x + 4.0f *g.z * g.z));
    float lambda2 = 0.5 * (g.y + g.x -
        sqrt(g.y * g.y - 2.0f * g.x * g.y + g.x * g.x + 4.0f * g.z * g.z));

    vec2 v = vec2(lambda1 - g.x, -g.z);
    vec2 t;
    if (length(v) > 0.0) { 
        t = normalize(v);
    } else {
        t = vec2(0.0, 1.0);
    }

    float phi = atan(t.y, t.x);

    float A = (lambda1 + lambda2 > 0.0f)?
        (lambda1 - lambda2) / (lambda1 + lambda2) : 0.0f;

    fragColor = vec4(t, phi, A);
}