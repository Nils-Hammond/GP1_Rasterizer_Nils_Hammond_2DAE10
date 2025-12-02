#include "Texture.h"
#include "Vector2.h"
#include <SDL_image.h>
#include <iostream>

namespace dae
{
	Texture::Texture(SDL_Surface* pSurface) :
		m_pSurface{ pSurface },
		m_pSurfacePixels{ (uint32_t*)pSurface->pixels }
	{}

	Texture::~Texture()
	{
		if (m_pSurface)
		{
			SDL_FreeSurface(m_pSurface);
			m_pSurface = nullptr;
		}
	}

	Texture* Texture::LoadFromFile(const std::string& path)
	{
		//TODO
		//Load SDL_Surface using IMG_LOAD
		//Create & Return a new Texture Object (using SDL_Surface)


		SDL_Surface* surface = IMG_Load(path.c_str());
		if (surface)
		{
			return new Texture(surface);
		}
		std::cerr << "Failed to load texture: " << path << "\n";
		return nullptr;
	}

	ColorRGB Texture::Sample(const Vector2& uv) const
	{
		//TODO
		//Sample the correct texel for the given uv
		int u = static_cast<int>(uv.x * m_pSurface->w) % m_pSurface->w;
		int v = static_cast<int>(uv.y * m_pSurface->h) % m_pSurface->h;

		if (u < 0) u += m_pSurface->w;
		if (v < 0) v += m_pSurface->h;
		uint32_t pixel = m_pSurfacePixels[u + v * m_pSurface->w];
		uint8_t r, g, b;
		SDL_GetRGB(pixel, m_pSurface->format, &r, &g, &b);
		ColorRGB color{ r / 255.f, g / 255.f, b / 255.f };
		return color;
	}
}