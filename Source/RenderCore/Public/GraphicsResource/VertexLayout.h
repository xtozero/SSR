#pragma once

#include "GraphicsApiResource.h"
#include "HashUtil.h"
#include "InlineMemoryAllocator.h"
#include "PipelineState.h"
#include "Shader.h"
#include "SizedTypes.h"

#include <tuple>
#include <vector>

namespace rendercore
{
	class VertexLayoutDesc
	{
	public:
		const agl::VertexLayoutData* GetData() const
		{
			return m_layoutData.data();
		}

		uint32 Size() const
		{
			return static_cast<uint32>( m_layoutData.size() );
		}

		void AddLayout( const char* name, uint32 index, agl::ResourceFormat format, uint32 slot, bool isInstanceData, uint32 instanceDataStep )
		{
			assert( Size() < agl::MaxVertexLayouts );

			m_layoutData.emplace_back();
			agl::VertexLayoutData& elem = m_layoutData.back();

			elem.m_name = Name( name );
			elem.m_isInstanceData = isInstanceData;
			elem.m_index = index;
			elem.m_format = format;
			elem.m_slot = slot;
			elem.m_instanceDataStep = instanceDataStep;
		}

		friend bool operator==( const VertexLayoutDesc& lhs, const VertexLayoutDesc& rhs )
		{
			if ( lhs.Size() == rhs.Size() )
			{
				for ( uint32 i = 0; i < lhs.Size(); ++i )
				{
					if ( lhs.m_layoutData[i] != rhs.m_layoutData[i] )
					{
						return false;
					}
				}

				return true;
			}

			return false;
		}

		virtual ~VertexLayoutDesc() = default;

		friend Archive& operator<<( Archive& ar, VertexLayoutDesc& desc );

	private:
		std::vector<agl::VertexLayoutData, InlineAllocator<agl::VertexLayoutData, 8>> m_layoutData;
	};

	struct VertexLayoutInstance final
	{
		const agl::VertexShader* m_vertexShader = nullptr;
		VertexLayoutDesc m_desc;
	};

	struct VertexLayoutInstanceEqual final
	{
		bool operator()( const VertexLayoutInstance& lhs, const VertexLayoutInstance& rhs ) const
		{
			return lhs.m_vertexShader == rhs.m_vertexShader
				&& lhs.m_desc == rhs.m_desc;
		}
	};

	struct VertexLayoutInstanceHasher final
	{
		size_t operator()( const VertexLayoutInstance& instance ) const
		{
			static size_t typeHash = typeid( VertexLayoutInstance ).hash_code();
			size_t hash = typeHash;
			HashCombine( hash, instance.m_vertexShader );

			const agl::VertexLayoutData* layoutData = instance.m_desc.GetData();
			for ( uint32 i = 0; i < instance.m_desc.Size(); ++i )
			{
				HashCombine( hash, layoutData[i].GetHash() );
			}

			return hash;
		}
	};

	class VertexLayout final
	{
	public:
		agl::VertexLayout* Resource();
		const agl::VertexLayout* Resource() const;

		VertexLayout( const VertexShader& vs, const VertexLayoutDesc& desc );

		VertexLayout() = default;
		~VertexLayout() = default;
		VertexLayout( const VertexLayout& ) = default;
		VertexLayout& operator=( const VertexLayout& ) = default;
		VertexLayout( VertexLayout&& ) = default;
		VertexLayout& operator=( VertexLayout&& ) = default;

	private:
		void InitResource( const VertexShader& vs, const VertexLayoutDesc& desc );

		RefHandle<agl::VertexLayout> m_layout;
	};
}
