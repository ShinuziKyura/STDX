#ifndef STDX_THREAD_HPP
#define STDX_THREAD_HPP

#include <csetjmp>

#include <new>
#include <any>
#include <stack>
#include <thread>

namespace stdx::thread
{
	class _jmp_state
	{
		struct _jmp_var_base
		{
			virtual ~_jmp_var_base() = default;
		};
		template <class Type>
		struct _jmp_var : public _jmp_var_base
		{
			~_jmp_var() override
			{
				std::launder(reinterpret_cast<Type *>(&_object))->~Type();
			}

			std::aligned_storage_t<sizeof(Type), alignof(Type)> _object;
		};
		struct _jmp_buffer
		{
			std::jmp_buf _env;
		};
		using _var_stack = std::stack<std::any, std::list<std::any>>;
	public:
		void push_stack()
		{
			_locals.emplace();
		}
		void pop_stack()
		{
			_locals.pop();
		}

		template <class Type, class ... ArgTypes>
		std::decay_t<Type> & push_var(ArgTypes && ... args)
		{
			// return void *, remove args, emplace<_jmp_var<Type>>() // Need to erase type with _jmp_var_base
			return _locals.top().emplace().emplace<Type>(std::forward<ArgTypes>(args) ...);
		}
		void pop_var()
		{
			_locals.top().pop();
		}
		bool check_var()
		{
			return !_locals.top().empty();
		}

		std::jmp_buf & push_env()
		{
			return _buffers.emplace()._env;
		}
		std::jmp_buf & get_env()
		{
			return _buffers.top()._env;
		}
		void pop_env()
		{
			_buffers.pop();
		}
		bool check_env()
		{
			return !_buffers.empty();
		}

		int get_status()
		{
			return _status;
		}
		void set_status(int status)
		{
			_status = status;
		}
	private: // Imperfect system, needs work
		std::stack<_var_stack, std::list<_var_stack>> _locals;
		std::stack<_jmp_buffer, std::list<_jmp_buffer>> _buffers;
		int _status;
	};

	template <class FuncType, class ... ArgTypes>
	void daemon(FuncType func, ArgTypes && ... args)
	{
		std::thread(func, std::forward<ArgTypes>(args) ...).detach();
	}
}

namespace stdx::this_thread
{
	inline auto & get_jmp_state()
	{
		thread_local static stdx::thread::_jmp_state state;
		return state;
	}
};

#endif

//=====

#if defined(STDX_USING_THREAD) || defined(STDX_USING_ALL)

namespace stdx
{ 
	using namespace thread;
}

#endif
