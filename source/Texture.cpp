#include "pch.h"
#include "Texture.h"
#include "Vector2.h"
#include <SDL_image.h>


Texture::Texture(ID3D11Device* pDevice, SDL_Surface* pSurface) :
	m_pSurface{ pSurface },
	m_pSurfacePixels{ (uint32_t*)pSurface->pixels }
{
	/*DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
	D3D11_TEXTURE2D_DESC desc{};
	desc.Width = pSurface->w;
	desc.Height = pSurface->h;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = format;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData{};
	initData.pSysMem = pSurface->pixels;
	initData.SysMemPitch = static_cast<UINT>(pSurface->pitch);
	initData.SysMemSlicePitch = static_cast<UINT>(pSurface->h * pSurface->pitch);


	HRESULT hr = pDevice->CreateTexture2D(&desc, &initData, &m_pResource);
	if (FAILED(hr)) return;


	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc{};
	SRVDesc.Format = format;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = 1;


	hr = pDevice->CreateShaderResourceView(m_pResource, &SRVDesc, &m_pSRV);
	if (FAILED(hr)) return;*/

	// SDL_Surface is not needed anymore, so release it
	//SDL_FreeSurface(pSurface);
}

Texture::~Texture()
{

	if (m_pSRV)
		m_pSRV->Release();
	if (m_pResource)
		m_pResource->Release();
	if (m_pSurface)
	{
		SDL_FreeSurface(m_pSurface);
		m_pSurface = nullptr;
	}
}

Texture* Texture::LoadFromFile(ID3D11Device* pDevice, const std::string& path)
{
	//TODO
	//Load SDL_Surface using IMG_LOAD
	//Create & Return a new Texture Object (using SDL_Surface)

	SDL_Surface* img = IMG_Load(path.c_str());
	Texture* texture = new Texture{ pDevice,img };
	DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
	D3D11_TEXTURE2D_DESC desc{};
	desc.Width = img->w;
	desc.Height = img->h;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = format;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData{};
	initData.pSysMem = img->pixels;
	initData.SysMemPitch = static_cast<UINT>(img->pitch);
	initData.SysMemSlicePitch = static_cast<UINT>(img->h * img->pitch);

	HRESULT hr = pDevice->CreateTexture2D(&desc, &initData, &texture->m_pResource);
	if (FAILED(hr))
	{
		delete texture;
		return nullptr;

	}

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc{};
	SRVDesc.Format = format;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = 1;


	hr = pDevice->CreateShaderResourceView(texture->m_pResource, &SRVDesc, &texture->m_pSRV);
	if (FAILED(hr))
	{
		delete texture;
		return nullptr;

	}


	return texture;
}

dae::ColorRGB Texture::Sample(const dae::Vector2& uv, bool useLinearFiltering) const
{
	if (useLinearFiltering)
	{
		// Calculate the floating-point pixel coordinates within the texture
		const float x = uv.x * m_pSurface->w;
		const float y = uv.y * m_pSurface->h;

		// Get the integer coordinates of the four texels surrounding the UV coordinates
		const int x0 = static_cast<int>(x);
		const int x1 = x0 + 1;
		const int y0 = static_cast<int>(y);
		const int y1 = y0 + 1;

		// Calculate the fractional parts of the coordinates
		const float dx = x - static_cast<float>(x0);
		const float dy = y - static_cast<float>(y0);

		// Sample the colors of the four surrounding texels
		const SDL_PixelFormat* format = m_pSurface->format;
		Uint8 red[4];
		Uint8 green[4];
		Uint8 blue[4];

		SDL_GetRGB(m_pSurfacePixels[x0 + y0 * m_pSurface->w], format, &red[0], &green[0], &blue[0]);
		SDL_GetRGB(m_pSurfacePixels[x1 + y0 * m_pSurface->w], format, &red[1], &green[1], &blue[1]);
		SDL_GetRGB(m_pSurfacePixels[x0 + y1 * m_pSurface->w], format, &red[2], &green[2], &blue[2]);
		SDL_GetRGB(m_pSurfacePixels[x1 + y1 * m_pSurface->w], format, &red[3], &green[3], &blue[3]);

		// Perform bilinear interpolation to get the final color
		const float invDx = 1.0f - dx;
		const float invDy = 1.0f - dy;

		const float redF = invDx * invDy * red[0] + dx * invDy * red[1] + invDx * dy * red[2] + dx * dy * red[3];
		const float greenF = invDx * invDy * green[0] + dx * invDy * green[1] + invDx * dy * green[2] + dx * dy * green[3];
		const float blueF = invDx * invDy * blue[0] + dx * invDy * blue[1] + invDx * dy * blue[2] + dx * dy * blue[3];

		// Convert the RGB values to the range [0, 1] and create the final ColorRGB structure
		dae::ColorRGB color;
		color.r = redF / 255.0f;
		color.g = greenF / 255.0f;
		color.b = blueF / 255.0f;

		return color;
	}

	//Sample the correct texel for the given uv
	SDL_PixelFormat* frmt = m_pSurface->format;
	Uint8 red{};
	Uint8 green{};
	Uint8 blue{};
	dae::Vector2 convertedUVRange{ uv.x* m_pSurface->w, uv.y* m_pSurface->h };

	SDL_GetRGB(m_pSurfacePixels[int(convertedUVRange.x) + int(convertedUVRange.y) * m_pSurface->w], frmt, &red, &green, &blue);
	dae::ColorRGB color = { red / 255.f, green / 255.f, blue / 255.f };
	return color;
}