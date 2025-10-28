#pragma once

#include "Buffer.h"
#include "NameTypes.h"
#include "RefHandle.h"
#include "Texture.h"

namespace rendercore
{
	class RenderGraph;
	class RenderGraphPass;

	class RenderGraphResource
	{
		friend RenderGraph;
		friend RenderGraphPass;

	protected:
		bool m_isExternal = false;
		bool m_isForUpload = false;
	};

	template<typename T>
	class TypedRenderGraphResource : public RenderGraphResource
	{
	public:
		T* Get() const
		{
			return m_ref.Get();
		}

		const auto& GetTrait() const
		{
			return m_trait;
		}

		agl::ShaderResourceView* SRV() const
		{
			assert( Get() != nullptr );
			return Get()->SRV();
		}

	private:
		friend RenderGraph;
		using TraitType = std::remove_cvref_t<decltype( std::declval<T>().GetTrait() )>;

		const char* m_name = nullptr;
		RefHandle<T> m_ref;
		TraitType m_trait = {};
	};

	using RenderGraphTexture = TypedRenderGraphResource<agl::Texture>;
	using RenderGraphBuffer = TypedRenderGraphResource<agl::Buffer>;

	enum class RenderGraphResourceFlag : uint16
	{
		None = 0x00,
		Buffer = 0x01,
		Texture = 0x02,
		NonPixelSRV = 0x04,
		PixelSRV = 0x08,
		SRV = NonPixelSRV | PixelSRV,
		UAV = 0x10,
		RTV = 0x20,
		DSV = 0x40,
		IndirectArgument = 0x80,
		CopyDest = 0x100,
		CopySource = 0x200,
	};
	ENUM_CLASS_FLAGS( RenderGraphResourceFlag )

	struct RenderGraphResourceMemberMetaData
	{
		RenderGraphResourceFlag m_flag = RenderGraphResourceFlag::None;
		Name m_name;
		size_t m_offset = 0;
		RenderGraphResourceMemberMetaData* m_next = nullptr;
	};

#define BEGIN_RG_RESOURCE_STRUCT( type ) \
class type \
{ \
public: \
\
	static const RenderGraphResourceMemberMetaData* GetMetaData() \
	{ \
		static RenderGraphResourceMemberMetaData* metaData = GetMemberMetaData(); \
		return metaData; \
	} \
\
private: \
	struct FirstMemberId {}; \
	static void* GetMetaData( FirstMemberId, [[maybe_unused]] RenderGraphResourceMemberMetaData*& next ) \
	{ \
		return nullptr; \
	} \
	using ThisType = type; \
	using MemberFunc = void* (*)(FirstMemberId, RenderGraphResourceMemberMetaData*&); \
\
	typedef FirstMemberId

#define DECLARE_RENDER_GRAPH_RESOURCE_GET_MEATADATA( name, memberFlag ) \
	struct MemberId##name{}; \
	static void* GetMetaData( MemberId##name, RenderGraphResourceMemberMetaData*& next ) \
	{ \
		static RenderGraphResourceMemberMetaData metaData##name = { \
			.m_flag = memberFlag, \
			.m_name = Name( #name ), \
			.m_offset = offsetof( ThisType, m_##name ), \
			.m_next = next \
		}; \
\
		next = &metaData##name; \
		void* ( *prevFunc )( PrevMemberId##name, RenderGraphResourceMemberMetaData*& ); \
		prevFunc = &GetMetaData; \
		return (void*)prevFunc; \
	} \
\
	typedef MemberId##name \

#define DECLARE_RG_TEXTURE( name, flag ) \
	PrevMemberId##name; \
public: \
	RenderGraphTexture* m_##name = nullptr; \
\
private: \
	DECLARE_RENDER_GRAPH_RESOURCE_GET_MEATADATA( name, RenderGraphResourceFlag::Texture | flag )

#define DECLARE_RG_TEXTURE_PIXEL_SRV( name ) DECLARE_RG_TEXTURE( name, RenderGraphResourceFlag::PixelSRV )
#define DECLARE_RG_TEXTURE_NONPIXEL_SRV( name ) DECLARE_RG_TEXTURE( name, RenderGraphResourceFlag::NonPixelSRV )
#define DECLARE_RG_TEXTURE_UAV( name ) DECLARE_RG_TEXTURE( name, RenderGraphResourceFlag::UAV )
#define DECLARE_RG_TEXTURE_COPY_DEST( name ) DECLARE_RG_TEXTURE( name, RenderGraphResourceFlag::CopyDest )
#define DECLARE_RG_TEXTURE_COPY_SOURCE( name ) DECLARE_RG_TEXTURE( name, RenderGraphResourceFlag::CopySource )

#define DECLARE_RG_BUFFER( name, flag ) \
	PrevMemberId##name; \
public: \
	RenderGraphBuffer* m_##name = nullptr; \
\
private: \
	DECLARE_RENDER_GRAPH_RESOURCE_GET_MEATADATA( name, RenderGraphResourceFlag::Buffer | flag )

#define DECLARE_RG_BUFFER_PIXEL_SRV( name ) DECLARE_RG_BUFFER( name, RenderGraphResourceFlag::PixelSRV )
#define DECLARE_RG_BUFFER_NONPIXEL_SRV( name ) DECLARE_RG_BUFFER( name, RenderGraphResourceFlag::NonPixelSRV )
#define DECLARE_RG_BUFFER_UAV( name ) DECLARE_RG_BUFFER( name, RenderGraphResourceFlag::UAV )
#define DECLARE_RG_BUFFER_INDRIECT_ARG( name ) DECLARE_RG_BUFFER( name, RenderGraphResourceFlag::IndirectArgument )
#define DECLARE_RG_BUFFER_COPY_DEST( name ) DECLARE_RG_BUFFER( name, RenderGraphResourceFlag::CopyDest )
#define DECLARE_RG_BUFFER_COPY_SOURCE( name ) DECLARE_RG_BUFFER( name, RenderGraphResourceFlag::CopySource )

#define END_RG_RESOURCE_STRUCT() \
	LastMemberId; \
\
	static RenderGraphResourceMemberMetaData* GetMemberMetaData()\
	{ \
		void* (*lastFunc)(LastMemberId, RenderGraphResourceMemberMetaData*&); \
		lastFunc = &GetMetaData; \
		RenderGraphResourceMemberMetaData* next = nullptr; \
		void* ptr = (void*)lastFunc; \
		do \
		{ \
			ptr = reinterpret_cast<MemberFunc>( ptr )( FirstMemberId(), next ); \
		} while( ptr ); \
		return next; \
	} \
}

	template <typename T>
	concept HasMetaData = requires
	{
		T::GetMetaData();
	};
}