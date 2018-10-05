#ifndef STDX_STREAM_HPP
#define STDX_STREAM_HPP

#include <algorithm>
#include <vector>
#include <streambuf>
#include <iostream>
#include <fstream>
#include <sstream>

#include "meta.hpp"

namespace stdx::stream
{
	// See https://en.cppreference.com/w/cpp/io/basic_streambuf

	template <class CharType, class Traits = std::char_traits<CharType>>
	class basic_multibuf final : public std::basic_streambuf<CharType, Traits> // Based on this: http://wordaligned.org/articles/cpp-streambufs#toca-generic-version
	{
		using base_type = std::basic_streambuf<CharType, Traits>;
	public:
		using char_type = CharType;
		using traits_type = Traits;
		using int_type = typename Traits::int_type;
		using pos_type = typename Traits::pos_type;
		using off_type = typename Traits::off_type;

		template <class ... StreambufTypes>
		basic_multibuf(StreambufTypes * ... streams)
			: _streams({ streams ... })
		{
			static_assert(stdx::meta::conjunction<stdx::meta::bind<std::is_base_of, base_type, stdx::meta::placeholders::_1>::invoke>::template trait<StreambufTypes ...>::value, 
						  "stdx::stream::basic_multibuf<CharType, Traits>::basic_multibuf<StreambufTypes ...>: StreambufTypes must be of or derived of type std::basic_streambuf<CharType, Traits>");
		}
	protected:
		// Positioning
/*		virtual basic_multibuf<CharType, Traits> * setbuf(char_type * s, std::streamsize n) override
		{
			// TODO
			return this;
		}
		virtual pos_type seekoff(off_type off, std::ios_base::seekdir dir, std::ios_base::openmode which = std::ios_base::in | std::ios_base::out) override
		{
			// TODO
			return pos_type(off_type(-1));
		}
		virtual pos_type seekpos(pos_type pos, std::ios_base::openmode which = std::ios_base::in | std::ios_base::out) override
		{
			// TODO
			return pos_type(off_type(-1));
		}
*/		virtual int sync() override
		{
			int_type retval = 0;
			std::for_each(std::begin(_streams), std::end(_streams), [&] (auto const & stream)
			{
				if (stream->pubsync() != 0)
				{
					retval = -1;
				}
			});
			return retval;
		}

		// Get area
/*		virtual std::streamsize showmanyc() override
		{
			// TODO
			return 0;
		}
		virtual int_type underflow() override
		{
			// TODO
			return traits_type::eof();
		}
		virtual int_type uflow() override
		{
			// TODO
			return traits_type::eof();
		}
		virtual std::streamsize xsgetn(char_type * s, std::streamsize count) override
		{
			// TODO
			return 0;
		}
*/
		// Put area
/*		virtual std::streamsize xsputn(const char_type* s, std::streamsize count) override
		{
			// TODO
			return 0;
		}
*/		virtual int_type overflow(int_type c) override
		{
			int_type retval = traits_type::not_eof(c);
			if (!traits_type::eq_int_type(c, traits_type::eof()))
			{
				std::for_each(std::begin(_streams), std::end(_streams), [&] (auto const & stream)
				{
					if (traits_type::eq_int_type(stream->sputc(traits_type::to_char_type(c)), traits_type::eof()))
					{
						retval = traits_type::eof();
					}
				});
			}
			return retval;
		}

		// Putback
/*		virtual int_type pbackfail(int_type c = traits_type::eof())
		{
			// TODO
			return traits_type::eof();
		}
*/
	private:
		std::vector<base_type *> const _streams;
	};

	using multibuf = basic_multibuf<std::streambuf::char_type>;

	enum class streamroute_policy // Yet unused
	{
		swap, // Maybe use other names
		copy,
	};

	class streamroute
	{
	public:
		template <class ... OstreamTypes>
		streamroute(std::ostream & source, OstreamTypes & ... destination) :
			_source(&source),
			_buffer(source.rdbuf(new multibuf(source.rdbuf(), destination.rdbuf() ...)))
		{
			static_assert(stdx::meta::conjunction<stdx::meta::bind<std::is_base_of, std::ostream, stdx::meta::placeholders::_1>::invoke>::template trait<OstreamTypes ...>::value, 
						  "stdx::stream::streamroute::streamroute<OstreamTypes ...>: OstreamTypes must be of or derived of type std::ostream");
		}
		~streamroute() noexcept
		{
			delete _source->rdbuf(_buffer);
		}
	private:
		std::ostream *		_source;
		std::streambuf *	_buffer;
	};
}

#if defined(STDX_USING_STREAM) || defined(STDX_USING_ALL)
namespace stdx { using namespace stream; }
#endif

#endif
