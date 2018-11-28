#ifndef STDX_implementation_THREAD_HEADER
#define STDX_implementation_THREAD_HEADER

#include <csetjmp>

#include <memory>
#include <stack>
#include <thread>

namespace stdx::thread
{
	class _jmp_state
	{
		struct _jmp_var_obj_base
		{
			virtual ~_jmp_var_obj_base() = default;

			virtual void * get_obj() = 0;
		};
		template <class Type>
		struct _jmp_var_obj : _jmp_var_obj_base
		{
			~_jmp_var_obj() override
			{
				std::destroy_at(std::launder(reinterpret_cast<Type *>(&_obj)));
			}

			void * get_obj() override
			{
				return &_obj;
			}

			std::aligned_storage_t<sizeof(Type), alignof(Type)> _obj;
		};

		struct _jmp_var
		{
			_jmp_var(std::unique_ptr<_jmp_var_obj_base> && obj_ptr) :
				_obj_ptr(std::move(obj_ptr))
			{
			}

			void * get_obj()
			{
				return _obj_ptr->get_obj();
			}

			std::unique_ptr<_jmp_var_obj_base> _obj_ptr;
		};
		struct _jmp_buf
		{
			std::jmp_buf & get_env()
			{
				return _env;
			}
		
			std::jmp_buf _env;
		};

		using _jmp_var_stack = std::stack<std::stack<_jmp_var>>;
	public:
		template <class Type>
		void * push_var()
		{
			return _var_stacks.top().top().emplace(std::make_unique<_jmp_var_obj<Type>>()).get_obj();
		}

		void push_stack()
		{
			_var_stacks.top().emplace();
		}
		void pop_stack()
		{
			auto & stack = _var_stacks.top().top();
			while (!stack.empty())
			{
				stack.pop();
			}
			_var_stacks.top().pop();
		}

		std::jmp_buf & push_buf()
		{
			_var_stacks.emplace();
			push_stack();
			return _buf_stack.emplace().get_env();
		}
		void pop_buf()
		{
			auto & stack = _var_stacks.top();
			while (!stack.empty())
			{
				pop_stack();
			}
			_var_stacks.pop();
			_buf_stack.pop();
		}
		std::jmp_buf & get_buf()
		{
			return _buf_stack.top().get_env();
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
		std::stack<_jmp_var_stack> _var_stacks;
		std::stack<_jmp_buf> _buf_stack;
		int _status{ 0 };
	};

	template <class FuncType, class ... ArgTypes>
	void daemon(FuncType func, ArgTypes && ... args) // TODO: register daemon in global table
	{
		std::thread(func, std::forward<ArgTypes>(args) ...).detach();
	}
}

namespace stdx::this_thread
{
	inline auto & jmp_state()
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
