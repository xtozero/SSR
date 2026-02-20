#include "VisibilityPassProcessor.h"

#include "Config/DefaultRenderCoreConfig.h"
#include "GuideTypes.h"
#include "MaterialResource.h"
#include "Scene/PrimitiveSceneInfo.h"
#include "VertexCollection.h"

namespace
{
    Owner<rendercore::IPassProcessor*> CreateVisibilityPassProcessor()
    {
        return new rendercore::VisibilityPassProcessor();
    }
}

namespace rendercore
{
    class VisibilityVS final : public GlobalShaderBase<VertexShader, VisibilityVS>
    {
        using GlobalShaderBase::GlobalShaderBase;
    };

    class VisibilityPS final : public GlobalShaderBase<PixelShader, VisibilityPS>
    {};

    REGISTER_GLOBAL_SHADER( VisibilityVS, "Visibility/VS_Visibility.fx", "main" );
    REGISTER_GLOBAL_SHADER( VisibilityPS, "Visibility/PS_Visibility.fx", "main" );

    std::optional<DrawSnapshot> VisibilityPassProcessor::ProcessInternal( const PrimitiveSubMesh& subMesh,
        const PassShader& passShader )
    {
        if ( ( subMesh.m_material == nullptr )
            || ( subMesh.m_material->SupportsVisibilityRendering() == false ) )
        {
            return {};
        }

        PassRenderOption passRenderOption;
        DepthStencilOption depthStencilOption;
        if ( const RenderOption* option = subMesh.m_renderOption )
        {
            if ( option->m_blendOption )
            {
                passRenderOption.m_blendOption = &( *option->m_blendOption );
            }

            if ( option->m_depthStencilOption )
            {
                depthStencilOption = *option->m_depthStencilOption;

                passRenderOption.m_depthStencilOption = &depthStencilOption;
            }

            if ( option->m_rasterizerOption )
            {
                passRenderOption.m_rasterizerOption = &( *option->m_rasterizerOption );
            }
        }

        if ( subMesh.m_vertexCollection )
        {
            const VertexStreamLayout& layout = subMesh.m_vertexCollection->VertexLayout( VertexStreamLayoutType::PositionOnly );
            if ( layout.Size() == 0 )
            {
                return {};
            }
        }

        depthStencilOption.m_depth.m_depthFunc = agl::ComparisonFunc::LessEqual;
        depthStencilOption.m_depth.m_writeDepth = false;
        return BuildDrawSnapshot( subMesh, passShader, passRenderOption, VertexStreamLayoutType::PositionOnly );
    }

    PassShader VisibilityPassProcessor::CollectPassShader( MaterialResource& material ) const
    {
        StaticShaderSwitches vsSwitches = VisibilityVS::GetSwitches();
        // StaticShaderSwitches msSwitches = VisibilityMS::GetSwitches();

        if ( DefaultRenderCore::IsTaaEnabled() )
        {
            vsSwitches.On( StaticName( "TAA" ), 1 );
            // msSwitches.On( Name( "TAA" ), 1 );
        }

        bool bUseMeshShader = material.UseMeshShader();

        PassShader passShader = {
            .m_vertexShader = VisibilityVS( vsSwitches ),
            // .m_vertexShader = bUseMeshShader ? nullptr : VisibilityVS( vsSwitches ),
            .m_pixelShader = VisibilityPS(),
            // .m_meshShader = bUseMeshShader ? VisibilityMS( msSwitches ) : nullptr,
            // .m_amplificationShader = bUseMeshShader ? DefaultAS() : nullptr,
        };

        return passShader;
    }

    PassProcessorRegister RegisterVisibilityPass( RenderPassType::Visibility, &CreateVisibilityPassProcessor );
}
