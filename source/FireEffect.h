#pragma once
#include "Effect.h"
class Texture;
class FireEffect final : public Effect
{
public:
		FireEffect(ID3D11Device* pDevice, const std::wstring& assetFile);
		~FireEffect() override;

		void SetDiffuseMap(const Texture* pDiffusetexture);
private:
		ID3DX11EffectShaderResourceVariable* m_pDiffuseMapVariable{};
	
};
