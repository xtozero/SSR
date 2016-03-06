#pragma once

#include <memory>

struct ID3D11DeviceContext;
struct ID3D11SamplerState;
enum SHADER_TYPE;
namespace Microsoft
{
	namespace WRL
	{
		template <typename T> class ComPtr;
	}
}

class ISampler
{
public:
	virtual void SetSampler( ID3D11DeviceContext* pDeviceContext, SHADER_TYPE type ) = 0;

	virtual ~ISampler( ) = default;

protected:
	ISampler( ) = default;
};

std::shared_ptr<ISampler> CreateSampler( const Microsoft::WRL::ComPtr<ID3D11SamplerState>& samplerState );
