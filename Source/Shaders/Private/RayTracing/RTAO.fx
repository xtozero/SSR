RaytracingAccelerationStructure AccelerationStructure;
RWTexture2D<float4> SceneColor;

struct [raypayload] PayLoad
{
    bool hit : read( caller ) : write( closesthit, miss );
};

[shader("raygeneration")]
void RayGen()
{
    RayDesc ray;
    ray.Origin = float3( 0.0f, 0.0f, -50.0f );
    ray.TMin = 0.001f;
    ray.Direction = normalize( float3( 0.f, 0.f, 1.0f ) );
    ray.TMax = 1000.0f;

    PayLoad payload;
    payload.hit = false;
    TraceRay( AccelerationStructure, RAY_FLAG_NONE, 0xFF, 0, 0, 0, ray, payload );

    SceneColor[DispatchRaysIndex().xy] = payload.hit ? float4( 1.f, 0.f, 0.f, 1.f ) : float4( 0.f, 1.f, 0.f, 1.f );
}

[shader("closesthit")]
void ClosestHit( inout PayLoad payLoad, in BuiltInTriangleIntersectionAttributes attr )
{
    payLoad.hit = true;
}

[shader("miss")]
void Miss( inout PayLoad payLoad )
{
    payLoad.hit = false;
}
