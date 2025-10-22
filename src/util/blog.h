/**
 * a simple logger, built on top of the guts of fmt. it offers distinct categories and colouring per category, along
 * with an easy way to have debug-only output channels that silence themselves in Release builds
 * 
 * in code, it looks a lot like fmt; blog::<channel>( etc )
 * 
 * blog::app( "x = {}", x );
 * blog::debug( "this doesn't appear in Release" );
 * 
 */

#pragma once
#include "fmt/format.h"
#include "fmt/color.h"
#include "fmt/chrono.h"
#include "fmt/xchar.h"

// ---------------------------------------------------------------------------------------------------------------------
namespace blog {

	// called once on startup; configure the default terminal for more practical
	// usage - UTF8 codepage, ansi colour processing
	void configureTerminal();

namespace detail {

	// takes a prefix (eg. "CORE") and a plain string to log out with the given colours; fg1 for the prefix, fg2 for the text
	template < bool emit_in_release, const fmt::color color_fg1, const fmt::color color_fg2 >
	void blog_print_raw( const std::string_view prefix, const std::string_view plain_str ) noexcept
	{
		if constexpr ( (emit_in_release && BUILD_RELEASE) || BUILD_DEBUG )  // allow masking of output based on template arg and build config
		{
			static constexpr std::string_view midsep = " | ";
			static constexpr std::string_view suffix = "\n";

			static constexpr auto foreground1 = fmt::detail::make_foreground_color<char>( color_fg1 );
			static constexpr auto foreground2 = fmt::detail::make_foreground_color<char>( color_fg2 );

			// assemble the final text to write out; this includes the terminal commands for colouring,
			// common prefixes / log context strings and finally the formatted text from formatBuffer above
			static thread_local fmt::basic_memory_buffer<char> outputBuffer;
			{
				// originally I would estimate the memory requirement to build the buffer based on 
				// the input string, terminal processing buffers, etc - but now we just reserve a decently big
				// chunk, probably larger than any log message will ever need, leaving a single allocation on the
				// first log (per thread)

				outputBuffer.try_reserve( 2048 );
				outputBuffer.clear();

				// prefix in colour 1
				outputBuffer.append( foreground1.begin(), foreground1.end() );
				outputBuffer.append( prefix );

				// separator in white
				fmt::detail::reset_color( outputBuffer );
				outputBuffer.append( midsep );

				// log string in colour 2
				outputBuffer.append( foreground2.begin(), foreground2.end() );
				outputBuffer.append( plain_str );
				outputBuffer.append( suffix );

				fmt::detail::reset_color( outputBuffer );

				outputBuffer.push_back( '\0' );
			}

			std::fwrite( outputBuffer.data(), 1, outputBuffer.size(), stdout );
		}
	}

	// call into fmt to format the given arguments into the format_string, passing the result down into the raw print function
	template < bool emit_in_release, const fmt::color color_fg1, const fmt::color color_fg2, typename... Args >
	void blog_format_and_print( const std::string_view prefix, const fmt::format_string<Args...> format_str, const Args&... args ) noexcept
	{
		if constexpr ( (emit_in_release && BUILD_RELEASE) || BUILD_DEBUG )  // allow masking of output based on template arg and build config
		{
			// run the formatting process, producing the final text to output
			// uses a per-thread static memory buffer to try and reduce allocations over time
			static thread_local fmt::basic_memory_buffer<char> formatBuffer;
			{
				formatBuffer.try_reserve( 2048 );
				formatBuffer.clear();

				fmt::detail::vformat_to(
					formatBuffer,
					format_str,
					fmt::make_format_args( args... ),
					{} );

				formatBuffer.push_back( '\0' );
			}

			blog_print_raw< emit_in_release, color_fg1, color_fg2 >( prefix, formatBuffer.begin() );
		}
	}
} // namespace detail

// ---------------------------------------------------------------------------------------------------------------------
#define ADD_BLOG( _name, _colour_1, _colour_2, _nameBold, _emitInRelease )															\
	template <typename... Args>																										\
	__forceinline void _name( const fmt::format_string<Args...> format_str, const Args&... args ) {									\
		detail::blog_format_and_print< _emitInRelease, _colour_1, (fmt::color)_colour_2, Args...>( _nameBold, format_str, args...);	\
	}																																\
	__forceinline void _name( const char* plain_str ) {																				\
		detail::blog_print_raw< _emitInRelease, _colour_1, (fmt::color)_colour_2>( _nameBold, plain_str );							\
	}

	ADD_BLOG(	error,	fmt::color::red,	0xE74C3C,	" ERR",		true	)
	ADD_BLOG(	app,	fmt::color::white,	0xA6E22E,	" APP",		true	)
	ADD_BLOG(	debug,	fmt::color::white,	0xE6A637,	" DBG",		false	)

} // namespace blog
