#if SupportsBindless == 1
Texture2D Tex2D[] : register(t0, space100);
Texture2DArray Tex2DArray[] : register(t0, space101);
TextureCube TexCube[] : register(t0, space102);
Texture3D Tex3D[] : register(t0, space103);

SamplerState Samplers[] : register(s0, space100);

#define DefineBindlessIndices cbuffer BindlessIndices : register( b0 )
#endif