#include "D3D12Buffer.h"

namespace agl
{
	ID3D12Resource* D3D12Buffer::Resource()
	{
		return nullptr;
	}

	const ID3D12Resource* D3D12Buffer::Resource() const
	{
		return nullptr;
	}

	uint32 D3D12Buffer::Stride() const
	{
		return uint32();
	}

	D3D12Buffer::D3D12Buffer( const BUFFER_TRAIT& trait, const void* initData )
	{
	}

	D3D12Buffer::~D3D12Buffer()
	{
	}

	void D3D12Buffer::InitResource()
	{
	}

	void D3D12Buffer::FreeResource()
	{
	}

	void D3D12Buffer::CreateBuffer()
	{
	}

	void D3D12Buffer::DestroyBuffer()
	{
	}
}
