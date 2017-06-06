#version 400 core

in vec4 vertexPosition;
out vec4 fragColor;

uniform sampler2D scene;
uniform sampler2D anisotropy;
uniform sampler2D kernel;

uniform float radius = 6.0f;
uniform float q = 8.0f;

#define TWO_M_PI 6.28318530718

void main (void)
{	
	const int N = 8;

	vec2 texSize = vec2(textureSize(scene, 0));
    vec2 uv = vertexPosition.xy / vertexPosition.w * 0.5 + vec2(0.5);

    vec4 m[8];
    vec3 s[8];
    for (int k = 0; k < N; ++k) {
        m[k] = vec4(0.0);
        s[k] = vec3(0.0);
    }

    float piN = TWO_M_PI / float(N);
	float val_cos = cos(piN);
	float val_sin = sin(piN);
    mat2 X = mat2(val_cos, val_sin, -val_sin, val_cos);

    vec4 t = texture2D(anisotropy, uv);
	
    float a = radius * clamp(1.0 + t.w, 0.1, 1.0); 
    float b = radius * clamp(1.0 / (1.0 + t.w), 0.1, 1.0);

    float cos_phi = cos(t.z);
    float sin_phi = sin(t.z);

    mat2 R = mat2(cos_phi, -sin_phi, sin_phi, cos_phi);
    mat2 S = mat2(0.5 / a, 0.0, 0.0, 0.5 / b);
    mat2 SR = S * R;

	float a_sq = a * a;
	float b_sq = b * b;
	
	float cos_phi_sq = cos_phi * cos_phi;
	float sin_phi_sq = sin_phi * sin_phi;
	
    int max_x = int(sqrt(a_sq * cos_phi_sq +
                          b_sq * sin_phi_sq));
    int max_y = int(sqrt(a_sq * sin_phi_sq +
                          b_sq * cos_phi_sq));

    for (int j = -max_y; j <= max_y; ++j) {
        for (int i = -max_x; i <= max_x; ++i) {
            vec2 v = SR * vec2(i,j);
            if (dot(v,v) <= 0.25) {
            vec3 c = texture2D(scene, uv + vec2(i,j) / texSize).xyz;
            for (int k = 0; k < N; ++k) {
                float w = texture2D(kernel, vec2(0.5, 0.5) + v).x;
				vec3 cw = c * w;
                m[k] += vec4(cw, w);
                s[k] += c * cw;
                v *= X;
                }
            }
        }
    }

    vec4 o = vec4(0.0);
    for (int k = 0; k < N; ++k) {
        m[k].xyz /= m[k].w;
        s[k] = abs(s[k] / m[k].w - m[k].xyz * m[k].xyz);

        float sigma2 = s[k].x + s[k].y + s[k].z;
        float w = 1.0 / (1.0 + pow(255.0 * sigma2, 0.5 * q));

        o += vec4(m[k].xyz * w, w);
    }

    fragColor = vec4(o.xyz / o.w, 1.0);
}