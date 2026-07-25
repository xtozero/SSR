#include "UserInterfaceRenderer.h"

#include "Canvas.h"
#include "CommandList.h"
#include "Config/DefaultRenderCoreConfig.h"
#include "CpuProfiler/CpuProfiler.h"
#include "GlobalShaders.h"
#include "GraphicsApiResource.h"
#include "IRenderCore.h"
#include "MeshDrawInfo.h"
#include "PassProcessor.h"
#include "RenderGraph.h"
#include "RenderOption.h"
#include "ResourceBarrierUtils.h"
#include "Scene/PrimitiveSceneInfo.h"
#include "SceneRenderer.h"
#include "ShaderParameterUtils.h"
#include "SizedTypes.h"
#include "StaticState.h"
#include "Texture.h"
#include "VertexCollection.h"

#include <imgui.h>

namespace rendercore
{
	class DrawImguiVS final : public GlobalShaderBase<VertexShader, DrawImguiVS>
	{
		using GlobalShaderBase::GlobalShaderBase;
	};

	class DrawImguiPS final : public GlobalShaderBase<PixelShader, DrawImguiPS>
	{};

	REGISTER_GLOBAL_SHADER( DrawImguiVS, "Material/VS_Imgui.fx", "main" );
	REGISTER_GLOBAL_SHADER( DrawImguiPS, "Material/PS_Imgui.fx", "main" );

	class ImguiDrawPassProcessor final : public IPassProcessor
	{
	public:
		ImguiDrawPassProcessor();

	protected:
		virtual std::optional<DrawSnapshot> ProcessInternal( const PrimitiveSubMesh& subMesh, const PassShader& passShader ) override;
		virtual PassShader CollectPassShader( MaterialResource& material ) const override;

	private:
		BlendOption m_blendOption;
		RasterizerOption m_rasterizerOption;
		DepthStencilOption m_depthStencilOption;
	};

	struct ImguiDrawCommand final
	{
		RectangleArea<int32> m_clipRect;
		ImTextureID m_textureId;
		uint32 m_vertexOffset;
		uint32 m_indexOffset;
		uint32 m_numElem;
		ImDrawCallback m_userCallback;
		void* m_userCallbackData;
	};

	struct ImguiDrawList final
	{
		int32 m_numVertex = 0;
		int32 m_numIndex = 0;

		std::vector<ImguiDrawCommand> m_drawCommands;
	};

	struct ImguiDrawInfo final
	{
		ImguiDrawInfo( Canvas& canvas, const ImDrawData& drawData ) noexcept
			: m_canvas( &canvas )
			, m_totalNumVertex( drawData.TotalVtxCount )
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

			uint32 vertexCopyOffset = 0;
			uint32 indexCopyOffset = 0;

			for ( int32 drawListIdx = 0; drawListIdx < drawData.CmdListsCount; ++drawListIdx )
			{
				uint32 vertexDrawOffset = vertexCopyOffset;
				uint32 indexDrawOffset = indexCopyOffset;

				const ImDrawList* cmdList = drawData.CmdLists[drawListIdx];
				auto& drawList = m_drawLists.emplace_back();

				drawList.m_numVertex = cmdList->VtxBuffer.Size;
				drawList.m_numIndex = cmdList->IdxBuffer.Size;

				for ( int32 vertexIdx = 0; vertexIdx < drawList.m_numVertex; ++vertexIdx )
				{
					uint32 destIdx = vertexCopyOffset + vertexIdx;
					const ImDrawVert& vertex = cmdList->VtxBuffer[vertexIdx];

					m_positions[destIdx].x = vertex.pos.x;
					m_positions[destIdx].y = vertex.pos.y;
					m_texCoords[destIdx].x = vertex.uv.x;
					m_texCoords[destIdx].y = vertex.uv.y;
					m_colors[destIdx] = vertex.col;
				}

				std::memcpy( m_indices.data() + indexCopyOffset, cmdList->IdxBuffer.Data, sizeof( ImDrawIdx ) * drawList.m_numIndex );

				vertexCopyOffset += drawList.m_numVertex;
				indexCopyOffset += drawList.m_numIndex;

				drawList.m_drawCommands.reserve( cmdList->CmdBuffer.Size );
				for ( int32 drawCmdIdx = 0; drawCmdIdx < cmdList->CmdBuffer.Size; ++drawCmdIdx )
				{
					const ImDrawCmd* cmd = &cmdList->CmdBuffer[drawCmdIdx];
					RectangleArea<int32> clipRect = {
						.m_left = static_cast<int32>( cmd->ClipRect.x - m_displayPos.x ),
						.m_top = static_cast<int32>( cmd->ClipRect.y - m_displayPos.y ),
						.m_right = static_cast<int32>( cmd->ClipRect.z - m_displayPos.x ),
						.m_bottom = static_cast<int32>( cmd->ClipRect.w - m_displayPos.y )
					};

					if ( clipRect.Valid() == false )
					{
						continue;
					}

					auto& drawCommand = drawList.m_drawCommands.emplace_back();

					drawCommand.m_clipRect = clipRect;
					drawCommand.m_textureId = cmd->TextureId;
					drawCommand.m_vertexOffset = vertexDrawOffset + cmd->VtxOffset;
					drawCommand.m_indexOffset = indexDrawOffset + cmd->IdxOffset;
					drawCommand.m_numElem = cmd->ElemCount;
					drawCommand.m_userCallback = cmd->UserCallback;
					drawCommand.m_userCallbackData = cmd->UserCallbackData;
				}
			}
		}

		ImguiDrawInfo() noexcept = default;

		Canvas* m_canvas = nullptr;

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

	struct ImguiRenderResource final
	{
		VertexCollection m_vertexCollection;
		IndexBuffer m_indexBuffer;
		bool m_initialized = false;

		void InitResource();
	};

	class ImguiRenderer final : public UserInterfaceRenderer
	{
	public:
		virtual bool BootUp() override;

		virtual void Render( RenderGraph& renderGraph ) override;

		virtual void UpdateUIDrawInfo( Canvas& canvas ) override;

		static void OnCreateWindow( ImGuiViewport* viewport );
		static void OnDestroyWindow( ImGuiViewport* viewport );
		static void OnSetWindowSize( ImGuiViewport* viewport, ImVec2 size );

		virtual ~ImguiRenderer() override;

	private:
		void Shutdown();

		void CreateFontsAtlas();
		void UpdateRenderResource();

		void CreateMultiViewportCanvas( ImGuiViewport& viewport );
		void DestroyMultiViewportCanvas( ImGuiViewport& viewport );
		void ResizeMultiViewportCanvas( ImGuiViewport& viewport, ImVec2 size );

		void SwapMultiViewportBuffers();

		static agl::ShaderParameter ProjectionMatrixShaderParam;

		ImguiDrawPassProcessor m_drawPassProcessor;
		std::vector<ImguiDrawInfo> m_imguiDrawInfo;
		std::vector<ImguiRenderResource> m_imguiRenderResource;
		std::vector<std::shared_ptr<Canvas>> m_multiViewportCanvas;

		RefHandle<agl::Texture> m_fontAtlas;
		SamplerState m_fontAtlasSampler;

		DelegateHandle m_onEndFrameRenderingHandle;
	};

	agl::ShaderParameter ImguiRenderer::ProjectionMatrixShaderParam( agl::ShaderType::Vertex, agl::ShaderParameterType::ConstantBufferValue, 0, 0, 0, sizeof( Matrix ) );

	void ImguiRenderResource::InitResource()
	{
		if ( m_initialized )
		{
			return;
		}

		static Name positionName( "POSITION" );
		static Name colorName( "COLOR" );
		static Name texCoordName( "TEXCOORD" );

		agl::VertexLayoutData layoutData[agl::MaxVertexLayouts] = {};
		uint32 numLayoutData = 0;
		uint32 slot = 0;

		// Only Position
		{
			VertexStream posStream( "POSITION", agl::ResourceFormat::R32G32_FLOAT, 0, true );

			m_vertexCollection.AddStream( std::move( posStream ) );

			layoutData[numLayoutData++] = {
				.m_isInstanceData = false,
				.m_index = 0,
				.m_format = agl::ResourceFormat::R32G32_FLOAT,
				.m_slot = slot++,
				.m_instanceDataStep = 0,
				.m_name = positionName
			};

			m_vertexCollection.InitLayout( layoutData, numLayoutData, VertexStreamLayoutType::PositionOnly );
		}

		// Color
		{
			VertexStream colorStream( "COLOR", agl::ResourceFormat::R8G8B8A8_UNORM, 0, true );

			m_vertexCollection.AddStream( std::move( colorStream ) );

			layoutData[numLayoutData++] = {
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
			VertexStream texCoordStream( "TEXCOORD", agl::ResourceFormat::R32G32_FLOAT, 0, true );

			m_vertexCollection.AddStream( std::move( texCoordStream ) );

			layoutData[numLayoutData++] = {
				.m_isInstanceData = false,
				.m_index = 0,
				.m_format = agl::ResourceFormat::R32G32_FLOAT,
				.m_slot = slot++,
				.m_instanceDataStep = 0,
				.m_name = texCoordName
			};
		}

		m_vertexCollection.InitLayout( layoutData, numLayoutData, VertexStreamLayoutType::Default );
		m_vertexCollection.InitResource();

		std::construct_at( &m_indexBuffer, 0, agl::ResourceState::GenericRead, nullptr, false, true );

		m_initialized = true;
	}

	bool ImguiRenderer::BootUp()
	{
		CreateFontsAtlas();

		ImGuiIO& io = ImGui::GetIO();
		io.BackendRendererUserData = this;

		ImGuiPlatformIO& platformIO = ImGui::GetPlatformIO();

		platformIO.Renderer_CreateWindow = &ImguiRenderer::OnCreateWindow;
		platformIO.Renderer_DestroyWindow = &ImguiRenderer::OnDestroyWindow;
		platformIO.Renderer_SetWindowSize = &ImguiRenderer::OnSetWindowSize;

		m_onEndFrameRenderingHandle = IRenderCore::OnEndFrameRendering.AddMemberFunction( this, &ImguiRenderer::SwapMultiViewportBuffers );

		return true;
	}

	void ImguiRenderer::Render( RenderGraph& renderGraph )
	{
		CPU_PROFILE( ImguiRenderer_Render );

		assert( IsInRenderThread() );

		if ( m_imguiDrawInfo.empty() )
		{
			return;
		}

		for ( size_t i = 0; i < m_imguiDrawInfo.size(); ++i )
		{
			if ( ( m_imguiDrawInfo[i].m_displaySize.x <= 0.f )
				|| ( m_imguiDrawInfo[i].m_displaySize.y <= 0.f ) )
			{
				continue;
			}

			if ( m_imguiDrawInfo[i].m_canvas == nullptr )
			{
				continue;
			}

			Canvas& canvas = *m_imguiDrawInfo[i].m_canvas;

			agl::Texture* canvasTexture = canvas.Texture();
			if ( ( canvasTexture == nullptr ) || ( canvasTexture->RTV() == nullptr ) )
			{
				return;
			}

			auto rgCanvasTexture = renderGraph.RegisterExternalResource( canvasTexture );

			auto [width, height] = canvas.Size();

			RasterOutput rasterOutput;
			rasterOutput.SetRenderTarget( 0, rgCanvasTexture );
			rasterOutput.SetViewport( width, height );

			renderGraph.AddPass(
				rasterOutput,
				[this, i]( CommandList& commandList ) mutable
				{
					const ImguiDrawInfo& imguiDrawInfo = m_imguiDrawInfo[i];
					if ( i > 0 )
					{
						imguiDrawInfo.m_canvas->OnBeginFrameRendering();
					}

					float left = imguiDrawInfo.m_displayPos.x;
					float right = imguiDrawInfo.m_displayPos.x + imguiDrawInfo.m_displaySize.x;
					float top = imguiDrawInfo.m_displayPos.y;
					float bottom = imguiDrawInfo.m_displayPos.y + imguiDrawInfo.m_displaySize.y;
					Matrix imguiProjection(
						2.0f / ( right - left ), 0.0f, 0.0f, 0.0f,
						0.0f, 2.0f / (top - bottom), 0.0f, 0.0f,
						0.0f, 0.0f, 0.5f, 0.0f,
						( right + left ) / ( left - right ), ( top + bottom ) / ( bottom - top ), 0.5f, 1.0f );

					for ( const ImguiDrawList& drawList : imguiDrawInfo.m_drawLists )
					{
						for ( const ImguiDrawCommand& drawCommand : drawList.m_drawCommands )
						{
							MeshDrawInfo drawinfo{
								.m_vertexCollection = &m_imguiRenderResource[i].m_vertexCollection,
								.m_indexBuffer = &m_imguiRenderResource[i].m_indexBuffer,
								.m_material = nullptr,
								.m_renderOption = nullptr,
								.m_startLocation = drawCommand.m_indexOffset,
								.m_baseVertexLocation = drawCommand.m_vertexOffset,
								.m_count = drawCommand.m_numElem,
								.m_lod = 0,
								.m_sectionIndex = 0,
							};

							PrimitiveSubMesh subMesh( drawinfo );
							auto result = m_drawPassProcessor.Process( subMesh );

							if ( result.has_value() == false )
							{
								continue;
							}

							DrawSnapshot& snapshot = *result;

							SetShaderValue( commandList, ProjectionMatrixShaderParam, imguiProjection );

							auto texture = reinterpret_cast<agl::Texture*>(drawCommand.m_textureId);
							commandList.AddTransition( Transition( *texture, agl::ResourceState::PixelShaderResource ) );

							ResourceBinder resourceBinder;
							resourceBinder.Add( StaticName( "texture0" ), texture->SRV() );
							resourceBinder.Add( StaticName( "sampler0" ), m_fontAtlasSampler.Resource() );

							resourceBinder.Bind( snapshot.m_pipelineState.m_shaderState, snapshot.m_shaderBindings );

							commandList.SetScissorRects( 1, &drawCommand.m_clipRect );

							AddSingleDrawPass( commandList, snapshot );
						}
					}

					if ( i > 0 )
					{
						imguiDrawInfo.m_canvas->OnEndFrameRendering();
					}

					// Submit the current command list and begin recording a new one for a different swap chain.
					RenderGraph::Commit();
				} );
		}
	}

	void ImguiRenderer::UpdateUIDrawInfo( Canvas& canvas )
	{
		assert( IsInGameThread() );
		ImGuiPlatformIO& platformIO = ImGui::GetPlatformIO();
		if (platformIO.Viewports.empty())
		{
			return;
		}

		std::vector<ImguiDrawInfo> imguiDrawInfo;
		for ( const ImGuiViewport* viewport : platformIO.Viewports )
		{
			ImDrawData* drawData = viewport->DrawData;
			assert( drawData != nullptr );

			Canvas& viewportCanvas = viewport->RendererUserData ? *static_cast<Canvas*>( viewport->RendererUserData ) : canvas;
			imguiDrawInfo.emplace_back( viewportCanvas, *drawData );
		}

		if ( imguiDrawInfo.empty() )
		{
			return;
		}

		EnqueueRenderTask(
			[this, drawInfo = std::move( imguiDrawInfo )]() mutable
			{
				m_imguiDrawInfo = std::move( drawInfo );
				UpdateRenderResource();
			} );
	}

	void ImguiRenderer::OnCreateWindow( ImGuiViewport* viewport )
	{
		ImGuiIO& io = ImGui::GetIO();
		auto& imguiRenderer = *static_cast<ImguiRenderer*>( io.BackendRendererUserData );

		imguiRenderer.CreateMultiViewportCanvas( *viewport );
	}

	void ImguiRenderer::OnDestroyWindow( ImGuiViewport* viewport )
	{
		ImGuiIO& io = ImGui::GetIO();
		auto& imguiRenderer = *static_cast<ImguiRenderer*>( io.BackendRendererUserData );

		imguiRenderer.DestroyMultiViewportCanvas( *viewport );
	}

	void ImguiRenderer::OnSetWindowSize( ImGuiViewport* viewport, ImVec2 size )
	{
		ImGuiIO& io = ImGui::GetIO();
		auto& imguiRenderer = *static_cast<ImguiRenderer*>( io.BackendRendererUserData );

		imguiRenderer.ResizeMultiViewportCanvas( *viewport, size );
	}

	ImguiRenderer::~ImguiRenderer()
	{
		Shutdown();
	}

	void ImguiRenderer::Shutdown()
	{
		ImGuiIO& io = ImGui::GetIO();
		io.BackendRendererUserData = nullptr;

		ImGuiPlatformIO& platformIO = ImGui::GetPlatformIO();

		platformIO.Renderer_CreateWindow = nullptr;
		platformIO.Renderer_DestroyWindow = nullptr;
		platformIO.Renderer_SetWindowSize = nullptr;

		for ( ImGuiViewport* viewport : platformIO.Viewports )
		{
			viewport->RendererUserData = nullptr;
		}

		IRenderCore::OnEndFrameRendering.Remove( m_onEndFrameRenderingHandle );
	}

	void ImguiRenderer::CreateFontsAtlas()
	{
		ImGuiIO& io = ImGui::GetIO();

		unsigned char* pixels;
		int32 width, height;
		io.Fonts->GetTexDataAsRGBA32( &pixels, &width, &height );

		agl::TextureDesc desc = {
			.m_width = static_cast<uint32>( width ),
			.m_height = static_cast<uint32>( height ),
			.m_depth = 1,
			.m_sampleCount = 1,
			.m_sampleQuality = 0,
			.m_mipLevels = 1,
			.m_format = agl::ResourceFormat::R8G8B8A8_UNORM,
			.m_access = agl::ResourceAccess::Default,
			.m_bindType = agl::ResourceBindType::ShaderResource,
			.m_miscFlag = agl::ResourceMisc::None
		};

		const uint32 formatSize = ( agl::BitPerPixel( desc.m_format ) + 7 ) / 8;

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

		m_fontAtlas = agl::Texture::Create( desc, "UI.FontAtlas", &initData );

		static_assert( sizeof( ImTextureID ) >= sizeof( m_fontAtlas.Get() ), "Can't pack descriptor handle into TexID" );
		io.Fonts->SetTexID( reinterpret_cast<ImTextureID>( m_fontAtlas.Get() ) );

		m_fontAtlasSampler = StaticSamplerState<>::Get();
	}

	void ImguiRenderer::UpdateRenderResource()
	{
		if ( m_imguiDrawInfo.empty() )
		{
			return;
		}

		while ( m_imguiRenderResource.size() < m_imguiDrawInfo.size() )
		{
			auto& resource = m_imguiRenderResource.emplace_back();
			resource.InitResource();
		}

		for ( size_t i = 0; i < m_imguiDrawInfo.size(); ++i )
		{
			const ImguiDrawInfo& imguiDrawInfo = m_imguiDrawInfo[i];

			if ( ( imguiDrawInfo.m_totalNumVertex == 0 )
				|| ( imguiDrawInfo.m_totalNumIndex == 0 ) )
			{
				continue;
			}

			static Name positionName( "POSITION" );
			static Name colorName( "COLOR" );
			static Name texCoordName( "TEXCOORD" );

			auto& vertexCollection = m_imguiRenderResource[i].m_vertexCollection;

			if ( VertexBuffer* positionBuffer = vertexCollection.GetVertexBuffer( positionName ) )
			{
				positionBuffer->Resize( imguiDrawInfo.m_totalNumVertex, false );
				void* positionData = positionBuffer->Lock();

				size_t copySize = imguiDrawInfo.m_positions.size() * positionBuffer->ElementSize();
				std::memcpy( positionData, imguiDrawInfo.m_positions.data(), copySize );

				positionBuffer->Unlock();
			}

			if ( VertexBuffer* colorBuffer = vertexCollection.GetVertexBuffer( colorName ) )
			{
				colorBuffer->Resize( imguiDrawInfo.m_totalNumVertex, false );
				void* colorData = colorBuffer->Lock();

				size_t copySize = imguiDrawInfo.m_colors.size() * colorBuffer->ElementSize();
				std::memcpy( colorData, imguiDrawInfo.m_colors.data(), copySize );

				colorBuffer->Unlock();
			}

			if ( VertexBuffer* texCoordBuffer = vertexCollection.GetVertexBuffer( texCoordName ) )
			{
				texCoordBuffer->Resize( imguiDrawInfo.m_totalNumVertex, false );
				void* texCoordData = texCoordBuffer->Lock();

				size_t copySize = imguiDrawInfo.m_texCoords.size() * texCoordBuffer->ElementSize();
				std::memcpy( texCoordData, imguiDrawInfo.m_texCoords.data(), copySize );

				texCoordBuffer->Unlock();
			}

			IndexBuffer& indexBuffer = m_imguiRenderResource[i].m_indexBuffer;
			indexBuffer.Resize( imguiDrawInfo.m_totalNumIndex, false );

			void* indexData = indexBuffer.Lock();

			size_t copySize = imguiDrawInfo.m_indices.size() * indexBuffer.ElementSize();
			std::memcpy( indexData, imguiDrawInfo.m_indices.data(), copySize );

			indexBuffer.Unlock();
		}
	}

	void ImguiRenderer::CreateMultiViewportCanvas( ImGuiViewport& viewport )
	{
		engine::PlatformWindowContext windowCtx = {
			.m_nativeWindow = static_cast<HWND>( viewport.PlatformHandle ),
			.m_nativeContext = nullptr,
		};

		auto canvas = std::make_shared<Canvas>(
			static_cast<uint32>( viewport.Size.x ),
			static_cast<uint32>( viewport.Size.y ),
			windowCtx,
			agl::ResourceFormat::R8G8B8A8_UNORM,
			DefaultRenderCore::GetDefaultBackgroundColor() );

		viewport.RendererUserData = canvas.get();

		m_multiViewportCanvas.emplace_back( std::move( canvas ) );
	}

	void ImguiRenderer::DestroyMultiViewportCanvas( ImGuiViewport& viewport )
	{
		// TODO: Remove GPU wait.
		TaskHandle handle = EnqueueThreadTask<ThreadType::RenderThread>(
			[]()
			{
				GetInterface<agl::IAgl>()->WaitGPU();
			});
		GetInterface<ITaskScheduler>()->Wait( handle );

		auto pred = [userData = viewport.RendererUserData]( const std::shared_ptr<Canvas>& canvas )
		{
			return canvas.get() == userData;
		};

		std::erase_if( m_multiViewportCanvas, pred );

		viewport.RendererUserData = nullptr;
	}

	void ImguiRenderer::ResizeMultiViewportCanvas( ImGuiViewport& viewport, ImVec2 size )
	{
		auto& canvas = *static_cast<Canvas*>( viewport.RendererUserData );

		canvas.Resize( static_cast<uint32>( size.x ), static_cast<uint32>( size.y ) );
	}

	void ImguiRenderer::SwapMultiViewportBuffers()
	{
		bool useVSync = agl::DefaultAgl::UseVSync();
		bool allowTearing = agl::DefaultAgl::AllowTearing();

		for ( size_t i = 1; i < m_imguiDrawInfo.size(); ++i )
		{
			m_imguiDrawInfo[i].m_canvas->Present( useVSync, allowTearing );
		}
	}

	Owner<UserInterfaceRenderer*> CreateUserInterfaceRenderer()
	{
		return new ImguiRenderer();
	}

	void DestroyUserInterfaceRenderer( Owner<UserInterfaceRenderer*> uiRenderer )
	{
		delete uiRenderer;
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

	std::optional<DrawSnapshot> ImguiDrawPassProcessor::ProcessInternal( const PrimitiveSubMesh& subMesh, const PassShader& passShader )
	{
		PassRenderOption passRenderOption = {
			.m_blendOption = &m_blendOption,
			.m_depthStencilOption = &m_depthStencilOption,
			.m_rasterizerOption = &m_rasterizerOption
		};

		return BuildDrawSnapshot( subMesh, passShader, passRenderOption, VertexStreamLayoutType::Default, false );
	}

	PassShader ImguiDrawPassProcessor::CollectPassShader( [[maybe_unused]] MaterialResource& material ) const
	{
		PassShader passShader = {
			.m_vertexShader = DrawImguiVS(),
			.m_pixelShader = DrawImguiPS()
		};

		return passShader;
	}
}
