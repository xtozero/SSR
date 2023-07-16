#include "UserInterfaceRenderer.h"

#include "AssetLoader.h"
#include "GlobalShaders.h"
#include "GraphicsApiResource.h"
#include "imgui.h"
#include "PassProcessor.h"
#include "RenderOption.h"
#include "SizedTypes.h"
#include "Texture.h"
#include "VertexCollection.h"

namespace rendercore
{
	class DrawImguiVS : public GlobalShaderCommon<VertexShader, DrawImguiVS>
	{};

	class DrawImguiPS : public GlobalShaderCommon<PixelShader, DrawImguiPS>
	{};

	REGISTER_GLOBAL_SHADER( DrawImguiVS, "./Assets/Shaders/VS_Imgui.asset" );
	REGISTER_GLOBAL_SHADER( DrawImguiPS, "./Assets/Shaders/PS_Imgui.asset" );

	class ImguiDrawPassProcessor : public IPassProcessor
	{
	public:
		virtual std::optional<DrawSnapshot> Process( const PrimitiveSubMesh& subMesh ) override;

		ImguiDrawPassProcessor();

	private:
		BlendOption m_blendOption;
		RasterizerOption m_rasterizerOption;
		DepthStencilOption m_depthStencilOption;
	};

	struct ImguiDrawCommand
	{
		RectangleArea<float> m_clipRect;
		ImTextureID m_textureId;
		uint32 m_vertexOffset;
		uint32 m_indexOffset;
		uint32 m_numElem;
		ImDrawCallback m_userCallback;
		void* m_userCallbackData;
	};

	struct ImguiDrawList
	{
		int32 m_numVertex;
		int32 m_numIndex;

		std::vector<ImguiDrawCommand> m_drawCommands;
	};

	struct ImguiDrawInfo
	{
		explicit ImguiDrawInfo( const ImDrawData& drawData ) noexcept
			: m_totalNumVertex( drawData.TotalVtxCount )
			, m_totalNumIndex( drawData.TotalIdxCount )
			, m_displayPos( drawData.DisplayPos.x, drawData.DisplayPos.y )
			, m_displaySize( drawData.DisplaySize.x, drawData.DisplaySize.y )
			, m_framebufferScale( drawData.FramebufferScale.x, drawData.FramebufferScale.y )
		{
			m_drawLists.reserve( drawData.CmdListsCount );

			m_positions.resize( m_totalNumVertex );
			m_texCoords.resize( m_totalNumVertex );
			m_colors.resize( m_totalNumVertex );
			m_indices.resize( m_totalNumIndex );

			size_t verticesCopyOffset = 0;
			size_t indicesCopyOffset = 0;

			for ( int32 drawListIdx = 0; drawListIdx < drawData.CmdListsCount; ++drawListIdx )
			{
				const ImDrawList* cmdList = drawData.CmdLists[drawListIdx];
				auto& drawList = m_drawLists.emplace_back();

				drawList.m_numVertex = cmdList->VtxBuffer.Size;
				drawList.m_numIndex = cmdList->IdxBuffer.Size;

				for ( int32 vertexIdx = 0; vertexIdx < drawList.m_numVertex; ++vertexIdx )
				{
					size_t destIdx = verticesCopyOffset + vertexIdx;
					const ImDrawVert& vertex = cmdList->VtxBuffer[vertexIdx];

					m_positions[destIdx].x = vertex.pos.x;
					m_positions[destIdx].y = vertex.pos.y;
					m_texCoords[destIdx].x = vertex.uv.x;
					m_texCoords[destIdx].y = vertex.uv.y;
					m_colors[destIdx] = vertex.col;
				}

				std::memcpy( m_indices.data() + indicesCopyOffset, cmdList->IdxBuffer.Data, sizeof( ImDrawIdx ) * drawList.m_numIndex );

				verticesCopyOffset += drawList.m_numVertex;
				indicesCopyOffset += drawList.m_numIndex;

				drawList.m_drawCommands.reserve( cmdList->CmdBuffer.Size );
				for ( int32 drawCmdIdx = 0; drawCmdIdx < cmdList->CmdBuffer.Size; ++drawCmdIdx )
				{
					const ImDrawCmd* cmd = &cmdList->CmdBuffer[drawCmdIdx];
					RectangleArea<float> clipRect = {
						.m_left = cmd->ClipRect.x - m_displayPos.x,
						.m_top = cmd->ClipRect.y - m_displayPos.y,
						.m_right = cmd->ClipRect.z - m_displayPos.x,
						.m_bottom = cmd->ClipRect.w - m_displayPos.y
					};

					if ( clipRect.Valid() == false )
					{
						continue;
					}

					auto& drawCommand = drawList.m_drawCommands.emplace_back();

					drawCommand.m_clipRect = clipRect;
					drawCommand.m_textureId = cmd->TextureId;
					drawCommand.m_vertexOffset = cmd->VtxOffset;
					drawCommand.m_indexOffset = cmd->IdxOffset;
					drawCommand.m_numElem = cmd->ElemCount;
					drawCommand.m_userCallback = cmd->UserCallback;
					drawCommand.m_userCallbackData = cmd->UserCallbackData;
				}
			}
		}

		ImguiDrawInfo() = default;

		uint32 m_totalNumVertex = 0;
		uint32 m_totalNumIndex = 0;

		std::vector<Vector2> m_positions;
		std::vector<Vector2> m_texCoords;
		std::vector<uint32> m_colors;
		std::vector<uint16> m_indices;

		std::vector<ImguiDrawList> m_drawLists;

		Vector2 m_displayPos;
		Vector2 m_displaySize;
		Vector2 m_framebufferScale;
	};

	struct ImguiRenderResource
	{
		VertexCollection m_vertexCollection;
		IndexBuffer m_indexBuffer;
		agl::RefHandle<agl::Texture> m_fontAtlas;
	};

	class ImguiRenderer : public UserInterfaceRenderer
	{
	public:
		virtual bool BootUp() override;
		virtual void Render( RenderViewGroup& renderViewGroup ) override;

		virtual void UpdateUIDrawInfo() override;

	private:
		void CreateFontsAtlas();
		void UpdateRenderResource();

		ImguiDrawPassProcessor m_drawPassProcessor;
		ImguiDrawInfo m_imguiDrawInfo;
		ImguiRenderResource m_imguiRenderResource;

		bool m_renderResourceCreated = false;
	};

	bool ImguiRenderer::BootUp()
	{
		CreateFontsAtlas();

		return true;
	}

	void ImguiRenderer::Render( RenderViewGroup& renderViewGroup )
	{
		assert( IsInRenderThread() );
	}

	void ImguiRenderer::UpdateUIDrawInfo()
	{
		assert( IsInGameThread() );
		ImDrawData* drawData = ImGui::GetDrawData();
		if ( drawData == nullptr )
		{
			return;
		}

		ImguiDrawInfo imguiDrawInfo( *drawData );

		EnqueueRenderTask( [this, drawInfo = std::move( imguiDrawInfo )]() mutable
			{
				m_imguiDrawInfo = std::move( drawInfo );
				UpdateRenderResource();
			} );
	}

	void ImguiRenderer::CreateFontsAtlas()
	{
		ImGuiIO& io = ImGui::GetIO();

		unsigned char* pixels;
		int32 width, height;
		io.Fonts->GetTexDataAsRGBA32( &pixels, &width, &height );

		agl::TextureTrait trait = {
			.m_width = static_cast<uint32>( width ),
			.m_height = static_cast<uint32>( height ),
			.m_depth = 1,
			.m_sampleCount = 1,
			.m_sampleQuality = 0,
			.m_mipLevels = 1,
			.m_format = agl::ResourceFormat::R8G8B8A8_UNORM,
			.m_access = agl::ResourceAccessFlag::GpuRead | agl::ResourceAccessFlag::GpuWrite,
			.m_bindType = agl::ResourceBindType::ShaderResource,
			.m_miscFlag = agl::ResourceMisc::None
		};

		const uint32 formatSize = agl::ResourceFormatSize( trait.m_format );

		agl::ResourceInitData initData;
		initData.m_srcData = pixels;
		initData.m_srcSize = width * height * formatSize;

		initData.m_sections.emplace_back(
			agl::ResourceSectionData{
				.m_offset = 0,
				.m_pitch = width * formatSize,
				.m_slicePitch = width * height * formatSize
			}
		);

		m_imguiRenderResource.m_fontAtlas = agl::Texture::Create( trait, &initData );

		EnqueueRenderTask( [texture = m_imguiRenderResource.m_fontAtlas]()
			{
				texture->Init();
			} );

		static_assert( sizeof( ImTextureID ) >= sizeof( m_imguiRenderResource.m_fontAtlas.Get() ), "Can't pack descriptor handle into TexID" );
		io.Fonts->SetTexID( (ImTextureID)m_imguiRenderResource.m_fontAtlas.Get() );
	}

	void ImguiRenderer::UpdateRenderResource()
	{
		if ( ( m_imguiDrawInfo.m_totalNumVertex == 0 )
			|| ( m_imguiDrawInfo.m_totalNumIndex == 0 ) )
		{
			return;
		}

		auto& vertexCollection = m_imguiRenderResource.m_vertexCollection;
		static Name positionName( "POSITION" );
		static Name colorName( "COLOR" );
		static Name texCoordName( "TEXCOORD" );

		if ( m_renderResourceCreated == false )
		{
			agl::VertexLayoutTrait trait[agl::MAX_VERTEX_LAYOUT_SIZE] = {};
			uint32 traitSize = 0;
			uint32 slot = 0;

			// Only Position
			{
				VertexStream posStream( "POSITION", agl::ResourceFormat::R32G32_FLOAT, m_imguiDrawInfo.m_totalNumVertex, true );

				vertexCollection.AddStream( std::move( posStream ) );

				trait[traitSize++] = {
					.m_isInstanceData = false,
					.m_index = 0,
					.m_format = agl::ResourceFormat::R32G32_FLOAT,
					.m_slot = slot++,
					.m_instanceDataStep = 0,
					.m_name = positionName
				};

				vertexCollection.InitLayout( trait, traitSize, VertexStreamLayoutType::PositionOnly );
			}

			// Color
			{
				VertexStream colorStream( "COLOR", agl::ResourceFormat::R8G8B8A8_UNORM, m_imguiDrawInfo.m_totalNumVertex, true );

				vertexCollection.AddStream( std::move( colorStream ) );

				trait[traitSize++] = {
					.m_isInstanceData = false,
					.m_index = 0,
					.m_format = agl::ResourceFormat::R8G8B8A8_UNORM,
					.m_slot = slot++,
					.m_instanceDataStep = 0,
					.m_name = colorName
				};
			}

			// Default
			{
				VertexStream texCoordStream( "TEXCOORD", agl::ResourceFormat::R32G32_FLOAT, m_imguiDrawInfo.m_totalNumVertex, true );

				vertexCollection.AddStream( std::move( texCoordStream ) );

				trait[traitSize++] = {
					.m_isInstanceData = false,
					.m_index = 0,
					.m_format = agl::ResourceFormat::R32G32_FLOAT,
					.m_slot = slot++,
					.m_instanceDataStep = 0,
					.m_name = texCoordName
				};
			}

			vertexCollection.InitLayout( trait, traitSize, VertexStreamLayoutType::Default );

			vertexCollection.InitResource();

			std::construct_at( &m_imguiRenderResource.m_indexBuffer, m_imguiDrawInfo.m_totalNumIndex, nullptr, false, true );

			m_renderResourceCreated = true;
		}

		if ( VertexBuffer* positionBuffer = vertexCollection.GetVertexBuffer( positionName ) )
		{
			positionBuffer->Resize( m_imguiDrawInfo.m_totalNumVertex, false );
			void* positionData = positionBuffer->Lock();

			std::memcpy( positionData, m_imguiDrawInfo.m_positions.data(), positionBuffer->Size() );

			positionBuffer->Unlock();
		}

		if ( VertexBuffer* colorBuffer = vertexCollection.GetVertexBuffer( colorName ) )
		{
			colorBuffer->Resize( m_imguiDrawInfo.m_totalNumVertex, false );
			void* colorData = colorBuffer->Lock();

			std::memcpy( colorData, m_imguiDrawInfo.m_colors.data(), colorBuffer->Size() );

			colorBuffer->Unlock();
		}

		if ( VertexBuffer* texCoordBuffer = vertexCollection.GetVertexBuffer( texCoordName ) )
		{
			texCoordBuffer->Resize( m_imguiDrawInfo.m_totalNumVertex, false );
			void* texCoordData = texCoordBuffer->Lock();

			std::memcpy( texCoordData, m_imguiDrawInfo.m_texCoords.data(), texCoordBuffer->Size() );

			texCoordBuffer->Unlock();
		}

		IndexBuffer& indexBuffer = m_imguiRenderResource.m_indexBuffer;
		indexBuffer.Resize( m_imguiDrawInfo.m_totalNumIndex, false );

		void* indexData = indexBuffer.Lock();

		std::memcpy( indexData, m_imguiDrawInfo.m_indices.data(), indexBuffer.Size() );

		indexBuffer.Unlock();
	}

	Owner<UserInterfaceRenderer*> CreateUserInterfaceRenderer()
	{
		return new ImguiRenderer();
	}

	void DestoryUserInterfaceRenderer( Owner<UserInterfaceRenderer*> uiRenderer )
	{
		delete uiRenderer;
	}

	std::optional<DrawSnapshot> ImguiDrawPassProcessor::Process( const PrimitiveSubMesh& subMesh )
	{
		PassShader passShader = {
			.m_vertexShader = DrawImguiVS().GetShader(),
			.m_geometryShader = nullptr,
			.m_pixelShader = DrawImguiPS().GetShader()
		};

		PassRenderOption passRenderOption = {
			.m_blendOption = &m_blendOption,
			.m_depthStencilOption = &m_depthStencilOption,
			.m_rasterizerOption = &m_rasterizerOption
		};

		return std::optional<DrawSnapshot>();
	}

	ImguiDrawPassProcessor::ImguiDrawPassProcessor()
	{
		for ( RenderTargetBlendOption& rtBlendOption : m_blendOption.m_renderTarget )
		{
			rtBlendOption.m_blendEnable = true;
			rtBlendOption.m_srcBlend = agl::Blend::SrcAlpha;
			rtBlendOption.m_destBlend = agl::Blend::InvSrcAlpha;
			rtBlendOption.m_blendOp = agl::BlendOp::Add;
			rtBlendOption.m_srcBlendAlpha = agl::Blend::One;
			rtBlendOption.m_destBlendAlpha = agl::Blend::InvSrcAlpha;
			rtBlendOption.m_blendOpAlpha = agl::BlendOp::Add;
			rtBlendOption.m_renderTargetWriteMask = agl::ColorWriteEnable::All;
		}

		m_rasterizerOption.m_isWireframe = false;
		m_rasterizerOption.m_cullMode = agl::CullMode::None;
		m_rasterizerOption.m_counterClockwise = false;
		m_rasterizerOption.m_depthBias = 0;
		m_rasterizerOption.m_depthClipEnable = true;
		m_rasterizerOption.m_scissorEnable = true;
		m_rasterizerOption.m_multisampleEnable = false;
		m_rasterizerOption.m_antialiasedLineEnable = false;

		m_depthStencilOption.m_depth.m_enable = false;
		m_depthStencilOption.m_stencil.m_enable = false;
	}
}