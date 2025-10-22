#include <windows.h>
#include <ios>
#include <conio.h>

#include "blog.h"

namespace blog {

void configureTerminal()
{
	// support terminal sequences to allow colouring of log outputs
	// https://learn.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences
	::SetConsoleMode( ::GetStdHandle( STD_OUTPUT_HANDLE ), ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING );

	// allow use of UTF-8
	if ( ::IsValidCodePage( CP_UTF8 ) )
	{
		::SetConsoleCP( CP_UTF8 );
		::SetConsoleOutputCP( CP_UTF8 );
	}

	// http://www.cplusplus.com/reference/ios/ios_base/sync_with_stdio/
	// Toggles [off] synchronization of all the iostream standard streams with their corresponding standard C streams
	std::ios_base::sync_with_stdio( false );
}

} // namespace blog