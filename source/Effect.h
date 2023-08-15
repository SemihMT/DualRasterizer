#pragma once
#include <vector>
class Texture;

struct dae::Matrix;
//base effect class
class Effect
{
public:
	Effect(ID3D11Device* pDevice, const std::wstring& assetFile);
	virtual ~Effect();

	Effect(const Effect& other) = delete;
	Effect(Effect&& other) = delete;
	Effect& operator=(const Effect& other) = delete;
	Effect& operator=(Effect&& other) = delete;

	void Update(const float* worldViewProj, const float* world, const float* invView);

	//Getters and Setters
	ID3DX11Effect* GetEffect() {  if(m_pEffect != nullptr ) return m_pEffect; else return nullptr;}
	ID3DX11EffectTechnique* GetTechnique() {if(m_pTechnique != nullptr) return m_pTechnique; else return nullptr;}
	ID3DX11EffectTechnique* GetTechniqueByName(const char* techniqueName);

protected:

	ID3DX11Effect* m_pEffect{};
	ID3DX11EffectTechnique* m_pTechnique{};
	std::vector<ID3DX11EffectTechnique*> m_pTechniques{};
	ID3DX11EffectMatrixVariable* m_pMatWorldViewProjVariable{};
	ID3DX11EffectMatrixVariable* m_pMatWorldVariable{};
	ID3DX11EffectMatrixVariable* m_pMatViewInverseVariable{};
	
};

