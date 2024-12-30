struct PS_INPUT
{
	float4 position : SV_POSITION;
};

float4 HitProxyId;

float4 main( PS_INPUT input ) : SV_Target0
{
	return float4( HitProxyId.rgb, 1.f ); 
}