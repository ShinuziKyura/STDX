#include <iostream>

#include "functional.hpp"
#include "utility.hpp"

class MyClass
{
public:
	int i;
	bool print = true;

	MyClass() : i(0)
	{
	};
	MyClass(MyClass const & other) : i(other.i)
	{
		std::cout << "\t\tCopy\n";
		print = false;
	}
	MyClass(MyClass && other) : i(std::move(other.i))
	{
		std::cout << "\t\tMove\n";
		print = false;
	}

	MyClass& operator=(MyClass other)
	{
		std::swap(*this, other);
		return *this;
	}

	void operator()(){}
//	void operator()(int){}
	void func(int){}
	int obj;
};
using MyClassC = MyClass const;
using MyClassL = MyClass &;
using MyClassLC = MyClass const &;
using MyClassR = MyClass &&;
using MyClassRC = MyClass const &&;

int MyMethod(MyClass object)
{
	if (object.print)
	{
		std::cout << "\t\t----\n";
	}
	return object.i += 1;
}
int MyMethodC(MyClass const object)
{
	if (object.print)
	{
		std::cout << "\t\t----\n";
	}
	return object.i + 1;
}
int MyMethodL(MyClass & object)
{
	if (object.print)
	{
		std::cout << "\t\t----\n";
	}
	return object.i += 1;
}
int MyMethodLC(MyClass const & object)
{
	if (object.print)
	{
		std::cout << "\t\t----\n";
	}
	return object.i + 1;
}
int MyMethodR(MyClass && object)
{
	if (object.print)
	{
		std::cout << "\t\t----\n";
	}
	return object.i += 1;
}
int MyMethodRC(MyClass const && object)
{
	if (object.print)
	{
		std::cout << "\t\t----\n";
	}
	return object.i + 1;
}

int example_4()
{
	MyClass MyObject;
	MyClassC MyObjectC;

	std::cout << "Call\n\tPRvalue\n";
	
	MyMethod(MyClass()); // Copy Elided
//	MyMethodL(MyClass()); // Error
	MyMethodR(MyClass());

	MyMethod(MyClassC()); // Copy Elided
//	MyMethodL(MyClassC()))); // Error
//	MyMethodR(MyClassC()))); // Error

	MyMethodC(MyClassC()); // Copy Elided
	MyMethodLC(MyClassC());
	MyMethodRC(MyClassC());

	std::cout << "\tLvalue\n";

	MyMethod(MyObject); // Copy
	MyMethodL(MyObject); // Modifies MyObject
//	MyMethodR(MyObject); // Error

	MyMethod(MyObjectC); // Copy
//	MyMethodL(MyObjectC); // Error
//	MyMethodR(MyObjectC); // Error

	MyMethodC(MyObjectC); // Copy
	MyMethodLC(MyObjectC);
//	MyMethodRC(MyObjectC); // Error

	std::cout << "\t\t\ti: " << MyObject.i << "\n\tXvalue\n";

	MyMethod(std::move(MyObject)); // Move
//	MyMethodL(std::move(MyObject)); // Error
	MyMethodR(std::move(MyObject)); // Modifies MyObject

	MyMethod(std::move(MyObjectC)); // Copy
//	MyMethodL(std::move(MyObjectC)); // Error
//	MyMethodR(std::move(MyObjectC)); // Error

	MyMethodC(std::move(MyObjectC)); // Copy
	MyMethodLC(std::move(MyObjectC));
	MyMethodRC(std::move(MyObjectC));

	std::cout << "\t\t\ti: " << MyObject.i << "\nBind\n\tPRvalue\n";

	stdx::functional::bind(MyMethod, MyClass())(); // Move
//	stdx::functional::bind(MyMethodL, std::forward<MyClass>(MyClass()))(); // Error
	stdx::functional::bind(MyMethodR, MyClass())();

	stdx::functional::bind(MyMethod, MyClassC())(); // Copy
//	stdx::functional::bind(MyMethodL, std::forward<MyClassC>(MyClassC()))(); // Error
//	stdx::functional::bind(MyMethodR, MyClassC())(); // Error

	stdx::functional::bind(MyMethodC, MyClassC())(); // Copy
	stdx::functional::bind(MyMethodLC, MyClassC())();
	stdx::functional::bind(MyMethodRC, MyClassC())();
	
	std::cout << "\tLvalue\n";

	stdx::functional::bind(MyMethod, MyObject)(); // Copy
	stdx::functional::bind(MyMethodL, MyObject)(); // Modifies MyObject
//	stdx::functional::bind(MyMethodR, MyObject)(); // Error

	stdx::functional::bind(MyMethod, MyObjectC)(); // Copy
//	stdx::functional::bind(MyMethodL, MyObjectC)(); // Error
//	stdx::functional::bind(MyMethodR, MyObjectC)(); // Error

	stdx::functional::bind(MyMethodC, MyObjectC)(); // Copy
	stdx::functional::bind(MyMethodLC, MyObjectC)();
//	stdx::functional::bind(MyMethodRC, MyObjectC)(); // Error

	std::cout << "\t\t\ti: " << MyObject.i << "\n\tXvalue\n";

	stdx::functional::bind(MyMethod, std::move(MyObject))(); // Move
//	stdx::functional::bind(MyMethodL, std::move(MyObject))(); // Error
	stdx::functional::bind(MyMethodR, std::move(MyObject))(); // Modifies MyObject
	
	stdx::functional::bind(MyMethod, std::move(MyObjectC))(); // Copy
//	stdx::functional::bind(MyMethodL, std::move(MyObjectC))(); // Error
//	stdx::functional::bind(MyMethodR, std::move(MyObjectC))(); // Error

	stdx::functional::bind(MyMethodC, std::move(MyObjectC))(); // Copy
	stdx::functional::bind(MyMethodLC, std::move(MyObjectC))();
	stdx::functional::bind(MyMethodRC, std::move(MyObjectC))();

	std::cout << "\t\t\ti: " << MyObject.i << "\nEnd" << std::endl;

	stdx::await_input();
	
	return 0;
}
