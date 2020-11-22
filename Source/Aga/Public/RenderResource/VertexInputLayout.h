#pragma once

#include "GraphicsApiResource.h"

#include <tuple>
#include <vector>

class VertexLayoutDescElem
{
public:
	VertexLayoutDescElem( const char* name, int index, RESOURCE_FORMAT format, int slot, bool isInstanceData, int instanceDataStep ) : m_name( name ), m_index( index ), m_format( format ), m_slot( slot ), m_isInstanceData( isInstanceData ), m_instanceDataStep( instanceDataStep ) {}

	friend bool operator<( const VertexLayoutDescElem& lhs, const VertexLayoutDescElem& rhs )
	{
		auto lElem = std::tie( lhs.m_instanceDataStep, lhs.m_isInstanceData, lhs.m_slot, lhs.m_format, lhs.m_index, lhs.m_name );
		auto rElem = std::tie( rhs.m_instanceDataStep, rhs.m_isInstanceData, rhs.m_slot, rhs.m_format, rhs.m_index, rhs.m_name );

		return lElem < rElem;
	}

	std::string m_name;
	int m_index = 0;
	RESOURCE_FORMAT m_format = RESOURCE_FORMAT::UNKNOWN;
	int m_slot = 0;
	bool m_isInstanceData = false;
	int m_instanceDataStep = 0;
};

class VertexLayoutDesc
{
public:
	void Initialize( std::size_t descSize )
	{
		m_layoutData.reserve( descSize );
	}

	const VertexLayoutDescElem* Desc( ) const
	{
		return m_layoutData.data( );
	}

	std::size_t Size( ) const
	{
		return m_layoutData.size( );
	}

	void AddLayout( const char* name, int index, RESOURCE_FORMAT format, int slot, bool isInstanceData, int instanceDataStep )
	{
		m_layoutData.emplace_back( name, index, format, slot, isInstanceData, instanceDataStep );
	}

	friend bool operator<( const VertexLayoutDesc& lhs, const VertexLayoutDesc& rhs )
	{
		if ( lhs.Size( ) == rhs.Size( ) )
		{
			const VertexLayoutDescElem* lDesc = lhs.Desc( );
			const VertexLayoutDescElem* rDesc = rhs.Desc( );

			for ( std::size_t i = 0; i < lhs.Size( ); ++i )
			{
				bool isLess = lDesc[i] < rDesc[i];
				bool isGreator = rDesc[i] < lDesc[i];

				if ( isLess )
				{
					return true;
				}
				else if ( isGreator )
				{
					return false;
				}
			}

			return false;
		}

		return lhs.Size( ) < rhs.Size( );
	}

private: 
	std::vector<VertexLayoutDescElem> m_layoutData;
};

class VertexLayout : public DeviceDependantResource
{
};