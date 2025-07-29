#pragma once

#include "CommandList.h"
#include "RenderGraphEvent.h"
#include "RenderGraphResource.h"
#include "SceneRenderer.h"

#include <unordered_set>

namespace agl
{
	class GraphicsApiResource;
}

namespace rendercore
{
	class RenderGraph;

	enum class RenderGraphPassType : uint8
	{
		Copy,
		Compute,
		AsyncCompute,
		Graphics
	};

	class RenderGraphPass
	{
	public:
		virtual void Execute( ComputeCommandList& commandList ) = 0;

		RenderGraphPass() = default;
		virtual ~RenderGraphPass() = default;

	protected:
		void ApplyResourceBarrier();
		void LoadRasterOutput( CommandList& commandList );

		const RasterOutput* m_rasterOutput = nullptr;

		struct PassResource
		{
			RenderGraphResource* m_ptr = nullptr;
			RenderGraphResourceFlag m_flag = RenderGraphResourceFlag::None;

			PassResource( RenderGraphResource* resource, RenderGraphResourceFlag flag )
				: m_ptr( resource )
				, m_flag( flag )
			{}
		};

		struct PassResourceHasher
		{
			size_t operator()( const PassResource& resource ) const
			{
				return std::hash<RenderGraphResource*>()( resource.m_ptr );
			}
		};

		struct PassResourceEqual
		{
			bool operator()( const PassResource& lhs, const PassResource& rhs ) const
			{
				return lhs.m_ptr == rhs.m_ptr;
			}
		};

		using RenderGraphResourceSet = std::unordered_set<PassResource, PassResourceHasher, PassResourceEqual>;

		RenderGraphResourceSet m_resourceReads;
		RenderGraphResourceSet m_resourceWrites;

	private:
		friend RenderGraph;

		void Setup( void* resourceData, const RenderGraphResourceMemberMetaData* metaData, const RasterOutput* rasterOutput );

		GpuProfileRenderGraphEvent* m_gpuProfileEvent = nullptr;
		PipelineStateRenderGraphEvent* m_pipelineStatEvent = nullptr;

		RenderGraphPassType m_type = RenderGraphPassType::Copy;

		bool m_waitForOtherQueue = false;
	};

	template <typename Lambda>
	class LambdaRenderGraphPass : public RenderGraphPass
	{
		template <typename Lambda>
		struct LambdaTrait : public LambdaTrait<decltype( &Lambda::operator() )> {};

		template <typename ReturnType, typename ClassType, typename LambdaArgType>
		struct LambdaTrait<ReturnType ( ClassType::* )( LambdaArgType& ) const>
		{
			using CommandListType = LambdaArgType;
		};

		template <typename ReturnType, typename ClassType, typename LambdaArgType>
		struct LambdaTrait<ReturnType ( ClassType::* )( LambdaArgType& )>
		{
			using CommandListType = LambdaArgType;
		};

	public:
		using CommandListType = typename LambdaTrait<Lambda>::CommandListType;

		virtual void Execute( ComputeCommandList& commandList ) override
		{
			auto concreteCommandList = static_cast<CommandListType&>( commandList );

			if constexpr ( std::is_base_of_v<CommandList, CommandListType> )
			{
				LoadRasterOutput( concreteCommandList );
			}

			if constexpr ( std::is_same_v<CommandListType, CommandList> )
			{
				m_rasterOutput->Bind( concreteCommandList );
			}

			m_passBody( concreteCommandList );
		}

		explicit LambdaRenderGraphPass( Lambda&& passBody ) : m_passBody( std::forward<Lambda>( passBody ) ) {}

		virtual ~LambdaRenderGraphPass()
		{
			if ( m_rasterOutput != nullptr )
			{
				std::destroy_at( m_rasterOutput );
			}
		}

	private:
		Lambda m_passBody;
	};
}