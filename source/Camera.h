#pragma once
#include <cassert>
#include <SDL_keyboard.h>
#include <SDL_mouse.h>

#include "Math.h"
#include "Timer.h"

namespace dae
{
	struct Camera
	{
		Camera() = default;

		Camera(const Vector3& _origin, float _fovAngle, float _aspectRatio) :
			origin{ _origin },
			fovAngle{ _fovAngle },
			aspectRatio{ _aspectRatio }
		{
			fov = tanf((fovAngle * TO_RADIANS) / 2.f);


			CalculateProjectionMatrix();
			CalculateViewMatrix();
		}


		Vector3 origin{};
		float fovAngle{ 90.f };
		float fov{ tanf((fovAngle * TO_RADIANS) / 2.f) };

		float aspectRatio{ 1 };

		const float nearPlane{ 0.1f };
		const float farPlane{ 100 };

		Vector3 forward{ Vector3::UnitZ };
		Vector3 up{ Vector3::UnitY };
		Vector3 right{ Vector3::UnitX };

		float totalPitch{};
		float totalYaw{};

		Matrix invViewMatrix{};
		Matrix viewMatrix{};
		Matrix projectionMatrix{};


		const float moveSpeed{ 7 };
		const float mouseMoveSpeed{ 2 };
		const float rotationSpeed{ 5 * TO_RADIANS };

		void Initialize(float _fovAngle = 90.f, Vector3 _origin = { 0.f,0.f,0.f }, float _aspectRatio = 1)
		{
			fovAngle = _fovAngle;
			fov = tanf((fovAngle * TO_RADIANS) / 2.f);

			origin = _origin;

			aspectRatio = _aspectRatio;


			CalculateProjectionMatrix();
			CalculateViewMatrix();

		}

		void CalculateViewMatrix()
		{
			//TODO W1
			//ONB => invViewMatrix
			right = Vector3::Cross(Vector3::UnitY, forward).Normalized();
			up = Vector3::Cross(forward, right);


			invViewMatrix = {
					right,
					up,
					forward,
					origin
			};
			//Inverse(ONB) => ViewMatrix
			viewMatrix = invViewMatrix.Inverse();

			//ViewMatrix => Matrix::CreateLookAtLH(...) [not implemented yet]
			//DirectX Implementation => https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3dxmatrixlookatlh
		}

		void CalculateProjectionMatrix()
		{
			//TODO W2

			//ProjectionMatrix => Matrix::CreatePerspectiveFovLH(...) [not implemented yet]
			projectionMatrix = Matrix::CreatePerspectiveFovLH(fov, aspectRatio, nearPlane, farPlane);
			//DirectX Implementation => https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3dxmatrixperspectivefovlh
		}

	};
	}

