RWTexture2D<float4> SceneColor;

struct PayLoad
{
    bool hit;
};

[shader("raygeneration")]
void RayGen()
{
    SceneColor[DispatchRaysIndex().xy] = float4( 0.f, 0.f, 0.f, 1.f );
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
