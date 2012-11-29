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
// Collide1.fp - Calculates Collided Dist[1-4]
//
// [He & Luo 1997]
//
// D2Q9i: fi_eq  = 1/9 p + p0 * (1/3 dot(ei,v) + 1/2 * dot(ei,v)^2 - 1/6 * dot(v, v))
//        fi_new = lerp(fi, fi_eq, Omega)

#include "TypesUtils.cg"

float4 main( v2f1 IN,
             samplerRECT VelDenMap,   // [u, v, wf, seep]
             samplerRECT Dist1Map,    // f[N, E, W, S]
             samplerRECT InkMap,      // [P1, P2, P3, glue]
     uniform float A = 1.0/9.0,
     uniform float B = 1.0/3.0,
     uniform float C = 0.5,
     uniform float D = 1.0/6.0,
     uniform float advect_p = 0,
     uniform float Omega    = 0.5 ) : COLOR // f[N, E, W, S]
{
    float4 OUT = 0;

    float4 VelDen = texRECT(VelDenMap, IN.Tex0);
    float2 v      = VelDen.xy;
    float  p      = VelDen.z;

    float4 eiDotv = float4(-v.y, v.x, -v.x, v.y); // N, E, W, S

    // Derive ad
    float ad = smoothstep(0, advect_p, p);

    float4 f_eq = A * p + ad * (B * eiDotv + C * eiDotv * eiDotv - D * dot(v, v));
    float4 f    = texRECT(Dist1Map, IN.Tex0);

    OUT = lerp(f, f_eq, Omega);

    return OUT;
}
