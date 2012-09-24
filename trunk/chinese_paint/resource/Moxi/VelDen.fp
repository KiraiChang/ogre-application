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
// VelDen.fp - Derive u, v, wf, seep
//
// [He & Luo 1997]
//
// D2Q9i: wf   = sigma[0-8](fi)
//        u,v  = sigma[1-8](fi.ei) / p0
//        seep = clamp(ws, 0, cap_s - wf)
//        wf  += seep
//        p0, cap_s assumed to be 1

float4 main( v2f1 IN,
             samplerRECT MiscMap,    // [block, f0, lwf, ws]
             samplerRECT Dist1Map,    // f[N, E, W, S]
             samplerRECT Dist2Map,    // f[NE, SE, NW, SW]
     uniform float wf_mul = 1,     // for artificial water supply/drain
     uniform float cap_s  = 5,     // Max. water on surface
     uniform float Evapor = 0   ) : COLOR // [u, v, wf, seep]
{
    float4 Misc0 = texRECT(MiscMap, IN.Tex0);
    float  f0    = Misc0.y;
    float  ws    = Misc0.w;

    float4 Dist1 = texRECT(Dist1Map, IN.Tex0);
    float4 Dist2 = texRECT(Dist2Map, IN.Tex0);

    // Derive v
    float2 v; // v.x = (E, NE, SE) - (W, NW, SW); v.y = (S, SE, SW) - (N, NE, NW)
    v.x = (Dist1.y + Dist2.x + Dist2.y) - (Dist1.z + Dist2.z + Dist2.w);
    v.y = (Dist1.w + Dist2.y + Dist2.w) - (Dist1.x + Dist2.x + Dist2.z);

    // Derive wf & seep
    float4 tmp = Dist1 + Dist2;
    float wf   = f0 + tmp.x + tmp.y + tmp.z + tmp.w;
          wf   = max(wf - Evapor, 0);
          wf  *= wf_mul;
    float seep = clamp(ws * cap_s, 0, max(1 - wf, 0)); // ws [0..1]
//    float seep = clamp(ws, 0, max(1 - wf, 0)); // ws can be > 1
          wf  += seep;

    return float4(v.x, v.y, wf, seep);
}






