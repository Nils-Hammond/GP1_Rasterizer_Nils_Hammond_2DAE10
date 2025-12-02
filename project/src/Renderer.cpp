//External includes
#include "SDL.h"
#include "SDL_surface.h"

//Project includes
#include "Renderer.h"
#include "Maths.h"
#include "Texture.h"
#include "Utils.h"

#include <iostream>

using namespace dae;

Renderer::Renderer(SDL_Window* pWindow) :
	m_pWindow(pWindow)
{
	//Initialize
	SDL_GetWindowSize(pWindow, &m_Width, &m_Height);

	//Create Buffers
	m_pFrontBuffer = SDL_GetWindowSurface(pWindow);
	m_pBackBuffer = SDL_CreateRGBSurface(0, m_Width, m_Height, 32, 0, 0, 0, 0);
	m_pBackBufferPixels = (uint32_t*)m_pBackBuffer->pixels;

	//m_pDepthBufferPixels = new float[m_Width * m_Height];

	//Load Texture
	m_pTexture = Texture::LoadFromFile("Resources/vehicle_diffuse.png");

	//Initialize Camera
	m_Camera.Initialize(45.f, { .0f,.0f,.0f });
	float aspectRatio = m_Width / static_cast<float>(m_Height);
	m_Camera.aspectRatio = aspectRatio;

	//Load Mesh
	m_pMeshTest = new Mesh{};
	Utils::ParseOBJ("Resources/vehicle.obj", m_pMeshTest->vertices, m_pMeshTest->indices);
	m_pMeshTest->primitiveTopology = PrimitiveTopology::TriangleList;
	m_pMeshTest->worldMatrix = Matrix::CreateTranslation(0, 0, 50);
}

Renderer::~Renderer()
{
	delete m_pMeshTest;
	//delete[] m_pDepthBufferPixels;
}

void Renderer::Update(Timer* pTimer)
{
	m_Camera.Update(pTimer);
	if (m_IsRotating)
		m_pMeshTest->worldMatrix = Matrix::CreateRotationY(pTimer->GetTotal() * 0.5f) * Matrix::CreateTranslation(0, 0, 50);
}

void Renderer::Render()
{
	//@START
	//Lock BackBuffer
	SDL_LockSurface(m_pBackBuffer);
	
	//Clear Buffers
	SDL_FillRect(m_pBackBuffer, 0, SDL_MapRGB(m_pBackBuffer->format, 100, 100, 100));

	float aspectRatio = m_Width / static_cast<float>(m_Height);
	float fov = m_Camera.fov;

	//RENDER LOGIC
	//RenderGradient();
	Render_Week2();

	//@END
	//Update SDL Surface
	SDL_UnlockSurface(m_pBackBuffer);
	SDL_BlitSurface(m_pBackBuffer, 0, m_pFrontBuffer, 0);
	SDL_UpdateWindowSurface(m_pWindow);
}

void Renderer::Render_Week2()
{
	//Test Meshes
	
	std::vector<Mesh> meshes_world{
		Mesh{
			{
				Vertex{{-3, 3, -2},		colors::White,	{ 0,0 }},
				Vertex{{0, 3, -2},		colors::White,	{.5f, 0}},
				Vertex{{3, 3, -2},		colors::White,	{1, 0}},
				Vertex{{-3, 0, -2},		colors::White,	{0, .5f}},
				Vertex{{0, 0, -2},		colors::White,	{.5f, .5f}},
				Vertex{{3, 0, -2},		colors::White,	{1, .5f}},
				Vertex{{-3, -3, -2},	colors::White,	{0, 1}},
				Vertex{{0, -3, -2},		colors::White,	{.5f, 1}},
				Vertex{{3, -3, -2},		colors::White,	{1, 1}}
			},
			{
				3, 0, 4, 1, 5, 2,
				2, 6,
				6, 3, 7, 4, 8, 5
			},
			PrimitiveTopology::TriangleStrip,
		},
		Mesh{
			{
				Vertex{{-3, 3, -2},		colors::White,	{ 0,0 }},
				Vertex{{0, 3, -2},		colors::White,	{.5f, 0}},
				Vertex{{3, 3, -2},		colors::White,	{1, 0}},
				Vertex{{-3, 0, -2},		colors::White,	{0, .5f}},
				Vertex{{0, 0, -2},		colors::White,	{.5f, .5f}},
				Vertex{{3, 0, -2},		colors::White,	{1, .5f}},
				Vertex{{-3, -3, -2},	colors::White,	{0, 1}},
				Vertex{{0, -3, -2},		colors::White,	{.5f, 1}},
				Vertex{{3, -3, -2},		colors::White,	{1, 1}}

			},
			{
			3, 0, 1,	1, 4, 3,	4, 1, 2,
			2, 5, 4,	6, 3, 4,	4, 7, 6,
			7, 4, 5,	5, 8, 7
			},
			PrimitiveTopology::TriangleList,
		}
	};
	

	std::vector<Mesh> meshes{};
	meshes.push_back(*m_pMeshTest);
	meshes.push_back(meshes_world[0]);

	std::vector<float> depthBuffer(m_Width * m_Height, FLT_MAX);

	for (Mesh& mesh : meshes)
	{
		VertexTransformationFunction(mesh.vertices, mesh.vertices_out, mesh.worldMatrix);

		for (size_t i = 0; i < mesh.indices.size();)
		{
			uint32_t v0Index{}, v1Index{}, v2Index{};
			if (IndicesIntermediaryFunction(mesh, v0Index, v1Index, v2Index, i) == false)
				break;
			
			// Check if triangle is valid
			if (v0Index == v1Index || v1Index == v2Index || v2Index == v0Index)
				continue;

			//Frustrum Culling
			if (mesh.vertices_out[v0Index].position.x < -1 || mesh.vertices_out[v0Index].position.x > 1 ||
				mesh.vertices_out[v1Index].position.x < -1 || mesh.vertices_out[v1Index].position.x > 1 ||
				mesh.vertices_out[v2Index].position.x < -1 || mesh.vertices_out[v2Index].position.x > 1)
			{
				continue;
			}
			if (mesh.vertices_out[v0Index].position.y < -1 || mesh.vertices_out[v0Index].position.y > 1 ||
				mesh.vertices_out[v1Index].position.y < -1 || mesh.vertices_out[v1Index].position.y > 1 ||
				mesh.vertices_out[v2Index].position.y < -1 || mesh.vertices_out[v2Index].position.y > 1)
			{
				continue;
			}
			if (mesh.vertices_out[v0Index].position.z < 0 || mesh.vertices_out[v0Index].position.z > 1 ||
				mesh.vertices_out[v1Index].position.z < 0 || mesh.vertices_out[v1Index].position.z > 1 ||
				mesh.vertices_out[v2Index].position.z < 0 || mesh.vertices_out[v2Index].position.z > 1)
			{
				continue;
			}

			Vertex_Out verticesScreenSpace[3]{
				mesh.vertices_out[v0Index],
				mesh.vertices_out[v1Index],
				mesh.vertices_out[v2Index]
			};

			//Viewport Transform
			verticesScreenSpace[0].position.x = (verticesScreenSpace[0].position.x + 1) / 2 * m_Width;
			verticesScreenSpace[0].position.y = (1 - verticesScreenSpace[0].position.y) / 2 * m_Height;

			verticesScreenSpace[1].position.x = (verticesScreenSpace[1].position.x + 1) / 2 * m_Width;
			verticesScreenSpace[1].position.y = (1 - verticesScreenSpace[1].position.y) / 2 * m_Height;

			verticesScreenSpace[2].position.x = (verticesScreenSpace[2].position.x + 1) / 2 * m_Width;
			verticesScreenSpace[2].position.y = (1 - verticesScreenSpace[2].position.y) / 2 * m_Height;

			//Bounding Box
			int topLeftPixelX = static_cast<int>(std::min({ verticesScreenSpace[0].position.x, verticesScreenSpace[1].position.x, verticesScreenSpace[2].position.x }));
			int topLeftPixelY = static_cast<int>(std::min({ verticesScreenSpace[0].position.y, verticesScreenSpace[1].position.y, verticesScreenSpace[2].position.y }));
			int bottomRightPixelX = static_cast<int>(std::max({ verticesScreenSpace[0].position.x, verticesScreenSpace[1].position.x, verticesScreenSpace[2].position.x })) + 1;
			int bottomRightPixelY = static_cast<int>(std::max({ verticesScreenSpace[0].position.y, verticesScreenSpace[1].position.y, verticesScreenSpace[2].position.y })) + 1;
			topLeftPixelX = std::clamp(topLeftPixelX, 0, m_Width);
			topLeftPixelY = std::clamp(topLeftPixelY, 0, m_Height);
			bottomRightPixelX = std::clamp(bottomRightPixelX, 0, m_Width);
			bottomRightPixelY = std::clamp(bottomRightPixelY, 0, m_Height);

			//Rasterization
			for (int px{ topLeftPixelX }; px < bottomRightPixelX; ++px)
			{
				for (int py{ topLeftPixelY }; py < bottomRightPixelY; ++py)
				{
					Vector2 pixelCenter{ px + 0.5f, py + 0.5f };
					const Vector2 v0{ verticesScreenSpace[0].position.x, verticesScreenSpace[0].position.y };
					const Vector2 v1{ verticesScreenSpace[1].position.x, verticesScreenSpace[1].position.y };
					const Vector2 v2{ verticesScreenSpace[2].position.x, verticesScreenSpace[2].position.y };

					const Vector2 v1v0 = v1 - v0;
					const Vector2 v2v1 = v2 - v1;
					const Vector2 v0v2 = v0 - v2;
					const Vector2 toPixel0 = pixelCenter - v0;
					const Vector2 toPixel1 = pixelCenter - v1;
					const Vector2 toPixel2 = pixelCenter - v2;

					const float crossV0 = Vector2::Cross(v2v1, toPixel1);
					const float crossV1 = Vector2::Cross(v0v2, toPixel2);
					const float crossV2 = Vector2::Cross(v1v0, toPixel0);

					if (crossV0 >= 0 &&
						crossV1 >= 0 &&
						crossV2 >= 0)
					{
						const float triangleArea = Vector2::Cross(v1v0, v2 - v0);
						const float barycentric0 = crossV0 / triangleArea;
						const float barycentric1 = crossV1 / triangleArea;
						const float barycentric2 = crossV2 / triangleArea;

						// Depth Interpolation
						const float zDepthValue = 1 / (barycentric0 / verticesScreenSpace[0].position.z +
							barycentric1 / verticesScreenSpace[1].position.z +
							barycentric2 / verticesScreenSpace[2].position.z);

						if (zDepthValue >= depthBuffer[px + py * m_Width])
							continue;

						depthBuffer[px + py * m_Width] = zDepthValue;


						const float interpolatedDepth = 1 / (barycentric0 * verticesScreenSpace[0].position.w +
							barycentric1 * verticesScreenSpace[1].position.w +
							barycentric2 * verticesScreenSpace[2].position.w);

						// Texture Coordinate Interpolation
						const Vector2 interpolatedUV{ (	barycentric0 * verticesScreenSpace[0].uv * verticesScreenSpace[0].position.w +
														barycentric1 * verticesScreenSpace[1].uv * verticesScreenSpace[1].position.w +
														barycentric2 * verticesScreenSpace[2].uv * verticesScreenSpace[2].position.w) * interpolatedDepth};

						ColorRGB sampledColor = m_pTexture->Sample(interpolatedUV);
						if (mesh.indices.size() <= 50)
						{
							sampledColor.MaxToOne();

							m_pBackBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBackBuffer->format,
								static_cast<uint8_t>(sampledColor.r * 255),
								static_cast<uint8_t>(sampledColor.g * 255),
								static_cast<uint8_t>(sampledColor.b * 255));
							continue;
						}

						if (m_DepthBufferVisualizationEnabled)
						{
							float depthGrayValue = DepthRemap(depthBuffer[px + py * m_Width], 0.93f, 1.f, 0.f, 1.f);
							sampledColor = ColorRGB{ depthGrayValue, depthGrayValue, depthGrayValue };

							sampledColor.MaxToOne();

							m_pBackBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBackBuffer->format,
								static_cast<uint8_t>(sampledColor.r * 255),
								static_cast<uint8_t>(sampledColor.g * 255),
								static_cast<uint8_t>(sampledColor.b * 255));

						}
						else
						{
							if (px == 173 && py == 336)
							{
								int debug = 0;
							}

							Vertex_Out interpolatedVertex{};
							interpolatedVertex.position = Vector4((barycentric0 * verticesScreenSpace[0].position.x * verticesScreenSpace[0].position.w +
								barycentric1 * verticesScreenSpace[1].position.x * verticesScreenSpace[1].position.w +
								barycentric2 * verticesScreenSpace[2].position.x * verticesScreenSpace[2].position.w) * interpolatedDepth,
								(barycentric0 * verticesScreenSpace[0].position.y * verticesScreenSpace[0].position.w +
								barycentric1 * verticesScreenSpace[1].position.y * verticesScreenSpace[1].position.w +
								barycentric2 * verticesScreenSpace[2].position.y * verticesScreenSpace[2].position.w) * interpolatedDepth,
								zDepthValue,
								interpolatedDepth);
							interpolatedVertex.normal = ((barycentric0 * verticesScreenSpace[0].normal +
								barycentric1 * verticesScreenSpace[1].normal +
								barycentric2 * verticesScreenSpace[2].normal)).Normalized();
							interpolatedVertex.color = colors::White;
							interpolatedVertex.uv = interpolatedUV;
							PixelShading(interpolatedVertex);
						}

					}
				}
			}
		}
	}
}

float dae::Renderer::DepthRemap(float value, float inMin, float inMax, float min, float max) const
{
	return min + (value - inMin) * (max - min) / (inMax - inMin);
}

void dae::Renderer::PixelShading(const Vertex_Out& v) const
{
	Vector3 lightDir{ 0.577f, -0.577f, 0.577f };

	float lambertTerm = Vector3::Dot(v.normal.Normalized(), -lightDir.Normalized());
	if (lambertTerm < 0)
		lambertTerm = 0;

	float diffuseReflectance = 7.0f;
	ColorRGB sampledColor = m_pTexture->Sample(v.uv);
	ColorRGB lambertBRDF = (ColorRGB(diffuseReflectance, diffuseReflectance, diffuseReflectance) * sampledColor) / M_PI;

	ColorRGB finalColor = lambertBRDF * lambertTerm;

	finalColor.MaxToOne();

	m_pBackBufferPixels[static_cast<int>(v.position.x) + (static_cast<int>(v.position.y) * m_Width)] = SDL_MapRGB(m_pBackBuffer->format,
		static_cast<uint8_t>(finalColor.r * 255),
		static_cast<uint8_t>(finalColor.g * 255),
		static_cast<uint8_t>(finalColor.b * 255));
}

void dae::Renderer::CullingFunction(std::vector<Vertex_Out>& vertices_out, const Mesh& mesh) const
{
}

void Renderer::RenderGradient()
{
	for (int px{}; px < m_Width; ++px)
	{
		for (int py{}; py < m_Height; ++py)
		{
			float gradient = px / static_cast<float>(m_Width);
			gradient += py / static_cast<float>(m_Width);
			gradient /= 2.0f;

			ColorRGB finalColor{ gradient, gradient, gradient };

			//Update Color in Buffer
			finalColor.MaxToOne();

			m_pBackBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBackBuffer->format,
				static_cast<uint8_t>(finalColor.r * 255),
				static_cast<uint8_t>(finalColor.g * 255),
				static_cast<uint8_t>(finalColor.b * 255));
		}
	}
}

// Don't forget to transform normals now
void Renderer::VertexTransformationFunction(const std::vector<Vertex>& vertices_in, std::vector<Vertex_Out>& vertices_out, const Matrix& worldMatrix) const
{
	Matrix viewMatrix = m_Camera.viewMatrix;
	Matrix projectionMatrix = m_Camera.projectionMatrix;
	Matrix worldViewProjectionMatrix = worldMatrix * viewMatrix * projectionMatrix;

	for (const Vertex& vertex : vertices_in)
	{
		Vertex_Out vertex_out{};
		vertex_out.position = worldViewProjectionMatrix.TransformPoint(Vector4(vertex.position,1));
		vertex_out.position.w = 1.f / vertex_out.position.w;

		//Matrix invTransposeWorld = worldMatrix;
		//invTransposeWorld.Inverse();
		//invTransposeWorld.Transpose();
		//vertex_out.normal = invTransposeWorld.TransformVector(vertex.normal).Normalized();
		vertex_out.normal = worldMatrix.TransformVector(vertex.normal).Normalized();

		//Perspective Divide
		vertex_out.position.x *= vertex_out.position.w;
		vertex_out.position.y *= vertex_out.position.w;
		vertex_out.position.z *= vertex_out.position.w;

		//vertex_out.position.x = vertex_out.position.x / (aspectRatio * camera.fov);
		//vertex_out.position.y = vertex_out.position.y / camera.fov;

		//Viewport Transform
		//vertex_out.position.x = (vertex_out.position.x + 1) / 2 * m_Width;
		//vertex_out.position.y = (1 - vertex_out.position.y) / 2 * m_Height;
		
		vertex_out.color = vertex.color;
		vertex_out.uv = vertex.uv;
		vertex_out.tangent = vertex.tangent;
		vertices_out.emplace_back(vertex_out);
	}
}

bool dae::Renderer::IndicesIntermediaryFunction(const Mesh& mesh, uint32_t& v0Index, uint32_t& v1Index, uint32_t& v2Index, size_t& i) const
{
	if (mesh.primitiveTopology == PrimitiveTopology::TriangleStrip)
	{
		if (i + 2 >= mesh.indices.size())
			return false;
		v0Index = mesh.indices[i];
		if (i % 2 == 0)
		{
			v1Index = mesh.indices[i + 1];
			v2Index = mesh.indices[i + 2];
		}
		else
		{
			v1Index = mesh.indices[i + 2];
			v2Index = mesh.indices[i + 1];
		}
		++i;
	}
	else //Triangle List
	{
		v0Index = mesh.indices[i];
		v1Index = mesh.indices[i + 1];
		v2Index = mesh.indices[i + 2];
		i += 3;
	}
	return true;
}

bool Renderer::SaveBufferToImage() const
{
	return SDL_SaveBMP(m_pBackBuffer, "Rasterizer_ColorBuffer.bmp");
}
