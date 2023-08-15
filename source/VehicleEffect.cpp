#include "pch.h"
#include "VehicleEffect.h"
#include "Texture.h"


VehicleEffect::VehicleEffect(ID3D11Device* pDevice, const std::wstring& assetFile):
	Effect{pDevice,assetFile}
{

	//Get stored variables
	m_pMatWorldViewProjVariable = m_pEffect->GetVariableByName("gWorldViewProj")->AsMatrix();
	if (!m_pMatWorldViewProjVariable->IsValid())
		std::wcout << L"m_pMatWorldViewProjVariable is not valid\n";

	m_pMatWorldVariable = m_pEffect->GetVariableByName("gWorldMatrix")->AsMatrix();
	if (!m_pMatWorldVariable->IsValid())
		std::wcout << L"m_pMatWorldVariable is not valid\n";

	m_pMatViewInverseVariable = m_pEffect->GetVariableByName("gViewInverseMatrix")->AsMatrix();
	if (!m_pMatViewInverseVariable->IsValid())
		std::wcout << L"m_pMatViewInverseVariable is not valid\n";


	m_pGlossinessMapVariable = m_pEffect->GetVariableByName("gGlossinessMap")->AsShaderResource();
	if (!m_pGlossinessMapVariable->IsValid())
		std::wcout << L"m_pGlossinessMapVariable is not valid\n";

	m_pNormalMapVariable = m_pEffect->GetVariableByName("gNormalMap")->AsShaderResource();
	if (!m_pNormalMapVariable->IsValid())
		std::wcout << L"m_pNormalMapVariable is not valid\n";

	m_pSpecularMapVariable = m_pEffect->GetVariableByName("gSpecularMap")->AsShaderResource();
	if (!m_pSpecularMapVariable->IsValid())
		std::wcout << L"m_pSpecularMapVariable is not valid\n";

	m_pDiffuseMapVariable = m_pEffect->GetVariableByName("gDiffuseMap")->AsShaderResource();
	if (!m_pDiffuseMapVariable->IsValid())
		std::wcout << L"m_pDiffuseMapVariable is not valid\n";

}

VehicleEffect::~VehicleEffect()
{
	m_pDiffuseMapVariable->Release();
	m_pNormalMapVariable->Release();
	m_pSpecularMapVariable->Release();
	m_pGlossinessMapVariable->Release();

}

void VehicleEffect::SetDiffuseMap(const Texture* pDiffusetexture)
{
	if (m_pDiffuseMapVariable)
		m_pDiffuseMapVariable->SetResource(pDiffusetexture->GetSRV());
}
void VehicleEffect::SetNormalMap(const Texture* pNormaltexture)
{
	if (m_pNormalMapVariable)
		m_pNormalMapVariable->SetResource(pNormaltexture->GetSRV());
}

void VehicleEffect::SetSpecularMap(const Texture* pSpeculartexture)
{
	if (m_pSpecularMapVariable)
		m_pSpecularMapVariable->SetResource(pSpeculartexture->GetSRV());
}

void VehicleEffect::SetGlossinessMap(const Texture* pGlossinesstexture)
{
	if (m_pGlossinessMapVariable)
		m_pGlossinessMapVariable->SetResource(pGlossinesstexture->GetSRV());
}
