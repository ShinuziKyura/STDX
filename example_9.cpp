#include <iostream>
#include <string>

#include "utility.hpp"
#include "meta.hpp"
#include "event.hpp"

// Any type we want to use to handle events must have 'STDX_EVENT_HANDLER' as a base
struct base_type : STDX_EVENT_HANDLER
{
	base_type(int t)
		: _t(t)
	{
	}

	int function(int i, std::string const & str) &
	{
		std::cout << _t << " " << i << "\t" << str << "\n";
		return 0;
	}

	void foobar(int i, std::string const & str) const
	{
		if (i > 3)
		{
			std::cout << _t << " " << i << "\t" << str << "\n";
		}
	}

	int _t;
};

// Redundant STDX_EVENT_HANDLER, base_type already has it as a base
struct derived_type : base_type, STDX_EVENT_HANDLER
{
	derived_type(int t)
		: base_type(t)
	{
	}

	derived_type(derived_type const & other)
		: base_type(other)
		, STDX_EVENT_HANDLER_COPY_CONSTRUCTOR(other)
	{
	}

	derived_type(derived_type && other) noexcept
		: base_type(other)
		, STDX_EVENT_HANDLER_MOVE_CONSTRUCTOR(other)
	{
	}

	derived_type & operator=(derived_type const & other)
	{
		STDX_EVENT_HANDLER_COPY_ASSIGNMENT(other);
		return *this;
	}

	derived_type & operator=(derived_type && other) noexcept
	{
		STDX_EVENT_HANDLER_MOVE_ASSIGNMENT(other);
		return *this;
	}

//	Alternatively, using the copy/move-and-swap idiom
/*	derived_type & operator=(derived_type other)
	{
		return *this;
	}
 */
};

int example_9()
{
	derived_type const e(4);
	stdx::event_future<void> result;
	stdx::event_future<void> result_empty;
	{
		STDX_DEFINE_EVENT_DISPATCHER(event_type, int, std::string const &);
		event_type test_event;

		// The only requirement for the actual functions is that the parameter types match
		result = test_event.bind(&e, &base_type::foobar);
		stdx::event_future result_copy_1 = result;
		stdx::event_future result_copy_2 = result;
		stdx::event_future result_move = std::move(result_copy_2);
		
		// Calling any function other than valid() and constructors/destructor for a event_future where valid() == false is undefined behaviour
		std::cout << std::boolalpha;
		std::cout << "result:\t" << result.valid() << "\n";
		std::cout << "empty:\t" << result_empty.valid() << "\n";
		std::cout << "copy 1:\t" << result_copy_1.valid() << "\n";
		std::cout << "copy 2:\t" << result_copy_2.valid() << "\n";
		std::cout << "move:\t" << result_move.valid() << "\n\n";

		{
			base_type a(3);
			test_event.bind(&a, &base_type::function);
			
			{
				base_type b(2);
				test_event.bind(&b, &base_type::function);
				
				{
					base_type c(1);
					test_event.bind(&c, &base_type::function);

					{
						base_type d(0);
						test_event.bind(&d, &base_type::function);

						test_event.broadcast(0, "H"); 
						// Event handler destructor will make sure any dispatcher which is bound to is informed that this object is no longer valid
					}

					test_event.broadcast(1, "e");
				}

				test_event.broadcast(2, "y");
			}

			test_event.broadcast(3, "a\n\t!");
		}

		test_event.unbind(&e);
		std::cout << "\nresult after unbind:\t" << result.valid() << "\n";

		// No one will receive this event
		test_event.broadcast(4, "Wait for me! :(");

		result = test_event.bind(&e, &base_type::foobar);
		std::cout << "result after rebind:\t" << result.valid() << "\n";
		// Event dispatcher destructor will also make sure any handler which bound itself to this event is informed that this object is no longer valid
	}

	std::cout << "result after scope:\t" << result.valid() << "\n";

	stdx::await_input();

	return 0;
}