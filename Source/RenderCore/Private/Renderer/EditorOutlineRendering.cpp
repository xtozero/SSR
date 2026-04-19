#include "EditorOutlineRendering.h"

#include "Config/DefaultRenderCoreConfig.h"
#include "ConsoleMessage/ConVar.h"
#include "PrimitiveProxy.h"
#include "RenderGraph.h"
#include "Scene/IScene.h"
#include "Scene/PrimitiveSceneInfo.h"
#include "Shader.h"
#include "VertexCollection.h"

namespace
{
	void ParseEditorOutlineColor( const std::string& colorStr, float3& outColor )
	{
		std::vector<std::string> values = SplitString( colorStr, ' ' );
		if ( values.size() == 3 )
		{
			outColor[0] = std::stof( values[0] );
			outColor[1] = std::stof( values[1] );
			outColor[2] = std::stof( values[2] );
		}
	}
}

namespace rendercore
{
	engine::ConVariable CVarEditorOutlineWidth(
		"r.Editor.Outline.Width",
		"3",
		"Controls the thickness of outlines in the editor (in pixels)." );

	engine::ConVariable CVarEditorOutlineColor(
		"r.Editor.Outline.Color",
		"1.0 0.5 0.0",
		"Specifies the RGB color of the editor outline (format: R G B).");

    class EditorOutlineMaskingVS : public GlobalShaderBase<VertexShader, EditorOutlineMaskingVS>
    {
        using GlobalShaderBase::GlobalShaderBase;

    public:
        using PermutationType = ShaderPermutation<TAADim>;
    };

    class EditorOutlineMaskingPS : public GlobalShaderBase<PixelShader, EditorOutlineMaskingPS>
    {
    };

	class EditorOutlineCS : public GlobalShaderBase<ComputeShader, EditorOutlineCS>
	{
		DEFINE_SHADER_PARAM( OutlineColor );
		DEFINE_SHADER_PARAM( OutlineWidth );
		DEFINE_SHADER_PARAM( ScreenSize );

		DEFINE_SHADER_PARAM( OutlineMask );
		DEFINE_SHADER_PARAM( SceneColor );
	};

    REGISTER_GLOBAL_SHADER( EditorOutlineMaskingVS, "Common/EditorOutline.fx", "VSMain" );
    REGISTER_GLOBAL_SHADER( EditorOutlineMaskingPS, "Common/EditorOutline.fx", "PSMain" );
	REGISTER_GLOBAL_SHADER( EditorOutlineCS, "Common/EditorOutline.fx", "CSMain" );

	class EditorOutlinePassProcessor : public IPassProcessor
	{
	protected:
		std::optional<DrawSnapshot> ProcessInternal( const PrimitiveSubMesh& subMesh, const PassShader& passShader ) override
		{
			DepthStencilOption depthStencilOption;
			depthStencilOption.m_depth.m_enable = false;
			depthStencilOption.m_depth.m_writeDepth = false;

			PassRenderOption passRenderOption = {
				.m_depthStencilOption = &depthStencilOption,
			};

			return BuildDrawSnapshot( subMesh, passShader, passRenderOption, VertexStreamLayoutType::PositionOnly );
		}

		virtual PassShader CollectPassShader( [[maybe_unused]] MaterialResource& material ) const override
		{
			EditorOutlineMaskingVS::PermutationType vsPermutation;
			vsPermutation.SetValue<TAADim>( DefaultRenderCore::IsTaaEnabled() );

			PassShader passShader = {
				.m_vertexShader = EditorOutlineMaskingVS( vsPermutation ),
				.m_pixelShader = EditorOutlineMaskingPS(),
			};

			return passShader;
		}
	};

    void EditorOutlinePass::Render( RenderGraph& renderGraph, const EditorOutlineRenderParams& params )
	{
    	if ( params.IsValid() == false )
    	{
    		return;
    	}

		const auto& primitives = params.m_scene->Primitives();

		struct OutlineDrawItem
		{
			DrawSnapshot m_snapshot;
			uint32 m_primitiveId;
		};

		RenderFrameArray<OutlineDrawItem> items;

		EditorOutlinePassProcessor passProcessor;
		for ( const auto primitive : primitives )
		{
			const PrimitiveProxy* proxy = primitive->Proxy();

			if ( proxy->IsSelected() == false )
			{
				continue;
			}

			for ( const PrimitiveSubMesh& subMesh : primitive->SubMeshs() )
			{
				auto snapshot = passProcessor.Process( subMesh );
				if ( snapshot.has_value() )
				{
					items.emplace_back( *snapshot, primitive->PrimitiveId() );
				}
			}
		}

		if ( items.empty() )
		{
			return;
		}

		auto bufferSize = static_cast<uint32>( items.size() * sizeof( uint32 ) );
		VertexBuffer primitiveIdsBuffer = GetPrimitiveIdPool().Alloc( bufferSize );
		if ( auto dest = static_cast<uint32*>( primitiveIdsBuffer.Lock() ) )
		{
			for (size_t i = 0; i < items.size(); ++i)
			{
				dest[i] = items[i].m_primitiveId;
			}
			primitiveIdsBuffer.Unlock();
		}

		for ( OutlineDrawItem& item : items )
		{
			DrawSnapshot& snapshot = item.m_snapshot;
			params.m_resourceBinder->Bind( snapshot.m_pipelineState.m_shaderState, snapshot.m_shaderBindings );
		}

    	uint32 width = params.m_sceneColor->GetDesc().m_width;
    	uint32 height = params.m_sceneColor->GetDesc().m_height;

		agl::TextureDesc outlineMaskDesc = {
			.m_width = width,
			.m_height = height,
			.m_depth = 1,
			.m_sampleCount = 1,
			.m_sampleQuality = 0,
			.m_mipLevels = 1,
			.m_format = agl::ResourceFormat::R32_UINT,
			.m_access = agl::ResourceAccess::Default,
			.m_bindType = agl::ResourceBindType::RenderTarget | agl::ResourceBindType::ShaderResource,
			.m_miscFlag = agl::ResourceMisc::None,
			.m_clearValue = agl::ResourceClearValue{
				.m_color = { 0.f, 0.f, 0.f, 0.f }
			}
		};

		auto rgOutlineMask = renderGraph.CreateTexture( outlineMaskDesc, "EditorOutlineMask" );

		RasterOutput rasterOutput;
		rasterOutput.SetRenderTarget( 0, rgOutlineMask, RasterOutputLoadAction::Clear );
		rasterOutput.SetViewport( width, height );
		rasterOutput.SetScissorRect( width, height );

		renderGraph.AddPass(
			rasterOutput,
			[items, primitiveIdsBuffer]( CommandList& commandList ) mutable
			{
				for ( size_t i = 0; i < items.size(); ++i )
				{
					VisibleDrawSnapshot visibleSnapshot = {
						.m_primitiveId = items[i].m_primitiveId,
						.m_primitiveIdOffset = static_cast<uint32>( i ),
						.m_numInstance = 1,
						.m_snapshotBucketId = -1,
						.m_drawSnapshot = &items[i].m_snapshot,
					};
					CommitDrawSnapshot( commandList, visibleSnapshot, primitiveIdsBuffer );
				}
			});

		auto rgSceneColor = renderGraph.RegisterExternalResource( params.m_sceneColor.Get() );

		BEGIN_RG_RESOURCE_STRUCT( EditorOutlinePassResource )
			DECLARE_RG_TEXTURE_NONPIXEL_SRV( outlineMask )
			DECLARE_RG_TEXTURE_UAV( sceneColor )
		END_RG_RESOURCE_STRUCT();

		EditorOutlinePassResource passResource = {
			.m_outlineMask = rgOutlineMask,
			.m_sceneColor = rgSceneColor,
		};

    	uint32 screenSize[2] = { width, height };

    	renderGraph.AddPass(
    		passResource,
    		[passResource, screenSize]( ComputeCommandList& commandList )
    		{
    			EditorOutlineCS editorOutlineCS;

    			RefHandle<agl::ComputePipelineState> pso = PrepareComputePipelineState( editorOutlineCS );

    			commandList.BindPipelineState( pso.Get() );

    			agl::ShaderBindings shaderBindings = CreateShaderBindings( editorOutlineCS );
				BindResource( shaderBindings, editorOutlineCS.OutlineMask(), passResource.m_outlineMask->Get() );
    			BindResource( shaderBindings, editorOutlineCS.SceneColor(), passResource.m_sceneColor->Get() );

    			float3 outlineColor = { 0.f, 0.f, 0.f };
    			ParseEditorOutlineColor( CVarEditorOutlineColor.GetString(), outlineColor );
    			SetShaderValue( commandList, editorOutlineCS.OutlineColor(), outlineColor );
    			SetShaderValue( commandList, editorOutlineCS.OutlineWidth(), CVarEditorOutlineWidth.GetFloat() );
    			SetShaderValue( commandList, editorOutlineCS.ScreenSize(), screenSize );

				commandList.BindShaderResources( shaderBindings );

    			uint32 numGroupX = CalcAlignment<uint32>( screenSize[0], 8 ) / 8;
    			uint32 numGroupY = CalcAlignment<uint32>( screenSize[1], 4 ) / 4;
    			commandList.Dispatch( numGroupX, numGroupY );
    		});
	}
}
