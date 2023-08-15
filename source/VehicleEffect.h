#pragma once
#include "Effect.h"
class Texture;
class VehicleEffect final : public Effect
{
public:

		VehicleEffect(ID3D11Device* pDevice, const std::wstring& assetFile);
		~VehicleEffect() override;

		void SetDiffuseMap(const Texture* pDiffusetexture);
		void SetNormalMap(const Texture* pNormaltexture);
		void SetSpecularMap(const Texture* pSpeculartexture);
		void SetGlossinessMap(const Texture* pGlossinesstexture);

private:
	ID3DX11EffectShaderResourceVariable* m_pDiffuseMapVariable{};
	ID3DX11EffectShaderResourceVariable* m_pNormalMapVariable{};
	ID3DX11EffectShaderResourceVariable* m_pSpecularMapVariable{};
	ID3DX11EffectShaderResourceVariable* m_pGlossinessMapVariable{};
	
	
};
