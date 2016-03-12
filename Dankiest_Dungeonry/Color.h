/*
* The Jinxes Library
*
* Jinxes is a Windows library that provides an alternative to PDCurses that uses the Windows
* API. It uses modern C++ standards for its code, and therefore its syntax will be familiar
* to C++ programmers, unlike the C-based PDCurses.
*
* The Jinx Library is licensed under the MIT License.
* Copyright(c) 2016 Nathanael Page
*
* Permission is hereby granted, free of charge, to any person obtaining a copy of this
* software and associated documentation files(the "Software"), to deal in the Software
* without restriction, including without limitation the rights to use, copy, modify, merge,
* publish, distribute, sublicense, and / or sell copies of the Software, and to permit persons
* to whom the Software is furnished to do so, subject to the following conditions :
* The above copyright notice and this permission notice shall be included in all copies
* or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED
* INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
* PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
* FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

// Color.h contains utilities for modifying the standard output stream's Colors.
#pragma once
#include <Windows.h>
#include <stack>

#include "WinConsole.h"

typedef WORD Color;

namespace jinxes
{

	const Color BLACK         = 0x0;
	const Color BLUE          = 0x1;
	const Color GREEN         = 0x2;
	const Color AQUA          = 0x3;
	const Color RED           = 0x4;
	const Color PURPLE        = 0x5;
	const Color YELLOW        = 0x6;
	const Color WHITE         = 0x7;
	const Color GREY          = 0x8;
	const Color BRIGHT_BLUE   = 0x9;
	const Color BRIGHT_GREEN  = 0xA;
	const Color BRIGHT_AQUA   = 0xB;
	const Color BRIGHT_RED    = 0xC;
	const Color BRIGHT_PURPLE = 0xD;
	const Color BRIGHT_YELLOW = 0xE;
	const Color BRIGHT_WHITE  = 0xF;

	// The stack of colors to be applied to the output stream.
	// Modify at your own risk.
	static std::stack<Color> COLOR_STACK;

	// The stack of background colors to be applied to the
	// output stream. Modify at your own risk.
	static std::stack<Color> BACKGROUND_STACK;

	// A dummy struct for applying colors to the output stream.
	// End users should avoid using this structure.
	struct __stream_operator__ {};

	// Allows for the push_color and push_background_color functions to be applied to the stream, similar
	// to the stream formatting functions in ios.h.
	inline std::ostream& operator<<(std::ostream& stream, __stream_operator__& colorization)
	{
		// Grab the foreground and background from the stacks.
		Color fore;
		Color back;

		if (COLOR_STACK.empty())
		{
			fore = WHITE;
		}
		else
		{
			fore = COLOR_STACK.top();
		}

		if (BACKGROUND_STACK.empty())
		{
			back = BLACK;
		}
		else
		{
			back = BACKGROUND_STACK.top();
		}

		// Build the full color byte from the background and foreground.
		Color full_color_def = fore | (back << 4);

		// Colorize the stream.
		SetConsoleTextAttribute(WinConsole::StandardOutput(), full_color_def);

		return stream;
	}

	// Changes the foreground color of the standard output.
	__stream_operator__ push_color(Color foreground)
	{
		// Put on the color.
		COLOR_STACK.push(foreground);

		return __stream_operator__{};
	}

	// Returns the foreground color of the standard output to
	// its state prior to the most recent push_color call.
	// To get the color popped from the stream, set popped to
	// a non-null value.
	__stream_operator__ pop_color(Color* popped = nullptr)
	{
		// Return the color if we were asked to do so.
		if (popped != nullptr)
		{
			if (COLOR_STACK.empty())
			{
				*popped = WHITE;
			}

			*popped = COLOR_STACK.top();
		}

		// Take off the color.
		if (!COLOR_STACK.empty())
			COLOR_STACK.pop();

		return __stream_operator__{};
	}

	// Changes the background color of the standard output.
	__stream_operator__ push_background_color(Color background)
	{
		// Put on the color.
		BACKGROUND_STACK.push(background);

		return __stream_operator__{};
	}

	// Returns the foreground color of the standard output to
	// its state prior to the most recent push_color call.
	// To get the color popped from the stream, set popped to
	// a non-null value.
	__stream_operator__ pop_background_color(Color* popped = nullptr)
	{
		// Return the color if we were asked to do so.
		if (popped != nullptr)
		{
			if (BACKGROUND_STACK.empty())
			{
				*popped = BLACK;
			}

			*popped = BACKGROUND_STACK.top();
		}

		// Take off the color.
		if (!BACKGROUND_STACK.empty())
		BACKGROUND_STACK.pop();

		return __stream_operator__{};
	}
}