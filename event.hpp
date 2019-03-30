#ifndef STDX_header_EVENT
#define STDX_header_EVENT

#include <set>
#include <map>
#include <memory>

#include "meta.hpp"

#define STDX_EVENT_HANDLER public ::stdx::event::_event_handler // TODO understand virtual base classes and figure out if they can be applied to this

namespace stdx::event
{
	class _event_dispatcher_base
	{
	public:
		virtual ~_event_dispatcher_base() = default;

		virtual void unbind(void *) = 0;

	};

	class _event_handler
	{
	public:
		virtual ~_event_handler()
		{
			for (auto const & dispatcher : _dispatcher_set)
			{
				dispatcher->unbind(this);
			}
		}

	private:
		void bind(_event_dispatcher_base * dispatcher)
		{
			_dispatcher_set.insert(dispatcher);
		}
		void unbind(_event_dispatcher_base * dispatcher)
		{
			_dispatcher_set.erase(dispatcher);
		}

		std::set<_event_dispatcher_base *> _dispatcher_set;

		template <class>
		friend class event_dispatcher;

	};

	template <class>
	class event_dispatcher;

	template <class ... ParamTypes>
	class event_dispatcher<void(ParamTypes ...)> final : public _event_dispatcher_base
	{
		static_assert(std::bool_constant<(... && (!std::is_rvalue_reference_v<ParamTypes>/* || std::is_trivially_copyable_v<ParamTypes>*/))>::value,
					  "'stdx::event::event_dispatcher<void(ParamTypes ...)>': ParamTypes must be of non-rvalue-reference type");

		class _handler_object_base
		{
		public:
			virtual ~_handler_object_base() = default;

			virtual void invoke(void * obj, ParamTypes && ... params) const = 0;

		};
		template <class ObjType, class FuncType>
		class _handler_object : public _handler_object_base
		{
		public:
			_handler_object(FuncType ObjType::* func)
				: _function(func)
			{
			}

			void invoke(void * obj, ParamTypes && ... params) const override
			{
				(static_cast<ObjType *>(obj)->*_function)(std::forward<ParamTypes>(params) ...);
			}

		private:
			FuncType ObjType::* _function;

		};

	public:
		~event_dispatcher()
		{
			for (auto const & handler : _handler_map)
			{
				static_cast<_event_handler *>(handler.first)->unbind(this);
			}
		}

		template <class ObjType, class FuncType>
		void bind(ObjType * obj, FuncType ObjType::* func)
		{
			static_assert(std::is_base_of_v<_event_handler, ObjType>, 
						  "'stdx::event::event_dispatcher<void(ParamTypes ...)>::bind(ObjType*, FuncType ObjType::*)': "
						  "ObjType must have base class of type 'stdx::event::_event_handler'");
			static_assert(std::is_function_v<FuncType>,
						  "'stdx::event::event_dispatcher<void(ParamTypes ...)>::bind(ObjType*, FuncType ObjType::*)': "
						  "FuncType must be a function type");
			static_assert(std::is_same_v<stdx::meta::pack<ParamTypes ...>, typename stdx::meta::function_signature<FuncType>::parameter_types>,
						  "'stdx::event::event_dispatcher<void(ParamTypes ...)>::bind(ObjType*, FuncType ObjType::*)': "
						  "FuncType parameters must have same type as ParamTypes");

			if (_handler_map.insert_or_assign(obj, std::make_unique<_handler_object<ObjType, FuncType>>(func)).second)
			{
				// We use reinterpret_cast so that if the first assert fails, it's the only error displayed
				reinterpret_cast<_event_handler *>(obj)->bind(this);
			}
		}
		void unbind()
		{
			_handler_map.clear();
		}
		void unbind(void * obj) override
		{
			_handler_map.erase(obj);
		}

		void broadcast(ParamTypes && ... params) const
		{
			for (auto const & handler : _handler_map)
			{
				handler.second->invoke(handler.first, std::forward<ParamTypes>(params) ...);
			}
		}

	private:
		std::map<void *, std::unique_ptr<_handler_object_base>> _handler_map;

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
