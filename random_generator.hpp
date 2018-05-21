#ifndef RANDOM_GENERATOR_HPP
#define RANDOM_GENERATOR_HPP

#include <random>
#include <array>

namespace stdx
{
	using fastest_engine = std::ranlux48_base;
	using smallest_engine = std::minstd_rand;
	using ficklest_engine = std::mt19937_64;

	// TODO
	class distribution_parameters
	{
	public:
		distribution_parameters()
		{
		}
		template <class NumberType>
		distribution_parameters(std::initializer_list<NumberType> parameters)
		{
		}
	};

	// TODO
	template <class Engine = smallest_engine, class Distribution = std::bernoulli_distribution>
	class random_generator
	{
		static_assert(sizeof(typename Distribution::param_type::distribution_type), 
					  "Distribution must satisfy the requirements of RandomNumberDistribution");
	public:
		using engine_type = Engine;
		using distribution_type = Distribution;
		using result_type = typename Distribution::result_type;
		using param_type = typename Distribution::param_type;

		template <class Parameters = distribution_parameters>
		random_generator<Engine, Distribution>(Parameters parameters = distribution_parameters()) :
			_seed(std::begin(_random_data),
				  std::transform(std::begin(_random_data), std::end(_random_data), 
								 std::begin(_random_data), [this] (auto const &) { return _random_device(); })),
			_engine(_seed),
			_distribution(parameters)
		{
		}

		result_type operator()()
		{
			return _distribution(_engine);
		}
		operator engine_type()
		{
			return _engine;
		}
	private:
		std::random_device _random_device;
		std::array<std::mt19937_64::result_type, std::mt19937_64::state_size> _random_data;
		std::seed_seq _seed; // seed_seq is bad
		engine_type _engine;
		distribution_type _distribution;
	};

	// Uniform distributions
	using random_uniform_int_generator = random_generator<std::uniform_int_distribution<unsigned long long>>;
	using random_uniform_real_generator = random_generator<std::uniform_real_distribution<long double>>;

	// Bernoulli distributions
	using random_bernoulli_generator = random_generator<std::bernoulli_distribution>;
	using random_binomial_generator = random_generator<std::binomial_distribution<unsigned long long>>;
	using random_negative_binomial_generator = random_generator<std::negative_binomial_distribution<unsigned long long>>;
	using random_geometric_generator = random_generator<std::geometric_distribution<unsigned long long>>;

	// Poisson distributions
	using random_poisson_generator = random_generator<std::poisson_distribution<unsigned long long>>;
	using random_exponential_generator = random_generator<std::exponential_distribution<long double>>;
	using random_gamma_generator = random_generator<std::gamma_distribution<long double>>;
	using random_weibull_generator = random_generator<std::weibull_distribution<long double>>;
	using random_extreme_value_generator = random_generator<std::extreme_value_distribution<long double>>;

	// Normal distributions
	using random_normal_generator = random_generator<std::normal_distribution<long double>>;
	using random_lognormal_generator = random_generator<std::lognormal_distribution<long double>>;
	using random_chi_squared_generator = random_generator<std::chi_squared_distribution<long double>>;
	using random_cauchy_generator = random_generator<std::cauchy_distribution<long double>>;
	using random_fisher_f_generator = random_generator<std::fisher_f_distribution<long double>>;
	using random_student_t_generator = random_generator<std::student_t_distribution<long double>>;

	// Sampling distributions
	using random_discrete_generator = random_generator<std::discrete_distribution<unsigned long long>>;
	using random_piecewise_constant_generator = random_generator<std::piecewise_constant_distribution<long double>>;
	using random_piecewise_linear_generator = random_generator<std::piecewise_linear_distribution<long double>>;
}

#endif//RANDOM_GENERATOR_HPP
