#pragma once
#include <cassert>
#include <SDL_keyboard.h>
#include <SDL_mouse.h>

#include "Maths.h"
#include "Timer.h"

#include <iostream>
namespace dae
{
	struct Camera final
	{
		Camera() = default;
		Camera(const Vector3& _origin, float _fovAngle):
			origin{_origin},
			fovAngle{_fovAngle}
		{}

		Vector3 origin{};
		float fovAngle{90.f};
		float fov{ tanf((fovAngle * TO_RADIANS) / 2.f) };
		float aspectRatio{1};

		float nearPlane{ 0.1f };
		float farPlane{ 1000.f };

		Vector3 forward{Vector3::UnitZ};
		Vector3 up{Vector3::UnitY};
		Vector3 right{Vector3::UnitX};

		float totalPitch{};
		float totalYaw{};

		Matrix invViewMatrix{};
		Matrix viewMatrix{};
		Matrix projectionMatrix{};

		float movementSpeed{ 5.f };
		float rotationSpeed{ 0.1f };

		void Initialize(float _fovAngle = 90.f, Vector3 _origin = {0.f,0.f,0.f})
		{
			fovAngle = _fovAngle;
			fov = tanf((fovAngle * TO_RADIANS) / 2.f);

			origin = _origin;
		}

		void CalculateViewMatrix()
		{
			right = Vector3::Cross(Vector3::UnitY, forward).Normalized();
			up = Vector3::Cross(forward, right).Normalized();
			invViewMatrix = Matrix(right, up, forward, origin);
			viewMatrix = Matrix::Inverse(invViewMatrix);


			//ViewMatrix => Matrix::CreateLookAtLH(...) [not implemented yet]
			//DirectX Implementation => https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3dxmatrixlookatlh
		}

		void CalculateProjectionMatrix()
		{
			projectionMatrix = Matrix::CreatePerspectiveFovLH(fov, aspectRatio, nearPlane, farPlane);

			//ProjectionMatrix => Matrix::CreatePerspectiveFovLH(...) [not implemented yet]
			//DirectX Implementation => https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3dxmatrixperspectivefovlh
		}

		void Update(Timer* pTimer)
		{
			const float deltaTime = pTimer->GetElapsed();

			//Camera Update Logic
			//Keyboard Input
			const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);
			if (pKeyboardState[SDL_SCANCODE_W])
				origin += forward * movementSpeed * deltaTime;
			if (pKeyboardState[SDL_SCANCODE_S])
				origin -= forward * movementSpeed * deltaTime;
			if (pKeyboardState[SDL_SCANCODE_A])
				origin -= right * movementSpeed * deltaTime;
			if (pKeyboardState[SDL_SCANCODE_D])
				origin += right * movementSpeed * deltaTime;
			if (pKeyboardState[SDL_SCANCODE_SPACE])
				origin += up * movementSpeed * deltaTime;
			if (pKeyboardState[SDL_SCANCODE_LCTRL])
				origin -= up * movementSpeed * deltaTime;

			if (pKeyboardState[SDL_SCANCODE_LEFT])
				fovAngle -= 10.f * deltaTime;
			if (pKeyboardState[SDL_SCANCODE_RIGHT])
				fovAngle += 10.f * deltaTime;
			// Function to find which pixel to debug
			if (pKeyboardState[SDL_SCANCODE_L])
			{
				int mouseXAbs, mouseYAbs;
				SDL_GetMouseState(&mouseXAbs, &mouseYAbs);
				std::cout << "Mouse Position: (" << mouseXAbs << ", " << mouseYAbs << ")\n";
			}


			//Mouse Input
			int mouseX{}, mouseY{};
			const uint32_t mouseState = SDL_GetRelativeMouseState(&mouseX, &mouseY);

			if (mouseState == (SDL_BUTTON(SDL_BUTTON_LEFT) | SDL_BUTTON(SDL_BUTTON_RIGHT)))
			{
				origin += up * static_cast<float>(mouseY) * movementSpeed * deltaTime;
				origin -= right * static_cast<float>(mouseX) * movementSpeed * deltaTime;
			}
			else if (mouseState & SDL_BUTTON(SDL_BUTTON_RIGHT))
			{
				totalYaw += mouseX * rotationSpeed;
				totalPitch -= mouseY * rotationSpeed;
				const float yawRad = totalYaw * TO_RADIANS;
				const float pitchRad = totalPitch * TO_RADIANS;
				Matrix rotation = Matrix::CreateRotation(pitchRad, yawRad, 0.f);
				forward = rotation.TransformVector(Vector3::UnitZ).Normalized();
			}
			else if (mouseState & SDL_BUTTON(SDL_BUTTON_LEFT))
			{
				origin -= forward * static_cast<float>(mouseY) * movementSpeed * deltaTime;
				//totalYaw += mouseX * rotationSpeed;
				//const float yawRad = totalYaw * TO_RADIANS;
				//Matrix rotation = Matrix::CreateRotation(0.f, yawRad, 0.f);
				//forward = rotation.TransformVector(forward).Normalized();
			}


			//Update Matrices
			CalculateViewMatrix();
			CalculateProjectionMatrix(); //Try to optimize this - should only be called once or when fov/aspectRatio changes
		}
	};
}
