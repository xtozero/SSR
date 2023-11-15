#pragma once

#include "GraphicsApiResource.h"
#include "HashUtil.h"
#include "Shader.h"
#include "SizedTypes.h"
#include "PipelineState.h"

#include <tuple>
#include <vector>

namespace rendercore
{
	class VertexLayoutDesc
	{
	public:
		const agl::VertexLayoutTrait* Data() const
		{
			return m_layoutData;
		}

		uint32 Size() const
		{
			return m_size;
		}

		void AddLayout( const char* name, uint32 index, agl::ResourceFormat format, uint32 slot, bool isInstanceData, uint32 instanceDataStep )
		{
			assert( m_size < agl::MAX_VERTEX_LAYOUT_SIZE );
			agl::VertexLayoutTrait& trait = m_layoutData[m_size];

			trait.m_name = Name( name );
			trait.m_isInstanceData = isInstanceData;
			trait.m_index = index;
			trait.m_format = format;
			trait.m_slot = slot;
			trait.m_instanceDataStep = instanceDataStep;

			++m_size;
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

	protected:
		uint32 m_size = 0;

	private:
		agl::VertexLayoutTrait m_layoutData[agl::MAX_VERTEX_LAYOUT_SIZE] = {};
	};

	struct VertexLayoutDescHasher final
	{
		size_t operator()( const VertexLayoutDesc& desc ) const
		{
			static size_t typeHash = typeid( VertexLayoutDesc ).hash_code();
			size_t hash = typeHash;
			const agl::VertexLayoutTrait* data = desc.Data();
			for ( uint32 i = 0; i < desc.Size(); ++i )
			{
				HashCombine( hash, std::hash<std::string_view>()( data[i].m_name.Str() ) );
				HashCombine( hash, data[i].m_isInstanceData );
				HashCombine( hash, data[i].m_index );
				HashCombine( hash, data[i].m_format );
				HashCombine( hash, data[i].m_slot );
				HashCombine( hash, data[i].m_instanceDataStep );
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

		agl::RefHandle<agl::VertexLayout> m_layout;
	};
}
