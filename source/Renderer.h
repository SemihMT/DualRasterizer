#pragma once
#include "Mesh.h"
#include "VehicleEffect.h"
#include "Camera.h"
#include "FireEffect.h"
struct SDL_Window;
struct SDL_Surface;
class Mesh;
namespace dae
{
	class Renderer final
	{
	public:
		enum class LightingMode
		{
			ObservedArea,
			Diffuse,
			Specular,
			Combined
		};


		Renderer(SDL_Window* pWindow);
		~Renderer();

		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) noexcept = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) noexcept = delete;

		void Update(const Timer* pTimer);
		void Render() const;
		void RenderDX() const;
		void RenderSoftware() const;

		Mesh* GetVehicleMesh() const { return m_pVehicleMesh; }
		Mesh* GetFireMesh() const { return m_pFireMesh; }

		void UpdateBGColor();
		void ToggleRenderMode();
		std::string GetRenderingMode() const { return m_UseDX ? "DirectX 11" : "Software"; }

		// Shared Settings
		void ToggleMeshRotation() { m_RotateMeshes = !m_RotateMeshes; }
		bool GetRotateMeshes() const { return m_RotateMeshes; }
		void CycleCullMode();
		void ToggleUniformClearColor();
		bool GetUseUniformClearColor() const { return m_UseUniformBgColor; }

		// Software only
		void CycleShadingMode();
		LightingMode GetShadingMode() const { return m_LightingMode; }
		void ToggleNormalMap() { m_UseNormalMap = !m_UseNormalMap; }
		bool GetUseNormalMap() const { return m_UseNormalMap; }
		void ToggleDepthBufferVis() { m_UseDepthBufferVis = !m_UseDepthBufferVis; }
		bool GetUseDepthBufferVis() const { return m_UseDepthBufferVis; }
		void ToggleBBVis() { m_UseBBVis = !m_UseBBVis; }
		bool GetUseBBVis() const { return m_UseBBVis; }




	private:
		SDL_Window* m_pWindow{};

		int m_Width{};
		int m_Height{};

		Camera* m_pCamera{};

		// ---Textures---
		Texture* m_pVehicleDiffuseTexture{};
		Texture* m_pVehicleNormalTexture{};
		Texture* m_pVehicleSpecularTexture{};
		Texture* m_pVehicleGlossinessTexture{};
		Texture* m_pFireDiffuseTexture{};

		// ---Effects---
		VehicleEffect* m_pVehicleEffect{};
		FireEffect* m_pFireEffect{};
		// ---Meshes---
		Mesh* m_pVehicleMesh{};
		Mesh* m_pFireMesh{};

		std::vector<Mesh*> m_pMeshes;

		bool m_IsInitialized{ false };

		ID3D11Device* m_pDevice{};
		ID3D11DeviceContext* m_pDeviceContext{};
		IDXGISwapChain* m_pSwapChain{};
		ID3D11Texture2D* m_pDepthStencilBuffer{};
		ID3D11DepthStencilView* m_pDepthStencilView{};

		ID3D11Resource* m_pRenderTargetBuffer{};
		ID3D11RenderTargetView* m_pRenderTargetView{};



		//Software
		SDL_Surface* m_pFrontBuffer{ nullptr };
		SDL_Surface* m_pBackBuffer{ nullptr };
		uint32_t* m_pBackBufferPixels{};
		float* m_pDepthBufferPixels{};


		void ClearBackground() const;
		void DestructDx();
		void DestructSoftware();

		void VertexTransformationFunction(const std::vector<Mesh*>& mesh_in) const;
		void ConvertToRaster(const std::vector<Mesh*>& meshes, std::vector<Vector2>& rasterVerts) const;

		float CalculateTriangleArea(const Vector2& edge01, const Vector2& edge12, const Vector2& edge20) const;
		void CreateBoundingBox(const Vector2& v0, const Vector2& v1, const Vector2& v2, Vector2& topLeft, Vector2& bottomRight) const;
		bool IsInFrustum(const Vertex_Out& v0, const Vertex_Out& v1, const Vertex_Out& v2) const;

		void PixelShading(const Vertex_Out& v, int pixelIdx) const;

		bool m_UseNormalMap{ true };
		bool m_UseDepthBufferVis{ false };
		bool m_UseBBVis{ false };



		LightingMode m_LightingMode{ LightingMode::Combined };

		bool m_UseDX{ false };
		bool m_RotateMeshes{ false };
		bool m_UseUniformBgColor{ false };
		ColorRGB m_BGColor{ m_UseDX ? colors::CornflowerBlue : colors::LightGray };
		//DIRECTX
		HRESULT InitializeDirectX();
		//...


	};
}
