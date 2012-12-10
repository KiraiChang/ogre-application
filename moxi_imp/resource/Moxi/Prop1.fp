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
// Prop1.fp - Fragment program for propagating Dist[1-4]
//
// D2Q9*: fi(x+ei) = fi(new)(x)
//
// with 1. Semi-permeable bounce-back
//      2. Pinning
//      3. Evaopration via fiber along edge

#include "TypesUtils.cg"

float4 main( v2f1 IN,
             sampler2D MiscMap : register(s0),    // [block, f0, wf, ws]
             sampler2D Dist1Map : register(s1),    // f[N, E, W, S]
     uniform float Evapor_b = 0   // Evapor at pinned edge
            ) : COLOR // f[N, E, W, S]
{
    float bO = tex2D(MiscMap, IN.Tex0).x;
    float bN = tex2D(MiscMap, IN.TexN).x;
    float bE = tex2D(MiscMap, IN.TexE).x;
    float bW = tex2D(MiscMap, IN.TexW).x;
    float bS = tex2D(MiscMap, IN.TexS).x;

    float4 b = float4(bS, bW, bE, bN); // b[S, W, E, N]

    b = (b + bO) / 2; // can be saved by tex. interpolation, but not much gain

    float4 pinned = (b > 1);
    b = min(b, 1);

    float4 f_Out, f_In;

    f_Out  = tex2D(Dist1Map, IN.Tex0);
    f_In.x = tex2D(Dist1Map, IN.TexS).x;
    f_In.y = tex2D(Dist1Map, IN.TexW).y;
    f_In.z = tex2D(Dist1Map, IN.TexE).z;
    f_In.w = tex2D(Dist1Map, IN.TexN).w;

    // Stream with partial bounce-back
    float4 OUT = lerp(f_In, f_Out.wzyx, b);

    // Lower wf at pinned edge
    OUT = max(OUT - pinned * Evapor_b, 0);

    return OUT;
}

