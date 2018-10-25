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

	template <class>
	class _matrix_expression;

	template <class ValueType, size_t Rows, size_t Columns = Rows>
	class matrix : public _matrix_expression<matrix<ValueType, Rows, Columns>>
	{
		static_assert(std::is_arithmetic_v<ValueType>, "'stdx::math::matrix<ValueType, Rows, Columns>': ValueType must be an arithmetic type");
		static_assert(Rows > 0, "'stdx::math::matrix<ValueType, Rows, Columns>': Rows must be greater than 0");
		static_assert(Columns > 0, "'stdx::math::matrix<ValueType, Rows, Columns>': Columns must be greater than 0");
	public:
		using value_type = ValueType;

		constexpr matrix(std::array<std::array<ValueType, Columns>, Rows> matrix = {}) :
			_matrix(matrix)
		{
		}
		constexpr matrix(value_type value) :
			_matrix(_initialize_matrix(value))
		{
		}
		constexpr matrix(matrix_name name) :
			_matrix(_initialize_matrix(name))
		{
			static_assert(Rows == Columns, 
						  "'stdx::math::matrix<ValueType, Rows, Columns>::matrix(Name)': Rows must be equal to Columns");
		}
		template <class MatrixType>
		constexpr matrix(_matrix_expression<MatrixType> const & expression) :
			_matrix(expression.matrix())
		{
			static_assert(std::is_same_v<value_type, typename MatrixType::value_type>,
						  "'stdx::math::matrix<ValueType, Rows, Columns>::matrix(expression)': ValueType must be the same type as decltype(expression)::value_type");
			static_assert(Rows == MatrixType::rows() && Columns == MatrixType::columns(),
						  "'stdx::math::matrix<ValueType, Rows, Columns>::matrix(expression)': This matrix and expression must have the same dimensions");
		}

		// Member access operator
		constexpr value_type const & operator()(size_t const & i, size_t const & j) const
		{
			if (i == 0 || j == 0 || i > Rows || j > Columns)
			{
				throw std::invalid_argument("'stdx::math::matrix<ValueType, Rows, Columns>::operator()(i, j)': i and j must be in the range of [1, Rows] and [1, Columns], respectively");
			}
			return _matrix[i - 1][j - 1];
		}
		// Member access operator
		value_type & operator()(size_t const & i, size_t const & j)
		{
			return const_cast<value_type &>(stdx::meta::add_const_through_ref_t<decltype(*this)>(*this)(i, j));
		}

		static constexpr size_t rows()
		{
			return Rows;
		}
		static constexpr size_t columns()
		{
			return Columns;
		}
	private:
		static constexpr std::array<std::array<ValueType, Columns>, Rows> _initialize_matrix(value_type value)
		{
			std::array<std::array<ValueType, Columns>, Rows> matrix = {};
			for (size_t i = 0; i != Rows; ++i)
			{
				for (size_t j = 0; j != Columns; ++j)
				{
					matrix[i][j] = value;
				}
			}
			return matrix;
		}
		static constexpr std::array<std::array<ValueType, Columns>, Rows> _initialize_matrix(matrix_name name)
		{
			std::array<std::array<ValueType, Columns>, Rows> matrix = {};
			switch (name)
			{
				case matrix_name::identity:
					for (size_t index = 0; index != Rows; ++index)
					{
						matrix[index][index] = value_type(1);
					}
					break;
				case matrix_name::exchange:
					for (size_t index = 0; index != Rows; ++index)
					{
						matrix[index][Rows - index - 1] = value_type(1);
					}
					break;
				case matrix_name::shift_upper:
					for (size_t index = 1; index != Rows; ++index)
					{
						matrix[index - 1][index] = value_type(1);
					}
					break;
				case matrix_name::shift_lower:
					for (size_t index = 1; index != Rows; ++index)
					{
						matrix[index][index - 1] = value_type(1);
					}
					break;
				case matrix_name::pascal_upper:
					for (size_t i = 0; i != (Rows + 1) / 2; ++i)
					{
						for (size_t index = i * 2; index != Rows; ++index)
						{
							matrix[i][index] = matrix[index - i][index] = (i != 0 ? matrix[i - 1][index - 1] + matrix[i][index - 1] : value_type(1));
						}
					}
					break;
				case matrix_name::pascal_lower:
					for (size_t j = 0; j != (Rows + 1) / 2; ++j)
					{
						for (size_t index = j * 2; index != Rows; ++index)
						{
							matrix[index][j] = matrix[index][index - j] = (j != 0 ? matrix[index - 1][j - 1] + matrix[index - 1][j] : value_type(1));
						}
					}
					break;
			/*	case matrix_name::pascal:
					for (size_t ij = 0; ij != Rows; ++ij)
					{
						for (size_t index = ij; index != Rows; ++index)
						{
							matrix[ij][index] = matrix[index][ij] = (ij != 0 ? matrix[ij - 1][index] + matrix[ij][index - 1] : value_type(1));
						}
					}
					break; */
				case matrix_name::hilbert:
					if constexpr (std::is_floating_point_v<value_type>)
					{
						for (size_t ij = 0; ij != Rows; ++ij)
						{
							for (size_t index = ij; index != Rows; ++index)
							{
								matrix[ij][index] = matrix[index][ij] = value_type(1.0) / (ij + index + 1);
							}
						}
					}
					break;
				case matrix_name::lehmer:
					if constexpr (std::is_floating_point_v<value_type>)
					{
						for (size_t ij = 0; ij != Rows; ++ij)
						{
							for (size_t index = ij; index != Rows; ++index)
							{
								matrix[ij][index] = matrix[index][ij] = value_type(ij + 1.0) / (index + 1);
							}
						}
					}
					break;
			}
			return matrix;
		}
		
		std::array<std::array<ValueType, Columns>, Rows> _matrix;
	};

	template <class MatrixType>
	matrix(_matrix_expression<MatrixType>) -> matrix<typename MatrixType::value_type, MatrixType::rows(), MatrixType::columns()>;

	template <class Matrix1, class Matrix2>
	class _matrix_addition : public _matrix_expression<_matrix_addition<Matrix1, Matrix2>>
	{
		static_assert(Matrix1::rows() == Matrix2::rows() && Matrix1::columns() == Matrix2::columns(), 
					  "'stdx::math::_matrix_addition<Matrix1, Matrix2>': Matrix1 and Matrix2 must have the same dimensions");
	
		constexpr _matrix_addition(Matrix1 const & a, Matrix2 const & b) :
			_matrix1(a),
			_matrix2(b)
		{
		}
	public:
		using value_type = decltype(std::declval<typename Matrix1::value_type const &>() + std::declval<typename Matrix2::value_type const &>());

		// Member access operator
		constexpr value_type operator()(size_t const & i, size_t const & j) const
		{
			return _matrix1(i, j) + _matrix2(i, j);
		}

		static constexpr size_t rows()
		{
			return Matrix1::rows();
		}
		static constexpr size_t columns()
		{
			return Matrix1::columns();
		}
	private:
		Matrix1 const & _matrix1;
		Matrix2 const & _matrix2;

		template <class>
		friend class _matrix_expression;
	};

	template <class MatrixType>
	class _matrix_expression
	{
	public:
		/* This is a constant expression when the operands have static storage duration */
		template <class OtherMatrixType>
		constexpr _matrix_addition<MatrixType, OtherMatrixType> operator+(_matrix_expression<OtherMatrixType> const & other) const
		{
			return _matrix_addition(static_cast<MatrixType const &>(*this), static_cast<OtherMatrixType const &>(other));
		}

	/*	matrix<ValueType> operator+(ValueType scalar)
		{
			// TODO
			return *this;
		}
		matrix<ValueType> operator-(_matrix_expression<ValueType> other)
		{
			// TODO
			return *this;
		}
		matrix<ValueType> operator-(ValueType scalar)
		{
			// TODO
			return *this;
		}
		matrix<ValueType> operator*(_matrix_expression<ValueType> other)
		{
			// TODO
			return *this;
		}
		matrix<ValueType> operator*(ValueType scalar)
		{
			// TODO
			return *this;
		} */
	private:
		constexpr auto matrix() const
		{
			auto this_matrix = static_cast<MatrixType const &>(*this);
			std::array<std::array<MatrixType::value_type, MatrixType::columns()>, MatrixType::rows()> matrix = {};
			for (size_t i = 0; i != MatrixType::rows(); ++i)
			{
				for (size_t j = 0; j != MatrixType::columns(); ++j)
				{
					matrix[i][j] = this_matrix(i + 1, j + 1);
				}
			}
			return matrix;
		}

		template <class, size_t, size_t>
		friend class matrix;
	};
}

#endif

//=====

#if defined(STDX_USING_MATH) || defined(STDX_USING_ALL)

namespace stdx
{
	using namespace math;
}

#endif