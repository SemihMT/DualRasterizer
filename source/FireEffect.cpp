#include "pch.h"
#include "FireEffect.h"
#include "Texture.h"



FireEffect::FireEffect(ID3D11Device* pDevice, const std::wstring& assetFile) :
	Effect{ pDevice,assetFile }
{

	m_pDiffuseMapVariable = m_pEffect->GetVariableByName("gDiffuseMap")->AsShaderResource();
	if (!m_pDiffuseMapVariable->IsValid())
		std::wcout << L"m_pDiffuseMapVariable is not valid\n";
}

FireEffect::~FireEffect()
{
	m_pDiffuseMapVariable->Release();
}

void FireEffect::SetDiffuseMap(const Texture* pDiffusetexture)
{
	if (m_pDiffuseMapVariable)
		m_pDiffuseMapVariable->SetResource(pDiffusetexture->GetSRV());
}
