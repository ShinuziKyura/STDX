#ifndef STDX_MATRIX_HPP
#define STDX_MATRIX_HPP

#include <limits>
#include <array>

#include "meta.hpp"

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

	template <class, class>
	class _matrix_addition;

	template <class, class>
	class _matrix_subtraction;

	template <class, class>
	class _matrix_multiplication;

	template <class>
	class _matrix_transposition;

	template <class, size_t, size_t>
	class _matrix_submatrix;

	template <class MatrixType>
	class _matrix_expression
	{
	public:
		template <class OtherMatrixType>
		constexpr auto operator+(_matrix_expression<OtherMatrixType> const & other) const
		{
			return _matrix_addition(static_cast<MatrixType const &>(*this), static_cast<OtherMatrixType const &>(other));
		}
		template <class OtherMatrixType>
		constexpr auto operator-(_matrix_expression<OtherMatrixType> const & other) const
		{
			return _matrix_subtraction(static_cast<MatrixType const &>(*this), static_cast<OtherMatrixType const &>(other));
		}
		template <class OtherMatrixType>
		constexpr auto operator*(_matrix_expression<OtherMatrixType> const & other) const
		{
			return _matrix_multiplication(static_cast<MatrixType const &>(*this), static_cast<OtherMatrixType const &>(other));
		}
		constexpr auto operator!() const
		{
			return _matrix_transposition(static_cast<MatrixType const &>(*this));
		}

		template <size_t I, size_t J>
		constexpr auto submatrix() const
		{
			return _matrix_submatrix<MatrixType, I, J>(static_cast<MatrixType const &>(*this));
		}
		constexpr bool is_upper_triangular() const
		{
			static_assert(MatrixType::rows == MatrixType::columns,
						  "'stdx::math::_matrix_expression<MatrixType>': MatrixType::rows must be equal to MatrixType::columns");
			return _is_upper_triangular(stdx::meta::make_integer_sequence<size_t, 1, MatrixType::rows>());
		}
		constexpr bool is_lower_triangular() const
		{
			static_assert(MatrixType::rows == MatrixType::columns,
						  "'stdx::math::_matrix_expression<MatrixType>': MatrixType::rows must be equal to MatrixType::columns");
			return _is_lower_triangular(stdx::meta::make_integer_sequence<size_t, 1, MatrixType::columns>());
		}
		constexpr bool is_triangular() const
		{
			return is_upper_triangular() || is_lower_triangular();
		}
		constexpr auto determinant() const
		{
			static_assert(MatrixType::rows == MatrixType::columns,
						  "'stdx::math::_matrix_expression<MatrixType>': MatrixType::rows must be equal to MatrixType::columns");
			// Applying ri  -> a*ri has the effect of multiplying det(A) by a.
			// Applying ri  -> ri + a*rj has no effect on det(A).
			// Applying ri <-> rj has the effect of multiplying det(A) by -1.
			return _determinant(stdx::meta::make_integer_sequence<size_t, 1, MatrixType::columns>());
		}
	private:
		template <size_t J, size_t ... IJ>
		constexpr bool _is_upper_triangular(std::index_sequence<J, IJ ...>) const
		{
			return (... && (static_cast<MatrixType const &>(*this)(IJ, J) == 0)) && _is_upper_triangular(std::index_sequence<IJ ...>());
		}
		constexpr bool _is_upper_triangular(std::index_sequence<>) const
		{
			return true;
		}
		template <size_t I, size_t ... IJ>
		constexpr bool _is_lower_triangular(std::index_sequence<I, IJ ...>) const
		{
			return (... && (static_cast<MatrixType const &>(*this)(I, IJ) == 0)) && _is_lower_triangular(std::index_sequence<IJ ...>());
		}
		constexpr bool _is_lower_triangular(std::index_sequence<>) const
		{
			return true;
		}
		// WARNING: Uses cofactor expansion, which has a time complexity for n*n matrices of O(n!); will be improved once LU decomposition is implemented
		template <size_t ... J>
		constexpr auto _determinant(std::index_sequence<J ...>) const 
		{
			return ((static_cast<MatrixType const &>(*this)(1, J) * submatrix<1, J>().determinant()) - ... - typename MatrixType::value_type(0));
		}
		constexpr auto _determinant(std::index_sequence<1>) const
		{
			return static_cast<MatrixType const &>(*this)(1, 1);
		}
/*		template <size_t ... IJ>
		auto _determinant(std::index_sequence<IJ ...>)
		{
			return (... * static_cast<MatrixType const &>(*this)(IJ, IJ));
		} */
	};

	template <class ValueType, size_t Rows, size_t Columns = Rows>
	class matrix : public _matrix_expression<matrix<ValueType, Rows, Columns>>
	{
		static_assert(std::is_arithmetic_v<ValueType>, "'stdx::math::matrix<ValueType, Rows, Columns>': ValueType must be an arithmetic type");
		static_assert(Rows > 0, "'stdx::math::matrix<ValueType, Rows, Columns>': Rows must be greater than 0");
		static_assert(Columns > 0, "'stdx::math::matrix<ValueType, Rows, Columns>': Columns must be greater than 0");
	public:
		using value_type = ValueType;
		static constexpr size_t rows = Rows;
		static constexpr size_t columns = Columns;

		constexpr matrix(std::array<std::array<value_type, columns>, rows> matrix = {}) :
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
			static_assert(rows == columns, 
						  "'stdx::math::matrix<ValueType, Rows, Columns>::matrix(Name)': Rows must be equal to Columns");
		}
		template <class MatrixType>
		constexpr matrix(_matrix_expression<MatrixType> const & expression) :
			_matrix(_initialize_matrix(expression))
		{
			static_assert(std::is_same_v<value_type, typename MatrixType::value_type>,
						  "'stdx::math::matrix<ValueType, Rows, Columns>::matrix(expression)': ValueType must be the same type as decltype(expression)::value_type");
			static_assert(rows == MatrixType::rows && columns == MatrixType::columns,
						  "'stdx::math::matrix<ValueType, Rows, Columns>::matrix(expression)': This matrix and expression must have the same dimensions");
		}

		constexpr value_type const & operator()(size_t const & i, size_t const & j) const
		{
			if (i == 0 || j == 0 || i > Rows || j > Columns)
			{
				throw std::invalid_argument("'stdx::math::matrix<ValueType, Rows, Columns>::operator()(i, j)': i and j must be in the range of [1, Rows] and [1, Columns], respectively");
			}
			return _matrix[i - 1][j - 1];
		}
		value_type & operator()(size_t const & i, size_t const & j)
		{
			return const_cast<value_type &>(stdx::meta::add_const_through_ref_t<decltype(*this)>(*this)(i, j));
		}
	private:
		static constexpr auto _initialize_matrix(value_type value)
		{
			std::array<std::array<value_type, columns>, rows> matrix = {};
			for (size_t i = 0; i != rows; ++i)
			{
				for (size_t j = 0; j != columns; ++j)
				{
					matrix[i][j] = value;
				}
			}
			return matrix;
		}
		static constexpr auto _initialize_matrix(matrix_name name)
		{
			std::array<std::array<value_type, columns>, rows> matrix = {};
			switch (name)
			{
				case matrix_name::identity:
					for (size_t ij = 0; ij != rows; ++ij)
					{
						matrix[ij][ij] = value_type(1);
					}
					break;
				case matrix_name::exchange:
					for (size_t ij = 0; ij != rows; ++ij)
					{
						matrix[ij][columns - ij - 1] = value_type(1);
					}
					break;
				case matrix_name::shift_upper:
					for (size_t ij = 1; ij != rows; ++ij)
					{
						matrix[ij - 1][ij] = value_type(1);
					}
					break;
				case matrix_name::shift_lower:
					for (size_t ij = 1; ij != rows; ++ij)
					{
						matrix[ij][ij - 1] = value_type(1);
					}
					break;
				case matrix_name::pascal_upper:
					for (size_t i = 0; i != (rows + 1) / 2; ++i)
					{
						for (size_t j = i * 2; j != columns; ++j)
						{
							matrix[i][j] = matrix[j - i][j] = (i != 0 ? matrix[i - 1][j - 1] + matrix[i][j - 1] : value_type(1));
						}
					}
					break;
				case matrix_name::pascal_lower:
					for (size_t j = 0; j != (columns + 1) / 2; ++j)
					{
						for (size_t i = j * 2; i != rows; ++i)
						{
							matrix[i][j] = matrix[i][i - j] = (j != 0 ? matrix[i - 1][j - 1] + matrix[i - 1][j] : value_type(1));
						}
					}
					break;
				case matrix_name::hilbert:
					if constexpr (std::is_floating_point_v<value_type>)
					{
						for (size_t i = 0; i != rows; ++i)
						{
							for (size_t j = i; j != columns; ++j)
							{
								matrix[i][j] = matrix[j][i] = value_type(1.0) / (i + j + 1);
							}
						}
					}
					break;
				case matrix_name::lehmer:
					if constexpr (std::is_floating_point_v<value_type>)
					{
						for (size_t i = 0; i != rows; ++i)
						{
							for (size_t j = i; j != columns; ++j)
							{
								matrix[i][j] = matrix[j][i] = value_type(i + 1.0) / (j + 1);
							}
						}
					}
					break;
			}
			return matrix;
		}
		template <class MatrixType>
		static constexpr auto _initialize_matrix(_matrix_expression<MatrixType> const & expression)
		{
			std::array<std::array<value_type, columns>, rows> matrix = {};
			for (size_t i = 0; i != rows; ++i)
			{
				for (size_t j = 0; j != columns; ++j)
				{
					matrix[i][j] = static_cast<MatrixType const &>(expression)(i + 1, j + 1);
				}
			}
			return matrix;
		}

		std::array<std::array<value_type, columns>, rows> _matrix;
	};

	template <class ValueType, size_t Rows, size_t Columns>
	matrix(std::array<std::array<ValueType, Columns>, Rows>) -> matrix<ValueType, Rows, Columns>;

	template <class MatrixType>
	matrix(_matrix_expression<MatrixType>) -> matrix<typename MatrixType::value_type, MatrixType::rows, MatrixType::columns>;

	template <class Matrix1, class Matrix2>
	class _matrix_addition : public _matrix_expression<_matrix_addition<Matrix1, Matrix2>>
	{
		static_assert(Matrix1::rows == Matrix2::rows && Matrix1::columns == Matrix2::columns, 
					  "'stdx::math::_matrix_addition<Matrix1, Matrix2>': Matrix1 and Matrix2 must have the same dimensions");
	
		constexpr _matrix_addition(Matrix1 const & a, Matrix2 const & b) :
			_matrix1(a),
			_matrix2(b)
		{
		}
	public:
		using value_type = decltype(typename Matrix1::value_type() + typename Matrix2::value_type());
		static constexpr size_t rows = Matrix1::rows;
		static constexpr size_t columns = Matrix1::columns;

		constexpr auto operator()(size_t const & i, size_t const & j) const
		{
			return _matrix1(i, j) + _matrix2(i, j);
		}
	private:
		Matrix1 const & _matrix1;
		Matrix2 const & _matrix2;

		template <class>
		friend class _matrix_expression;
	};

	template <class Matrix1, class Matrix2>
	class _matrix_subtraction : public _matrix_expression<_matrix_subtraction<Matrix1, Matrix2>>
	{
		static_assert(Matrix1::rows == Matrix2::rows && Matrix1::columns == Matrix2::columns,
					  "'stdx::math::_matrix_subtraction<Matrix1, Matrix2>': Matrix1 and Matrix2 must have the same dimensions");

		constexpr _matrix_subtraction(Matrix1 const & a, Matrix2 const & b) :
			_matrix1(a),
			_matrix2(b)
		{
		}
	public:
		using value_type = decltype(typename Matrix1::value_type() - typename Matrix2::value_type());
		static constexpr size_t rows = Matrix1::rows;
		static constexpr size_t columns = Matrix1::columns;

		constexpr auto operator()(size_t const & i, size_t const & j) const
		{
			return _matrix1(i, j) - _matrix2(i, j);
		}
	private:
		Matrix1 const & _matrix1;
		Matrix2 const & _matrix2;

		template <class>
		friend class _matrix_expression;
	};

	template <class Matrix1, class Matrix2>
	class _matrix_multiplication : public _matrix_expression<_matrix_multiplication<Matrix1, Matrix2>>
	{
		static_assert(Matrix1::columns == Matrix2::rows,
					  "'stdx::math::_matrix_multiplication<Matrix1, Matrix2>': Matrix1::columns() must equal Matrix2::rows()");

		constexpr _matrix_multiplication(Matrix1 const & a, Matrix2 const & b) :
			_matrix1(a),
			_matrix2(b)
		{
		}
	public:
		using value_type = decltype((typename Matrix1::value_type() * typename Matrix2::value_type()) + (typename Matrix1::value_type() * typename Matrix2::value_type()));
		static constexpr size_t rows = Matrix1::rows;
		static constexpr size_t columns = Matrix2::columns;

		constexpr auto operator()(size_t const & i, size_t const & j) const
		{
			return _dot_product(i, j, stdx::meta::make_integer_sequence<size_t, 1, Matrix1::columns>());
		}
	private:
		template <size_t ... IJ>
		constexpr auto _dot_product(size_t const & i, size_t const & j, std::index_sequence<IJ ...>) const
		{
			return (value_type(0) + ... + (_matrix1(i, IJ) * _matrix2(IJ, j)));
		}

		Matrix1 const & _matrix1;
		Matrix2 const & _matrix2;

		template <class>
		friend class _matrix_expression;
	};

	template <class Matrix>
	class _matrix_transposition : public _matrix_expression<_matrix_transposition<Matrix>>
	{
		constexpr _matrix_transposition(Matrix const & a) :
			_matrix(a)
		{
		}
	public:
		using value_type = typename Matrix::value_type;
		static constexpr size_t rows = Matrix::columns;
		static constexpr size_t columns = Matrix::rows;

		constexpr value_type operator()(size_t const & i, size_t const & j) const
		{
			return _matrix(j, i);
		}
	private:
		Matrix const & _matrix;

		template <class>
		friend class _matrix_expression;
	};

	template <class Matrix, size_t I, size_t J>
	class _matrix_submatrix : public _matrix_expression<_matrix_submatrix<Matrix, I, J>>
	{
		static_assert(Matrix::rows == Matrix::columns,
					  "'stdx::math::_matrix_submatrix<Matrix, I, J>': Matrix::rows must be equal to Matrix::columns");
		static_assert(1 <= I && I <= Matrix::rows && 1 <= J && J <= Matrix::columns,
					  "'stdx::math::_matrix_submatrix<Matrix, I, J>': I and J must be in the range of [1, Matrix::rows] and [1, Matrix::columns], respectively");
		constexpr _matrix_submatrix(Matrix const & a) :
			_matrix(a)
		{
		}
	public:
		using value_type = typename Matrix::value_type;
		static constexpr size_t rows = Matrix::rows - 1;
		static constexpr size_t columns = Matrix::columns - 1;

		constexpr value_type operator()(size_t const & i, size_t const & j) const
		{
			return _matrix(i + size_t(i >= I), j + size_t(j >= J));
		}
	private:
		Matrix const & _matrix;

		template <class>
		friend class _matrix_expression;
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
