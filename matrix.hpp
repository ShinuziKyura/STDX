#ifndef STDX_header_MATRIX
#define STDX_header_MATRIX

#include <limits>
#include <array>
#include <tuple>

#include "meta.hpp" // Consider including necessary metafunctions only

// See http://graphics.stanford.edu/~seander/bithacks.html

namespace stdx::math
{
	enum matrix_type
	{
		/*	Identity matrix, i.e.:
			1 0 0 0 0
			0 1 0 0 0
			0 0 1 0 0
			0 0 0 1 0
			0 0 0 0 1 */
		identity_matrix,
		/*	Exchange matrix, i.e.:
			0 0 0 0 1
			0 0 0 1 0
			0 0 1 0 0
			0 1 0 0 0
			1 0 0 0 0 */
		exchange_matrix,
		/*	Shift upper matrix, i.e.:
			0 1 0 0 0
			0 0 1 0 0
			0 0 0 1 0
			0 0 0 0 1
			0 0 0 0 0 */
		shift_upper_matrix,
		/*	Shift lower matrix, i.e.:
			0 0 0 0 0
			1 0 0 0 0
			0 1 0 0 0
			0 0 1 0 0
			0 0 0 1 0 */
		shift_lower_matrix,
		/*	Pascal upper matrix, i.e.:
			1  1  1  1  1
			0  1  2  3  4
			0  0  1  3  6
			0  0  0  1  4
			0  0  0  0  1 */
		pascal_upper_matrix,
		/*	Pascal lower matrix, i.e.:
			1  0  0  0  0
			1  1  0  0  0
			1  2  1  0  0
			1  3  3  1  0
			1  4  6  4  1 */
		pascal_lower_matrix,
		/*	Hilbert matrix, i.e.:
			1/1 1/2 1/3 1/4 1/5
			1/2 1/3 1/4 1/5 1/6
			1/3 1/4 1/5 1/6 1/7
			1/4 1/5 1/6 1/7 1/8
			1/5 1/6 1/7 1/8 1/9 */
		hilbert_matrix,
		/*	Lehmer matrix, i.e.:
			1/1 1/2 1/3 1/4 1/5
			1/2 2/2 2/3 2/4 2/5
			1/3 2/3 3/3 3/4 3/5
			1/4 2/4 3/4 4/4 4/5
			1/5 2/5 3/5 4/5 5/5 */
		lehmer_matrix,
	};

	template <class>
	class _matrix_expression;

	////////////////////////////////////////////////////////////////////////////////
	// Matrix
	////////////////////////////////////////////////////////////////////////////////

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
			_m(matrix)
		{
		}
		constexpr matrix(value_type value) :
			_m(_initialize_matrix(value))
		{
		}
		constexpr matrix(matrix_type name) :
			_m(_initialize_matrix(name))
		{
			static_assert(rows == columns, 
						  "'stdx::math::matrix<ValueType, Rows, Columns>::matrix(Name)': Rows must be equal to Columns");
		}
		template <class MatrixType>
		constexpr matrix(_matrix_expression<MatrixType> const & expression) :
			_m(_initialize_matrix(expression))
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
			return _m[i - 1][j - 1];
		}
		value_type & operator()(std::size_t const & i, std::size_t const & j)
		{
			return const_cast<value_type &>(stdx::meta::add_const_through_ref_t<decltype(*this)>(*this)(i, j));
		}
	private:
		static constexpr auto _initialize_matrix(value_type value)
		{
			std::array<std::array<value_type, columns>, rows> m = {};
			for (std::size_t i = 0; i != rows; ++i)
			{
				for (std::size_t j = 0; j != columns; ++j)
				{
					m[i][j] = value;
				}
			}
			return m;
		}
		static constexpr auto _initialize_matrix(matrix_type type)
		{
			std::array<std::array<value_type, columns>, rows> m = {};
			switch (type)
			{
				case identity_matrix:
					for (std::size_t ij = 0; ij != rows; ++ij)
					{
						m[ij][ij] = value_type(1);
					}
					break;
				case exchange_matrix:
					for (std::size_t ij = 0; ij != rows; ++ij)
					{
						m[ij][columns - ij - 1] = value_type(1);
					}
					break;
				case shift_upper_matrix:
					for (std::size_t ij = 1; ij != rows; ++ij)
					{
						m[ij - 1][ij] = value_type(1);
					}
					break;
				case shift_lower_matrix:
					for (std::size_t ij = 1; ij != rows; ++ij)
					{
						m[ij][ij - 1] = value_type(1);
					}
					break;
				case pascal_upper_matrix:
					for (std::size_t i = 0; i != (rows + 1) / 2; ++i)
					{
						for (std::size_t j = i * 2; j != columns; ++j)
						{
							m[i][j] = m[j - i][j] = (i != 0 ? m[i - 1][j - 1] + m[i][j - 1] : value_type(1));
						}
					}
					break;
				case pascal_lower_matrix:
					for (std::size_t j = 0; j != (columns + 1) / 2; ++j)
					{
						for (std::size_t i = j * 2; i != rows; ++i)
						{
							m[i][j] = m[i][i - j] = (j != 0 ? m[i - 1][j - 1] + m[i - 1][j] : value_type(1));
						}
					}
					break;
				case hilbert_matrix:
					if constexpr (std::is_floating_point_v<value_type>)
					{
						for (std::size_t i = 0; i != rows; ++i)
						{
							for (std::size_t j = i; j != columns; ++j)
							{
								m[i][j] = m[j][i] = value_type(1.0) / (i + j + 1);
							}
						}
					}
					break;
				case lehmer_matrix:
					if constexpr (std::is_floating_point_v<value_type>)
					{
						for (std::size_t i = 0; i != rows; ++i)
						{
							for (std::size_t j = i; j != columns; ++j)
							{
								m[i][j] = m[j][i] = value_type(i + 1.0) / (j + 1);
							}
						}
					}
					break;
			}
			return m;
		}
		template <class MatrixType>
		static constexpr auto _initialize_matrix(_matrix_expression<MatrixType> const & expression)
		{
			std::array<std::array<value_type, columns>, rows> m = {};
			for (std::size_t i = 0; i != rows; ++i)
			{
				for (std::size_t j = 0; j != columns; ++j)
				{
					m[i][j] = static_cast<MatrixType const &>(expression)(i + 1, j + 1);
				}
			}
			return m;
		}

		std::array<std::array<value_type, columns>, rows> _m;
	};

	// TODO unsure about utility of this
/*	template <class ValueType, std::size_t Rows, std::size_t Columns>
	matrix(std::array<std::array<ValueType, Columns>, Rows>) -> matrix<ValueType, Rows, Columns>; */

	template <class MatrixType>
	matrix(_matrix_expression<MatrixType>) -> matrix<typename MatrixType::value_type, MatrixType::rows, MatrixType::columns>;

	////////////////////////////////////////////////////////////////////////////////
	// Matrix operations
	////////////////////////////////////////////////////////////////////////////////

	template <class MatrixType1, class MatrixType2>
	class _matrix_addition : public _matrix_expression<_matrix_addition<MatrixType1, MatrixType2>>
	{
		static_assert(MatrixType1::rows == MatrixType2::rows && MatrixType1::columns == MatrixType2::columns, 
					  "'stdx::math::_matrix_addition<MatrixType1, MatrixType2>': MatrixType1 and MatrixType2 must have the same dimensions");
	
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
					  "'stdx::math::_matrix_subtraction<MatrixType1, MatrixType2>': MatrixType1 and MatrixType2 must have the same dimensions");

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
					  "'stdx::math::_matrix_multiplication<MatrixType1, MatrixType2>': MatrixType1::columns must equal MatrixType2::rows");

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
			return _dot_product(i, j, stdx::meta::make_index_sequence<1, MatrixType1::columns>());
		}
	private:
		template <std::size_t ... K>
		constexpr value_type _dot_product(std::size_t const & i, std::size_t const & j, std::index_sequence<K ...>) const
		{
			return (value_type(0) + ... + (_a(i, K) * _b(K, j)));
		}

		MatrixType1 const & _a;
		MatrixType2 const & _b;

		template <class>
		friend class _matrix_expression;
	};

	template <class MatrixType1, class MatrixType2>
	class _matrix_entrywise_multiplication : public _matrix_expression<_matrix_entrywise_multiplication<MatrixType1, MatrixType2>>
	{
		static_assert(MatrixType1::rows == MatrixType2::rows && MatrixType1::columns == MatrixType2::columns,
					  "'stdx::math::_matrix_entrywise_multiplication<MatrixType1, MatrixType2>': MatrixType1 and MatrixType2 must have the same dimensions");

		constexpr _matrix_entrywise_multiplication(MatrixType1 const & a, MatrixType2 const & b) :
			_a(a),
			_b(b)
		{
		}
	public:
		using value_type = decltype(typename MatrixType1::value_type() * typename MatrixType2::value_type());
		static constexpr std::size_t rows = MatrixType1::rows;
		static constexpr std::size_t columns = MatrixType1::columns;

		constexpr value_type operator()(std::size_t const & i, std::size_t const & j) const
		{
			return _a(i, j) * _b(i, j);
		}
	private:
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
	class _sub_matrix : public _matrix_expression<_sub_matrix<MatrixType>>
	{
		constexpr _sub_matrix(MatrixType const & a, std::size_t const & i, std::size_t const & j) :
			_a(a),
			_i(i),
			_j(j)
		{
			if (i == 0 || j == 0 || i > MatrixType::rows || j > MatrixType::columns)
			{
				throw std::invalid_argument("'stdx::math::_sub_matrix<Matrix>::_sub_matrix(m, i, j)': i and j must be in the range of [1, Matrix::rows] and [1, Matrix::columns], respectively");
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

	template <class MatrixType>
	class _permutated_matrix : public _matrix_expression<_permutated_matrix<MatrixType>>
	{
		constexpr _permutated_matrix(MatrixType const & a, std::array<std::size_t, MatrixType::rows> const & p, bool const n) :
			_a(a),
			_p(p),
			_n(n)
		{
		}
	public:
		using value_type = typename MatrixType::value_type;
		static constexpr std::size_t rows = MatrixType::rows;
		static constexpr std::size_t columns = MatrixType::columns;

		constexpr value_type const & operator()(std::size_t const & i, std::size_t const & j) const
		{
			return _a(_p[i - 1], j);
		}

		constexpr std::array<std::size_t, rows> const & permutation() const // TODO ensure these names are unique
		{
			return _p;
		}
		constexpr bool is_negated() const
		{
			return _n;
		}
	private:
		MatrixType const & _a;
		std::array<std::size_t, rows> const _p;
		bool const _n;

		template <class>
		friend class _matrix_expression;
	};

	template <class ValueType, std::size_t Size>
	class _LUP_matrix : public _matrix_expression<_LUP_matrix<ValueType, Size>>
	{
		constexpr _LUP_matrix(matrix<ValueType, Size> const & m, std::array<std::array<ValueType, Size>, Size> const & l, std::array<std::array<ValueType, Size>, Size> const & u, std::array<std::size_t, Size> const & p) :
			matrix<ValueType, Size>(m),
			_l(l),
			_u(u),
			_p(p)
		{
		}
	public:
		using value_type = ValueType;
		static constexpr std::size_t rows = Size;
		static constexpr std::size_t columns = Size;

		// TODO rest of implementation

	private:
		std::array<std::array<value_type, columns>, rows> _l;
		std::array<std::array<value_type, columns>, rows> _u;
		std::array<std::size_t, rows> _p;
	};

	////////////////////////////////////////////////////////////////////////////////
	// Matrix expression
	////////////////////////////////////////////////////////////////////////////////

	namespace _math
	{
		template <class MatrixType, std::size_t Size = MatrixType::rows, std::size_t Index = Size - MatrixType::rows>
		constexpr auto _pivot(_matrix_expression<MatrixType> const & expression, std::array<std::size_t, Size> permutation = {}, std::size_t sign = Size)
		{
			if constexpr (Index < Size - 1)
			{
				auto max_element = std::numeric_limits<typename MatrixType::value_type>::lowest();
				std::size_t max_i = 0;

				for (std::size_t i = 1; i <= MatrixType::rows; ++i)
				{
					if (const auto element = static_cast<MatrixType const &>(expression)(i, 1); element > max_element)
					{
						max_element = element;
						max_i = i;
					}
				}

				permutation[Index] = max_i;
				sign ^= max_i;

				return _pivot(expression.submatrix(max_i, 1), permutation, sign);
			}
			else
			{
				permutation[Index] = 1;
				sign ^= sign | 1;

				for (std::size_t index_1 = Index; index_1 > 0; --index_1)
				{
					for (std::size_t index_2 = index_1; index_2 > 0; --index_2)
					{
						permutation[index_1] += std::size_t(permutation[index_2 - 1] <= permutation[index_1]);
					}
				}

				return std::make_tuple(permutation, sign);
			}
		}

		template <class ValueType, std::size_t Size, std::size_t ... Index>
		constexpr std::array<std::array<ValueType, Size>, Size> _identity(std::index_sequence<Index ...>)
		{
			return { ValueType(Index % (Size + 1) == 0) ... };
		}
		template <class ValueType, std::size_t Size>
		constexpr auto _identity()
		{
			return _identity<ValueType, Size>(stdx::meta::make_index_sequence<0, Size * Size - 1>());
		}
		
		template <class MatrixType, std::size_t ... K>
		constexpr auto _upper_element(_matrix_expression<MatrixType> const & expression,
									  std::array<std::array<typename MatrixType::value_type, MatrixType::columns>, MatrixType::rows> const & lower,
									  std::array<std::array<typename MatrixType::value_type, MatrixType::columns>, MatrixType::rows> const & upper,
									  std::size_t i,
									  std::size_t j,
									  std::index_sequence<K ...>)
		{
			return expression(i + 1, j + 1) - (typename MatrixType::value_type(0) + ... + (lower[i][K] * upper[K][j]));
		}

		template <class MatrixType, std::size_t ... K>
		constexpr auto _lower_element(_matrix_expression<MatrixType> const & expression,
									  std::array<std::array<typename MatrixType::value_type, MatrixType::columns>, MatrixType::rows> const & lower,
									  std::array<std::array<typename MatrixType::value_type, MatrixType::columns>, MatrixType::rows> const & upper,
									  std::size_t i,
									  std::size_t j,
									  std::index_sequence<K ...> k)
		{
			return _upper_element(expression, lower, upper, i, j, k) / upper[j][j];
		}

		template <class MatrixType, std::size_t Index = 0>
		constexpr auto _LUP_decomposition(_matrix_expression<MatrixType> const & expression, 
										  std::array<std::array<typename MatrixType::value_type, MatrixType::columns>, MatrixType::rows> lower = _identity<typename MatrixType::value_type, MatrixType::rows>(),
										  std::array<std::array<typename MatrixType::value_type, MatrixType::columns>, MatrixType::rows> upper = {})
		{
			// TODO separate this further into methods
			if constexpr (Index/* < MatrixType::rows*/)
			{
				for (std::size_t j = Index; j < upper.size(); ++j)
				{
					upper[Index][j] = _upper_element(expression, lower, upper, Index, j, stdx::meta::make_index_sequence<0, Index>());
				}
				for (std::size_t i = Index; i < lower.size(); ++i)
				{
					lower[i][Index] = _lower_element(expression, lower, upper, i, Index, stdx::meta::make_index_sequence<0, Index>());
				}

				return _LUP_decomposition(expression, lower, upper);
			}
			else
			{
				(void) expression;
				return std::make_tuple(lower, upper);
			}
		}
	}

	template <class MatrixType>
	class _matrix_expression
	{
	public:
		// Operations
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
		template <class OtherMatrixType>
		constexpr auto operator%(_matrix_expression<OtherMatrixType> const & other) const
		{
			return _matrix_entrywise_multiplication(static_cast<MatrixType const &>(*this), static_cast<OtherMatrixType const &>(other));
		}
		constexpr auto operator+() const
		{
			return _matrix_transposition(static_cast<MatrixType const &>(*this));
		}

		// Alternate operations
		constexpr auto transpose() const
		{
			return this->operator*();
		}

		// Transformations
		constexpr auto submatrix(std::size_t const & i, std::size_t const & j) const
		{
			return _sub_matrix(static_cast<MatrixType const &>(*this), i, j);
		}
		// TODO
	/*	constexpr auto permutate(std::array<std::size_t, MatrixType::rows> const & permutation) const
		{
			return _permutated_matrix(static_cast<MatrixType const &>(*this), permutation, _math::_is_negated(permutation));
		} */
		constexpr auto pivot() const // TODO should this be renamed to 'partial_pivot'?
		{
			const auto & [permutation, sign] = _math::_pivot(*this);
			return _permutated_matrix(static_cast<MatrixType const &>(*this), permutation, bool(sign));
		}
		constexpr auto LUP_decomposition() const
		{
			return _math::_LUP_decomposition(pivot());
		}

		// Utilities
		constexpr bool is_upper_triangular() const
		{
			static_assert(MatrixType::rows == MatrixType::columns,
						  "'stdx::math::_matrix_expression<MatrixType>': MatrixType::rows must be equal to MatrixType::columns");
			return _is_upper_triangular(stdx::meta::make_index_sequence<1, MatrixType::rows>());
		}
		constexpr bool is_lower_triangular() const
		{
			static_assert(MatrixType::rows == MatrixType::columns,
						  "'stdx::math::_matrix_expression<MatrixType>': MatrixType::rows must be equal to MatrixType::columns");
			return _is_lower_triangular(stdx::meta::make_index_sequence<1, MatrixType::columns>());
		}
		constexpr bool is_triangular() const
		{
			return is_upper_triangular() || is_lower_triangular();
		}
		constexpr auto determinant() const // TODO use LUP_decomposition when above size N (need to profile and find N)
		{
			static_assert(MatrixType::rows == MatrixType::columns,
						  "'stdx::math::_matrix_expression<MatrixType>': MatrixType::rows must be equal to MatrixType::columns");
			// Applying ri  -> a*ri has the effect of multiplying det(A) by a.
			// Applying ri  -> ri + a*rj has no effect on det(A).
			// Applying ri <-> rj has the effect of multiplying det(A) by -1.

			return _determinant(stdx::meta::make_index_sequence<1, MatrixType::columns>());
		}
	protected: // TODO RS eventually move these to _math
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
		// WARNING: Uses Laplace expansion, which has a time complexity of O(n!)
		template <std::size_t ... J>
		constexpr auto _determinant(std::index_sequence<J ...>) const
		{
			return ((static_cast<MatrixType const &>(*this)(1, J) * submatrix(1, J).determinant()) - ... - typename MatrixType::value_type(0));
		}
		constexpr auto _determinant(std::index_sequence<1>) const
		{
			return static_cast<MatrixType const &>(*this)(1, 1);
		}
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
