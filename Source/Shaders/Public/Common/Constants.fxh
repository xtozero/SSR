static const float EPSILON = 1e-9;
static const float FLT_MAX = 3.402823466e+38;

static const int MAX_HALTON_SEQUENCE = 16;

static const float2 HALTON_SEQUENCE[MAX_HALTON_SEQUENCE] = {
	float2( 0.5, 0.333333 ),
	float2( 0.25, 0.666667 ),
	float2( 0.75, 0.111111 ),
	float2( 0.125, 0.444444 ),
	float2( 0.625, 0.777778 ),
	float2( 0.375, 0.222222 ),
	float2( 0.875, 0.555556 ) ,
	float2( 0.0625, 0.888889 ),
	float2( 0.5625, 0.037037 ),
	float2( 0.3125, 0.37037 ),
	float2( 0.8125, 0.703704 ),
	float2( 0.1875, 0.148148 ),
	float2( 0.6875, 0.481482 ),
	float2( 0.4375, 0.814815 ),
	float2( 0.9375, 0.259259 ),
	float2( 0.03125, 0.592593 )
};