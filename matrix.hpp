#ifndef STDX_MATRIX_HPP
#define STDX_MATRIX_HPP

#include <vector>
#include <valarray>

#include "meta.hpp"

// Note to self: check https://en.wikipedia.org/wiki/Expression_templates

#if defined(STDX_INDEXING_RESERVED_LITERALS) // defined(STDX_MATH_INDEXING_RESERVED_LITERALS)
#if defined(_MSC_VER)
#pragma warning(disable: 4455)
#elif // TODO other compilers (at least gcc and clang)
#endif
#endif

namespace stdx // math
{
	namespace indexing
	{
		struct _i_index
		{
			constexpr _i_index(unsigned long long index) : _index(index)
			{
			}

			unsigned long long const _index;
		};

		constexpr _i_index operator "" _i(unsigned long long index)
		{
			return _i_index(index - 1);
		}
#if defined(STDX_INDEXING_RESERVED_LITERALS) // defined(STDX_MATH_INDEXING_RESERVED_LITERALS)
		constexpr _i_index operator "" i(size_t index)
		{
			return _i_index(index - 1);
		}
#endif

		struct _j_index
		{
			constexpr _j_index(unsigned long long index) : _index(index)
			{
			}

			unsigned long long const _index;
		};

		constexpr _j_index operator "" _j(unsigned long long index)
		{
			return _j_index(index - 1);
		}
#if defined(STDX_INDEXING_RESERVED_LITERALS) // defined(STDX_MATH_INDEXING_RESERVED_LITERALS)
		constexpr _j_index operator "" j(size_t index)
		{
			return _j_index(index - 1);
		}
#endif

		struct _index
		{
			constexpr _index(_i_index i, _j_index j) : _i(i._index), _j(j._index)
			{
			}

			unsigned long long const _i;
			unsigned long long const _j;
		};

		_index operator ,(_i_index i, _j_index j)
		{
			return _index(i, j);
		}
	}

	enum class matrix_type : int
	{
		/*	Identity matrix, i.e.:
			1 0 0 0 0
			0 1 0 0 0
			0 0 1 0 0
			0 0 0 1 0
			0 0 0 0 1
		*/
		identity = 2,
		/*	Exchange matrix, i.e.:
			0 0 0 0 1
			0 0 0 1 0
			0 0 1 0 0
			0 1 0 0 0
			1 0 0 0 0
		*/
		exchange,
		/*	Shift upper matrix, i.e.:
			0 1 0 0 0
			0 0 1 0 0
			0 0 0 1 0
			0 0 0 0 1
			0 0 0 0 0
		*/
		shift_upper,
		/*	Shift lower matrix, i.e.:
			0 0 0 0 0
			1 0 0 0 0
			0 1 0 0 0
			0 0 1 0 0
			0 0 0 1 0
		*/
		shift_lower,
		/*	Pascal matrix, i.e.:
			1  1  1  1  1
			1  2  3  4  5
			1  3  6 10 15
			1  4 10 20 35
			1  5 15 35 70
		*/
		pascal,
		/*	Hilbert matrix, i.e.:
			1/1 1/2 1/3 1/4 1/5
			1/2 1/3 1/4 1/5 1/6
			1/3 1/4 1/5 1/6 1/7
			1/4 1/5 1/6 1/7 1/8
			1/5 1/6 1/7 1/8 1/9
		*/
		hilbert,
		/*	Lehmer matrix, i.e.:
			1/1 1/2 1/3 1/4 1/5
			1/2 2/2 2/3 2/4 2/5
			1/3 2/3 3/3 3/4 3/5
			1/4 2/4 3/4 4/4 4/5
			1/5 2/5 3/5 4/5 5/5
		*/
		lehmer,
		/*	One matrix, i.e.:
			1 1 1 1 1
			1 1 1 1 1
			1 1 1 1 1
			1 1 1 1 1
			1 1 1 1 1
		*/
		one = 1,
		/*	Zero matrix, i.e.:
			0 0 0 0 0
			0 0 0 0 0
			0 0 0 0 0
			0 0 0 0 0
			0 0 0 0 0
		*/
		zero = 0,
	};

	template <class Type>
	class matrix
	{
		static_assert(std::disjunction_v<std::is_arithmetic<Type>, stdx::meta::is_complex<Type>>, "'stdx::matrix<Type>': Type must be an arithmetic or std::complex type");
	public:
		
		matrix(size_t rows, size_t columns, matrix_type type = matrix_type::zero) :
			_matrix(rows, std::valarray<Type>(Type(type), columns))
		{
			if (rows != columns && type != matrix_type::one && type != matrix_type::zero)
			{
				throw std::invalid_argument("'stdx::matrix<Type>::matrix(size_t rows, size_t columns, stdx::matrix_type type)': "
											"Only one and zero matrices can be created when rows != columns");
			}

			if constexpr (std::negation_v<stdx::meta::is_complex<Type>>)
			{
				_initialize(type);
			}
		}
		matrix(std::initializer_list<std::initializer_list<Type>>)
		{
			// TODO
		}

		matrix<Type> operator+(matrix<Type> other)
		{
			// TODO
			return *this;
		}
		matrix<Type> operator+(Type scalar)
		{
			// TODO
			return *this;
		}
		matrix<Type> operator-(matrix<Type> other)
		{
			// TODO
			return *this;
		}
		matrix<Type> operator-(Type scalar)
		{
			// TODO
			return *this;
		}
		matrix<Type> operator*(matrix<Type> other)
		{
			// TODO
			return *this;
		}
		matrix<Type> operator*(Type scalar)
		{
			// TODO
			return *this;
		}
		// first row, then column
		Type operator[](indexing::_index index)
		{
			// TODO
			(void) index;
			return Type(); // _matrix[index._i][index._j];
		}
	private:
		struct _matrix_row
		{

		};

		void _initialize(matrix_type type)
		{
			// TODO
			switch (type)
			{
				case matrix_type::identity:
					break;
				case matrix_type::exchange:
					break;
				case matrix_type::shift_upper:
					break;
				case matrix_type::shift_lower:
					break;
				case matrix_type::pascal:
					break;
				case matrix_type::hilbert:
					break;
				case matrix_type::lehmer:
					break;
				case matrix_type::one:
				case matrix_type::zero:
					break;
			}
		}
		
		std::vector<std::valarray<Type>> _matrix;
	/*
		int _matrix[10][10] = 
		{
			{  0,  1,  2,  3,  4,  5,  6,  7,  8,  9 },
			{ 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 },
			{ 20, 21, 22, 23, 24, 25, 26, 27, 28, 29 },
			{ 30, 31, 32, 33, 34, 35, 36, 37, 38, 39 },
			{ 40, 41, 42, 43, 44, 45, 46, 47, 48, 49 },
			{ 50, 51, 52, 53, 54, 55, 56, 57, 58, 59 },
			{ 60, 61, 62, 63, 64, 65, 66, 67, 68, 69 },
			{ 70, 71, 72, 73, 74, 75, 76, 77, 78, 79 },
			{ 80, 81, 82, 83, 84, 85, 86, 87, 88, 89 },
			{ 90, 91, 92, 93, 94, 95, 96, 97, 98, 99 }
		};
	*/
	};
}

#endif
