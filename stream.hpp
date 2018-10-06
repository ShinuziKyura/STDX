#ifndef STDX_STREAM_HPP
#define STDX_STREAM_HPP

#include <vector>
#include <streambuf>
#include <iostream>
#include <fstream>
#include <sstream>

#include "meta.hpp"

namespace stdx::stream
{
	// Multibuf

	template <class CharType, class Traits = std::char_traits<CharType>>
	class basic_multibuf final : public std::basic_streambuf<CharType, Traits> // Based on this: http://wordaligned.org/articles/cpp-streambufs#toca-generic-version
	{
		using this_type = basic_multibuf<CharType, Traits>;
		using base_type = std::basic_streambuf<CharType, Traits>;
	public:
		using char_type = CharType;
		using traits_type = Traits;
		using int_type = typename Traits::int_type;
		using pos_type = typename Traits::pos_type;
		using off_type = typename Traits::off_type;

		template <class ... StreambufTypes>
		basic_multibuf(StreambufTypes * ... streams) : 
			base_type(),
			_streams({ streams ... })
		{
			static_assert(stdx::meta::conjunction<stdx::meta::bind<std::is_base_of, base_type, stdx::meta::placeholders::_1>::invoke>::template trait<StreambufTypes ...>::value, 
						  "stdx::stream::basic_multibuf<CharType, Traits>::basic_multibuf<StreambufTypes ...>: StreambufTypes must be of type or derived of type std::basic_streambuf<CharType, Traits>");
		}
		basic_multibuf(basic_multibuf const & other) :
			base_type(other),
			_streams(other._streams)
		{
		}
		basic_multibuf & operator=(basic_multibuf const & other)
		{
			base_type::operator=(other);
			_streams = other._streams;
			return *this;
		}
		basic_multibuf(basic_multibuf &&) = delete;
		basic_multibuf & operator=(basic_multibuf &&) = delete;

		void swap(basic_multibuf & other)
		{
			base_type::swap(other);
			_streams.swap(other._streams);
		}
	protected: // See https://en.cppreference.com/w/cpp/io/basic_streambuf
		// Locales
		virtual void imbue(std::locale const & loc) override
		{
			for (auto const & stream : _streams)
			{
				stream->pubimbue(loc);
			}
		}

		// Positioning
		virtual this_type * setbuf(char_type * s, std::streamsize n) override
		{
			this_type * retval = this;
			for (auto const & stream : _streams)
			{
				if (stream->pubsetbuf(s, n) == nullptr)
				{
					retval = nullptr;
				}
			}
			return retval;
		}
		virtual pos_type seekoff(off_type off, std::ios_base::seekdir dir, std::ios_base::openmode which = std::ios_base::in | std::ios_base::out) override
		{
			pos_type retval = off;
			for (auto const & stream : _streams)
			{
				if (stream->pubseekoff(off, dir, which) == pos_type(off_type(-1)))
				{
					retval = pos_type(off_type(-1));
				}
			}
			return retval;
		}
		virtual pos_type seekpos(pos_type pos, std::ios_base::openmode which = std::ios_base::in | std::ios_base::out) override
		{
			pos_type retval = pos;
			for (auto const & stream : _streams)
			{
				if (stream->pubseekpos(pos, which) == pos_type(off_type(-1)))
				{
					retval = pos_type(off_type(-1));
				}
			}
			return retval;
		}
		virtual int sync() override
		{
			int_type retval = 0;
			for (auto const & stream : _streams)
			{
				if (stream->pubsync() == -1)
				{
					retval = -1;
				}
			}
			return retval;
		}

		// Get area // NOTE: no sense in overriding these functions for now (what would be the effect of reading from multiple buffers?)
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
		virtual std::streamsize xsputn(char_type const * s, std::streamsize count) override
		{
			std::streamsize retval = count;
			for (auto const & stream : _streams)
			{
				if (auto const actual_count = stream->sputn(s, count); actual_count < retval)
				{
					retval = actual_count;
				}
			}
			return retval;
		}
		virtual int_type overflow(int_type ch = traits_type::eof()) override
		{
			int_type retval = traits_type::not_eof(ch);
			if (!traits_type::eq_int_type(ch, traits_type::eof()))
			{
				for (auto const & stream : _streams)
				{
					if (traits_type::eq_int_type(stream->sputc(traits_type::to_char_type(ch)), traits_type::eof()))
					{
						retval = traits_type::eof();
					}
				}
			}
			return retval;
		}

		// Putback // NOTE: no sense in overriding this function for now (what would be the effect of unreading from multiple buffers?)
/*		virtual int_type pbackfail(int_type c = traits_type::eof())
		{
			// TODO
			return traits_type::eof();
		}
*/
	private:
		std::vector<base_type *> _streams;
	};

	using multibuf = basic_multibuf<std::streambuf::char_type>;

	namespace policy
	{
		struct repeat;
		struct redirect;
	}

	// Streamroute

	template <class>
	class streamroute; // May be subject to name change

	template <>
	class streamroute<policy::repeat>
	{
	public:
		template <class ... OstreamTypes>
		streamroute(std::ostream & source, OstreamTypes & ... destination) :
			_source(source),
			_buffer(source.rdbuf(new multibuf(source.rdbuf(), destination.rdbuf() ...)))
		{
			static_assert(stdx::meta::conjunction<stdx::meta::bind<std::is_base_of, std::ostream, stdx::meta::placeholders::_1>::invoke>::template trait<OstreamTypes ...>::value, 
						  "stdx::stream::streamroute::streamroute<OstreamTypes ...>: OstreamTypes must be of type or derived of type std::ostream");
		}
		streamroute(streamroute const &) = delete;
		streamroute & operator=(streamroute const &) = delete;
		streamroute(streamroute &&) = delete;
		streamroute & operator=(streamroute &&) = delete;
		~streamroute() noexcept
		{
			delete _source.rdbuf(_buffer);
		}
	private:
		std::ostream &		_source;
		std::streambuf *	_buffer;
	};

	template <>
	class streamroute<policy::redirect>
	{
	public:
		streamroute(std::ostream & source, std::ostream & destination) :
			_source(source),
			_buffer(source.rdbuf(destination.rdbuf()))
		{
		}
		streamroute(streamroute const &) = delete;
		streamroute & operator=(streamroute const &) = delete;
		streamroute(streamroute &&) = delete;
		streamroute & operator=(streamroute &&) = delete;
		~streamroute() noexcept
		{
			_source.rdbuf(_buffer);
		}
	private:
		std::ostream &		_source;
		std::streambuf *	_buffer;
	};
}

#if defined(STDX_USING_STREAM) || defined(STDX_USING_ALL)
namespace stdx { using namespace stream; }
#endif

#endif
