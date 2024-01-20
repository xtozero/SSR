#pragma once

#include "Query.h"

#include <d3d11.h>

namespace agl
{
	class D3D11GpuTimer final : public GpuTimer
	{
	public:
		virtual void InitResource() override;
		virtual void FreeResource() override;

		virtual void Begin( ICommandListBase& commandList ) override;
		virtual void End( ICommandListBase& commandList ) override;

		virtual double GetDuration() override;

	private:
		ID3D11Query* m_timeStampBegin = nullptr;
		ID3D11Query* m_timeStampEnd = nullptr;
		ID3D11Query* m_timeStampDisjoint = nullptr;
	};
}