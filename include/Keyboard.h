#pragma once
#include"Macros.h"
#include"Helper.h"

#include<wrl.h>
using namespace Microsoft::WRL;

// Direct Input
#define DIRECTINPUT_VERSION 0x0800
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#include<dinput.h>
#include<cassert>

namespace Win32
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
		Input(HWND parentHdl, HINSTANCE parentIns);

		void Acquire();

		void Check();

		RETURN std::pair<long, long> pos() const;

		RETURN long x() const;

		RETURN long y() const;

		RETURN bool pressed(Key dik) const;

		RETURN bool down(Key dik) const;

		RETURN bool up(Key dik) const;

		RETURN long mouseMoveX() const;

		RETURN long mouseMoveY() const;

		RETURN long mouseMoveWheel() const;

		RETURN bool mousePressL() const;

		RETURN bool mousePressR() const;

		RETURN bool mouseWheelPress() const;

		RETURN bool mouseDownL() const;

		RETURN bool mouseDownR() const;

		RETURN bool mouseWheelDown() const;

		RETURN bool mouseUpL() const;

		RETURN bool mouseUpR() const;

		RETURN bool mouseWheelUp() const;
	};
}
