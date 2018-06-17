#ifndef GENERATIVE_ITERATOR_HPP
#define GENERATIVE_ITERATOR_HPP

#include <functional>

namespace stdx
{
	template <class Iterator>
	class [[deprecated]] gen_iterator : public Iterator
	{
	public:
		template <class Function, class = std::enable_if_t<bool(sizeof(std::iterator_traits<Iterator>::iterator_category))>>
		gen_iterator<Iterator>(Iterator && iterator, Function func) : Iterator(std::forward<Iterator>(iterator)),
			_generator([func] (Iterator & _this) { *_this = func(); })
		{
		}
		template <class = std::enable_if_t<bool(sizeof(std::iterator_traits<Iterator>::iterator_category))>>
		gen_iterator<Iterator>(Iterator && iterator) : Iterator(std::forward<Iterator>(iterator)),
			_generator(nullptr)
		{
		}

		auto operator*()
		{
			_generator(*this);
			return *static_cast<Iterator &>(*this);
		}
		gen_iterator<Iterator> & operator++()
		{
			++static_cast<Iterator &>(*this);
			return *this;
		}
		gen_iterator<Iterator> operator++(int)
		{
			auto tmp{ *this };
			++static_cast<Iterator &>(*this);
			return tmp;
		}
		gen_iterator<Iterator> & operator--()
		{
			--static_cast<Iterator &>(*this);
			return *this;
		}
		gen_iterator<Iterator> operator--(int)
		{
			auto tmp{ *this };
			--static_cast<Iterator &>(*this);
			return tmp;
		}
	private:
		std::function<void(Iterator &)> _generator;
	};

	template <class Container, class Function>
	gen_iterator<typename Container::iterator> gen_begin(Container & cont, Function func)
	{
		return gen_iterator<typename Container::iterator>(std::begin(cont), func);
	}
	template <class Container>
	gen_iterator<typename Container::iterator> gen_end(Container & cont)
	{
		return gen_iterator<typename Container::iterator>(std::end(cont));
	}
	template <class Container, class Function>
	gen_iterator<typename Container::reverse_iterator> gen_rbegin(Container & cont, Function func)
	{
		return gen_iterator<typename Container::reverse_iterator>(std::rbegin(cont), func);
	}
	template <class Container>
	gen_iterator<typename Container::reverse_iterator> gen_rend(Container & cont)
	{
		return gen_iterator<typename Container::reverse_iterator>(std::rend(cont));
	}
}

#endif//GENERATIVE_ITERATOR_HPP
