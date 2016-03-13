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
#include <string>

#include "WinConsole.h"


namespace jinxes
{
	typedef WORD Color;

	namespace Colors
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
	}

	// Defines a colored text segment.
	class set_color
	{
	private:
		std::string _text;
		Color _color;
		Color _background;
	public:
		// Create a segment of colored text.
		set_color(std::string text, Color color, Color background = Colors::BLACK) : _text(text), _color(color), _background(background) {}

		// Create a colored character.
		set_color(char text, Color color, Color background = Colors::BLACK) : _text(string(1, text)), _color(color), _background(background) {} 

		// Allows for colored text to be output to the standard output stream.
		inline friend std::ostream& operator <<(std::ostream& stream, set_color& text)
		{
			// Put the background into the upper nibble of the color byte.
			Color stitched_color = text._color | (text._background << 4);

			// Save the output color.
			Color previous_color = WinConsole::CurrentTextAttributes();

			// Change the output color.
			SetConsoleTextAttribute(WinConsole::StandardOutput(), stitched_color);

			// Output the actual text.
			stream << text._text;

			// Reset the colors.
			SetConsoleTextAttribute(WinConsole::StandardOutput(), previous_color);

			return stream;
		}
	};

	void set_color_all(Color color, Color background = Colors::BLACK)
	{
		// Put the background into the upper nibble of the color byte.
		Color stitched_color = color | (background << 4);

		// Change the output color.
		SetConsoleTextAttribute(WinConsole::StandardOutput(), stitched_color);
	}

	void reset_color_all()
	{
		// Change the output color to white on black.
		SetConsoleTextAttribute(WinConsole::StandardOutput(), 0x07);
	}
}