!!ARBfp1.0
# cgc version 2.2.0017, build date Jan 29 2010
# command line args: -profile arbfp1 -O
# source file: StereoViewport_fs.cg
#vendor NVIDIA Corporation
#version 2.2.0.17
#profile arbfp1
#program fs_main
#semantic fs_main.RT0 : TEXUNIT0
#var float2 iTexCoord : $vin.TEXCOORD0 : TEX0 : 0 : 1
#var float4 oColor : $vout.COLOR : COL : 1 : 1
#var sampler2D RT0 : TEXUNIT0 : texunit 0 : 2 : 1
#const c[0] = 1080 3840 0 4
#const c[1] = 0.2 5 0.5 3
#const c[2] = 2 1 0.33333334 0.66666669
PARAM c[3] = { { 1080, 3840, 0, 4 },
		{ 0.2, 5, 0.5, 3 },
		{ 2, 1, 0.33333334, 0.66666669 } };
TEMP R0;
TEMP R1;
TEMP R2;
TEMP R3;
TEMP R4;
TEMP R5;
MUL R2.w, fragment.texcoord[0].y, c[2].z;
MUL R5.x, fragment.texcoord[0], c[1].z;
ADD R4.x, R5, c[1].z;
ADD R3.y, R2.w, c[2].z;
MUL R0.x, fragment.texcoord[0], c[0].y;
MAD R0.x, fragment.texcoord[0].y, c[0], R0;
ADD R0.y, R0.x, c[0].w;
ABS R0.y, R0;
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
ABS R0.w, R1;
MOV R0.x, R5;
ADD R0.y, R2.w, c[2].w;
TEX R0.xyz, R0, texture[0], 2D;
CMP R2.x, -R0.w, R2, R0;
ADD R0.x, R1.w, -c[2].y;
ABS R0.x, R0;
MOV R1.y, R3;
MOV R1.x, R4;
TEX R1.xyz, R1, texture[0], 2D;
CMP R2.x, -R0, R2, R1;
MOV R3.x, R5;
ADD R1.x, R1.w, -c[2];
MOV R4.y, R2.w;
MOV R5.y, R2.w;
TEX R5.xyz, R5, texture[0], 2D;
CMP R2.y, -R0.w, R2, R5;
TEX R4.xyz, R4, texture[0], 2D;
CMP R0.w, -R0, R2.z, R4.z;
TEX R3.xyz, R3, texture[0], 2D;
ABS R1.x, R1;
CMP R3.x, -R1, R2, R3;
ADD R2.x, R1.w, -c[1].w;
ABS R2.x, R2;
ADD R1.w, R1, -c[0];
ABS R1.w, R1;
CMP R3.x, -R2, R3, R4;
CMP R0.y, -R0.x, R2, R0;
CMP R0.w, -R0.x, R0, R5.z;
CMP R0.x, -R1, R0.y, R1.y;
CMP R0.y, -R1.x, R0.w, R0.z;
CMP R0.x, -R2, R0, R3.y;
CMP R0.y, -R2.x, R0, R1.z;
CMP result.color.x, -R1.w, R3, R5;
CMP result.color.y, -R1.w, R0.x, R4;
CMP result.color.z, -R1.w, R0.y, R3;
MOV result.color.w, c[0].z;
END
# 56 instructions, 6 R-regs
