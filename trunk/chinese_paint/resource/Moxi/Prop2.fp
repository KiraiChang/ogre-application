//----------------------------------------------------------------------------
// Copyright 2005 Nelson S.-H. Chu and The Hong Kong University of Science and
// Technology.
//
// Permission to use, copy, modify and distribute this software and its
// documentation for research or educational purpose is hereby granted without
// fee, provided that the above copyright notice appear in all copies and that
// both that copyright notice and this permission notice appear in supporting
// documentation. This software is provided "as is" without express or implied
// warranty.
//----------------------------------------------------------------------------
//
// Prop2.fp - Fragment program for propagating Dist[5-8]
//
// D2Q9*: fi(x+ei) = fi(new)(x)
//
// with 1. Semi-permeable bounce-back
//      2. Pinning
//      3. Evaopration via fiber along edge

#include "TypesUtils.cg"

float4 main( v2f2 IN,
             samplerRECT MiscMap,    // [block, f0, wf, ws]
             samplerRECT Dist2Map,    // f[NE, SE, NW, SW]
     uniform float Evapor_b = 0   // Evapor at pinned edge
            ) : COLOR // f[NE, SE, NW, SW]
{
    float bO  = texRECT(MiscMap, IN.Tex0).x;
    float bNE = texRECT(MiscMap, IN.TexNE).x;
    float bSE = texRECT(MiscMap, IN.TexSE).x;
    float bNW = texRECT(MiscMap, IN.TexNW).x;
    float bSW = texRECT(MiscMap, IN.TexSW).x;

    float4 b = float4(bSW, bNW, bSE, bNE); // b[SW, NW, SE, NE]

    b = (b + bO) / 2;

    float4 pinned = (b > 1);
    b = min(b, 1);

    float4 f_Out, f_In;
        
    f_Out  = texRECT(Dist2Map, IN.Tex0);
    f_In.x = texRECT(Dist2Map, IN.TexSW).x;
    f_In.y = texRECT(Dist2Map, IN.TexNW).y;
    f_In.z = texRECT(Dist2Map, IN.TexSE).z;
    f_In.w = texRECT(Dist2Map, IN.TexNE).w;

    // Stream with partial bounce-back
    float4 OUT = lerp(f_In, f_Out.wzyx, b);

    // Lower wf at pinned edge
    OUT = max(OUT - pinned * Evapor_b, 0);

    return OUT;
}


