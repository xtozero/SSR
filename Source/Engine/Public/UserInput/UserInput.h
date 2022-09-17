#pragma once

#include "SizedTypes.h"

enum UserInputCode : uint8
{
	UIC_UNKNOWN = 0,
	// Character Keys
	UIC_A,
	UIC_B,
	UIC_C,
	UIC_D,
	UIC_E,
	UIC_F,
	UIC_G,
	UIC_H,
	UIC_I,
	UIC_J,
	UIC_K,
	UIC_L,
	UIC_M,
	UIC_N,
	UIC_O,
	UIC_P,
	UIC_Q,
	UIC_R,
	UIC_S,
	UIC_T,
	UIC_U,
	UIC_V,
	UIC_W,
	UIC_X,
	UIC_Y,
	UIC_Z,

	// Arrow keys
	UIC_LEFT,
	UIC_UP,
	UIC_RIGHT,
	UIC_DOWN,

	// Numbers
	UIC_ZERO,
	UIC_ONE,
	UIC_TWO,
	UIC_THREE,
	UIC_FOUR,
	UIC_FIVE,
	UIC_SIX,
	UIC_SEVEN,
	UIC_EIGHT,
	UIC_NINE,

	UIC_NUM_PAD_ZERO,
	UIC_NUM_PAD_ONE,
	UIC_NUM_PAD_TWO,
	UIC_NUM_PAD_THREE,
	UIC_NUM_PAD_FOUR,
	UIC_NUM_PAD_FIVE,
	UIC_NUM_PAD_SIX,
	UIC_NUM_PAD_SEVEN,
	UIC_NUM_PAD_EIGHT,
	UIC_NUM_PAD_NINE,

	// Num Pad
	UIC_MULTIPLY,
	UIC_ADD,
	UIC_SUBTRACT,
	UIC_DECIMAL,
	UIC_DIVIDE,
	UIC_BACKSPACE,
	UIC_TAB,
	UIC_ENTER,
	UIC_PAUSE,
	UIC_NUMLOCK,
	UIC_SCROLLLOCK,
	UIC_CAPSLOCK,
	UIC_ESCAPE,
	UIC_SPACEBAR,
	UIC_PAGEUP,
	UIC_PAGEDOWN,
	UIC_END,
	UIC_HOME,
	UIC_INSERT,
	UIC_DELETE,

	// Function Keys
	UIC_F1,
	UIC_F2,
	UIC_F3,
	UIC_F4,
	UIC_F5,
	UIC_F6,
	UIC_F7,
	UIC_F8,
	UIC_F9,
	UIC_F10,
	UIC_F11,
	UIC_F12,

	// Modifier Keys
	UIC_LEFTSHIFT,
	UIC_RIGHTSHIFT,
	UIC_LEFTCONTROL,
	UIC_RIGHTCONTROL,
	UIC_LEFTALT,
	UIC_RIGHTALT,

	// Misc Keys
	UIC_SEMICOLON,
	UIC_EQUALS,
	UIC_COMMA,
	UIC_UNDERSOCRE,
	UIC_PERIOD,
	UIC_SLASH,
	UIC_TILDE,
	UIC_LEFTBRACKET,
	UIC_BACKSLASH,
	UIC_RIGHTBRACKET,
	UIC_QUOTE,

	// Mouse Axis
	UIC_MOUSE_MOVE,
	UIC_MOUSE_SCROLLUP,
	UIC_MOUSE_SCROLLDOWN,
	UIC_MOUSE_WHEELSPIN,

	// Mouse Buttons
	UIC_MOUSE_LEFT,
	UIC_MOUSE_RIGHT,
	UIC_MOUSE_MIDDLE,
};

struct UserInput
{
	enum
	{
		X_AXIS = 0,
		Y_AXIS,
		Z_AXIS,
		INPUT_AXIS_COUNT
	};

	UserInputCode m_code = UIC_UNKNOWN;
	float m_axis[INPUT_AXIS_COUNT] = { 0.f, 0.f, 0.f };
};