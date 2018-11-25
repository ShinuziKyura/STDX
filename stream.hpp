#ifndef STDX_STREAM_HPP
#define STDX_STREAM_HPP

#include <vector>
#include <streambuf>
#include <iostream>
#include <fstream>
#include <sstream>
#if STDX_USING_CXX2A
#include <syncstream>
#endif

#include "meta.hpp"

namespace stdx::stream
{
#if _HAS_CXX20
	template <class CharType, class Traits>
	inline auto _initialize_osyncstream(std::basic_ostream<CharType, Traits> & stream)
	{
		std::basic_osyncstream<CharType, Traits> osyncstream(stream);
		osyncstream << std::emit_on_flush;
		return osyncstream;
	}

	inline auto csout = _initialize_osyncstream(std::cout);
	inline auto wcsout = _initialize_osyncstream(std::wcout);
	inline auto cserr = _initialize_osyncstream(std::cerr);
	inline auto wcserr = _initialize_osyncstream(std::wcerr);
	inline auto cslog = _initialize_osyncstream(std::clog);
	inline auto wcslog = _initialize_osyncstream(std::wclog);
#endif

	// Multibuf

	template <class CharType, class TraitsType = std::char_traits<CharType>>
	class basic_multibuf final : public std::basic_streambuf<CharType, TraitsType> // Based on this: http://wordaligned.org/articles/cpp-streambufs#toca-generic-version
	{
	public:
		using char_type = CharType;
		using traits_type = TraitsType;
		using int_type = typename TraitsType::int_type;
		using pos_type = typename TraitsType::pos_type;
		using off_type = typename TraitsType::off_type;
		using streambuf_type = std::basic_streambuf<CharType, TraitsType>;

		template <class ... StreambufTypes>
		basic_multibuf(StreambufTypes * ... streams) : 
			streambuf_type(),
			_streams({ streams ... })
		{
			static_assert(stdx::meta::conjunction_v<stdx::meta::bind<std::is_base_of, streambuf_type, stdx::meta::placeholders::_1>::invoke, StreambufTypes ...>, 
						  "'stdx::stream::basic_multibuf<CharType, TraitsType>::basic_multibuf(StreambufTypes * ...)': StreambufTypes must be of type or derived of type std::basic_streambuf<CharType, TraitsType>");
		}
		basic_multibuf(basic_multibuf const & other) :
			streambuf_type(other),
			_streams(other._streams)
		{
		}
		basic_multibuf & operator=(basic_multibuf const & other)
		{
			streambuf_type::operator=(other);
			_streams = other._streams;
			return *this;
		}
		basic_multibuf(basic_multibuf &&) = delete;
		basic_multibuf & operator=(basic_multibuf &&) = delete;

		void swap(basic_multibuf & other)
		{
			streambuf_type::swap(other);
			_streams.swap(other._streams);
		}
	protected: // See https://en.cppreference.com/w/cpp/io/basic_streambuf // TODO: possibly better error-handling
		// Locales
		virtual void imbue(std::locale const & loc) override
		{
			for (auto const & stream : _streams)
			{
				stream->pubimbue(loc);
			}
		}

		// Positioning
		virtual streambuf_type * setbuf(char_type * s, std::streamsize n) override
		{
			streambuf_type * retval = this;
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

		// Putback // NOTE: no sense in overriding this function for now (what would be the effect of undoing reading from multiple buffers?)
/*		virtual int_type pbackfail(int_type c = traits_type::eof())
		{
			// TODO
			return traits_type::eof();
		}
*/	private:
		std::vector<streambuf_type *> _streams;
	};

	using multibuf = basic_multibuf<std::streambuf::char_type>;

	// Streamroute

	namespace policy
	{
		struct replicate;
		struct redirect;
	}

	template <class, class>
	class streamroute;

	template <class StreamType>
	class streamroute<StreamType, policy::replicate>
	{
		static_assert(std::is_base_of_v<std::basic_ostream<typename StreamType::char_type, typename StreamType::traits_type>, StreamType>,
					  "'stdx::stream::streamroute<StreamType, policy::replicate>': StreamType must be of type std::basic_ostream or derived from it");

		using StreambufType = std::decay_t<decltype(*std::declval<StreamType>().rdbuf())>;
	public:
		template <class ... OutstreamTypes>
		streamroute(StreamType & source, OutstreamTypes & ... destination) :
			_source(source),
			_buffer(source.rdbuf(new multibuf(source.rdbuf(), destination.rdbuf() ...)))
		{
			static_assert(stdx::meta::conjunction_v<stdx::meta::bind<std::is_base_of, StreamType, stdx::meta::placeholders::_1>::invoke, OutstreamTypes ...>,
						  "'stdx::stream::streamroute<StreamType, policy::replicate>::streamroute(StreamType &, OutstreamTypes & ...)': OstreamTypes must be an instance of or derived from type StreamType");
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
		StreamType & _source;
		StreambufType * _buffer;
	};

	template <class StreamType>
	class streamroute<StreamType, policy::redirect>
	{
		static_assert(std::is_base_of_v<std::basic_ios<typename StreamType::char_type, typename StreamType::traits_type>, StreamType>,
					  "'stdx::stream::streamroute<StreamType, policy::redirect>': StreamType must be of type std::basic_ios or derived from it");

		using StreambufType = std::decay_t<decltype(*std::declval<StreamType>().rdbuf())>;
	public:
		streamroute(StreamType & source, StreamType & destination) :
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
		StreamType & _source;
		StreambufType * _buffer;
	};
}

#endif

//=====

#if defined(STDX_USING_STREAM) || defined(STDX_USING_ALL)

namespace stdx 
{ 
	using namespace stream;
}

#endif
