#include "pch.h"
#include "Renderer.h"
#include "Utils.h"
#include "BRDF.h"
namespace dae {

	Renderer::Renderer(SDL_Window* pWindow) :
		m_pWindow(pWindow)
	{
		//Initialize
		SDL_GetWindowSize(pWindow, &m_Width, &m_Height);

		//Initialize DirectX pipeline
		const HRESULT result = InitializeDirectX();
		if (result == S_OK)
		{
			m_IsInitialized = true;
			std::cout << "DirectX is initialized and ready!\n";
		}
		else
		{
			std::cout << "DirectX initialization failed!\n";
		}

		//Software
		m_pFrontBuffer = SDL_GetWindowSurface(pWindow);
		m_pBackBuffer = SDL_CreateRGBSurface(0, m_Width, m_Height, 32, 0, 0, 0, 0);
		m_pBackBufferPixels = (uint32_t*)m_pBackBuffer->pixels;

		m_pDepthBufferPixels = new float[m_Width * m_Height];

		for (size_t i{}; i < m_Width * m_Height; ++i)
		{
			m_pDepthBufferPixels[i] = FLT_MAX;
		}

		m_pCamera = new Camera{ {0,0,0},45,float(m_Width) / float(m_Height) };

		//Dx

		std::vector<Vertex> vehicleVertices{};
		std::vector<uint32_t> vehicleIndices{};

		std::vector<Vertex> fireVertices{};
		std::vector<uint32_t> fireIndices{};

		Utils::ParseOBJ("Resources/vehicle.obj", vehicleVertices, vehicleIndices);
		Utils::ParseOBJ("Resources/fireFX.obj", fireVertices, fireIndices);


		// Textures
		m_pVehicleDiffuseTexture = Texture::LoadFromFile(m_pDevice, "Resources/vehicle_diffuse.png");
		m_pVehicleNormalTexture = Texture::LoadFromFile(m_pDevice, "Resources/vehicle_normal.png");
		m_pVehicleSpecularTexture = Texture::LoadFromFile(m_pDevice, "Resources/vehicle_specular.png");
		m_pVehicleGlossinessTexture = Texture::LoadFromFile(m_pDevice, "Resources/vehicle_gloss.png");

		m_pFireDiffuseTexture = Texture::LoadFromFile(m_pDevice, "Resources/fireFX_diffuse.png");

		// Effects
		m_pVehicleEffect = new VehicleEffect{ m_pDevice,L"Resources/VehicleShader.fx" };
		m_pVehicleEffect->SetDiffuseMap(m_pVehicleDiffuseTexture);
		m_pVehicleEffect->SetNormalMap(m_pVehicleNormalTexture);
		m_pVehicleEffect->SetSpecularMap(m_pVehicleSpecularTexture);
		m_pVehicleEffect->SetGlossinessMap(m_pVehicleGlossinessTexture);

		m_pFireEffect = new FireEffect{ m_pDevice,L"Resources/FireShader.fx" };
		m_pFireEffect->SetDiffuseMap(m_pFireDiffuseTexture);

		// Meshes
		m_pVehicleMesh = new Mesh{ m_pDevice,vehicleVertices,vehicleIndices, m_pVehicleEffect, m_pCamera->GetOrigin() +
			Vector3{0,0,50} };
		m_pFireMesh = new Mesh{ m_pDevice,fireVertices,fireIndices,m_pFireEffect,m_pCamera->GetOrigin() + Vector3{0,0,50} };


		m_pMeshes.emplace_back(m_pVehicleMesh);
		m_pMeshes.emplace_back(m_pFireMesh);



	}

	Renderer::~Renderer()
	{
		DestructDx();
		DestructSoftware();
	}

	void Renderer::Update(const Timer* pTimer)
	{
		m_pCamera->Update(pTimer);
		Matrix WVPMatrix = m_pVehicleMesh->GetWorldMatrix() * m_pCamera->GetViewMatrix() * m_pCamera->GetProjectionMatrix();
		m_pVehicleMesh->Update(pTimer, WVPMatrix, m_pCamera->GetInvViewMatrix(), m_RotateMeshes);
		m_pFireMesh->Update(pTimer, WVPMatrix, m_pCamera->GetInvViewMatrix(), m_RotateMeshes);
	}


	void Renderer::Render() const
	{
		if (m_UseDX)
			RenderDX();
		else
			RenderSoftware();
	}


	void Renderer::RenderDX() const
	{
		if (!m_IsInitialized)
			return;
		//Clear RTV & DSV
		ColorRGB clearColor = ColorRGB{ m_BGColor };
		m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, &clearColor.r);
		m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

		for (const auto& mesh : m_pMeshes)
		{
			mesh->Render(m_pDeviceContext);
		}

		//Present Backbuffer (swap)
		m_pSwapChain->Present(1, 0);
	}

	void Renderer::RenderSoftware() const
	{
		ClearBackground();
		//@START
		//Lock BackBuffer
		SDL_LockSurface(m_pBackBuffer);
		//fill depthbuffer with max float value each frame
		std::fill_n(m_pDepthBufferPixels, m_Width * m_Height, FLT_MAX);
		SDL_FillRect(m_pBackBuffer, NULL, SDL_MapRGB(m_pBackBuffer->format, Uint8(m_BGColor.r * 255), Uint8(m_BGColor.g * 255), Uint8(m_BGColor.b * 255)));


		VertexTransformationFunction(m_pMeshes);
		//convert NDC to Raster/Screen Space
		std::vector<Vector2> rasterVertices{};
		ConvertToRaster(m_pMeshes, rasterVertices);

		for (size_t meshIdx{}; meshIdx < m_pMeshes.size(); ++meshIdx)
		{

			//loop over each defined triangle
			for (size_t i{}; i < m_pMeshes[meshIdx]->GetIndices().size(); i += 3)
			{
				//indices of the triangle 
				const uint32_t v0Idx{ m_pMeshes[meshIdx]->GetIndices()[i] };
				const uint32_t v1Idx{ m_pMeshes[meshIdx]->GetIndices()[i + 1] };
				const uint32_t v2Idx{ m_pMeshes[meshIdx]->GetIndices()[i + 2] };

				//dont render degenerate triangles
				if (v0Idx == v1Idx || v1Idx == v2Idx || v0Idx == v2Idx)
					continue;

				//Vertices
				const Vertex_Out worldV0{ m_pMeshes[meshIdx]->GetVerticesOut()[v0Idx] };
				const Vertex_Out worldV1{ m_pMeshes[meshIdx]->GetVerticesOut()[v1Idx] };
				const Vertex_Out worldV2{ m_pMeshes[meshIdx]->GetVerticesOut()[v2Idx] };

				//Screen-space vertex coordinates
				const Vector2 v0{ rasterVertices[v0Idx] };
				const Vector2 v1{ rasterVertices[v1Idx] };
				const Vector2 v2{ rasterVertices[v2Idx] };

				if (!IsInFrustum(worldV0, worldV1, worldV2))
				{
					continue;
				}
				if (m_pMeshes[meshIdx]->GetCullMode() != Mesh::CullMode::None)
				{
					//Calculate average triangle normal from the vertex normals we get from parsing the OBJ
					Vector3 avgNormal{ (worldV0.normal + worldV1.normal + worldV2.normal) / 3.0f };
					avgNormal = avgNormal.Normalized();

					Vector3 camViewVec{ -m_pCamera->GetInvViewMatrix().GetAxisZ() };
				
					float dotProduct{ Vector3::Dot(avgNormal,camViewVec) };

					if (m_pMeshes[meshIdx]->GetCullMode() == Mesh::CullMode::Back)
					{
						if(dotProduct < 0)
							continue;
					}
					else if (m_pMeshes[meshIdx]->GetCullMode() == Mesh::CullMode::Front)
					{
						if(dotProduct > 0)
							continue;
					}

				}

				//Edges in screen-space 
				const Vector2 edge01{ v1 - v0 };
				const Vector2 edge12{ v2 - v1 };
				const Vector2 edge20{ v0 - v2 };

				const float totalTriangleArea{ Vector2::Cross(edge01,edge12) };

				Vector2 BBTopLeft{};
				Vector2 BBBottomRight{};
				CreateBoundingBox(v0, v1, v2, BBTopLeft, BBBottomRight);

				//RENDER LOGIC
				for (int px{ int(std::ceil(BBTopLeft.x)) }; px < int(std::ceil(BBBottomRight.x)); ++px)
				{
					for (int py{ int(std::ceil(BBTopLeft.y)) }; py < int(std::ceil(BBBottomRight.y)); ++py)
					{
						const Vector2 currentPixel{ float(px), float(py) };

						const Vector2 v0ToCurrentPixel{ currentPixel - v0 };
						const Vector2 v1ToCurrentPixel{ currentPixel - v1 };
						const Vector2 v2ToCurrentPixel{ currentPixel - v2 };

						const float edge01Check{ Vector2::Cross(edge01,v0ToCurrentPixel) };
						const float edge12Check{ Vector2::Cross(edge12,v1ToCurrentPixel) };
						const float edge20Check{ Vector2::Cross(edge20,v2ToCurrentPixel) };


						if (m_UseBBVis)
						{
							ColorRGB finalColor{ 1,0,0 };
							finalColor.MaxToOne();
							m_pBackBufferPixels[py * m_Width + px] = SDL_MapRGB(m_pBackBuffer->format, Uint8(finalColor.r * 255), Uint8(finalColor.g * 255), Uint8(finalColor.b * 255));
							continue;
						}

						//check if point is in triangle
						if ((edge01Check > 0 && edge12Check > 0 && edge20Check > 0) == false)
						{
							continue;
						}




						const float signedAreav0v1{ edge01Check / 2.f };
						const float signedAreav1v2{ edge12Check / 2.f };
						const float signedAreav2v0{ edge20Check / 2.f };

						//Weights
						const float weightV2{ signedAreav0v1 / totalTriangleArea };
						const float weightV0{ signedAreav1v2 / totalTriangleArea };
						const float weightV1{ signedAreav2v0 / totalTriangleArea };

						const float interpolatedZ
						{
							 1.f / (
									weightV0 / worldV0.position.z +
									weightV1 / worldV1.position.z +
									weightV2 / worldV2.position.z
								   )
						};
						const float interpolatedW
						{
							 1.f / (
									(weightV0 / worldV0.position.w) +
									(weightV1 / worldV1.position.w) +
									(weightV2 / worldV2.position.w)
								   )
						};
						const Vector2 interpolatedUV
						{
							(
								(weightV0 * worldV0.uv / worldV0.position.w) +
								(weightV1 * worldV1.uv / worldV1.position.w) +
								(weightV2 * worldV2.uv / worldV2.position.w)
							) * interpolatedW
						};
						const Vector3 interpolatedNormal
						{
							(
								(weightV0 * worldV0.normal / worldV0.position.w) +
								(weightV1 * worldV1.normal / worldV1.position.w) +
								(weightV2 * worldV2.normal / worldV2.position.w)
							) * interpolatedW
						};

						const Vector3 interpolatedTangent
						{
							(
								(weightV0 * worldV0.tangent / worldV0.position.w) +
								(weightV1 * worldV1.tangent / worldV1.position.w) +
								(weightV2 * worldV2.tangent / worldV2.position.w)
							) * interpolatedW
						};

						const Vector3 interpolatedViewDir
						{
							(
								(weightV0 * worldV0.viewDirection / worldV0.position.w) +
								(weightV1 * worldV1.viewDirection / worldV1.position.w) +
								(weightV2 * worldV2.viewDirection / worldV2.position.w)
							) * interpolatedW
						};

						float depth = m_pDepthBufferPixels[py * m_Width + px];

						//depth test
						if (interpolatedZ < depth)
						{

							//depth write
							const float depthColor{ Remap(interpolatedZ,1.995f,2.f) - 1.f };
							m_pDepthBufferPixels[py * m_Width + px] = interpolatedZ;
							if (m_UseDepthBufferVis)
							{
								ColorRGB finalColor{};
								finalColor = { depthColor,depthColor,depthColor };
								finalColor.MaxToOne();
								m_pBackBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBackBuffer->format,
									static_cast<uint8_t>(finalColor.r * 255),
									static_cast<uint8_t>(finalColor.g * 255),
									static_cast<uint8_t>(finalColor.b * 255));
							}
							else
							{
								PixelShading({ {},{depthColor,depthColor,depthColor},interpolatedUV,interpolatedNormal,interpolatedTangent, interpolatedViewDir }, py * m_Width + px);
							}
						}



					}

				}
			}

			//@END
		//Update SDL Surface
			SDL_UnlockSurface(m_pBackBuffer);
			SDL_BlitSurface(m_pBackBuffer, 0, m_pFrontBuffer, 0);
			SDL_UpdateWindowSurface(m_pWindow);
		}
	}

	void Renderer::UpdateBGColor()
	{
		if (m_UseUniformBgColor == false)
		{
			if (m_UseDX == true)
			{
				m_BGColor = colors::CornflowerBlue;
			}
			else
			{
				m_BGColor = colors::LightGray;
			}
		}
		else
			m_BGColor = colors::DarkGray;
	}

	void Renderer::ToggleRenderMode()
	{
		m_UseDX = !m_UseDX;
		UpdateBGColor();
	}

	void Renderer::CycleCullMode()
	{
		m_pVehicleMesh->UpdateCullMode();
	}

	void Renderer::ToggleUniformClearColor()
	{
		m_UseUniformBgColor = !m_UseUniformBgColor;
		UpdateBGColor();
	}

	void Renderer::CycleShadingMode()
	{
		switch (m_LightingMode)
		{
		case LightingMode::ObservedArea:
			m_LightingMode = LightingMode::Diffuse;
			break;
		case LightingMode::Diffuse:
			m_LightingMode = LightingMode::Specular;
			break;
		case LightingMode::Specular:
			m_LightingMode = LightingMode::Combined;
			break;
		case LightingMode::Combined:
			m_LightingMode = LightingMode::ObservedArea;
			break;
		default:
			// Handle invalid mode or add more cases if necessary.
			std::cerr << "Error: Invalid lighting mode.\n";
			break;
		}
	}


	HRESULT Renderer::InitializeDirectX()
	{
		//Create Device & Device Context:

		D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_1;
		uint32_t createDeviceFlags = 0;

#if defined(DEBUG) || defined(_DEBUG)
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		HRESULT result = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, createDeviceFlags, &featureLevel, 1, D3D11_SDK_VERSION, &m_pDevice, nullptr, &m_pDeviceContext);
		if (FAILED(result))
			return result;

		//Setup Swapchain - Create DXGI Factory

		IDXGIFactory1* pDxgiFactory{};
		result = CreateDXGIFactory1(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&pDxgiFactory));
		if (FAILED(result))
			return result;

		//Setup Swapchain - Create Swapchain

		DXGI_SWAP_CHAIN_DESC swapChainDesc{};
		swapChainDesc.BufferDesc.Width = m_Width;
		swapChainDesc.BufferDesc.Height = m_Height;
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 1;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 60;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 1;
		swapChainDesc.Windowed = true;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swapChainDesc.Flags = 0;


		//Setup Swapchain - Get the handle (HWND) from the SDL backbuffer
		SDL_SysWMinfo sysWMInfo{};
		SDL_VERSION(&sysWMInfo.version);
		SDL_GetWindowWMInfo(m_pWindow, &sysWMInfo);
		swapChainDesc.OutputWindow = sysWMInfo.info.win.window;

		//Setup Swapchain - Create SwapChain
		result = pDxgiFactory->CreateSwapChain(m_pDevice, &swapChainDesc, &m_pSwapChain);
		if (FAILED(result))
			return result;

		//Create DepthStencil (DS) & DepthStencilView (DSV)

		// Resource
		D3D11_TEXTURE2D_DESC depthStencilDesc{};
		depthStencilDesc.Width = m_Width;
		depthStencilDesc.Height = m_Height;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.ArraySize = 1;
		depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
		depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthStencilDesc.CPUAccessFlags = 0;
		depthStencilDesc.MiscFlags = 0;

		// View
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
		depthStencilViewDesc.Format = depthStencilDesc.Format;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;

		result = m_pDevice->CreateTexture2D(&depthStencilDesc, nullptr, &m_pDepthStencilBuffer);
		if (FAILED(result))
			return result;

		result = m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, &depthStencilViewDesc, &m_pDepthStencilView);
		if (FAILED(result))
			return result;

		//Create RenderTarget (RT) & RenderTargetView (RTV)

		// Resource
		result = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&m_pRenderTargetBuffer));
		if (FAILED(result))
			return result;

		// View
		result = m_pDevice->CreateRenderTargetView(m_pRenderTargetBuffer, nullptr, &m_pRenderTargetView);
		if (FAILED(result))
			return result;

		//Bind RTV & DSV to Output Merger Stage

		m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

		//Set Viewport

		D3D11_VIEWPORT viewport{};
		viewport.Width = static_cast<float>(m_Width);
		viewport.Height = static_cast<float>(m_Height);
		viewport.TopLeftX = 0.f;
		viewport.TopLeftY = 0.f;
		viewport.MinDepth = 0.f;
		viewport.MaxDepth = 1.f;
		m_pDeviceContext->RSSetViewports(1, &viewport);


		return result;
	}

	void Renderer::DestructDx()
	{
		m_pRenderTargetView->Release();
		m_pRenderTargetBuffer->Release();
		m_pDepthStencilView->Release();
		m_pDepthStencilBuffer->Release();
		m_pSwapChain->Release();
		if (m_pDeviceContext)
		{
			m_pDeviceContext->ClearState();
			m_pDeviceContext->Flush();
			m_pDeviceContext->Release();
		}
		delete m_pVehicleDiffuseTexture;
		delete m_pVehicleNormalTexture;
		delete m_pVehicleSpecularTexture;
		delete m_pVehicleGlossinessTexture;
		delete m_pFireDiffuseTexture;

		delete m_pVehicleMesh;
		delete m_pFireMesh;
		delete m_pCamera;
		m_pDevice->Release();
	}

	void Renderer::DestructSoftware()
	{
		delete[] m_pDepthBufferPixels;
	}

	void Renderer::VertexTransformationFunction(const std::vector<Mesh*>& mesh_in) const
	{
		std::vector<Vertex_Out> temp{};
		for (const auto& m : mesh_in)
		{
			const Matrix worldViewProjection{ m->GetWorldMatrix() * m_pCamera->GetViewMatrix() * m_pCamera->GetProjectionMatrix() };

			for (const Vertex& v : m->GetVertices())
			{
				Vertex_Out vOut{ {},v.color,v.uv };
				vOut.position = worldViewProjection.TransformPoint({ v.position,1.f });

				//perspective divide to convert to NDC
				vOut.position.x = vOut.position.x / vOut.position.w;
				vOut.position.y = vOut.position.y / vOut.position.w;
				vOut.position.z = vOut.position.z / vOut.position.w;

				vOut.normal = m->GetWorldMatrix().TransformVector(v.normal);
				vOut.tangent = m->GetWorldMatrix().TransformVector(v.tangent);

				vOut.viewDirection = v.position;
				vOut.viewDirection = vOut.viewDirection.Normalized();


				temp.emplace_back(vOut);
			}
			m->SetVerticesOut(temp);

		}
	}

	void Renderer::ConvertToRaster(const std::vector<Mesh*>& meshes, std::vector<Vector2>& rasterVerts) const
	{
		for (const auto& mesh : meshes)
		{
			for (const auto& ndc : mesh->GetVerticesOut())
			{

				rasterVerts.push_back(

					{ ((ndc.position.x + 1) / 2.0f) * m_Width,
					((1.0f - ndc.position.y) / 2.0f) * m_Height }
				);
			}
		}
	}

	//Fill screen with black
	void Renderer::ClearBackground() const
	{
		SDL_FillRect(m_pBackBuffer, NULL, SDL_MapRGB(m_pBackBuffer->format, 0, 0, 0));
	}
}

float dae::Renderer::CalculateTriangleArea(const Vector2& edge01, const Vector2& edge12, const Vector2& edge20) const
{
	const float s{ (edge01.Magnitude() + edge12.Magnitude() + edge20.Magnitude()) / 2.f };
	return sqrtf(s * (s - edge01.Magnitude()) * (s - edge12.Magnitude()) * (s - edge20.Magnitude()));
}

void dae::Renderer::CreateBoundingBox(const Vector2& v0, const Vector2& v1, const Vector2& v2, Vector2& topLeft,
	Vector2& bottomRight)const
{
	//find leftmost point 
	topLeft = { std::min({v0.x,v1.x,v2.x}),std::min({v0.y,v1.y,v2.y}) };
	//find rightmost point
	bottomRight = { std::max({v0.x,v1.x,v2.x}),std::max({v0.y,v1.y,v2.y}) };


	topLeft.y = std::clamp(topLeft.y - 1.f, 0.f, float(m_Height) - 1.f);
	topLeft.x = std::clamp(topLeft.x - 1.f, 0.f, float(m_Width) - 1.f);

	bottomRight.y = std::clamp(bottomRight.y + 1.f, 0.f, float(m_Height) - 1.f);
	bottomRight.x = std::clamp(bottomRight.x + 1.f, 0.f, float(m_Width) - 1.f);
}

bool dae::Renderer::IsInFrustum(const Vertex_Out& v0, const Vertex_Out& v1, const Vertex_Out& v2) const
{
	if (v0.position.x < -1.0f || v0.position.x > 1.0f || v0.position.y < -1.0f || v0.position.y > 1.0f || v0.position.z < 0.0f || v0.position.z > 1.0f ||
		v1.position.x < -1.0f || v1.position.x > 1.0f || v1.position.y < -1.0f || v1.position.y > 1.0f || v1.position.z < 0.0f || v1.position.z > 1.0f ||
		v2.position.x < -1.0f || v2.position.x > 1.0f || v2.position.y < -1.0f || v2.position.y > 1.0f || v2.position.z < 0.0f || v2.position.z > 1.0f)
	{
		return false;
	}
	return true;
}

void dae::Renderer::PixelShading(const Vertex_Out& v, int pixelIdx) const
{
	bool useLinearFilter{ false };
	if (m_pVehicleMesh->GetFilterModeName().find("Linear") != std::string::npos)
	{
		useLinearFilter = true;
	}

	const Vector3 lightDirection{ .577f,-.577f,.577f };


	const float shininess{ 25.f };
	const ColorRGB ambient{ 0.025f,0.025f,0.025f };

	//Normal stuff
	const Vector3 binormal{ Vector3::Cross(v.normal,v.tangent) };
	const Matrix tangentSpaceAxis{ v.tangent,binormal,v.normal,{0,0,0} };
	const ColorRGB normal{ (2.f * m_pVehicleNormalTexture->Sample(v.uv,useLinearFilter)) - ColorRGB{1.f,1.f,1.f} };
	const Vector3 sample{ normal.r,normal.g,normal.b };
	Vector3 sampledNormal = tangentSpaceAxis.TransformVector(sample.Normalized());

	if (!m_UseNormalMap)
	{
		sampledNormal = v.normal;
	}

	const ColorRGB lambert{ BRDF::Lambert(1.f, m_pVehicleDiffuseTexture->Sample(v.uv,useLinearFilter)) };
	const float phongExp{ shininess * m_pVehicleGlossinessTexture->Sample(v.uv,useLinearFilter).r };
	const ColorRGB specular{ m_pVehicleSpecularTexture->Sample(v.uv).r * BRDF::Phong(1.0f,phongExp,lightDirection.Normalized(),v.viewDirection,sampledNormal.Normalized()) };
	const float observedArea{ std::max(Vector3::Dot(sampledNormal.Normalized(),-lightDirection),0.0f) };


	ColorRGB finalColor{};

	const float lightIntensity{ 7.f };
	switch (m_LightingMode)
	{
	case LightingMode::ObservedArea:
		finalColor += {observedArea, observedArea, observedArea};
		break;
	case LightingMode::Diffuse:
		finalColor += lambert;
		break;
	case LightingMode::Specular:
		finalColor += specular;
		break;
	case LightingMode::Combined:
		finalColor += (lightIntensity * (lambert + specular)) * observedArea;
		break;
	default:
		break;
	}

	finalColor += ambient;
	finalColor.MaxToOne();

	m_pBackBufferPixels[pixelIdx] = SDL_MapRGB(m_pBackBuffer->format,
		static_cast<uint8_t>(finalColor.r * 255),
		static_cast<uint8_t>(finalColor.g * 255),
		static_cast<uint8_t>(finalColor.b * 255));
}
