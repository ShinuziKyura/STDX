#ifndef STDX_implementation_MATRIX_HEADER
#define STDX_implementation_MATRIX_HEADER

#include <limits>
#include <array>

#include "meta.hpp" // Consider including necessary metafunctions only

// See http://graphics.stanford.edu/~seander/bithacks.html

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

	template <class>
	class _matrix_permutation;
	
	template <class>
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

		constexpr auto transpose() const
		{
			return _matrix_transposition(static_cast<MatrixType const &>(*this));
		}
		constexpr auto pivot() const
		{
			return _matrix_permutation(static_cast<MatrixType const &>(*this), _pivot());
		}
		constexpr auto submatrix(std::size_t const & i, std::size_t const & j) const
		{
			return _matrix_submatrix(static_cast<MatrixType const &>(*this), i, j);
		}

		constexpr bool is_upper_triangular() const
		{
			static_assert(MatrixType::rows == MatrixType::columns,
						  "'stdx::math::_matrix_expression<MatrixType>': MatrixType::rows must be equal to MatrixType::columns");
			return _is_upper_triangular(stdx::meta::make_integer_sequence<std::size_t, 1, MatrixType::rows>());
		}
		constexpr bool is_lower_triangular() const
		{
			static_assert(MatrixType::rows == MatrixType::columns,
						  "'stdx::math::_matrix_expression<MatrixType>': MatrixType::rows must be equal to MatrixType::columns");
			return _is_lower_triangular(stdx::meta::make_integer_sequence<std::size_t, 1, MatrixType::columns>());
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

			// Use different method/overload depending on type of MatrixType
			return _determinant(stdx::meta::make_integer_sequence<std::size_t, 1, MatrixType::columns>());
		}
	protected:
		template <std::size_t I = 0, std::size_t Rows = MatrixType::rows>
		constexpr auto _pivot(std::array<std::size_t, Rows> permutation = {}) const
		{
			auto max = std::numeric_limits<typename MatrixType::value_type>::lowest();
			std::size_t max_i = 0;

			for (std::size_t i = 1; i <= MatrixType::rows; ++i)
			{
				if (auto element = static_cast<MatrixType const &>(*this)(i, 1); element > max)
				{
					max = element;
					max_i = i;
				}
			}

			permutation[I] = max_i;

			if constexpr (I + 2 < Rows)
			{
				return submatrix(max_i, 1)._pivot<I + 1>(permutation);
			}
			else
			{
				permutation[I + 1] = 1;

				for (std::size_t idx1 = I + 1; idx1 > 0; --idx1)
				{
					for (std::size_t idx2 = idx1 - 1; idx2 > 0; --idx2)
					{
						permutation[idx1] += std::size_t(permutation[idx2] <= permutation[idx1]);
					}
				}

				return permutation;
			}
		}
	private:
		template <std::size_t J, std::size_t ... I>
		constexpr bool _is_upper_triangular(std::index_sequence<J, I ...>) const
		{
			return (... && (static_cast<MatrixType const &>(*this)(I, J) == 0)) && _is_upper_triangular(std::index_sequence<I ...>());
		}
		template <std::size_t J>
		constexpr bool _is_upper_triangular(std::index_sequence<J>) const
		{
			return true;
		}
		template <std::size_t I, std::size_t ... J>
		constexpr bool _is_lower_triangular(std::index_sequence<I, J ...>) const
		{
			return (... && (static_cast<MatrixType const &>(*this)(I, J) == 0)) && _is_lower_triangular(std::index_sequence<J ...>());
		}
		template <std::size_t I>
		constexpr bool _is_lower_triangular(std::index_sequence<I>) const
		{
			return true;
		}
		// WARNING: Uses cofactor expansion, which has a time complexity for n*n matrices of O(n!); will be improved once LU decomposition is implemented (maybe maintain this method for matrices up to size 4)
		template <std::size_t ... J>
		constexpr auto _determinant(std::index_sequence<J ...>) const
		{
			return ((static_cast<MatrixType const &>(*this)(1, J) * submatrix(1, J).determinant()) - ... - typename MatrixType::value_type(0));
		}
		constexpr auto _determinant(std::index_sequence<1>) const
		{
			return static_cast<MatrixType const &>(*this)(1, 1);
		}
		
	/*	template <std::size_t ... IJ>
		auto _determinant(std::index_sequence<IJ ...>)
		{
			return (... * static_cast<MatrixType const &>(*this)(IJ, IJ));
		}	*/
	};

	template <class ValueType, std::size_t Rows, std::size_t Columns = Rows>
	class matrix : public _matrix_expression<matrix<ValueType, Rows, Columns>>
	{
		static_assert(std::is_arithmetic_v<ValueType>, "'stdx::math::matrix<ValueType, Rows, Columns>': ValueType must be an arithmetic type");
		static_assert(Rows > 0, "'stdx::math::matrix<ValueType, Rows, Columns>': Rows must be greater than 0");
		static_assert(Columns > 0, "'stdx::math::matrix<ValueType, Rows, Columns>': Columns must be greater than 0");
	public:
		using value_type = ValueType;
		static constexpr std::size_t rows = Rows;
		static constexpr std::size_t columns = Columns;

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

		constexpr value_type const & operator()(std::size_t const & i, std::size_t const & j) const
		{
			if (i == 0 || j == 0 || i > Rows || j > Columns)
			{
				throw std::invalid_argument("'stdx::math::matrix<ValueType, Rows, Columns>::operator()(i, j)': i and j must be in the range of [1, Rows] and [1, Columns], respectively");
			}
			return _matrix[i - 1][j - 1];
		}
		value_type & operator()(std::size_t const & i, std::size_t const & j)
		{
			return const_cast<value_type &>(stdx::meta::add_const_through_ref_t<decltype(*this)>(*this)(i, j));
		}
	private:
		static constexpr auto _initialize_matrix(value_type value)
		{
			std::array<std::array<value_type, columns>, rows> matrix = {};
			for (std::size_t i = 0; i != rows; ++i)
			{
				for (std::size_t j = 0; j != columns; ++j)
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
					for (std::size_t ij = 0; ij != rows; ++ij)
					{
						matrix[ij][ij] = value_type(1);
					}
					break;
				case matrix_name::exchange:
					for (std::size_t ij = 0; ij != rows; ++ij)
					{
						matrix[ij][columns - ij - 1] = value_type(1);
					}
					break;
				case matrix_name::shift_upper:
					for (std::size_t ij = 1; ij != rows; ++ij)
					{
						matrix[ij - 1][ij] = value_type(1);
					}
					break;
				case matrix_name::shift_lower:
					for (std::size_t ij = 1; ij != rows; ++ij)
					{
						matrix[ij][ij - 1] = value_type(1);
					}
					break;
				case matrix_name::pascal_upper:
					for (std::size_t i = 0; i != (rows + 1) / 2; ++i)
					{
						for (std::size_t j = i * 2; j != columns; ++j)
						{
							matrix[i][j] = matrix[j - i][j] = (i != 0 ? matrix[i - 1][j - 1] + matrix[i][j - 1] : value_type(1));
						}
					}
					break;
				case matrix_name::pascal_lower:
					for (std::size_t j = 0; j != (columns + 1) / 2; ++j)
					{
						for (std::size_t i = j * 2; i != rows; ++i)
						{
							matrix[i][j] = matrix[i][i - j] = (j != 0 ? matrix[i - 1][j - 1] + matrix[i - 1][j] : value_type(1));
						}
					}
					break;
				case matrix_name::hilbert:
					if constexpr (std::is_floating_point_v<value_type>)
					{
						for (std::size_t i = 0; i != rows; ++i)
						{
							for (std::size_t j = i; j != columns; ++j)
							{
								matrix[i][j] = matrix[j][i] = value_type(1.0) / (i + j + 1);
							}
						}
					}
					break;
				case matrix_name::lehmer:
					if constexpr (std::is_floating_point_v<value_type>)
					{
						for (std::size_t i = 0; i != rows; ++i)
						{
							for (std::size_t j = i; j != columns; ++j)
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
			for (std::size_t i = 0; i != rows; ++i)
			{
				for (std::size_t j = 0; j != columns; ++j)
				{
					matrix[i][j] = static_cast<MatrixType const &>(expression)(i + 1, j + 1);
				}
			}
			return matrix;
		}

		std::array<std::array<value_type, columns>, rows> _matrix;
	};

	template <class ValueType, std::size_t Rows, std::size_t Columns>
	matrix(std::array<std::array<ValueType, Columns>, Rows>) -> matrix<ValueType, Rows, Columns>;

	template <class MatrixType>
	matrix(_matrix_expression<MatrixType>) -> matrix<typename MatrixType::value_type, MatrixType::rows, MatrixType::columns>;

	template <class MatrixType1, class MatrixType2>
	class _matrix_addition : public _matrix_expression<_matrix_addition<MatrixType1, MatrixType2>>
	{
		static_assert(MatrixType1::rows == MatrixType2::rows && MatrixType1::columns == MatrixType2::columns, 
					  "'stdx::math::_matrix_addition<Matrix1, Matrix2>': Matrix1 and Matrix2 must have the same dimensions");
	
		constexpr _matrix_addition(MatrixType1 const & a, MatrixType2 const & b) :
			_a(a),
			_b(b)
		{
		}
	public:
		using value_type = decltype(typename MatrixType1::value_type() + typename MatrixType2::value_type());
		static constexpr std::size_t rows = MatrixType1::rows;
		static constexpr std::size_t columns = MatrixType1::columns;

		constexpr value_type operator()(std::size_t const & i, std::size_t const & j) const
		{
			return _a(i, j) + _b(i, j);
		}
	private:
		MatrixType1 const & _a;
		MatrixType2 const & _b;

		template <class>
		friend class _matrix_expression;
	};

	template <class MatrixType1, class MatrixType2>
	class _matrix_subtraction : public _matrix_expression<_matrix_subtraction<MatrixType1, MatrixType2>>
	{
		static_assert(MatrixType1::rows == MatrixType2::rows && MatrixType1::columns == MatrixType2::columns,
					  "'stdx::math::_matrix_subtraction<Matrix1, Matrix2>': Matrix1 and Matrix2 must have the same dimensions");

		constexpr _matrix_subtraction(MatrixType1 const & a, MatrixType2 const & b) :
			_a(a),
			_b(b)
		{
		}
	public:
		using value_type = decltype(typename MatrixType1::value_type() - typename MatrixType2::value_type());
		static constexpr std::size_t rows = MatrixType1::rows;
		static constexpr std::size_t columns = MatrixType1::columns;

		constexpr value_type operator()(std::size_t const & i, std::size_t const & j) const
		{
			return _a(i, j) - _b(i, j);
		}
	private:
		MatrixType1 const & _a;
		MatrixType2 const & _b;

		template <class>
		friend class _matrix_expression;
	};

	template <class MatrixType1, class MatrixType2>
	class _matrix_multiplication : public _matrix_expression<_matrix_multiplication<MatrixType1, MatrixType2>>
	{
		static_assert(MatrixType1::columns == MatrixType2::rows,
					  "'stdx::math::_matrix_multiplication<Matrix1, Matrix2>': Matrix1::columns() must equal Matrix2::rows()");

		constexpr _matrix_multiplication(MatrixType1 const & a, MatrixType2 const & b) :
			_a(a),
			_b(b)
		{
		}
	public:
		using value_type = decltype((typename MatrixType1::value_type() * typename MatrixType2::value_type()) + (typename MatrixType1::value_type() * typename MatrixType2::value_type()));
		static constexpr std::size_t rows = MatrixType1::rows;
		static constexpr std::size_t columns = MatrixType2::columns;

		constexpr value_type operator()(std::size_t const & i, std::size_t const & j) const
		{
			return _dot_product(i, j, stdx::meta::make_integer_sequence<std::size_t, 1, MatrixType1::columns>());
		}
	private:
		template <std::size_t ... IJ>
		constexpr value_type _dot_product(std::size_t const & i, std::size_t const & j, std::index_sequence<IJ ...>) const
		{
			return (value_type(0) + ... + (_a(i, IJ) * _b(IJ, j)));
		}

		MatrixType1 const & _a;
		MatrixType2 const & _b;

		template <class>
		friend class _matrix_expression;
	};

	template <class MatrixType>
	class _matrix_transposition : public _matrix_expression<_matrix_transposition<MatrixType>>
	{
		constexpr _matrix_transposition(MatrixType const & a) :
			_a(a)
		{
		}
	public:
		using value_type = typename MatrixType::value_type;
		static constexpr std::size_t rows = MatrixType::columns;
		static constexpr std::size_t columns = MatrixType::rows;

		constexpr value_type const & operator()(std::size_t const & i, std::size_t const & j) const
		{
			return _a(j, i);
		}
	private:
		MatrixType const & _a;

		template <class>
		friend class _matrix_expression;
	};

	template <class MatrixType>
	class _matrix_permutation : public _matrix_expression<_matrix_permutation<MatrixType>>
	{
		constexpr _matrix_permutation(MatrixType const & a, std::array<std::size_t, MatrixType::rows> const & p) :
			_a(a),
			_p(p)
		{
		}
	public:
		using value_type = typename MatrixType::value_type;
		static constexpr std::size_t rows = MatrixType::columns;
		static constexpr std::size_t columns = MatrixType::rows;

		constexpr value_type const & operator()(std::size_t const & i, std::size_t const & j) const
		{
			return _a(_p[i - 1], j);
		}
	private:
		MatrixType const & _a;
		std::array<std::size_t, rows> const _p;

		template <class>
		friend class _matrix_expression;
	};

	template <class MatrixType>
	class _matrix_submatrix : public _matrix_expression<_matrix_submatrix<MatrixType>>
	{
		constexpr _matrix_submatrix(MatrixType const & a, std::size_t const & i, std::size_t const & j) :
			_a(a),
			_i(i),
			_j(j)
		{
			if (i == 0 || j == 0 || i > MatrixType::rows || j > MatrixType::columns)
			{
				throw std::invalid_argument("'stdx::math::_matrix_submatrix<Matrix>::_matrix_submatrix(m, i, j)': i and j must be in the range of [1, Matrix::rows] and [1, Matrix::columns], respectively");
			}
		}
	public:
		using value_type = typename MatrixType::value_type;
		static constexpr std::size_t rows = MatrixType::rows - 1;
		static constexpr std::size_t columns = MatrixType::columns - 1;

		constexpr value_type const & operator()(std::size_t const & i, std::size_t const & j) const
		{
			return _a(i + std::size_t(i >= _i), j + std::size_t(j >= _j));
		}
	private:
		MatrixType const & _a;
		std::size_t const _i;
		std::size_t const _j;

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
