#pragma once
#include "Math.h"
#include <string>
#include <SDL_surface.h>

class Texture
{
public:
	~Texture();

	static Texture* LoadFromFile(ID3D11Device* pDevice,const std::string& path);
	dae::ColorRGB Sample(const dae::Vector2& uv, bool useLinearFiltering = false) const;
	ID3D11ShaderResourceView* GetSRV() const {return m_pSRV;}

private:
	Texture(ID3D11Device* pDevice,SDL_Surface* pSurface);

	SDL_Surface* m_pSurface{ nullptr };
	uint32_t* m_pSurfacePixels{ nullptr };
	SDL_PixelFormat* m_pFormat{};

	ID3D11Texture2D* m_pResource{nullptr};
	ID3D11ShaderResourceView* m_pSRV{nullptr};
};

