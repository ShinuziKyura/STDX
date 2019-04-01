#include <iostream>
#include <string>

#include "event.hpp"
#include "utility.hpp"

// Any type we want to use to handle events must have 'STDX_EVENT_HANDLER' as a base
struct base_type : STDX_EVENT_HANDLER
{
	base_type(int t)
		: _t(t)
	{
	}

	int function(int i, std::string const & str) const &
	{
		std::cout << _t << " " << i << "\t" << str << "\n";
		return 0;
	}

	int _t;
};

// Redundant base, will have no effect
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

/*	Or alternatively, using the copy/move-and-swap idiom
 *	derived_type & operator=(derived_type other)
 *	{
 *		return *this;
 *	}
 */
};

int example_9()
{
	derived_type const e(4);

	{
		// Instantiation must be made with void-returning function type
		stdx::event::event_dispatcher<void(int, std::string const &)> test_event;

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
						// The only requirement for the actual functions is that the parameter types match
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

		test_event.bind(&e, &base_type::function);
		test_event.unbind(static_cast<base_type const *>(&e));

		// No one will receive this event
		test_event.broadcast(4, "Wait for me! :(");

		test_event.bind(static_cast<base_type const *>(&e), &base_type::function);
		// Event dispatcher destructor will also make sure any handler which bound itself to this event is informed that this object is no longer valid
	}

	stdx::await_input();

	return 0;
}