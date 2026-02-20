#include "RTAORendering.h"

#include "GlobalShaders.h"

namespace rendercore
{
    class RTAORayGen : public GlobalShaderBase<RayGenerationShader, RTAORayGen>
    {
    };

    class RTAOClosestHit : public GlobalShaderBase<ClosestHitShader, RTAOClosestHit>
    {
    };

    class RTAOMiss : public GlobalShaderBase<MissShader, RTAOMiss>
    {
    };

    REGISTER_GLOBAL_SHADER( RTAORayGen, "RayTracing/RTAO.fx", "RayGen" );
    REGISTER_GLOBAL_SHADER( RTAOClosestHit, "RayTracing/RTAO.fx", "ClosestHit" );
    REGISTER_GLOBAL_SHADER( RTAOMiss, "RayTracing/RTAO.fx", "Miss" );
}