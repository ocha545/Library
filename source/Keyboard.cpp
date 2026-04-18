#include"../include/Keyboard.h"

namespace Win32
{
	Input::Input(HWND parentHdl, HINSTANCE parentIns)
	{
//		Core::parentHdl = parentHdl;
		//DirectInputの作成
		{
			Hr = DirectInput8Create(parentIns, DIRECTINPUT_VERSION, IID_IDirectInput8W, (void**)Core::input8.GetAddressOf(), nullptr);
			assert(SUCCEEDED(Hr));
			AST_NULL(Core::input8.Get());


			Hr = Core::input8->CreateDevice(GUID_SysKeyboard, Core::device8.GetAddressOf(), nullptr);
			assert(SUCCEEDED(Hr));
			AST_NULL(Core::device8.Get());

			Hr = Core::device8->SetDataFormat(&c_dfDIKeyboard);
			assert(SUCCEEDED(Hr));

			Hr = Core::device8->SetCooperativeLevel(parentHdl, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
			assert(SUCCEEDED(Hr));


			Hr = Core::input8->CreateDevice(GUID_SysMouse, Core::mouseDevice8.GetAddressOf(), nullptr);
			assert(SUCCEEDED(Hr));
			AST_NULL(Core::mouseDevice8.Get());

			Hr = Core::mouseDevice8->SetDataFormat(&c_dfDIMouse);
			assert(SUCCEEDED(Hr));

			Hr = Core::mouseDevice8->SetCooperativeLevel(parentHdl, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
			assert(SUCCEEDED(Hr));
		}
	}

	void Input::Acquire()
	{
		Hr = Core::device8->Acquire();
		assert(SUCCEEDED(Hr));
		Hr = Core::mouseDevice8->Acquire();
		assert(SUCCEEDED(Hr));
	}

	void Input::Check()
	{
		memcpy(Core::statePrev, Core::state, sizeof(Core::state));
		Hr = Core::device8->GetDeviceState(sizeof(Core::state), (void**)Core::state);
		if (FAILED(Hr))
		{
			Core::device8->Acquire();
			Core::device8->GetDeviceState(sizeof(Core::state), Core::state);
		}

		memcpy(&Core::mouseStatePrev, &Core::mouseState, sizeof(Core::mouseState));
		Hr = Core::mouseDevice8->GetDeviceState(sizeof(Core::mouseState), &Core::mouseState);
		if (FAILED(Hr))
		{
			Core::mouseDevice8->Acquire();
			Core::mouseDevice8->GetDeviceState(sizeof(Core::mouseState), &Core::mouseState);
		}

		::GetCursorPos(&Core::position);
	}

	RETURN std::pair<long, long> Input::pos() const
	{
		return std::pair(
			Core::position.x,
			Core::position.y
		);
	}

	RETURN long Input::x() const
	{
		return Core::position.x;
	}

	RETURN long Input::y() const
	{
		return Core::position.y;
	}

	RETURN bool Input::pressed(Key dik) const
	{
		//今押されていたらtrue
		return (Core::state[(BYTE)dik] & 0x80) != 0;
	}

	RETURN bool Input::down(Key dik) const
	{
		//前回押されていなくて、今押されていたらtrue
		return !(Core::statePrev[(BYTE)dik] & 0x80) && (Core::state[(BYTE)dik] & 0x80);
	}

	RETURN bool Input::up(Key dik) const
	{
		//前回押されていて、今押されていなかったらtrue
		return (Core::statePrev[(BYTE)dik] & 0x80) && !(Core::state[(BYTE)dik] & 0x80);
	}

	RETURN long Input::mouseMoveX() const
	{
		return Core::mouseState.lX;
	}

	RETURN long Input::mouseMoveY() const
	{
		return Core::mouseState.lY;
	}

	RETURN long Input::mouseMoveWheel() const
	{
		return Core::mouseState.lZ;
	}

	RETURN bool Input::mousePressL() const
	{
		return (Core::mouseState.rgbButtons[Core::MouseCode::L] & 0x80) != 0;
	}

	RETURN bool Input::mousePressR() const
	{
		return (Core::mouseState.rgbButtons[Core::MouseCode::R] & 0x80) != 0;
	}

	RETURN bool Input::mouseWheelPress() const
	{
		return (Core::mouseState.rgbButtons[Core::MouseCode::WHEEL] & 0x80) != 0;
	}

	RETURN bool Input::mouseDownL() const
	{
		return !(
			Core::mouseState.rgbButtons[Core::MouseCode::L] & 0x80) &&
			(Core::mouseStatePrev.rgbButtons[Core::MouseCode::L] & 0x80
		);
	}

	RETURN bool Input::mouseDownR() const
	{
		return !(
			Core::mouseState.rgbButtons[Core::MouseCode::R] & 0x80) &&
			(Core::mouseStatePrev.rgbButtons[Core::MouseCode::R] & 0x80
		);
	}

	RETURN bool Input::mouseWheelDown() const
	{
		return !(
			Core::mouseState.rgbButtons[Core::MouseCode::WHEEL] & 0x80) &&
			(Core::mouseStatePrev.rgbButtons[Core::MouseCode::WHEEL] & 0x80
		);
	}

	RETURN bool Input::mouseUpL() const
	{
		return(
			Core::mouseStatePrev.rgbButtons[Core::MouseCode::L] & 0x80) &&
			!(Core::mouseState.rgbButtons[Core::MouseCode::L] & 0x80
		);
	}

	RETURN bool Input::mouseUpR() const
	{
		return(
			Core::mouseStatePrev.rgbButtons[Core::MouseCode::R] & 0x80) &&
			!(Core::mouseState.rgbButtons[Core::MouseCode::R] & 0x80
		);
	}

	RETURN bool Input::mouseWheelUp() const
	{
		return(
			Core::mouseStatePrev.rgbButtons[Core::MouseCode::WHEEL] & 0x80) &&
			!(Core::mouseState.rgbButtons[Core::MouseCode::WHEEL] & 0x80
		);
	}
}