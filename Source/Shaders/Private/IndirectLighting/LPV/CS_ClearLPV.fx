RWTexture3D<float4> CoeffR : register( u0 );
RWTexture3D<float4> CoeffG : register( u1 );
RWTexture3D<float4> CoeffB : register( u2 );

[numthreads(4, 4, 4)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    CoeffR[DTid] = 0.f;
    CoeffG[DTid] = 0.f;
    CoeffB[DTid] = 0.f;
}