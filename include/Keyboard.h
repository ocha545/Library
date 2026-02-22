#pragma once
#include"Macros.h"
#include"Helper.h"

#include<wrl.h>
using namespace Microsoft::WRL;

//キーボードとかマウスとか
#define UsingDevice Win32_CPP::Device

// Direct Input
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#include<dinput.h>
#include<cassert>

namespace Win32_CPP
{
	namespace Device
	{
		enum class Key : BYTE
		{
			ESC = 0x01,
			ONE = 0x02,
			TWO = 0x03,
			THREE = 0x04,
			FOUR = 0x05,
			FIVE = 0x06,
			SIX = 0x07,
			SEVEN = 0x08,
			EIGHT = 0x90,
			NINE = 0x0a,
			ZERO = 0x0b,
			BACKSLASH = 0x2b,
			MINUS = 0x0c,
			EQUALS = 0x0d,
			BACK = 0x0e,
			TAB = 0x0f,
			L_BRACKET = 0x1a,
			R_BRACKET = 0x1b,
			ENTER = 0x1c,
			L_CONTROL = 0x1d,
			SEMICOLON = 0x27,
			APOSTROPHE = 0x28,
			GRAVE = 0x29,
			L_SHIFT = 0x2a,
			COMMA = 0x33,
			PERIOD = 0x34,
			SLASH = 0x35,
			R_SHIFT = 0x36,
			MULTIPLY = 0x37,
			L_ALT = 0x38,
			R_ALT = 0xb8,
			SPACE = 0x39,
			CAPSLOCK = 0x3a,
			A = 0x1e,
			B = 0x30,
			C = 0x2e,
			D = 0x20,
			E = 0x12,
			F = 0x21,
			G = 0x22,
			H = 0x23,
			I = 0x17,
			J = 0x24,
			K = 0x25,
			L = 0x26,
			M = 0x32,
			N = 0x31,
			O = 0x18,
			P = 0x19,
			Q = 0x10,
			R = 0x13,
			S = 0x1f,
			T = 0x14,
			U = 0x16,
			V = 0x2f,
			W = 0x11,
			X = 0x2d,
			Y = 0x15,
			Z = 0x2c,
			F1 = 0x3b,
			F2 = 0x3c,
			F3 = 0x3d,
			F4 = 0x3e,
			F5 = 0x3f,
			F6 = 0x40,
			F7 = 0x41,
			F8 = 0x42,
			F9 = 0x43,
			F10 = 0x44,
			F11 = 0x57,
			F12 = 0x58,
			F13 = 0x64,
			F14 = 0x65,
			F15 = 0x66,
			NUM_LOCK = 0x45,
			SCROLL_LOCK = 0x46,
			NUM0 = 0x52,
			NUM1 = 0x4f,
			NUM2 = 0x50,
			NUM3 = 0x51,
			NUM4 = 0x4b,
			NUM5 = 0x4c,
			NUM6 = 0x4d,
			NUM7 = 0x47,
			NUM8 = 0x48,
			NUM9 = 0x49,
			NUM_ADD = 0x4e,
			NUM_SUBTRACT = 0x41,
			NUM_DECIMAL = 0x53,
			UP = 0xc8,
			LEFT = 0xcb,
			DOWN = 0xd0,
			RIGHT = 0xcd,
			KANA = 0x70,
			CONVERT = 0x79,
			NO_CONVERT = 0x7b,
			YEN = 0x7d,
			NUM_EQUALS = 0x8d,
			CIRCUMFLEX = 0x90,
			AT = 0x91,
			COLON = 0x91,
			UNDERLINE = 0x93,
			KANJI = 0x94,
			STOP = 0x95,
			AX = 0x96,
			UNLABELD = 0x97,
			NUM_ENTER = 0x9c,
			NUM_COMMA = 0xb3,
			HOME = 0xc7,
			PAGE_UP = 0xc9,
			END = 0xcf,
			PAGE_DOWN = 0xd1,
			INSERT = 0xd2,
			DEL = 0xd3
		};

		namespace Core
		{
			static BYTE state[256]{};
			static BYTE statePrev[256]{};
			static DIMOUSESTATE mouseState{};
			static DIMOUSESTATE mouseStatePrev{};
			static ComPtr<IDirectInput8W> input8 = nullptr;
			static ComPtr<IDirectInputDevice8W> device8 = nullptr;
			static ComPtr<IDirectInputDevice8W> mouseDevice8 = nullptr;
			static POINT position{};
			static HWND parentHdl;

			enum MouseCode
			{
				L = 0x00,
				R = 0x01,
				WHEEL = 0x02,
			};
		}

		class Input SEALED
		{
		private:
			HRESULT Hr;

		public:
			Input() = default;
			Input(const Input&) = default;

			Input(const HWND& parentHdl, const HINSTANCE& parentIns)
			{
				Core::parentHdl = parentHdl;
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

			void Acquire()
			{
				Hr = Core::device8->Acquire();
				assert(SUCCEEDED(Hr));
				Hr = Core::mouseDevice8->Acquire();
				assert(SUCCEEDED(Hr));
			}

			void Check()
			{
				memcpy(Core::statePrev, Core::state, sizeof(Core::state));

				Hr = Core::device8->GetDeviceState(sizeof(Core::state), (void**)Core::state);
				if (FAILED(Hr))
				{
					Core::device8->Acquire();
					Core::device8->GetDeviceState(sizeof(Core::state), Core::state);
				}


				Core::mouseStatePrev = Core::mouseState;
				Hr = Core::mouseDevice8->GetDeviceState(sizeof(Core::mouseState), &Core::mouseState);
				if (FAILED(Hr))
				{
					Core::mouseDevice8->Acquire();
					Core::mouseDevice8->GetDeviceState(sizeof(Core::mouseState), &Core::mouseState);
				}

				::GetCursorPos(&Core::position);
			}

			RETURN std::pair<long, long> pos() const
			{
				return std::pair(
					Core::position.x,
					Core::position.y
				);
			}

			RETURN long x() const
			{
				return Core::position.x;
			}

			RETURN long y() const
			{
				return Core::position.y;
			}

			RETURN bool pressed(Key dik) const
			{
				//今押されていたらtrue
				return (Core::state[(BYTE)dik] & 0x80) != 0;
			}

			RETURN bool down(Key dik) const
			{
				//前回押されていなくて、今押されていたらtrue
				return !(Core::statePrev[(BYTE)dik] & 0x80) && (Core::state[(BYTE)dik] & 0x80);
			}

			RETURN bool up(Key dik) const
			{
				//前回押されていて、今押されていなかったらtrue
				return (Core::statePrev[(BYTE)dik] & 0x80) && !(Core::state[(BYTE)dik] & 0x80);
			}

			RETURN long mouseMoveX() const
			{
				return Core::mouseState.lX;
			}

			RETURN long mouseMoveY() const
			{
				return Core::mouseState.lY;
			}

			RETURN long mouseMoveWheel() const
			{
				return Core::mouseState.lZ;
			}

			RETURN bool mousePressL() const
			{
				return (Core::mouseState.rgbButtons[Core::MouseCode::L] & 0x80) != 0;
			}

			RETURN bool mousePressR() const
			{
				return (Core::mouseState.rgbButtons[Core::MouseCode::R] & 0x80) != 0;
			}

			RETURN bool mouseWheelPress() const
			{
				return (Core::mouseState.rgbButtons[Core::MouseCode::WHEEL] & 0x80) != 0;
			}

			RETURN bool mouseDownL() const
			{
				return !(
					Core::mouseState.rgbButtons[Core::MouseCode::L] & 0x80) &&
					(Core::mouseStatePrev.rgbButtons[Core::MouseCode::L] & 0x80
				);
			}

			RETURN bool mouseDownR() const
			{
				return !(
					Core::mouseState.rgbButtons[Core::MouseCode::R] & 0x80) &&
					(Core::mouseStatePrev.rgbButtons[Core::MouseCode::R] & 0x80
				);
			}

			RETURN bool mouseWheelDown() const
			{
				return !(
					Core::mouseState.rgbButtons[Core::MouseCode::WHEEL] & 0x80) &&
					(Core::mouseStatePrev.rgbButtons[Core::MouseCode::WHEEL] & 0x80
				);
			}

			RETURN bool mouseUpL() const
			{
				return(
					Core::mouseStatePrev.rgbButtons[Core::MouseCode::L] & 0x80) &&
					!(Core::mouseState.rgbButtons[Core::MouseCode::L] & 0x80
				);
			}

			RETURN bool mouseUpR() const
			{
				return(
					Core::mouseStatePrev.rgbButtons[Core::MouseCode::R] & 0x80) &&
					!(Core::mouseState.rgbButtons[Core::MouseCode::R] & 0x80
				);
			}

			RETURN bool mouseWheelUp() const
			{
				return(
					Core::mouseStatePrev.rgbButtons[Core::MouseCode::WHEEL] & 0x80) &&
					!(Core::mouseState.rgbButtons[Core::MouseCode::WHEEL] & 0x80
				);
			}
		};
	}
}