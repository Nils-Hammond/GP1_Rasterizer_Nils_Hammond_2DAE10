#pragma once
#include <cstdint>
#include <vector>
#include "Camera.h"

struct SDL_Window;
struct SDL_Surface;

namespace dae
{
	class Texture;
	struct Mesh;
	struct Vertex;
	struct Vertex_Out;
	class Timer;
	class Scene;
	struct Mesh;

	class Renderer final
	{
	public:
		Renderer(SDL_Window* pWindow);
		~Renderer();

		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) noexcept = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) noexcept = delete;

		void Update(Timer* pTimer);
		void Render();

		bool SaveBufferToImage() const;

		void VertexTransformationFunction(const std::vector<Vertex>& vertices_in, std::vector<Vertex_Out>& vertices_out, const Matrix& worldMatrix) const;

		bool IndicesIntermediaryFunction(const Mesh& mesh, uint32_t& index0, uint32_t& index1, uint32_t& index2, size_t& i) const;

		void ToggleDepthBufferVisualization() { m_DepthBufferVisualizationEnabled = !m_DepthBufferVisualizationEnabled; }
		void ToggleRotation() { m_IsRotating = !m_IsRotating; }

	private:
		void RenderGradient();
		void Render_Week2();
		float DepthRemap(float value, float inMin, float inMax, float min, float max) const;
		void PixelShading(const Vertex_Out& v) const;
		void CullingFunction(std::vector<Vertex_Out>& vertices_out, const Mesh& mesh) const;

		SDL_Window* m_pWindow{};

		Texture* m_pTexture{ nullptr };

		SDL_Surface* m_pFrontBuffer{ nullptr };
		SDL_Surface* m_pBackBuffer{ nullptr };
		uint32_t* m_pBackBufferPixels{};

		//float* m_pDepthBufferPixels{};

		Camera m_Camera{};

		int m_Width{};
		int m_Height{};

		bool m_DepthBufferVisualizationEnabled{ false };
		bool m_IsRotating{ true };

		Mesh* m_pMeshTest{ nullptr };
	};
}
