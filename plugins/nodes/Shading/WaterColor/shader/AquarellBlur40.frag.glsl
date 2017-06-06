//Author: Jens Metzner
//Date  : 19.10.2012

#version 400 core

uniform sampler2D in_tex;
uniform int in_horizontal;

in vec4 VertTexture;

// Relative filter weights indexed by distance from "home" texel
//    This set for 9-texel sampling

#define WT9_0 1.0
#define WT9_1 0.9
#define WT9_2 0.7
#define WT9_3 0.5
#define WT9_4 0.3
#define WT9_5 0.2
#define WT9_6 0.1
#define WT9_NORMALIZE (WT9_0+2.0*(WT9_1+WT9_2+WT9_3+WT9_4+WT9_5+WT9_6))

vec4 blurH(sampler2D tex, vec2 coord, vec2 dimensions, float stride)
{
	float TexelIncrement = stride/dimensions.x;
    //float3 Coord = float3(TexCoord.xy+QuadTexelOffsets.xy, 1);

    vec2 c0 = vec2(coord.x + TexelIncrement, coord.y);
    vec2 c1 = vec2(coord.x + TexelIncrement * 2, coord.y);
    vec2 c2 = vec2(coord.x + TexelIncrement * 3, coord.y);
    vec2 c3 = vec2(coord.x + TexelIncrement * 4, coord.y);
    vec2 c4 = vec2(coord.x + TexelIncrement * 5, coord.y);
    vec2 c5 = vec2(coord.x + TexelIncrement * 6, coord.y);
    vec2 c6 = vec2(coord.x,                      coord.y);
    vec2 c7 = vec2(coord.x - TexelIncrement,     coord.y);
    vec2 c8 = vec2(coord.x - TexelIncrement * 2, coord.y);
    vec2 c9 = vec2(coord.x - TexelIncrement * 3, coord.y);
    vec2 c10 = vec2(coord.x - TexelIncrement * 4, coord.y);
    vec2 c11 = vec2(coord.x - TexelIncrement * 5, coord.y);
    vec2 c12 = vec2(coord.x - TexelIncrement * 6, coord.y);

    vec4 OutCol;

    OutCol = texture(tex, c0) * (WT9_1/WT9_NORMALIZE);
    OutCol += texture(tex, c1) * (WT9_2/WT9_NORMALIZE);
    OutCol += texture(tex, c2) * (WT9_3/WT9_NORMALIZE);
    OutCol += texture(tex, c3) * (WT9_4/WT9_NORMALIZE);
    OutCol += texture(tex, c4) * (WT9_5/WT9_NORMALIZE);
    OutCol += texture(tex, c5) * (WT9_6/WT9_NORMALIZE);
    OutCol += texture(tex, c6) * (WT9_0/WT9_NORMALIZE);
    OutCol += texture(tex, c7) * (WT9_1/WT9_NORMALIZE);
    OutCol += texture(tex, c8) * (WT9_2/WT9_NORMALIZE);
    OutCol += texture(tex, c9) * (WT9_3/WT9_NORMALIZE);
    OutCol += texture(tex, c10) * (WT9_4/WT9_NORMALIZE);
    OutCol += texture(tex, c11) * (WT9_5/WT9_NORMALIZE);
    OutCol += texture(tex, c12) * (WT9_6/WT9_NORMALIZE);

    return OutCol;
}
vec4 blurV(sampler2D tex, vec2 coord, vec2 dimensions, float stride)
{
	float TexelIncrement = stride/dimensions.x;
    //float3 Coord = float3(TexCoord.xy+QuadTexelOffsets.xy, 1);

    vec2 c0 = vec2(coord.x, coord.y + TexelIncrement);
    vec2 c1 = vec2(coord.x, coord.y + TexelIncrement * 2);
    vec2 c2 = vec2(coord.x, coord.y + TexelIncrement * 3);
    vec2 c3 = vec2(coord.x, coord.y + TexelIncrement * 4);
    vec2 c4 = vec2(coord.x, coord.y + TexelIncrement * 5);
    vec2 c5 = vec2(coord.x, coord.y + TexelIncrement * 6);
    vec2 c6 = vec2(coord.x, coord.y);
    vec2 c7 = vec2(coord.x, coord.y - TexelIncrement);
    vec2 c8 = vec2(coord.x, coord.y - TexelIncrement * 2);
    vec2 c9 = vec2(coord.x, coord.y - TexelIncrement * 3);
    vec2 c10 = vec2(coord.x, coord.y - TexelIncrement * 4);
    vec2 c11 = vec2(coord.x, coord.y - TexelIncrement * 5);
    vec2 c12 = vec2(coord.x, coord.y - TexelIncrement * 6);

    vec4 OutCol;

    OutCol = texture(tex, c0) * (WT9_1/WT9_NORMALIZE);
    OutCol += texture(tex, c1) * (WT9_2/WT9_NORMALIZE);
    OutCol += texture(tex, c2) * (WT9_3/WT9_NORMALIZE);
    OutCol += texture(tex, c3) * (WT9_4/WT9_NORMALIZE);
    OutCol += texture(tex, c4) * (WT9_5/WT9_NORMALIZE);
    OutCol += texture(tex, c5) * (WT9_6/WT9_NORMALIZE);
    OutCol += texture(tex, c6) * (WT9_0/WT9_NORMALIZE);
    OutCol += texture(tex, c7) * (WT9_1/WT9_NORMALIZE);
    OutCol += texture(tex, c8) * (WT9_2/WT9_NORMALIZE);
    OutCol += texture(tex, c9) * (WT9_3/WT9_NORMALIZE);
    OutCol += texture(tex, c10) * (WT9_4/WT9_NORMALIZE);
    OutCol += texture(tex, c11) * (WT9_5/WT9_NORMALIZE);
    OutCol += texture(tex, c12) * (WT9_6/WT9_NORMALIZE);

    return OutCol;
}


void main()
{
	vec2 coords = vec2(VertTexture.x, 1.0-VertTexture.t);	
	vec2 dim = textureSize(in_tex, 0);
	//vec4 p = texture(in_tex,coords);
	
	if(in_horizontal == 1)
	{
		vec4 blur = blurH(in_tex, coords, dim, 3);
		gl_FragData[0] = vec4(blur.xyz,1);
	} else {
		vec4 blur = blurV(in_tex, coords, dim, 3);
		gl_FragData[0] = vec4(blur.xyz,1);
	}
	//vec4 color = radialBlur(in_tex, coords, 10, 64);
	//gl_FragData[0] = color;

	//segmentation(id,p);
	//gl_FragData[0] = vec4(horizontal);
}