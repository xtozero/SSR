static const float EPSILON = 1e-9;
static const float FLT_MAX = 3.402823466e+38;

static const int MAX_HALTON_SEQUENCE = 16;

static const float3 HALTON_SEQUENCE[MAX_HALTON_SEQUENCE] = {
	float3( 0.5, 0.333333, 0.2 ),
	float3( 0.25, 0.666667, 0.4 ),
	float3( 0.75, 0.111111, 0.6 ),
	float3( 0.125, 0.444444, 0.8 ),
	float3( 0.625, 0.777778, 0.04 ),
	float3( 0.375, 0.222222, 0.24 ),
	float3( 0.875, 0.555556, 0.44 ) ,
	float3( 0.0625, 0.888889, 0.64 ),
	float3( 0.5625, 0.037037, 0.84 ),
	float3( 0.3125, 0.37037, 0.08 ),
	float3( 0.8125, 0.703704, 0.28 ),
	float3( 0.1875, 0.148148, 0.48 ),
	float3( 0.6875, 0.481482, 0.68 ),
	float3( 0.4375, 0.814815, 0.88 ),
	float3( 0.9375, 0.259259, 0.12 ),
	float3( 0.03125, 0.592593, 0.32 )
};

static const float PI = 3.14159265359;
