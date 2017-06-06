#version 400 core

in vec4 vertexPosition;
out vec4 fragColor;

uniform sampler2D structureTensors;

uniform float sigma = 2.0f;


void main (void)
{
	vec2 uv = vertexPosition.xy / vertexPosition.w * 0.5 + vec2(0.5);
	vec2 src_size = textureSize(structureTensors, 0);

    float twoSigma2 = 2.0f * sigma * sigma;
    int halfWidth = int(ceil(2.0f * sigma));

    vec3 sum = vec3(0.0);
    float norm = 0.0f;
    if (halfWidth > 0) {
        for (int i = -halfWidth; i <= halfWidth; ++i) {
            for (int j = -halfWidth; j <= halfWidth; ++j) {
                float d = length(vec2(i,j));
                float kernel = exp(-d *d / twoSigma2);
                vec3 c = texture(structureTensors, uv + vec2(i,j) / src_size).xyz;
                sum += kernel * c;
                norm += kernel;
            }
        }
    } else {
        sum = texture(structureTensors, uv).xyz;
        norm = 1.0f;
    }
    vec3 dst = sum / norm;
	
	fragColor = vec4(dst,1);
}