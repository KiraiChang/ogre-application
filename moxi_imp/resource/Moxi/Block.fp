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
// Block.fp - Derive block, f0 & ws
//
// [He & Luo 1997]
//
// D2Q9i: f0_eq  = 4/9 p - 3/2 p0 * dot(u, u)
//        f0_new = lerp(f0, f0_eq, omega)
//        block  = func(glue, DisorderMap, pin)
//        wf     = wf [just copy from VelDenMap]
//        ws    -= seep

#include "TypesUtils.cg"

float4 main( v2fAll IN,
             sampler2D MiscMap : register(s0),     // [block, f0, lwf, ws]
             sampler2D VelDenMap : register(s1),   // [u, v, wf, seep]
             sampler2D FlowInkMap : register(s2),  // [P1, P2, P3, glue]
             sampler2D FixInkMap : register(s3),   // [P1, P2, P3, block]
             sampler2D   DisorderMap : register(s4), // d[grain, -cap-, block, pin]
     uniform float  AlumTexScale = 1,
     uniform float  A0       = 4.0/9.0,
     uniform float  advect_p = 0,
     uniform float3 Blk_w    = float3(0.1, 0.1, -0.1), // blk wgt.[Glue, FixBlk, Swell]
     uniform float3 Pin_w    = float3(0.1, 0.1, 0.1),  // Pin wgt.[base, disorder, FixBlk]
     uniform float  toe_p    = 0.3,
     uniform float  Omega    = 0.5,
     uniform float  cap_s    = 5,   // Max. water on surface
     uniform float  Corn_mul = 1    // corner multipler
            ) : COLOR // [block, f0, lwf, ws]
{
    float4 Misc0 = tex2D(MiscMap, IN.Tex0);
    float  f0    = Misc0.y;
    float  ws    = Misc0.w;
    float4 VelDen = tex2D(VelDenMap, IN.Tex0);
    float2 v      = VelDen.xy;
    float  wf     = VelDen.z;
    // just copy
    float  seep   = VelDen.w;
    float glue   = tex2D(FlowInkMap, IN.Tex0).w;
    float FixBlk = tex2D(FixInkMap, IN.Tex0).w;
    float4 Disorder = tex2D(DisorderMap, IN.TexGrain);

    // Derive ad: Less advection for lower wf
    float ad = smoothstep(0, advect_p, wf);
    // Derive f0
    float f0_eq = A0 * wf - ad * (2.0/3.0) * dot(v, v);
    f0    = lerp(f0, f0_eq, Omega);
    float AlumBlock = Disorder.z;
    // For easier experimentation, we can change the tex.coord. scale on the fly.
	//    float AlumBlock = tex2D(DisorderMap, IN.TexGrain * AlumTexScale).z;

    // Derive Block
    float block = AlumBlock + // AlumBlock = blk_b + blk_g * (1-grain)
                  dot(Blk_w.xy,  float2(glue, FixBlk));
    // Make sure it's within range
    block = min(1, block);
    /// Derive pinning = self dry and all nei.rho < pin threshold
    bool pinning = (wf==0);
    // self dry

    float Pindisor = lerp(Disorder.x, Disorder.w, smoothstep(0, toe_p, glue));
    float pin = Pin_w.x + dot(Pin_w.yz, float2(Pindisor, FixBlk));
    // for Nearest Nei.
    pinning = (pinning) && (tex2D(VelDenMap, IN.TexN_NE.xy).z < pin);
    pinning = (pinning) && (tex2D(VelDenMap, IN.TexE_SE.xy).z < pin);
    pinning = (pinning) && (tex2D(VelDenMap, IN.TexW_NW.xy).z < pin);
    pinning = (pinning) && (tex2D(VelDenMap, IN.TexS_SW.xy).z < pin);
    pin *= Corn_mul;
    // for Next Nearest Nei.
    pinning = (pinning) && (tex2D(VelDenMap, IN.TexN_NE.zw).z < pin);
    pinning = (pinning) && (tex2D(VelDenMap, IN.TexE_SE.zw).z < pin);
    pinning = (pinning) && (tex2D(VelDenMap, IN.TexW_NW.zw).z < pin);
    pinning = (pinning) && (tex2D(VelDenMap, IN.TexS_SW.zw).z < pin);
    /// Derive final block
    //if (pinning) block = 1.0/0;
    // infinite

    return float4(block, f0, wf, max(ws - seep / cap_s, 0));
    // ws [0..1]
	//    return float4(f0, block, wf, max(ws - seep, 0)); // ws can be > 1 (+0.1 fps)
}

float4 main1( v2fAll IN,
             samplerRECT MiscMap : register(s0),     // [block, f0, lwf, ws]
             samplerRECT VelDenMap : register(s1),   // [u, v, wf, seep]
             samplerRECT FlowInkMap : register(s2),  // [P1, P2, P3, glue]
             samplerRECT FixInkMap : register(s3),   // [P1, P2, P3, block]
             sampler2D   DisorderMap : register(s4) // d[grain, -cap-, block, pin]
			 ) : COLOR
{
	float4 OUT;
	OUT = float4(IN.Tex0, 0.0, 0.0);
	//OUT = texRECT(FlowInkMap, IN.Tex0);
	//OUT = tex2D(DisorderMap, IN.Tex0);
	return OUT;
}
