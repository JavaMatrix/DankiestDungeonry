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
		// hacky static variables, basically singletons

		static HANDLE& _stdin()
		{
			static HANDLE _val = nullptr;
			return _val;
		}

		static HANDLE& _stdout()
		{
			static HANDLE _val = nullptr;
			return _val;
		}
	
		static COORD& _cursor_home()
		{
			static COORD _val = { 0, 0 };
			return _val;
		}
		static bool& _cursor_at_home()
		{
			static bool _val = true;
			return _val;
		}

		static const WORD MASK_FOREGROUND = 0x0F;
		static const WORD MASK_BACKGROUND = 0xF0;

		static const WORD SHIFT_FOREGROUND = 0x4;
		static const WORD SHIFT_BACKGROUND = 0x0;

	public:
		// This function represents a safe way to obtain access to the
		// Windows input stream. End users of the library should avoid this function.
		inline static HANDLE& StandardInput()
		{
			if (_stdin() == nullptr)
			{
				_stdin() = GetStdHandle(STD_INPUT_HANDLE);
			}
			return _stdin();
		}

		// This function represents a safe way to obtain access to the
		// Windows output stream. End users of the library should avoid this function.
		inline static HANDLE& StandardOutput()
		{
			if (_stdout() == nullptr)
			{
				_stdout() = GetStdHandle(STD_OUTPUT_HANDLE);
			}
			return _stdout();
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

		// A safe way to access the console's current text state.
		static WORD CurrentTextAttributes()
		{
			CONSOLE_SCREEN_BUFFER_INFO info;
			GetConsoleScreenBufferInfo(StandardOutput(), &info);
			return info.wAttributes;
		}

		// A safe way to move the console cursor.
		static void MoveCursorTo(SHORT x, SHORT y)
		{
			CONSOLE_SCREEN_BUFFER_INFO info;
			GetConsoleScreenBufferInfo(StandardOutput(), &info);

			if (_cursor_at_home())
			{
				_cursor_home() = info.dwCursorPosition;
			}

			SetConsoleCursorPosition(StandardOutput(), { x + info.srWindow.Left, y +info.srWindow.Top });
			
			_cursor_at_home() = false;
		}

		// A safe way to move the cursor back to its home position.
		static void ReturnCursorToHome()
		{
			if (_cursor_at_home()) return;

			SetConsoleCursorPosition(StandardOutput(), _cursor_home());

			_cursor_at_home() = true;
		}

		// A safe way to determine the width of the screen.
		static SHORT GetWidth()
		{
			CONSOLE_SCREEN_BUFFER_INFO info;
			GetConsoleScreenBufferInfo(StandardOutput(), &info);
			return info.dwSize.X;
		}

		// A safe way to determine the height of the screen.
		static SHORT GetHeight()
		{
			CONSOLE_SCREEN_BUFFER_INFO info;
			GetConsoleScreenBufferInfo(StandardOutput(), &info);
			return info.dwSize.Y;
		}
	};
}