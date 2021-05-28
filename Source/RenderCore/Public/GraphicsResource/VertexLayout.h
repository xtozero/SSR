#pragma once

#include "GraphicsApiResource.h"
#include "HashUtil.h"
#include "Shader.h"
#include "PipelineState.h"

#include <tuple>
#include <vector>

class VertexLayoutDesc
{
public:
	void Initialize( std::size_t descSize )
	{
		m_layoutData.reserve( descSize );
	}

	const VERTEX_LAYOUT_TRAIT* Data( ) const
	{
		return m_layoutData.data( );
	}

	std::size_t Size( ) const
	{
		return m_layoutData.size( );
	}

	void AddLayout( const char* name, int index, RESOURCE_FORMAT format, int slot, bool isInstanceData, int instanceDataStep )
	{
		m_layoutData.emplace_back( );
		VERTEX_LAYOUT_TRAIT& trait = m_layoutData.back( );

		trait.m_name = name;
		trait.m_isInstanceData = isInstanceData;
		trait.m_index = index;
		trait.m_format = format;
		trait.m_slot = slot;
		trait.m_instanceDataStep = instanceDataStep;
	}

	friend bool operator==( const VertexLayoutDesc& lhs, const VertexLayoutDesc& rhs )
	{
		if ( lhs.Size( ) == rhs.Size( ) )
		{
			for ( std::size_t i = 0; i < lhs.Size( ); ++i )
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

private: 
	std::vector<VERTEX_LAYOUT_TRAIT> m_layoutData;
};

struct VertexLayoutDescHasher
{
	std::size_t operator()( const VertexLayoutDesc& desc ) const
	{
		static std::size_t typeHash = typeid( VertexLayoutDesc ).hash_code( );
		std::size_t hash = typeHash;
		const VERTEX_LAYOUT_TRAIT* data = desc.Data( );
		for ( int i = 0; i < desc.Size( ); ++i )
		{
			HashCombine( hash, std::hash<std::string>()( data[i].m_name ) );
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
	aga::VertexLayout* Resource( );
	const aga::VertexLayout* Resource( ) const;

	VertexLayout( const VertexShader& vs, const VertexLayoutDesc& desc );

	VertexLayout( ) = default;
	~VertexLayout( ) = default;
	VertexLayout( const VertexLayout& ) = default;
	VertexLayout& operator=( const VertexLayout& ) = default;
	VertexLayout( VertexLayout&& ) = default;
	VertexLayout& operator=( VertexLayout&& ) = default;

private:
	void InitResource( const VertexShader& vs, const VertexLayoutDesc& desc );

	RefHandle<aga::VertexLayout> m_layout;
};