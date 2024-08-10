#if 0
#pragma once

#include "GameObject.h"

namespace logic
{
	class CDisplayShaderResourceHelper : public CGameObject
	{
	public:
		//virtual void SetPosition( const CXMFLOAT3& pos ) override;
		//virtual void Render( CGameLogic& gameLogic ) override;
		virtual void Think( float elapsedTime ) override;

		//virtual void LoadProperty( CGameLogic& gameLogic, const json::Value& json ) override;

		//virtual bool IgnorePicking( ) const override { return ShouldDraw( ); }

		//virtual bool ShouldDraw( ) const override;

	protected:
		// virtual bool LoadModelMesh( CGameLogic& gameLogic ) override;

	private:
		float m_width;
		float m_height;

		std::string m_textureName;

	public:
		CDisplayShaderResourceHelper();
		virtual ~CDisplayShaderResourceHelper() override = default;
	};
}
#endif
