#ifndef STDX_MATRIX_HPP
#define STDX_MATRIX_HPP

#include <limits>
#include <array>

#include "meta.hpp"

// Note to self: check https://en.wikipedia.org/wiki/Expression_templates

namespace stdx::math
{
	enum class matrix_name
	{
		/*	Zero matrix, i.e.:
			0 0 0 0 0
			0 0 0 0 0
			0 0 0 0 0
			0 0 0 0 0
			0 0 0 0 0 */
		zero,
		/*	One matrix, i.e.:
			1 1 1 1 1
			1 1 1 1 1
			1 1 1 1 1
			1 1 1 1 1
			1 1 1 1 1 */
		one,
		/*	Identity matrix, i.e.:
			1 0 0 0 0
			0 1 0 0 0
			0 0 1 0 0
			0 0 0 1 0
			0 0 0 0 1 */
		identity,
		/*	Exchange matrix, i.e.:
			0 0 0 0 1
			0 0 0 1 0
			0 0 1 0 0
			0 1 0 0 0
			1 0 0 0 0 */
		exchange,
		/*	Shift upper matrix, i.e.:
			0 1 0 0 0
			0 0 1 0 0
			0 0 0 1 0
			0 0 0 0 1
			0 0 0 0 0 */
		shift_upper,
		/*	Shift lower matrix, i.e.:
			0 0 0 0 0
			1 0 0 0 0
			0 1 0 0 0
			0 0 1 0 0
			0 0 0 1 0 */
		shift_lower,
		/*	Pascal upper matrix, i.e.:
			1  1  1  1  1
			0  1  2  3  4
			0  0  1  3  6
			0  0  0  1  4
			0  0  0  0  1 */
		pascal_upper,
		/*	Pascal lower matrix, i.e.:
			1  0  0  0  0
			1  1  0  0  0
			1  2  1  0  0
			1  3  3  1  0
			1  4  6  4  1 */
		pascal_lower,
		/*	Pascal matrix, i.e.:
			1  1  1  1  1
			1  2  3  4  5
			1  3  6 10 15
			1  4 10 20 35
			1  5 15 35 70 */
//		pascal,
		/*	Hilbert matrix, i.e.:
			1/1 1/2 1/3 1/4 1/5
			1/2 1/3 1/4 1/5 1/6
			1/3 1/4 1/5 1/6 1/7
			1/4 1/5 1/6 1/7 1/8
			1/5 1/6 1/7 1/8 1/9 */
		hilbert,
		/*	Lehmer matrix, i.e.:
			1/1 1/2 1/3 1/4 1/5
			1/2 2/2 2/3 2/4 2/5
			1/3 2/3 3/3 3/4 3/5
			1/4 2/4 3/4 4/4 4/5
			1/5 2/5 3/5 4/5 5/5 */
		lehmer,
	};

	template <class MatrixType>
	class _matrix//_expression
	{
		/*matrix<Type> operator+(_matrix<Type> other)
		{
			// TODO
			return *this;
		}
		matrix<Type> operator+(Type scalar)
		{
			// TODO
			return *this;
		}
		matrix<Type> operator-(_matrix<Type> other)
		{
			// TODO
			return *this;
		}
		matrix<Type> operator-(Type scalar)
		{
			// TODO
			return *this;
		}
		matrix<Type> operator*(_matrix<Type> other)
		{
			// TODO
			return *this;
		}
		matrix<Type> operator*(Type scalar)
		{
			// TODO
			return *this;
		} */
	};

	template <class Type, size_t Rows, size_t Columns = Rows>
	class matrix : public _matrix<matrix<Type, Rows, Columns>>
	{
		static_assert(std::is_arithmetic_v<Type>, "'stdx::math::matrix<Type, Rows, Columns>': Type must be an arithmetic type");
		static_assert(Rows > 0, "'stdx::math::matrix<Type, Rows, Columns>': Rows must be greater than 0");
		static_assert(Columns > 0, "'stdx::math::matrix<Type, Rows, Columns>': Columns must be greater than 0");
	public:
		constexpr matrix(std::array<std::array<Type, Columns>, Rows> matrix = {}) :
			_matrix(matrix)
		{
		}
		constexpr matrix(matrix_name name) :
			_matrix({})
		{
			static_assert(Rows == Columns, "");
			_initialize_matrix(name);
		}

		/* Matrix member access operator, one-indexed */
		constexpr Type & operator()(size_t const & i, size_t const & j)
		{
			if (i == 0 || j == 0 || i > Rows || j > Columns)
			{
				throw std::invalid_argument("'stdx::math::matrix<Type, Rows, Columns>::operator()(i, j)': " 
											"Member access operators' arguments 'i' and 'j' must be in the range of [1, Rows] and [1, Columns], respectively");
			}
			return _matrix[i - 1][j - 1];
		}
		/* Matrix member access operator, one-indexed */
		constexpr Type const & operator()(size_t const & i, size_t const & j) const
		{
			return const_cast<stdx::meta::remove_const_through_ref_t<decltype(*this)>>(*this).operator()(i, j);
		}

		constexpr size_t rows() const
		{
			return Rows;
		}
		constexpr size_t columns() const
		{
			return Columns;
		}
	private:
		constexpr void _initialize_matrix(matrix_name name)
		{
			switch (name)
			{
				case matrix_name::one:
					for (size_t ij = 0; ij != Rows; ++ij)
					{
						for (size_t index = ij; index != Rows; ++index)
						{
							_matrix[ij][index] = _matrix[index][ij] = Type(1);
						}
					}
					break;
				case matrix_name::identity:
					for (size_t index = 0; index != Rows; ++index)
					{
						_matrix[index][index] = Type(1);
					}
					break;
				case matrix_name::exchange:
					for (size_t index = 0; index != Rows; ++index)
					{
						_matrix[index][Rows - index - 1] = Type(1);
					}
					break;
				case matrix_name::shift_upper:
					for (size_t index = 1; index != Rows; ++index)
					{
						_matrix[index - 1][index] = Type(1);
					}
					break;
				case matrix_name::shift_lower:
					for (size_t index = 1; index != Rows; ++index)
					{
						_matrix[index][index - 1] = Type(1);
					}
					break;
				case matrix_name::pascal_upper:
					for (size_t i = 0; i != (Rows + 1) / 2; ++i)
					{
						for (size_t index = i * 2; index != Rows; ++index)
						{
							_matrix[i][index] = _matrix[index - i][index] = (i != 0 ? _matrix[i - 1][index - 1] + _matrix[i][index - 1] : Type(1));
						}
					}
					break;
				case matrix_name::pascal_lower:
					for (size_t j = 0; j != (Rows + 1) / 2; ++j)
					{
						for (size_t index = j * 2; index != Rows; ++index)
						{
							_matrix[index][j] = _matrix[index][index - j] = (j != 0 ? _matrix[index - 1][j - 1] + _matrix[index - 1][j] : Type(1));
						}
					}
					break;
			/*	case matrix_name::pascal:
					for (size_t ij = 0; ij != Rows; ++ij)
					{
						for (size_t index = ij; index != Rows; ++index)
						{
							_matrix[ij][index] = _matrix[index][ij] = (ij != 0 ? _matrix[ij - 1][index] + _matrix[ij][index - 1] : Type(1));
						}
					}
					break; */
				case matrix_name::hilbert:
					if constexpr (std::is_floating_point_v<Type>)
					{
						for (size_t ij = 0; ij != Rows; ++ij)
						{
							for (size_t index = ij; index != Rows; ++index)
							{
								_matrix[ij][index] = _matrix[index][ij] = Type(1.0) / (ij + index + 1);
							}
						}
					}
					break;
				case matrix_name::lehmer:
					if constexpr (std::is_floating_point_v<Type>)
					{
						for (size_t ij = 0; ij != Rows; ++ij)
						{
							for (size_t index = ij; index != Rows; ++index)
							{
								_matrix[ij][index] = _matrix[index][ij] = Type(ij + 1.0) / (index + 1);
							}
						}
					}
					break;
			}
		}
		
		std::array<std::array<Type, Columns>, Rows> _matrix;
	};
}

#endif
