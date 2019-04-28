#ifndef STDX_header_EVENT
#define STDX_header_EVENT

#include <type_traits>
#include <utility>
#include <exception>
#include <memory>
#include <variant>
#include <unordered_set>
#include <unordered_map>

// Types that are intended to be event handlers must derive from this
#define STDX_EVENT_HANDLER public virtual ::stdx::event::_event_handler_base

#define STDX_EVENT_HANDLER_COPY_CONSTRUCTOR(object) _event_handler_base(object)
#define STDX_EVENT_HANDLER_COPY_ASSIGNMENT(object) _event_handler_base::operator=(object)
#define STDX_EVENT_HANDLER_MOVE_CONSTRUCTOR(object) _event_handler_base(std::move(object))
#define STDX_EVENT_HANDLER_MOVE_ASSIGNMENT(object) _event_handler_base::operator=(std::move(object))

// Event dispatcher classes should be defined through this
#define STDX_DEFINE_EVENT_DISPATCHER(identifier, ...) using identifier = ::stdx::event::_event_dispatcher<__VA_ARGS__>

namespace stdx
{
#if !(defined(STDX_NAMESPACE_EVENT) || defined(STDX_NAMESPACE_ALL))
inline
#endif
namespace event
{
	class _any_type
	{
	public:
		template <class ... Types>
		_any_type(Types && ...)
		{
		}

	};

	class event_error final : public std::logic_error
	{
	public:
		event_error()
			: logic_error("'stdx::event::event_error': no state")
		{
		}

	};

	enum class _event_result_type
	{
		value,
		exception,
		none
	};

	template <class Type>
	class _event_result final
	{
		static_assert(!(std::is_array_v<Type> || std::is_rvalue_reference_v<Type>),
					  "'stdx::event::_event_result<Type>': 'Type' must be of non-array, non-rvalue-reference type");

		using _value_type = std::conditional_t<std::is_void_v<Type>, _any_type, std::conditional_t<std::is_lvalue_reference_v<Type>, std::reference_wrapper<std::remove_reference_t<Type>>, Type>>;

		class _exception_type
		{
		public:
			_exception_type(std::exception_ptr && exception)
				: _exception(exception)
			{
			}
			operator std::exception_ptr() const
			{
				return _exception;
			}

		private:
			std::exception_ptr _exception;

		};

		static constexpr std::size_t _value_index = 1;
		static constexpr std::size_t _exception_index = 2;

	public:
		auto get_value() const -> Type
		{
			return Type(std::get<_value_index>(_result));
		}
		auto get_exception() const -> std::exception_ptr
		{
			return std::get<_exception_index>(_result);
		}
		auto get_type() const noexcept -> _event_result_type
		{
			switch (_result.index())
			{
				case _value_index:
					return _event_result_type::value;
				case _exception_index:
					return _event_result_type::exception;
				default:
					return _event_result_type::none;
			}
		}
		template <class ValueType>
		void set_value(ValueType && value)
		{
			_result.emplace<_value_index>(std::forward<std::conditional_t<std::is_void_v<Type>, ValueType, Type>>(value));
		}
		void set_exception(std::exception_ptr && exception)
		{
			_result.emplace<_exception_index>(std::move(exception));
		}
		
	private:
		std::variant<std::monostate, _value_type, _exception_type> _result;

	};

	template <class Type>
	class event_future final
	{
	public:
		auto get() const -> Type
		{
			auto const result_ptr = _result_ptr.lock();

			switch (result_ptr->get_type())
			{
				case _event_result_type::value:
					return result_ptr->get_value();
				case _event_result_type::exception:
					std::rethrow_exception(result_ptr->get_exception());
				default:
					throw event_error();
			}
		}
		bool has_state() const
		{
			return _result_ptr.lock()->get_type() != _event_result_type::none;
		}
		bool valid() const noexcept
		{
			return _result_ptr.use_count() != 0;
		}

	private:
		std::weak_ptr<_event_result<Type>> _result_ptr;

		template <class>
		friend class event_promise;

	};

	template <class Type>
	class event_promise final
	{
	public:
		event_promise()
			: _result_ptr(std::make_shared<_event_result<Type>>())
		{
		}
		event_promise(event_promise const &) = delete;
		event_promise & operator=(event_promise const &) = delete;
		event_promise(event_promise && other) noexcept
			: _result_ptr(std::move(other._result_ptr))
		{
		}
		event_promise & operator=(event_promise && other) noexcept
		{
			_result_ptr = std::move(other._result_ptr);

			return *this;
		}

		auto get_future() const -> event_future<Type>
		{
			event_future<Type> future;

			future._result_ptr = _result_ptr;

			return future;
		}
		template <class ValueType>
		void set_value(ValueType && value)
		{
			_result_ptr->set_value(std::forward<ValueType>(value));
		}
		void set_exception(std::exception_ptr && exception)
		{
			_result_ptr->set_exception(std::move(exception));
		}

	private:
		std::shared_ptr<_event_result<Type>> _result_ptr;

	};

	class _event_dispatcher_base
	{
	protected:
		~_event_dispatcher_base() noexcept = default;

	private:
		virtual bool _unbind(class _event_handler_base const *) noexcept = 0;

		friend class _event_handler_base;

	};

	class _event_handler_base
	{
	protected:
		_event_handler_base() = default;
		_event_handler_base(_event_handler_base const &) noexcept
		{
			_unbind();
		}
		_event_handler_base & operator=(_event_handler_base const &) noexcept
		{
			_unbind();

			return *this;
		}
		_event_handler_base(_event_handler_base && other) noexcept
		{
			other._unbind();
		}
		_event_handler_base & operator=(_event_handler_base && other) noexcept
		{
			other._unbind();

			return *this;
		}
		~_event_handler_base() noexcept
		{
			_unbind();
		}

	private:
		auto _this() const noexcept -> _event_handler_base const *
		{
			return this; // Because virtual base classes are weird, we need to register each _event_handler_base through its 'this' pointer, which will be different from the derived class one
		}
		void _bind(_event_dispatcher_base * dispatcher) const
		{
			_dispatcher_set.insert(dispatcher);
		}
		void _unbind(_event_dispatcher_base * dispatcher) const noexcept
		{
			_dispatcher_set.erase(dispatcher);
		}
		void _unbind() const noexcept
		{
			for (auto const dispatcher : _dispatcher_set)
			{
				dispatcher->_unbind(this);
			}

			_dispatcher_set.clear();
		}

		mutable std::unordered_set<_event_dispatcher_base *> _dispatcher_set;
		
		template <class ...>
		friend class _event_dispatcher;

	};

	template <class ... ParamTypes>
	class _event_dispatcher final : public _event_dispatcher_base
	{
		static_assert(std::bool_constant<(... && (!std::is_rvalue_reference_v<ParamTypes> || std::is_trivially_copyable_v<ParamTypes>))>::value,
					  "'stdx::event::_event_dispatcher<ParamTypes ...>': "
					  "'ParamTypes' must be of non-rvalue-reference type or otherwise trivially copyable");

		class _event_handler_data_base
		{
		public:
			virtual ~_event_handler_data_base() noexcept = default;

			virtual void invoke(ParamTypes && ... params) noexcept = 0;

		};

		template <class ObjType, class FuncType>
		class _event_handler_data : public _event_handler_data_base
		{
			using result_type = std::invoke_result_t<FuncType std::decay_t<ObjType>::*, ObjType *, ParamTypes ...>;

		public:
			_event_handler_data(ObjType * obj, FuncType std::decay_t<ObjType>::* func)
				: _object(obj)
				, _function(func)
			{
			}

			auto get_future() const -> event_future<result_type>
			{
				return _promise.get_future();
			}

			void invoke(ParamTypes && ... params) noexcept override
			{
				try
				{
					if constexpr (std::is_void_v<result_type>)
					{
						(_object->*_function)(std::forward<ParamTypes>(params) ...);

						_promise.set_value(0); // Value needs to be set
					}
					else
					{
						_promise.set_value((_object->*_function)(std::forward<ParamTypes>(params) ...));
					}
				}
				catch (...)
				{
					_promise.set_exception(std::current_exception());
				}
			}

		private:
			ObjType * _object;
			FuncType std::decay_t<ObjType>::* _function;
			event_promise<result_type> _promise;

		};

	public:
		_event_dispatcher() = default;
		_event_dispatcher(_event_dispatcher const &) = delete;
		_event_dispatcher & operator=(_event_dispatcher const &) = delete;
		_event_dispatcher(_event_dispatcher &&) = delete;
		_event_dispatcher & operator=(_event_dispatcher &&) = delete;
		~_event_dispatcher() noexcept
		{
			unbind();
		}

		template <class ObjType, class FuncType>
		auto bind(ObjType * obj, FuncType std::decay_t<ObjType>::* func)
		{
			static_assert(std::is_base_of_v<_event_handler_base, std::decay_t<ObjType>>,
						  "'stdx::event::_event_dispatcher<ParamTypes ...>::bind(ObjType*, FuncType std::decay_t<ObjType>::*)': "
						  "'std::decay_t<ObjType>' must derive from 'stdx::event::_event_handler_base'");

			auto handler = std::make_unique<_event_handler_data<ObjType, FuncType>>(obj, func);

			auto future = handler->get_future();

			if (_handler_map.insert_or_assign(obj->_event_handler_base::_this(), std::move(handler)).second)
			{
				obj->_event_handler_base::_bind(this);
			}

			return future;
		}
		void unbind(_event_handler_base const * handler) noexcept
		{
			if (_unbind(handler))
			{
				handler->_unbind(this);
			}
		}
		void unbind() noexcept
		{
			for (auto const & handler : _handler_map)
			{
				handler.first->_unbind(this);
			}

			_handler_map.clear();
		}
		bool is_bound(_event_handler_base const * handler) const noexcept
		{
			return _handler_map.count(handler);
		}
		void broadcast(ParamTypes && ... params) noexcept
		{
			for (auto const & handler : _handler_map)
			{
				handler.second->invoke(std::forward<ParamTypes>(params) ...);
			}
		}

	private:
		bool _unbind(_event_handler_base const * handler) noexcept override
		{
			return _handler_map.erase(handler);
		}

		std::unordered_map<_event_handler_base const *, std::unique_ptr<_event_handler_data_base>> _handler_map;

	};
}
}

#endif
