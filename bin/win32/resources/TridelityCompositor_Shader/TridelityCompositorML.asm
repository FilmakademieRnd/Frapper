!!NVfp4.0
# cgc version 2.2.0017, build date Jan 29 2010
# command line args: -profile gp4fp -O
# source file: v:\Tridelity\StereoShaderML\TridelityCompositorML.cg
#vendor NVIDIA Corporation
#version 2.2.0.17
#profile gp4fp
#program TridelityCompositor_ps
#semantic TridelityCompositor_ps.inputMap4 : TEXUNIT0
#semantic TridelityCompositor_ps.inputMap3 : TEXUNIT1
#semantic TridelityCompositor_ps.inputMap2 : TEXUNIT2
#semantic TridelityCompositor_ps.inputMap1 : TEXUNIT3
#semantic TridelityCompositor_ps.inputMap0 : TEXUNIT4
#semantic @TMP2 : __LOCAL
#var float2 texCoord : $vin.TEXCOORD0 : TEX0 : 0 : 1
#var float4 oColor : $vout.COLOR : COL : 1 : 1
#var sampler2D inputMap4 : TEXUNIT0 : texunit 0 : 2 : 1
#var sampler2D inputMap3 : TEXUNIT1 : texunit 1 : 3 : 1
#var sampler2D inputMap2 : TEXUNIT2 : texunit 2 : 4 : 1
#var sampler2D inputMap1 : TEXUNIT3 : texunit 3 : 5 : 1
#var sampler2D inputMap0 : TEXUNIT4 : texunit 4 : 6 : 1
#var int @TMP2[0] :  : local[0] : -1 : 1
#var int @TMP2[1] :  : local[16] : -1 : 1
#var int @TMP2[2] :  : local[32] : -1 : 1
#var int @TMP2[3] :  : local[48] : -1 : 1
#var int @TMP2[4] :  : local[64] : -1 : 1
#var int @TMP2[5] :  : local[80] : -1 : 1
#var int @TMP2[6] :  : local[96] : -1 : 1
#var int @TMP2[7] :  : local[112] : -1 : 1
#var int @TMP2[8] :  : local[128] : -1 : 1
#var int @TMP2[9] :  : local[144] : -1 : 1
#var int @TMP2[10] :  : local[160] : -1 : 1
#var int @TMP2[11] :  : local[176] : -1 : 1
#var int @TMP2[12] :  : local[192] : -1 : 1
#var int @TMP2[13] :  : local[208] : -1 : 1
#var int @TMP2[14] :  : local[224] : -1 : 1
#var int @TMP2[15] :  : local[240] : -1 : 1
#var int @TMP2[16] :  : local[256] : -1 : 1
#var int @TMP2[17] :  : local[272] : -1 : 1
#var int @TMP2[18] :  : local[288] : -1 : 1
#var int @TMP2[19] :  : local[304] : -1 : 1
#var int @TMP2[20] :  : local[320] : -1 : 1
#var int @TMP2[21] :  : local[336] : -1 : 1
#var int @TMP2[22] :  : local[352] : -1 : 1
#var int @TMP2[23] :  : local[368] : -1 : 1
#var int @TMP2[24] :  : local[384] : -1 : 1
#var int @TMP2[25] :  : local[400] : -1 : 1
#var int @TMP2[26] :  : local[416] : -1 : 1
#var int @TMP2[27] :  : local[432] : -1 : 1
#var int @TMP2[28] :  : local[448] : -1 : 1
#var int @TMP2[29] :  : local[464] : -1 : 1
#var int @TMP2[30] :  : local[480] : -1 : 1
#var int @TMP2[31] :  : local[496] : -1 : 1
#var int @TMP2[32] :  : local[512] : -1 : 1
#var int @TMP2[33] :  : local[528] : -1 : 1
#var int @TMP2[34] :  : local[544] : -1 : 1
#var int @TMP2[35] :  : local[560] : -1 : 1
#var int @TMP2[36] :  : local[576] : -1 : 1
#var int @TMP2[37] :  : local[592] : -1 : 1
#var int @TMP2[38] :  : local[608] : -1 : 1
#var int @TMP2[39] :  : local[624] : -1 : 1
#var int @TMP2[40] :  : local[640] : -1 : 1
#var int @TMP2[41] :  : local[656] : -1 : 1
#var int @TMP2[42] :  : local[672] : -1 : 1
#var int @TMP2[43] :  : local[688] : -1 : 1
#var int @TMP2[44] :  : local[704] : -1 : 1
#var int @TMP2[45] :  : local[720] : -1 : 1
#var int @TMP2[46] :  : local[736] : -1 : 1
#var int @TMP2[47] :  : local[752] : -1 : 1
#var int @TMP2[48] :  : local[768] : -1 : 1
#var int @TMP2[49] :  : local[784] : -1 : 1
#var int @TMP2[50] :  : local[800] : -1 : 1
#var int @TMP2[51] :  : local[816] : -1 : 1
#var int @TMP2[52] :  : local[832] : -1 : 1
#var int @TMP2[53] :  : local[848] : -1 : 1
#var int @TMP2[54] :  : local[864] : -1 : 1
#var int @TMP2[55] :  : local[880] : -1 : 1
#var int @TMP2[56] :  : local[896] : -1 : 1
#var int @TMP2[57] :  : local[912] : -1 : 1
#var int @TMP2[58] :  : local[928] : -1 : 1
#var int @TMP2[59] :  : local[944] : -1 : 1
#var int @TMP2[60] :  : local[960] : -1 : 1
#var int @TMP2[61] :  : local[976] : -1 : 1
#var int @TMP2[62] :  : local[992] : -1 : 1
#var int @TMP2[63] :  : local[1008] : -1 : 1
#var int @TMP2[64] :  : local[1024] : -1 : 1
#var int @TMP2[65] :  : local[1040] : -1 : 1
#var int @TMP2[66] :  : local[1056] : -1 : 1
#var int @TMP2[67] :  : local[1072] : -1 : 1
#var int @TMP2[68] :  : local[1088] : -1 : 1
#var int @TMP2[69] :  : local[1104] : -1 : 1
#var int @TMP2[70] :  : local[1120] : -1 : 1
#var int @TMP2[71] :  : local[1136] : -1 : 1
#var int @TMP2[72] :  : local[1152] : -1 : 1
#var int @TMP2[73] :  : local[1168] : -1 : 1
#var int @TMP2[74] :  : local[1184] : -1 : 1
ATTRIB fragment_texcoord[] = { fragment.texcoord[0..0] };
TEMP R0, R1;
TEMP RC, HC;
TEMP lmem[75];
OUTPUT oCol = result.color;
MOV.S lmem[0].x, {0};
MOV.S lmem[1].x, {0};
MOV.S lmem[2].x, {4};
MOV.S lmem[3].x, {3};
MOV.S lmem[4].x, {3};
MOV.S lmem[5].x, {2};
MOV.S lmem[6].x, {1};
MOV.S lmem[7].x, {1};
MOV.S lmem[8].x, {0};
MOV.S lmem[9].x, {4};
MOV.S lmem[10].x, {4};
MOV.S lmem[11].x, {3};
MOV.S lmem[12].x, {2};
MOV.S lmem[13].x, {2};
MOV.S lmem[14].x, {1};
MOV.S lmem[15].x, {3};
MOV.S lmem[16].x, {3};
MOV.S lmem[17].x, {2};
MOV.S lmem[18].x, {1};
MOV.S lmem[19].x, {1};
MOV.S lmem[20].x, {0};
MOV.S lmem[21].x, {4};
MOV.S lmem[22].x, {4};
MOV.S lmem[23].x, {3};
MOV.S lmem[24].x, {2};
MOV.S lmem[25].x, {2};
MOV.S lmem[26].x, {1};
MOV.S lmem[27].x, {0};
MOV.S lmem[28].x, {0};
MOV.S lmem[29].x, {4};
MOV.S lmem[30].x, {1};
MOV.S lmem[31].x, {1};
MOV.S lmem[32].x, {0};
MOV.S lmem[33].x, {4};
MOV.S lmem[34].x, {4};
MOV.S lmem[35].x, {3};
MOV.S lmem[36].x, {2};
MOV.S lmem[37].x, {2};
MOV.S lmem[38].x, {1};
MOV.S lmem[39].x, {0};
MOV.S lmem[40].x, {0};
MOV.S lmem[41].x, {4};
MOV.S lmem[42].x, {3};
MOV.S lmem[43].x, {3};
MOV.S lmem[44].x, {2};
MOV.S lmem[45].x, {4};
MOV.S lmem[46].x, {4};
MOV.S lmem[47].x, {3};
MOV.S lmem[48].x, {2};
MOV.S lmem[49].x, {2};
MOV.S lmem[50].x, {1};
MOV.S lmem[51].x, {0};
MOV.S lmem[52].x, {0};
MOV.S lmem[53].x, {4};
MOV.S lmem[54].x, {3};
MOV.S lmem[55].x, {3};
MOV.S lmem[56].x, {2};
MOV.S lmem[57].x, {1};
MOV.S lmem[58].x, {1};
MOV.S lmem[59].x, {0};
MOV.S lmem[60].x, {2};
MOV.S lmem[61].x, {2};
MOV.S lmem[62].x, {1};
MOV.S lmem[63].x, {0};
MOV.S lmem[64].x, {0};
MOV.S lmem[65].x, {4};
MOV.S lmem[66].x, {3};
MOV.S lmem[67].x, {3};
MOV.S lmem[68].x, {2};
MOV.S lmem[69].x, {1};
MOV.S lmem[70].x, {1};
MOV.S lmem[71].x, {0};
MOV.S lmem[72].x, {4};
MOV.S lmem[73].x, {4};
MUL.F R0.y, fragment.texcoord[0], {1080}.x;
MUL.F R0.x, fragment.texcoord[0], {1920};
TRUNC.S R0.y, R0;
TRUNC.S R0.x, R0;
MOD.S R0.y, R0, {15}.x;
MOD.S R0.x, R0, {5}.x;
MAD.S R0.x, R0, {15}, R0.y;
MOV.U R0.x, R0;
MOV.S lmem[74].x, {3};
MOV.S R0.w, lmem[R0.x].x;
SEQ.S R1.x, R0.w, {0};
MOV.S.CC HC.x, -R1;
MOV.F R0.xyz, {1}.x;
IF    NE.x;
TEX.F R0.z, fragment.texcoord[0], texture[2], 2D;
TEX.F R0.y, fragment.texcoord[0], texture[3], 2D;
TEX.F R0.x, fragment.texcoord[0], texture[4], 2D;
ELSE;
SEQ.S R1.x, R0.w, {1};
MOV.S.CC HC.x, -R1;
IF    NE.x;
TEX.F R0.z, fragment.texcoord[0], texture[1], 2D;
TEX.F R0.y, fragment.texcoord[0], texture[2], 2D;
TEX.F R0.x, fragment.texcoord[0], texture[3], 2D;
ELSE;
SEQ.S R1.x, R0.w, {2};
MOV.S.CC HC.x, -R1;
IF    NE.x;
TEX.F R0.z, fragment.texcoord[0], texture[0], 2D;
TEX.F R0.y, fragment.texcoord[0], texture[1], 2D;
TEX.F R0.x, fragment.texcoord[0], texture[2], 2D;
ELSE;
SEQ.S R1.x, R0.w, {3};
MOV.S.CC HC.x, -R1;
IF    NE.x;
TEX.F R0.z, fragment.texcoord[0], texture[4], 2D;
TEX.F R0.y, fragment.texcoord[0], texture[0], 2D;
TEX.F R0.x, fragment.texcoord[0], texture[1], 2D;
ELSE;
SEQ.S R0.w, R0, {4}.x;
MOV.S.CC HC.x, -R0.w;
IF    NE.x;
TEX.F R0.z, fragment.texcoord[0], texture[3], 2D;
TEX.F R0.y, fragment.texcoord[0], texture[4], 2D;
TEX.F R0.x, fragment.texcoord[0], texture[0], 2D;
ENDIF;
ENDIF;
ENDIF;
ENDIF;
ENDIF;
MOV.F oCol.xyz, R0;
MOV.F oCol.w, {0}.x;
END
# 126 instructions, 2 R-regs
