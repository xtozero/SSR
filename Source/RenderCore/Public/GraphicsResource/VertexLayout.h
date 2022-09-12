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
		const VERTEX_LAYOUT_TRAIT* Data() const
		{
			return m_layoutData;
		}

		uint32 Size() const
		{
			return m_size;
		}

		void AddLayout( const char* name, uint32 index, RESOURCE_FORMAT format, uint32 slot, bool isInstanceData, uint32 instanceDataStep )
		{
			assert( m_size < MAX_VERTEX_LAYOUT_SIZE );
			VERTEX_LAYOUT_TRAIT& trait = m_layoutData[m_size];

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
		VERTEX_LAYOUT_TRAIT m_layoutData[MAX_VERTEX_LAYOUT_SIZE] = {};
	};

	struct VertexLayoutDescHasher
	{
		size_t operator()( const VertexLayoutDesc& desc ) const
		{
			static size_t typeHash = typeid( VertexLayoutDesc ).hash_code();
			size_t hash = typeHash;
			const VERTEX_LAYOUT_TRAIT* data = desc.Data();
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

	class VertexLayout
	{
	public:
		aga::VertexLayout* Resource();
		const aga::VertexLayout* Resource() const;

		VertexLayout( const VertexShader& vs, const VertexLayoutDesc& desc );

		VertexLayout() = default;
		~VertexLayout() = default;
		VertexLayout( const VertexLayout& ) = default;
		VertexLayout& operator=( const VertexLayout& ) = default;
		VertexLayout( VertexLayout&& ) = default;
		VertexLayout& operator=( VertexLayout&& ) = default;

	private:
		void InitResource( const VertexShader& vs, const VertexLayoutDesc& desc );

		RefHandle<aga::VertexLayout> m_layout;
	};
}
