float4 ShFunctionL1( float3 v )
{
    return float4( 0.282095f, -0.488603f * v.y, 0.488603f * v.z, -0.488603f * v.x );
}

void ShFunctionL2( float3 v, out float Y[9] )
{
    // L0
    Y[0] = 0.282095f; // Y_00
    
    // L1
    Y[1] = 0.488603f * v.y; // Y_1-1
    Y[2] = 0.488603f * v.z; // Y_10
    Y[3] = 0.488603f * v.x; // Y_11

    // L2
    Y[4] = 1.092548f * v.x * v.y; // Y_2-2
    Y[5] = 1.092548f * v.y * v.z; // Y_2-1
    Y[6] = 0.315392f * ( 3.f * v.z * v.z - 1.f ) ; // Y_20
    Y[7] = 1.092548f * v.x * v.z; // Y_21
    Y[8] = 0.546274f * ( v.x * v.x - v.y * v.y ); // Y_22
}