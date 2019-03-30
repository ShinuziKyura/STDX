#include <iostream>

#include "event.hpp"
#include "utility.hpp"

// Any type we want to use to handle events must have 'STDX_EVENT_HANDLER as a base
struct test_type : STDX_EVENT_HANDLER
{
	test_type(int t)
		: _t(t)
	{
	}

	int function(int i, std::string const & str) const &
	{
		std::cout << _t << " " << i << " " << str << "\n";
		return 0;
	}

	int _t;
};

int example_9()
{
	test_type e(4);

	{
		// Instantiation must be with void-returning function type
		stdx::event::event_dispatcher<void(int, std::string const &)> test_event;

		{
			test_type a(3);
			test_event.bind(&a, &test_type::function);

			{
				test_type b(2);
				test_event.bind(&b, &test_type::function);

				{
					test_type c(1);
					test_event.bind(&c, &test_type::function);

					{
						test_type d(0);
						// The only requirement for the actual functions is that the parameter types match
						test_event.bind(&d, &test_type::function);

						test_event.broadcast(0, "H"); 
						// Event handler destructor will make sure any dispatcher which is bound to is informed that this object is no longer valid
					}

					test_event.broadcast(1, "e");
				}

				test_event.broadcast(2, "y");
			}

			test_event.broadcast(3, "a!");
		}

		// No one will receive this event
		test_event.broadcast(4, "Wait for me! :(");

		test_event.bind(&e, &test_type::function);
		// Event dispatcher destructor will also make sure any handler which bound itself to this event is informed that this object is no longer valid
	}

	stdx::await_input();

	return 0;
}