#include "pch.h"
#include "Effect.h"

static ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile)
{
	HRESULT result;
	ID3D10Blob* pErrorBlob{ nullptr };
	ID3DX11Effect* pEffect;

	DWORD shaderFlags = 0;
#if defined(DEBUG) || defined( _DEBUG )
	shaderFlags |= D3DCOMPILE_DEBUG;
	shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	result = D3DX11CompileEffectFromFile(assetFile.c_str(),
		nullptr,
		nullptr,
		shaderFlags,
		0,
		pDevice,
		&pEffect,
		&pErrorBlob);

	if (FAILED(result))
	{
		if (pErrorBlob != nullptr)
		{
			const char* pErrors = static_cast<char*>(pErrorBlob->GetBufferPointer());

			std::wstringstream ss;
			for (unsigned int i{ 0 }; i < pErrorBlob->GetBufferSize(); ++i)
			{
				ss << pErrors[i];
			}

			OutputDebugStringW(ss.str().c_str());
			pErrorBlob->Release();
			pErrorBlob = nullptr;

			std::wcout << ss.str() << std::endl;
		}
		else
		{
			std::wstringstream ss;
			ss << "EffectLoader: failed to CreateEffectFromFile!\nPath: " << assetFile;
			std::wcout << ss.str() << std::endl;
			return nullptr;
		}
	}

	return pEffect;
}

Effect::Effect(ID3D11Device* pDevice, const std::wstring& assetFile)
{
	//Create the effect
	m_pEffect = LoadEffect(pDevice, assetFile);


	//Load in the techniques from the assetFile
	m_pTechniques.push_back(m_pEffect->GetTechniqueByName("PointFilteringNoCull"));
	m_pTechniques.push_back(m_pEffect->GetTechniqueByName("LinearFilteringNoCull"));
	m_pTechniques.push_back(m_pEffect->GetTechniqueByName("AnisotropicFilteringNoCull"));

	m_pTechniques.push_back(m_pEffect->GetTechniqueByName("PointFilteringFrontCull"));
	m_pTechniques.push_back(m_pEffect->GetTechniqueByName("LinearFilteringFrontCull"));
	m_pTechniques.push_back(m_pEffect->GetTechniqueByName("AnisotropicFilteringFrontCull"));

	m_pTechniques.push_back(m_pEffect->GetTechniqueByName("PointFilteringBackCull"));
	m_pTechniques.push_back(m_pEffect->GetTechniqueByName("LinearFilteringBackCull"));
	m_pTechniques.push_back(m_pEffect->GetTechniqueByName("AnisotropicFilteringBackCull"));
	

	m_pTechnique = m_pTechniques[0];

	//Report if any of the techniques are not valid
	for (const auto technique : m_pTechniques)
	{
		if (!technique->IsValid())
		{
			D3DX11_TECHNIQUE_DESC* techniqueDesc{ nullptr };
			technique->GetDesc(techniqueDesc);
			std::wstring techniqueName{std::to_wstring(*techniqueDesc->Name)};

			std::wstring techniqueInvalidMessage{L"Technique (" + techniqueName + L") is invalid\n" };
			std::wcout << techniqueInvalidMessage;

		}
	}

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

}

Effect::~Effect()
{
	for (const auto technique : m_pTechniques)
	{
		technique->Release();
	}
	m_pEffect->Release();
}

ID3DX11EffectTechnique* Effect::GetTechniqueByName(const char* techniqueName)
{
	for (const auto technique : m_pTechniques)
	{
		D3DX11_TECHNIQUE_DESC desc{};
		technique->GetDesc(&desc);


		if (std::strcmp(desc.Name, techniqueName) == 0)
		{
			return technique;
		}
	}
	return nullptr;
}


void Effect::Update(const float* worldViewProj, const float* world, const float* invView)
{
	m_pMatWorldViewProjVariable->SetMatrix(worldViewProj);
	m_pMatWorldVariable->SetMatrix(world);
	m_pMatViewInverseVariable->SetMatrix(invView);
}
