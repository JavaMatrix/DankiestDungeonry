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

// WinConsole.h contains handles to the console windows and several functions for dealing with
// this console.
#pragma once
#include <Windows.h>

namespace jinxes
{
	class WinConsole
	{
	private:
		static HANDLE _stdin;
		static HANDLE _stdout;

		static const WORD MASK_FOREGROUND = 0x0F;
		static const WORD MASK_BACKGROUND = 0xF0;

		static const WORD SHIFT_FOREGROUND = 0x4;
		static const WORD SHIFT_BACKGROUND = 0x0;
	public:
		// This function represents a safe way to obtain access to the
		// Windows input stream. End users of the library should avoid this function.
		inline static HANDLE& StandardInput()
		{
			if (_stdin == nullptr)
			{
				_stdin = GetStdHandle(STD_INPUT_HANDLE);
			}
			return _stdin;
		}

		// This function represents a safe way to obtain access to the
		// Windows output stream. End users of the library should avoid this function.
		inline static HANDLE& StandardOutput()
		{
			if (_stdout == nullptr)
			{
				_stdout = GetStdHandle(STD_OUTPUT_HANDLE);
			}
			return _stdout;
		}


		// A safe way to access the console's current color.
		static WORD CurrentColor()
		{
			CONSOLE_SCREEN_BUFFER_INFO info;
			GetConsoleScreenBufferInfo(StandardOutput(), &info);
			return (info.wAttributes & MASK_FOREGROUND) >> SHIFT_FOREGROUND;
		}

		// A safe way to access the console's current background color.
		static WORD CurrentBackground()
		{
			CONSOLE_SCREEN_BUFFER_INFO info;
			GetConsoleScreenBufferInfo(StandardOutput(), &info);
			return (info.wAttributes & MASK_BACKGROUND) >> SHIFT_BACKGROUND;
		}
	};

	// Either a handle to the standard input or null
	// if the handle hasn't been fetched yet.
	HANDLE WinConsole::_stdin = nullptr;

	// Either a handle to the standard output or null
	// if the handle hasn't been fetched yet.
	HANDLE WinConsole::_stdout = nullptr;
}