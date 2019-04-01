#ifndef STDX_header_EVENT
#define STDX_header_EVENT

#include <set>
#include <map>
#include <memory>
#include <functional>

#define STDX_EVENT_HANDLER public virtual ::stdx::event::_event_handler_base
#define STDX_EVENT_HANDLER_COPY_CONSTRUCTOR(object) _event_handler_base(object)
#define STDX_EVENT_HANDLER_COPY_ASSIGNMENT(object) _event_handler_base::operator=(object)
#define STDX_EVENT_HANDLER_MOVE_CONSTRUCTOR(object) _event_handler_base(std::move(object))
#define STDX_EVENT_HANDLER_MOVE_ASSIGNMENT(object) _event_handler_base::operator=(std::move(object))

namespace stdx::event
{
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

		auto _this() const noexcept -> _event_handler_base const *
		{
			return this; // Because virtual base classes are weird, we need to register each _event_handler_base through its 'this' pointer, which will be different from the derived class one
		}

		mutable std::set<_event_dispatcher_base *> _dispatcher_set;
		
		template <class>
		friend class event_dispatcher;

	};

	template <class>
	class event_dispatcher;

	template <class ... ParamTypes>
	class event_dispatcher<void(ParamTypes ...)> final : public _event_dispatcher_base
	{
		static_assert(std::bool_constant<(... && (!std::is_rvalue_reference_v<ParamTypes> || std::is_trivially_copyable_v<ParamTypes>))>::value,
					  "'stdx::event::event_dispatcher<void(ParamTypes ...)>': ParamTypes must be of non-rvalue-reference type or otherwise trivially copyable");

		class _handler_object_base
		{
		public:
			virtual ~_handler_object_base() noexcept = default;

			virtual void invoke(ParamTypes && ... params) = 0;

			auto get_exception() noexcept -> std::exception_ptr
			{
				return _exception;
			}
			void set_exception(std::exception_ptr && exception) noexcept
			{
				_exception = exception;
			}

		private:
			std::exception_ptr _exception;

		};
		template <class ObjType, class FuncType, class ClassType>
		class _handler_object : public _handler_object_base
		{
		public:
			_handler_object(ObjType * obj, FuncType ClassType::* func)
				: _object(obj)
				, _function(func)
			{
			}

			void invoke(ParamTypes && ... params) override
			{
				(_object->*_function)(std::forward<ParamTypes>(params) ...);
			}

		private:
			ObjType * _object;
			FuncType ClassType::* _function;

		};

	public:
		event_dispatcher() = default;
		event_dispatcher(event_dispatcher const &) = delete;
		event_dispatcher & operator=(event_dispatcher const &) = delete;
		event_dispatcher(event_dispatcher &&) = delete;
		event_dispatcher & operator=(event_dispatcher &&) = delete;
		~event_dispatcher() noexcept
		{
			unbind();
		}

		template <class ObjType, class FuncType, class ClassType>
		void bind(ObjType * obj, FuncType ClassType::* func)
		{
			static_assert(std::is_base_of_v<_event_handler_base, ObjType>,
						  "'stdx::event::event_dispatcher<void(ParamTypes ...)>::bind(ObjType*, FuncType ClassType::*)': "
						  "ObjType must have base class of type 'stdx::event::_event_handler_base'");
			static_assert(std::is_base_of_v<ClassType, ObjType>,
						  "'stdx::event::event_dispatcher<void(ParamTypes ...)>::bind(ObjType*, FuncType ClassType::*)': "
						  "ClassType must be the same class or a base class of ObjType");

			if (_handler_map.insert_or_assign(obj->_event_handler_base::_this(), std::make_unique<_handler_object<ObjType, FuncType, ClassType>>(obj, func)).second)
			{
				obj->_event_handler_base::_bind(this);
			}
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

		void broadcast(ParamTypes && ... params) noexcept
		{
			for (auto const & handler : _handler_map)
			{
				std::exception_ptr exception;

				try
				{
					handler.second->invoke(std::forward<ParamTypes>(params) ...);
				}
				catch (...)
				{
					exception = std::current_exception();
				}

				handler.second->set_exception(exception);
			}
		}

		auto last_exception(_event_handler_base const * handler) const noexcept -> std::exception_ptr
		{
			std::exception_ptr exception;

			if (auto const iterator = _handler_map.find(handler); iterator != _handler_map.end())
			{
				exception = iterator->second->get_exception();
			}

			return exception;
		}
		auto last_exception() const noexcept -> std::map<_event_handler_base const *, std::exception_ptr>
		{
			std::map<_event_handler_base const *, std::exception_ptr> _exception_map;

			for (auto const & handler : _handler_map)
			{
				_exception_map.insert_or_assign(handler.first, handler.second->get_exception());
			}

			return _exception_map;
		}

	private:
		bool _unbind(_event_handler_base const * handler) noexcept override
		{
			return _handler_map.erase(handler);
		}

		std::map<_event_handler_base const *, std::unique_ptr<_handler_object_base>> _handler_map;

	};
}

#endif

//=====

#if defined(STDX_USING_EVENT) || defined(STDX_USING_ALL)

namespace stdx
{
	using namespace event;
}

#endif
