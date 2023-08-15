#include "pch.h"
#include "Mesh.h"
#include <assert.h>
Mesh::Mesh(ID3D11Device* pDevice, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, Effect* effect, const dae::Vector3& pos) :
	m_pDevice{ pDevice },
	m_pIndexBuffer{},
	m_pVertexBuffer{},
	m_pInputLayout{},
	m_pEffect{ effect },
	m_Vertices{ vertices },
	m_Indices{ indices }
{
	m_pTechnique = m_pEffect->GetTechnique();
	m_WorldMatrix = dae::Matrix::CreateScale({ 1.0f,1.0f,1.0f }) * dae::Matrix::CreateRotation({ 0.0f,0.0f,0.0f }) * dae::Matrix::CreateTranslation(pos);
	m_PrimitiveTopology = PrimitiveTopology::TriangleList;

	//Create Vertex Layout
	static constexpr uint32_t numElements{ 5 };
	D3D11_INPUT_ELEMENT_DESC vertexDesc[numElements]{};

	vertexDesc[0].SemanticName = "POSITION";
	vertexDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[0].AlignedByteOffset = 0;
	vertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[1].SemanticName = "COLOR";
	vertexDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	vertexDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[2].SemanticName = "TEXCOORD";
	vertexDesc[2].Format = DXGI_FORMAT_R32G32_FLOAT;
	vertexDesc[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	vertexDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[3].SemanticName = "NORMAL";
	vertexDesc[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	vertexDesc[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[4].SemanticName = "TANGENT";
	vertexDesc[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[4].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	vertexDesc[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	//Create Input Layout
	D3DX11_PASS_DESC passDesc{};
	m_pTechnique->GetPassByIndex(0)->GetDesc(&passDesc);

	HRESULT result = pDevice->CreateInputLayout(
		vertexDesc,
		numElements,
		passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize,
		&m_pInputLayout);
	if (FAILED(result))
		assert(false);

	//Create Vertex Buffer
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(Vertex) * static_cast<uint32_t>(vertices.size());
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = vertices.data();

	result = pDevice->CreateBuffer(&bd, &initData, &m_pVertexBuffer);
	if (FAILED(result))
		return;

	//Create Index Buffer
	m_NumIndices = static_cast<uint32_t>(indices.size());
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(uint32_t) * m_NumIndices;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	initData.pSysMem = indices.data();
	result = pDevice->CreateBuffer(&bd, &initData, &m_pIndexBuffer);
	if (FAILED(result))
		return;
}

void Mesh::UpdateFilterMode(bool isUsingDX)
{
	if(isUsingDX)
		m_FilteringTechnique = static_cast<FilteringTechnique>((static_cast<int>(m_FilteringTechnique) + 1) % 3);
	else
		m_FilteringTechnique = static_cast<FilteringTechnique>((static_cast<int>(m_FilteringTechnique) + 1) % 2);

}

std::string Mesh::GetFilterModeName()
{
	switch (m_FilteringTechnique)
	{
	case FilteringTechnique::Point:
		return "Point Filter";
	case FilteringTechnique::Linear:
		return "Linear Filter";
	case FilteringTechnique::Anisotropic:
		return "Anisotropic Filter";
	}
}

void Mesh::UpdateCullMode()
{
	m_CullMode = static_cast<CullMode>((static_cast<int>(m_CullMode) + 1) % 3);
}

std::string Mesh::GetCullModeName()
{
	switch (m_CullMode)
	{
	case CullMode::None:
		return "No Culling";
	case CullMode::Front:
		return "Front Culling";
	case CullMode::Back:
		return "Back Culling";
	}
}

void Mesh::ToggleIsEnabled()
{
	m_IsEnabled = !m_IsEnabled;
}

Mesh::~Mesh()
{

	m_pIndexBuffer->Release();
	m_pVertexBuffer->Release();
	m_pTechnique->Release();
	delete m_pEffect;
	m_pInputLayout->Release();
}

void Mesh::Render(ID3D11DeviceContext* pDeviceContext)
{
	if (m_IsEnabled == false)
		return;

	//Set Primitive Topology
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//Set Input Layout
	pDeviceContext->IASetInputLayout(m_pInputLayout);

	//Set Vertex Buffer
	constexpr UINT stride = sizeof(Vertex);
	constexpr UINT offset = 0;
	pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	//Set Index Buffer
	pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);


	switch (m_FilteringTechnique)
	{
	case Mesh::FilteringTechnique::Point:
		m_pTechniqueName = "PointFiltering";
		break;
	case Mesh::FilteringTechnique::Linear:
		m_pTechniqueName = "LinearFiltering";
		break;
	case Mesh::FilteringTechnique::Anisotropic:
		m_pTechniqueName = "AnisotropicFiltering";
		break;
	}

	switch (m_CullMode)
	{
	case CullMode::None:
	{
		m_pTechniqueName += "NoCull";
	}
	break;
	case CullMode::Front:
	{
		
		m_pTechniqueName += "FrontCull";
	}
	break;
	case CullMode::Back:
	{
		m_pTechniqueName += "BackCull";
	}
	break;
	}


	//Draw
	D3DX11_TECHNIQUE_DESC techDesc{};
	m_pEffect->GetTechniqueByName(m_pTechniqueName.c_str())->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		m_pEffect->GetTechniqueByName(m_pTechniqueName.c_str())->GetPassByIndex(p)->Apply(0, pDeviceContext);
		pDeviceContext->DrawIndexed(m_NumIndices, 0, 0);
	}
}

void Mesh::Update(const dae::Timer* pTimer, const dae::Matrix& worldViewProj, const dae::Matrix& invView, bool enableRotation)
{
	if (enableRotation)
	{
		const float rotationSpeed{ 45 * dae::TO_RADIANS };
		m_WorldMatrix = dae::Matrix::CreateRotationY(rotationSpeed * pTimer->GetElapsed()) * m_WorldMatrix;
	}

	const int matrixSize{ 4 * 4 };
	float WVPMatrix[matrixSize]{};
	float worldMatrix[matrixSize]{};
	float invViewMatrix[matrixSize]{};
	for (int i{}; i < 4; ++i)
	{
		for (int j{}; j < 4; ++j)
		{
			const int index{ i * 4 + j };
			WVPMatrix[index] = worldViewProj[i][j];
			worldMatrix[index] = m_WorldMatrix[i][j];
			invViewMatrix[index] = invView[i][j];
		}
	}

	m_pEffect->Update(WVPMatrix, worldMatrix, invViewMatrix);
}

void Mesh::SetVerticesOut(const std::vector<Vertex_Out>& vOut)
{
	m_Vertices_out = vOut;
}
