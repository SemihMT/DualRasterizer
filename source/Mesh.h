#pragma once
#include "Math.h"
#include "Effect.h"
#include "Texture.h"

class Texture;
struct Vertex
{
	dae::Vector3 position{};
	dae::ColorRGB color{};
	dae::Vector2 uv{};
	dae::Vector3 normal{};
	dae::Vector3 tangent{};
};

struct Vertex_Out
{
	dae::Vector4 position{};
	dae::ColorRGB color{ };
	dae::Vector2 uv{};
	dae::Vector3 normal{};
	dae::Vector3 tangent{};
	dae::Vector3 viewDirection{};
};

enum class PrimitiveTopology
{
	TriangleList,
	TriangleStrip
};


class Mesh final
{
public:
	enum class FilteringTechnique
	{
		Point,
		Linear,
		Anisotropic
	};

	enum class CullMode
	{
		None,
		Front,
		Back
	};

	Mesh(ID3D11Device* pDevice, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, Effect* effect, const dae::Vector3& pos);
	~Mesh();

	Mesh(const Mesh& other) = delete;
	Mesh(Mesh&& other) = delete;
	Mesh& operator=(const Mesh& other) = delete;
	Mesh& operator=(Mesh&& other) = delete;

	void Render(ID3D11DeviceContext* pDeviceContext);
	void Update(const dae::Timer* pTimer, const dae::Matrix& worldViewProj, const dae::Matrix& invView, bool enableRotation = true);

	void UpdateFilterMode(bool isUsingDX);
	std::string GetFilterModeName();
	FilteringTechnique GetFilterMode() const { return m_FilteringTechnique; }

	void UpdateCullMode();
	std::string GetCullModeName();
	CullMode GetCullMode() const { return m_CullMode; }

	void ToggleIsEnabled();
	bool GetIsEnabled() const { return m_IsEnabled; }

	dae::Matrix GetWorldMatrix()const { return m_WorldMatrix; }

	std::vector<Vertex>& GetVertices() { return m_Vertices; }
	std::vector<Vertex_Out>& GetVerticesOut() { return m_Vertices_out; }
	void SetVerticesOut(const std::vector<Vertex_Out>& vOut);

	std::vector<uint32_t>& GetIndices() { return m_Indices; }
	PrimitiveTopology GetPrimitiveTopology()const { return m_PrimitiveTopology; }
private:
	ID3D11Device* m_pDevice{};
	ID3D11Buffer* m_pVertexBuffer{};
	uint32_t m_NumIndices{};
	ID3D11Buffer* m_pIndexBuffer{};
	ID3D11InputLayout* m_pInputLayout{};
	Effect* m_pEffect{};
	ID3DX11EffectTechnique* m_pTechnique{};

	dae::Matrix m_WorldMatrix{};
	std::vector<Vertex> m_Vertices{};
	std::vector<uint32_t> m_Indices{};
	PrimitiveTopology m_PrimitiveTopology{ PrimitiveTopology::TriangleStrip };

	std::vector<Vertex_Out> m_Vertices_out{};



	FilteringTechnique m_FilteringTechnique{ FilteringTechnique::Point };
	CullMode m_CullMode{ CullMode::None };
	std::string m_pTechniqueName{ "PointFiltering" };
	bool m_IsEnabled{ true };
};

