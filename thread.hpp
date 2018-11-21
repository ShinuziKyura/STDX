#ifndef STDX_THREAD_HPP
#define STDX_THREAD_HPP

#include <csetjmp>

#include <memory>
#include <stack>
#include <thread>

namespace stdx::thread
{
	class _jmp_state
	{
		struct _jmp_var_object_base
		{
			virtual ~_jmp_var_object_base() = default;
		};
		template <class Type>
		struct _jmp_var_object : _jmp_var_object_base
		{
			~_jmp_var_object() override
			{
				std::launder(reinterpret_cast<Type *>(&_object))->~Type();
			}

			std::aligned_storage_t<sizeof(Type), alignof(Type)> _object;
		};

		struct _jmp_var
		{
			template <class Type>
			_jmp_var(std::unique_ptr<_jmp_var_object<Type>> && object) :
				_object_base(object)
			{
			}

			std::unique_ptr<_jmp_var_object_base> _object_base;
		};
		struct _jmp_buf
		{
			std::jmp_buf _env;
		};

		using _jmp_var_stack = std::stack<std::stack<_jmp_var>>;
	public:
		void push_stack()
		{
			_locals.top().emplace();
		}
		void pop_stack()
		{
			_locals.top().pop();
		}

		template <class Type>
		void * push_var()
		{
			return &_locals.top().top().emplace(std::make_unique<_jmp_var_object<Type>>())._object;
		}
		void pop_var()
		{
			_locals.top().top().pop();
		}
		bool check_var()
		{
			return !_locals.top().top().empty();
		}

		std::jmp_buf & push_env()
		{
			_locals.emplace();
			return _buffers.emplace()._env;
		}
		std::jmp_buf & get_env()
		{
			return _buffers.top()._env;
		}
		void pop_env()
		{
			_locals.pop();
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
	private:
		std::stack<_jmp_var_stack> _locals;
		std::stack<_jmp_buf> _buffers;
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
