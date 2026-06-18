#pragma once

#include "Buffer.h"
#include "GraphicsApiResource.h"
#include "IAgl.h"
#include "PipelineState.h"
#include "ResourceViews.h"
#include "ShaderParameterInfo.h"
#include "ShaderResource.h"
#include "SizedTypes.h"
#include "Texture.h"

#include <algorithm>
#include <cassert>
#include <memory>
#include <optional>
#include <vector>

namespace agl
{
	class ShaderBindingLayout
	{
	public:
		size_t GetDataSize() const
		{
			return ( m_parameterInfo->m_cbvs.size()
				+ m_parameterInfo->m_globalCb.size()
				+ m_parameterInfo->m_srvs.size()
				+ m_parameterInfo->m_uavs.size()
				+ m_parameterInfo->m_samplers.size()
				+ m_parameterInfo->m_bindless.size() ) * sizeof( RefHandle<GraphicsApiResource> );
		}

		const ShaderParameterInfo& ParameterInfo() const
		{
			return *m_parameterInfo;
		}

		ShaderType GetShaderType() const
		{
			return m_shaderType;
		}

		size_t GetHash() const
		{
			static size_t typeHash = typeid( ShaderBindingLayout ).hash_code();
			size_t hash = typeHash;

			HashCombine( hash, m_shaderType );
			HashCombine( hash, m_parameterInfo->GetHash() );

			return hash;
		}

		uint32 NumSRV() const
		{
			return static_cast<uint32>( m_parameterInfo->m_srvs.size() );
		}

		uint32 NumUAV() const
		{
			return static_cast<uint32>( m_parameterInfo->m_uavs.size() );
		}

		uint32 NumGlobalCB() const
		{
			return static_cast<uint32>( m_parameterInfo->m_globalCb.size() );
		}

		uint32 NumCBV() const
		{
			return static_cast<uint32>( m_parameterInfo->m_cbvs.size() );
		}

		uint32 NumSampler() const
		{
			return static_cast<uint32>( m_parameterInfo->m_samplers.size() );
		}

		uint32 NumBindless() const
		{
			return static_cast<uint32>( m_parameterInfo->m_bindless.size() );
		}

		ShaderBindingLayout() = default;
		ShaderBindingLayout( ShaderType type, const ShaderParameterInfo& parameterInfo ) : m_shaderType( type ), m_parameterInfo( &parameterInfo ) {}
		ShaderBindingLayout( const ShaderBindingLayout& other )
		{
			( *this ) = other;
		}
		ShaderBindingLayout& operator=( const ShaderBindingLayout& other )
		{
			if ( this != &other )
			{
				m_shaderType = other.m_shaderType;
				m_parameterInfo = other.m_parameterInfo;
			}

			return *this;
		}

		ShaderBindingLayout( ShaderBindingLayout&& other ) noexcept
		{
			( *this ) = std::move( other );
		}
		ShaderBindingLayout& operator=( ShaderBindingLayout&& other ) noexcept
		{
			if ( this != &other )
			{
				m_shaderType = other.m_shaderType;
				m_parameterInfo = std::move( other.m_parameterInfo );
			}

			return *this;
		}

		friend bool operator==( const ShaderBindingLayout& lhs, const ShaderBindingLayout& rhs )
		{
			return lhs.m_shaderType == rhs.m_shaderType &&
				lhs.m_parameterInfo == rhs.m_parameterInfo;
		}

		friend bool operator!=( const ShaderBindingLayout& lhs, const ShaderBindingLayout& rhs )
		{
			return !( lhs == rhs );
		}

	protected:
		size_t GetGlobalCBOffset() const
		{
			return 0;
		}

		size_t GetCBVOffset() const
		{
			return GetGlobalCBOffset() + m_parameterInfo->m_globalCb.size() * sizeof( RefHandle<GraphicsApiResource> );
		}

		size_t GetSRVOffset() const
		{
			return GetCBVOffset() + m_parameterInfo->m_cbvs.size() * sizeof( RefHandle<GraphicsApiResource> );
		}

		size_t GetUAVOffset() const
		{
			return GetSRVOffset() + m_parameterInfo->m_srvs.size() * sizeof( RefHandle<GraphicsApiResource> );
		}

		size_t GetSamplerOffset() const
		{
			return GetUAVOffset() + m_parameterInfo->m_uavs.size() * sizeof( RefHandle<GraphicsApiResource> );
		}

		size_t GetBindlessOffset() const
		{
			return GetSamplerOffset() + m_parameterInfo->m_samplers.size() * sizeof( RefHandle<GraphicsApiResource> );
		}

		ShaderType m_shaderType = ShaderType::None;
		const ShaderParameterInfo* m_parameterInfo = nullptr;
	};

	class SingleShaderBindings : public ShaderBindingLayout
	{
	public:
		void AddConstantBuffer( const ShaderParameter& param, Buffer* buffer )
		{
			if ( param.m_shader != m_shaderType )
			{
				return;
			}

			const bool isGlobalCB = param.m_bindPoint == 0;
			if ( isGlobalCB )
			{
				std::optional<uint32> foundSlot = FindBindingSlot( m_parameterInfo->m_globalCb, param );
				if ( foundSlot )
				{
					GetGlobalCBStart()[*foundSlot] = buffer;
				}
			}
			else
			{
				std::optional<uint32> foundSlot = FindBindingSlot( m_parameterInfo->m_cbvs, param );
				if ( foundSlot )
				{
					GetCBVStart()[*foundSlot] = buffer;
				}
			}
		}

		void AddSRV( const ShaderParameter& param, ShaderResourceView* srv )
		{
			if ( param.m_shader != m_shaderType )
			{
				return;
			}

			std::optional<uint32> foundSlot = FindBindingSlot( m_parameterInfo->m_srvs, param );
			if ( foundSlot )
			{
				GetSRVStart()[*foundSlot] = srv;
			}
		}

		void AddUAV( const ShaderParameter& param, UnorderedAccessView* uav )
		{
			if ( param.m_shader != m_shaderType )
			{
				return;
			}

			std::optional<uint32> foundSlot = FindBindingSlot( m_parameterInfo->m_uavs, param );
			if ( foundSlot )
			{
				GetUAVStart()[*foundSlot] = uav;
			}
		}

		void AddSampler( const ShaderParameter& param, SamplerState* sampler )
		{
			if ( param.m_shader != m_shaderType )
			{
				return;
			}

			std::optional<uint32> foundSlot = FindBindingSlot( m_parameterInfo->m_samplers, param );
			if ( foundSlot )
			{
				GetSamplerStart()[*foundSlot] = sampler;
			}
		}

		void AddBindless( const ShaderParameter& param, GraphicsApiResource* resource )
		{
			if ( param.m_shader != m_shaderType )
			{
				return;
			}

			std::optional<uint32> foundSlot;
			for ( size_t i = 0; i < m_parameterInfo->m_bindless.size(); ++i )
			{
				const ShaderParameter& targetParam = m_parameterInfo->m_bindless[i];
				if ( ( targetParam.m_bindPoint == param.m_bindPoint )
					&& ( targetParam.m_space == param.m_space )
					&& ( targetParam.m_offset == param.m_offset )
					&& ( targetParam.m_sizeInByte == param.m_sizeInByte ) )
				{
					foundSlot = static_cast<uint32>( i );
					break;
				}
			}

			if ( foundSlot )
			{
				GetBindlessStart()[*foundSlot] = resource;
			}
		}

		RefHandle<Buffer>* GetGlobalCBStart() const
		{
			return reinterpret_cast<RefHandle<Buffer>*>( m_data + GetGlobalCBOffset() );
		}

		RefHandle<Buffer>* GetCBVStart() const
		{
			return reinterpret_cast<RefHandle<Buffer>*>( m_data + GetCBVOffset() );
		}

		RefHandle<ShaderResourceView>* GetSRVStart() const
		{
			return reinterpret_cast<RefHandle<ShaderResourceView>*>( m_data + GetSRVOffset() );
		}

		RefHandle<UnorderedAccessView>* GetUAVStart() const
		{
			return reinterpret_cast<RefHandle<UnorderedAccessView>*>( m_data + GetUAVOffset() );
		}

		RefHandle<SamplerState>* GetSamplerStart() const
		{
			return reinterpret_cast<RefHandle<SamplerState>*>( m_data + GetSamplerOffset() );
		}

		RefHandle<GraphicsApiResource>* GetBindlessStart() const
		{
			return reinterpret_cast<RefHandle<GraphicsApiResource>*>( m_data + GetBindlessOffset() );
		}

		SingleShaderBindings( const ShaderBindingLayout& bindingLayout, unsigned char* data ) : ShaderBindingLayout( bindingLayout ), m_data( data ) {}

	private:
		static std::optional<uint32> FindBindingSlot( const std::vector<ShaderParameter>& slots, const ShaderParameter& param )
		{
			for ( size_t i = 0; i < slots.size(); ++i )
			{
				const ShaderParameter& targetParam = slots[i];
				if ( ( targetParam.m_bindPoint == param.m_bindPoint )
					&& ( targetParam.m_space == param.m_space ) )
				{
					return static_cast<uint32>( i );
				}
			}

			return {};
		}

		unsigned char* m_data = nullptr;
	};

	class ShaderBindingsInitializer final
	{
	public:
		size_t NumShaders() const
		{
			auto pred = []( const ShaderParameterInfo* info )
			{
				return info != nullptr;
			};

			return std::ranges::count_if( m_shaderParameterInfos, pred );
		}

		const ShaderParameterInfo*& operator[]( ShaderType type )
		{
			assert( ShaderType::None < type && type < ShaderType::Count );
			return m_shaderParameterInfos[static_cast<uint32>( type )];
		}

		const ShaderParameterInfo* operator[]( ShaderType type ) const
		{
			assert( ShaderType::None < type && type < ShaderType::Count );
			return m_shaderParameterInfos[static_cast<uint32>( type )];
		}

	private:
		const ShaderParameterInfo* m_shaderParameterInfos[NumNonRTShaderTypes<uint32>] = {};
	};

	class ShaderBindings final
	{
	public:
		void Initialize( const ShaderBindingsInitializer& initializer )
		{
			size_t numShaders = initializer.NumShaders();

			size_t shaderBindsDataSize = 0;
			m_shaderLayouts = new ShaderBindingLayout[numShaders];

			for ( uint32 i = 0; i < NumNonRTShaderTypes<uint32>; ++i )
			{
				auto shaderType = static_cast<ShaderType>( i );
				if ( initializer[shaderType] )
				{
					std::construct_at( &m_shaderLayouts[m_shaderLayoutSize], shaderType, *initializer[shaderType] );
					shaderBindsDataSize += m_shaderLayouts[m_shaderLayoutSize].GetDataSize();
					++m_shaderLayoutSize;

					m_numCBV += static_cast<uint32>( initializer[shaderType]->m_cbvs.size() );
					m_numGlobalCB += static_cast<uint32>( initializer[shaderType]->m_globalCb.size() );
					m_numSRV += static_cast<uint32>( initializer[shaderType]->m_srvs.size() );
					m_numUAV += static_cast<uint32>( initializer[shaderType]->m_uavs.size() );
					m_numSampler += static_cast<uint32>( initializer[shaderType]->m_samplers.size() );
					m_numBindless += static_cast<uint32>( initializer[shaderType]->m_bindless.size() );
				}
			}

			m_bCompute = ( initializer[ShaderType::Compute] != nullptr );

			Allocate( shaderBindsDataSize );
		}

		SingleShaderBindings GetSingleShaderBindings( ShaderType shaderType ) const
		{
			size_t dataOffset = 0;
			for ( uint32 i = 0; i < m_shaderLayoutSize; ++i )
			{
				if ( m_shaderLayouts[i].GetShaderType() == shaderType )
				{
					return SingleShaderBindings( m_shaderLayouts[i], m_data + dataOffset );
				}

				dataOffset += m_shaderLayouts[i].GetDataSize();
			}

			static ShaderParameterInfo emptyParameterInfo;
			static ShaderBindingLayout emptyLayout( ShaderType::None, emptyParameterInfo );
			return SingleShaderBindings( emptyLayout, nullptr );
		}

		bool IsCompute() const
		{
			return m_bCompute;
		}

		bool HasBindless() const
		{
			return m_numBindless > 0;
		}

		uint32 NumSRV() const
		{
			return m_numSRV;
		}

		uint32 NumUAV() const
		{
			return m_numUAV;
		}

		uint32 NumCBV() const
		{
			return m_numCBV;
		}

		uint32 NumGlobalCB() const
		{
			return m_numGlobalCB;
		}

		uint32 NumSampler() const
		{
			return m_numSampler;
		}

		ShaderBindings() = default;
		ShaderBindings( const ShaderBindings& other )
		{
			( *this ) = other;
		}
		ShaderBindings& operator=( const ShaderBindings& other )
		{
			if ( this != &other )
			{
				Free();

				m_shaderLayouts = new ShaderBindingLayout[other.m_shaderLayoutSize];
				m_shaderLayoutSize = other.m_shaderLayoutSize;
				m_numSRV = other.m_numSRV;
				m_numUAV = other.m_numUAV;
				m_numCBV = other.m_numCBV;
				m_numGlobalCB = other.m_numGlobalCB;
				m_numSampler = other.m_numSampler;
				m_numBindless = other.m_numBindless;
				std::copy_n( other.m_shaderLayouts, m_shaderLayoutSize, m_shaderLayouts );
				Allocate( other.m_size );

				for ( size_t offset = 0; offset < m_size; offset += sizeof( RefHandle<GraphicsApiResource> ) )
				{
					auto handle = reinterpret_cast<RefHandle<GraphicsApiResource>*>( m_data + offset );
					auto otherHandle = reinterpret_cast<RefHandle<GraphicsApiResource>*>( other.m_data + offset );

					*handle = *otherHandle;
				}

				m_bCompute = other.m_bCompute;
			}

			return *this;
		}

		ShaderBindings( ShaderBindings&& other ) noexcept
		{
			( *this ) = std::move( other );
		}
		ShaderBindings& operator=( ShaderBindings&& other ) noexcept
		{
			if ( this != &other )
			{
				Free();

				m_shaderLayouts = other.m_shaderLayouts;
				m_shaderLayoutSize = other.m_shaderLayoutSize;
				m_numSRV = other.m_numSRV;
				m_numUAV = other.m_numUAV;
				m_numCBV = other.m_numCBV;
				m_numGlobalCB = other.m_numGlobalCB;
				m_numSampler = other.m_numSampler;
				m_numBindless = other.m_numBindless;
				m_data = other.m_data;
				m_size = other.m_size;
				m_bCompute = other.m_bCompute;

				other.m_shaderLayouts = nullptr;
				other.m_shaderLayoutSize = 0;
				other.m_numSRV = 0;
				other.m_numUAV = 0;
				other.m_numCBV = 0;
				other.m_numGlobalCB = 0;
				other.m_numSampler = 0;
				other.m_numBindless = 0;
				other.m_data = nullptr;
				other.m_size = 0;
				other.m_bCompute = false;
			}

			return *this;
		}

		~ShaderBindings()
		{
			Free();
		}

		bool MatchsForDynamicInstancing( const ShaderBindings& other ) const
		{
			if ( ( m_shaderLayoutSize != other.m_shaderLayoutSize )
				|| ( m_size != other.m_size )
				|| ( m_numSRV != other.m_numSRV )
				|| ( m_numUAV != other.m_numUAV )
				|| ( m_numCBV != other.m_numCBV )
				|| ( m_numGlobalCB != other.m_numGlobalCB )
				|| ( m_numSampler != other.m_numSampler )
				|| ( m_numBindless != other.m_numBindless )
				|| ( m_bCompute != other.m_bCompute ) )
			{
				return false;
			}

			for ( uint32 i = 0; i < m_shaderLayoutSize; ++i )
			{
				if ( m_shaderLayouts[i] != other.m_shaderLayouts[i] )
				{
					return false;
				}
			}

			return std::memcmp( m_data, other.m_data, m_size ) == 0;
		}

		size_t HashForDynamicInstaning() const
		{
			static size_t typeHash = typeid( ShaderBindings ).hash_code();
			size_t hash = typeHash;

			HashCombine( hash, m_size );

			for ( uint32 i = 0; i < m_shaderLayoutSize; ++i )
			{
				HashCombine( hash, m_shaderLayouts[i].GetHash() );
			}

			for ( size_t offset = 0; offset < m_size; offset += sizeof( RefHandle<GraphicsApiResource> ) )
			{
				auto handle = reinterpret_cast<RefHandle<GraphicsApiResource>*>( m_data + offset );
				HashCombine( hash, handle->Get() );
			}

			return hash;
		}

	private:
		void Allocate( size_t size )
		{
			if ( size > 0 )
			{
				assert( m_data == nullptr && m_size == 0 );
				assert( size % sizeof( RefHandle<GraphicsApiResource> ) == 0 );
				m_data = new unsigned char[size];
				m_size = size;

				for ( size_t offset = 0; offset < m_size; offset += sizeof( RefHandle<GraphicsApiResource> ) )
				{
					auto handle = reinterpret_cast<RefHandle<GraphicsApiResource>*>( m_data + offset );
					std::construct_at( handle );
				}
			}
		}

		void Free()
		{
			delete[] m_shaderLayouts;
			m_shaderLayouts = nullptr;
			m_shaderLayoutSize = 0;

			if ( m_size > 0 )
			{
				for ( size_t offset = 0; offset < m_size; offset += sizeof( RefHandle<GraphicsApiResource> ) )
				{
					auto handle = reinterpret_cast<RefHandle<GraphicsApiResource>*>( m_data + offset );
					handle->~RefHandle<GraphicsApiResource>();
				}
			}

			m_numSRV = 0;
			m_numUAV = 0;
			m_numCBV = 0;
			m_numGlobalCB = 0;
			m_numSampler = 0;
			m_numBindless = 0;

			delete[] m_data;
			m_data = nullptr;
			m_size = 0;

			m_bCompute = false;
		}

		ShaderBindingLayout* m_shaderLayouts = nullptr;
		uint32 m_shaderLayoutSize = 0;

		uint32 m_numSRV = 0;
		uint32 m_numUAV = 0;
		uint32 m_numCBV = 0;
		uint32 m_numGlobalCB = 0;
		uint32 m_numSampler = 0;
		uint32 m_numBindless = 0;

		unsigned char* m_data = nullptr;
		size_t m_size = 0;

		bool m_bCompute = false;
	};
}