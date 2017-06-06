!!ARBfp1.0
# cgc version 2.2.0017, build date Jan 29 2010
# command line args: -profile arbfp1 -O
# source file: TridelityCompositor.cg
#vendor NVIDIA Corporation
#version 2.2.0.17
#profile arbfp1
#program TridelityCompositor_ps
#semantic TridelityCompositor_ps.inputMap0 : TEXUNIT0
#semantic TridelityCompositor_ps.inputMap1 : TEXUNIT1
#semantic TridelityCompositor_ps.inputMap2 : TEXUNIT2
#semantic TridelityCompositor_ps.inputMap3 : TEXUNIT3
#semantic TridelityCompositor_ps.inputMap4 : TEXUNIT4
#var float2 texCoord : $vin.TEXCOORD0 : TEX0 : 0 : 1
#var float4 oColor : $vout.COLOR : COL : 1 : 1
#var sampler2D inputMap0 : TEXUNIT0 : texunit 0 : 2 : 1
#var sampler2D inputMap1 : TEXUNIT1 : texunit 1 : 3 : 1
#var sampler2D inputMap2 : TEXUNIT2 : texunit 2 : 4 : 1
#var sampler2D inputMap3 : TEXUNIT3 : texunit 3 : 5 : 1
#var sampler2D inputMap4 : TEXUNIT4 : texunit 4 : 6 : 1
#const c[0] = 1080 3840 0 4
#const c[1] = 0.2 5 0.5 3
#const c[2] = 2 1
PARAM c[3] = { { 1080, 3840, 0, 4 },
		{ 0.2, 5, 0.5, 3 },
		{ 2, 1 } };
TEMP R0;
TEMP R1;
TEMP R2;
TEMP R3;
TEMP R4;
MUL R0.x, fragment.texcoord[0], c[0].y;
MAD R0.x, fragment.texcoord[0].y, c[0], R0;
ADD R0.y, R0.x, c[0].w;
ABS R0.y, R0;
TEX R1.xyz, fragment.texcoord[0], texture[1], 2D;
TEX R3.xyz, fragment.texcoord[0], texture[3], 2D;
TEX R4.xyz, fragment.texcoord[0], texture[4], 2D;
FLR R0.y, R0;
SLT R0.x, R0, -c[0].w;
CMP R0.x, -R0, -R0.y, R0.y;
MUL R0.x, R0, c[1];
FRC R0.x, R0;
MUL R0.x, R0, c[1].y;
ABS R0.y, R0.x;
ADD R0.y, R0, c[1].z;
FLR R0.y, R0;
SLT R0.x, R0, c[0].z;
CMP R1.w, -R0.x, -R0.y, R0.y;
TEX R0.xyz, fragment.texcoord[0], texture[0], 2D;
ABS R0.w, R1;
CMP R2.x, -R0.w, c[2].y, R0;
ADD R0.x, R1.w, -c[2].y;
ABS R0.x, R0;
CMP R2.w, -R0.x, R2.x, R1.x;
ADD R1.x, R1.w, -c[2];
TEX R2.xyz, fragment.texcoord[0], texture[2], 2D;
ABS R1.x, R1;
CMP R2.w, -R1.x, R2, R2.x;
ADD R2.x, R1.w, -c[1].w;
ABS R2.x, R2;
ADD R1.w, R1, -c[0];
ABS R1.w, R1;
CMP R2.w, -R2.x, R2, R3.x;
CMP result.color.x, -R1.w, R2.w, R4;
CMP R2.w, -R0, c[2].y, R4.y;
CMP R0.w, -R0, c[2].y, R3.z;
CMP R0.y, -R0.x, R2.w, R0;
CMP R0.w, -R0.x, R0, R4.z;
CMP R0.x, -R1, R0.y, R1.y;
CMP R0.y, -R1.x, R0.w, R0.z;
CMP R0.x, -R2, R0, R2.y;
CMP R0.y, -R2.x, R0, R1.z;
CMP result.color.y, -R1.w, R0.x, R3;
CMP result.color.z, -R1.w, R0.y, R2;
MOV result.color.w, c[0].z;
END
# 45 instructions, 5 R-regs
