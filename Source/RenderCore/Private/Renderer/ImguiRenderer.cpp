#include "UserInterfaceRenderer.h"

#include "AssetLoader.h"
#include "GlobalShaders.h"
#include "GraphicsApiResource.h"
#include "imgui.h"
#include "PassProcessor.h"
#include "RenderOption.h"
#include "SizedTypes.h"
#include "Texture.h"

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

	class ImguiRenderer : public UserInterfaceRenderer
	{
	public:
		virtual bool BootUp() override;
		virtual void Render( RenderViewGroup& renderViewGroup ) override;

		virtual void UpdateUIDrawData() override;

	private:
		void CreateFontsAtlas();

		agl::RefHandle<agl::Texture> m_fontAtlas;
		ImguiDrawPassProcessor m_drawPassProcessor;
	};

	bool ImguiRenderer::BootUp()
	{
		CreateFontsAtlas();

		return true;
	}

	void ImguiRenderer::Render( RenderViewGroup& renderViewGroup )
	{
	}

	void ImguiRenderer::UpdateUIDrawData()
	{
		ImDrawData* drawData = ImGui::GetDrawData();
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

		m_fontAtlas = agl::Texture::Create( trait, &initData );

		EnqueueRenderTask( [texture = m_fontAtlas]()
			{
				texture->Init();
			});

		static_assert( sizeof( ImTextureID ) >= sizeof( m_fontAtlas.Get() ), "Can't pack descriptor handle into TexID" );
		io.Fonts->SetTexID( ( ImTextureID )m_fontAtlas.Get() );
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