#ifndef STDX_header_EVENT
#define STDX_header_EVENT

#include <type_traits>
#include <utility>
#include <exception>
#include <memory>
#include <variant>
#include <functional>
#include <unordered_set>
#include <unordered_map>

// Types that are intended to be event handlers must derive from this
#define STDX_EVENT_HANDLER_BASE	public virtual ::stdx::event::_event_handler_base

#define STDX_EVENT_HANDLER_BASE_COPY_CONSTRUCT(object) ::stdx::event::_event_handler_base(object)
#define STDX_EVENT_HANDLER_BASE_COPY_ASSIGN(object) ::stdx::event::_event_handler_base::operator=(object)
#define STDX_EVENT_HANDLER_BASE_MOVE_CONSTRUCT(object) ::stdx::event::_event_handler_base(::std::move(object))
#define STDX_EVENT_HANDLER_BASE_MOVE_ASSIGN(object) ::stdx::event::_event_handler_base::operator=(::std::move(object))

// Event dispatcher classes should be defined through this
#define STDX_DEFINE_EVENT_DISPATCHER(identifier, ...) using identifier = ::stdx::event::_event_dispatcher<__VA_ARGS__>

namespace stdx
{
#if !(defined(STDX_NAMESPACE_EVENT) || defined(STDX_NAMESPACE_ALL))
inline
#endif
namespace event
{
	class event_error final : public std::logic_error
	{
	public:
		explicit event_error()
			: logic_error("'stdx::event::event_error': empty result")
		{
		}

	};

	struct _event_result_type
	{
		enum : std::size_t
		{
			empty = 0,
			value = 1,
			exception = 2,
		};
	};

	template <class Type>
	class _event_result final
	{
		class _any_type
		{
		public:
			template <class ... Types>
			_any_type(Types && ...) noexcept
			{
			}

		};

		using _value_type =
			std::conditional_t<
				std::is_void_v<Type>,
				_any_type,
				std::conditional_t<
					std::is_reference_v<Type>,
					std::reference_wrapper<std::remove_reference_t<Type>>,
					Type
				>
			>;

		class _exception_type
		{
		public:
			_exception_type(std::exception_ptr && exception) noexcept
				: _exception(std::move(exception))
			{
			}

			operator std::exception_ptr() const noexcept
			{
				return _exception;
			}

		private:
			std::exception_ptr _exception;

		};

	public:
		_event_result() noexcept = default;

		auto get_value() -> Type
		{
			_value_type value(std::move(std::get<_event_result_type::value>(_result)));
			
			_result.template emplace<_event_result_type::empty>();

			return std::conditional_t<std::is_void_v<Type>, void, Type&&>(value);
		}
		auto get_exception() -> std::exception_ptr
		{
			_exception_type exception(std::move(std::get<_event_result_type::exception>(_result)));

			_result.template emplace<_event_result_type::empty>();

			return exception;
		}
		auto get_type() const noexcept -> std::size_t
		{
			return _result.index();
		}
		template <class ValueType>
		void set_value(ValueType && value)
		{
			_result.template emplace<_event_result_type::value>(std::forward<ValueType>(value));
		}
		void set_exception(std::exception_ptr && exception) noexcept
		{
			_result.template emplace<_event_result_type::exception>(std::move(exception));
		}
		
	private:
		std::variant<std::monostate, _value_type, _exception_type> _result;

	};

	template <class Type>
	class event_future final
	{
	public:
		event_future() noexcept = default;

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
		bool has_result() const noexcept
		{
			return _result_ptr.lock()->get_type() != _event_result_type::empty;
		}
		bool valid() const noexcept
		{
			return _result_ptr.use_count() != 0;
		}

	private:
		event_future(std::shared_ptr<_event_result<Type>> const & result_ptr) noexcept
			: _result_ptr(result_ptr)
		{
		}

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

		auto get_future() const noexcept -> event_future<Type>
		{
			return event_future(_result_ptr);
		}
		template <class ValueType>
		void set_value(ValueType && value)
		{
			_result_ptr->set_value(std::forward<ValueType>(value));
		}
		void set_exception(std::exception_ptr && exception) noexcept
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
			// We need to register each _event_handler_base through its 'this' pointer, because it will be different from the derived class one (due to virtual inheritance)
			return this;
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
			for (auto const & dispatcher : _dispatcher_set)
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

		template <class ObjType, class FuncType, class ClassType>
		class _event_handler_data : public _event_handler_data_base
		{
			using result_type = std::invoke_result_t<FuncType ClassType::*, ObjType *, ParamTypes ...>;

		public:
			_event_handler_data(ObjType * obj, FuncType ClassType::* func)
				: _object(obj)
				, _function(func)
			{
			}

			auto get_future() const noexcept -> event_future<result_type>
			{
				return _promise.get_future();
			}

			void invoke(ParamTypes && ... params) noexcept override
			{
				try
				{
					if constexpr (std::is_void_v<result_type>)
					{
						std::invoke(_function, _object, std::forward<ParamTypes>(params) ...);
						_promise.set_value(0);
					}
					else
					{
						_promise.set_value(std::invoke(_function, _object, std::forward<ParamTypes>(params) ...));
					}
				}
				catch (...)
				{
					_promise.set_exception(std::current_exception());
				}
			}

		private:
			ObjType * _object;
			FuncType ClassType::* _function;
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

		template <class ObjType, class FuncType, class ClassType>
		auto bind(ObjType * obj, FuncType ClassType::* func)
			-> event_future<std::invoke_result_t<FuncType ClassType::*, ObjType *, ParamTypes ...>>
		{
			static_assert(std::is_base_of_v<_event_handler_base, ObjType>,
						  "'stdx::event::_event_dispatcher<ParamTypes ...>::bind(ObjType*, FuncType ClassType::*)': "
						  "'ObjType' must derive from 'stdx::event::_event_handler_base'");
			static_assert(std::is_base_of_v<ClassType, ObjType>,
						  "'stdx::event::_event_dispatcher<ParamTypes ...>::bind(ObjType*, FuncType ClassType::*)': "
						  "'ObjType' must derive from 'ClassType'");

			auto handler = std::make_shared<_event_handler_data<ObjType, FuncType, ClassType>>(obj, func);

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
		bool is_bound(_event_handler_base const * handler) noexcept
		{
			return _handler_map.count(handler);
		}
		void broadcast(ParamTypes && ... params) noexcept
		{
			if (!_is_broadcasting)
			{
				_is_broadcasting = true;

				// Allows binding and unbinding to dispatcher inside handlers without affecting this invocation
				auto handler_map_image = _handler_map;

				for (auto const & handler : handler_map_image)
				{
					handler.second->invoke(std::forward<ParamTypes>(params) ...);
				}

				_is_broadcasting = false;
			}
		}

	private:
		bool _unbind(_event_handler_base const * handler) noexcept override
		{
			return _handler_map.erase(handler);
		}

		std::unordered_map<_event_handler_base const *, std::shared_ptr<_event_handler_data_base>> _handler_map;
		bool _is_broadcasting = false;

	};
}
}

#endif
