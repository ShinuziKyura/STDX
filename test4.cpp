#include <iostream>

#include "binder.hpp"

class MyClass
{
public:
	int i;

	MyClass() : i(0)
	{
	};
	MyClass(MyClass const & obj) : i(obj.i)
	{
		std::cout << "\t\tCopy\n";
	}
	MyClass(MyClass && obj) : i(std::move(obj.i))
	{
		std::cout << "\t\tMove\n";
	}
};
using MyClassC = MyClass const;
using MyClassL = MyClass &;
using MyClassLC = MyClass const &;
using MyClassR = MyClass &&;
using MyClassRC = MyClass const &&;

int MyMethod(MyClass object)
{
	return object.i += 1;
}
int MyMethodC(MyClass const object)
{
	return object.i + 1;
}
int MyMethodL(MyClass & object)
{
	return object.i += 1;
}
int MyMethodLC(MyClass const & object)
{
	return object.i + 1;
}
int MyMethodR(MyClass && object)
{
	return object.i += 1;
}
int MyMethodRC(MyClass const && object)
{
	return object.i + 1;
}

int main4()
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

	std::bind(MyMethod, stdx::bind_forward<MyClass>(MyClass()))(); // Move
//	std::bind(MyMethodL, stdx::bind_forward<MyClass>(MyClass()))(); // Error
	std::bind(MyMethodR, stdx::bind_forward<MyClass>(MyClass()))();

	std::bind(MyMethod, stdx::bind_forward<MyClassC>(MyClassC()))(); // Copy
//	std::bind(MyMethodL, stdx::bind_forward<MyClassC>(MyClassC()))(); // Error
//	std::bind(MyMethodR, stdx::bind_forward<MyClassC>(MyClassC()))(); // Error

	std::bind(MyMethodC, stdx::bind_forward<MyClassC>(MyClassC()))(); // Copy
	std::bind(MyMethodLC, stdx::bind_forward<MyClassC>(MyClassC()))();
	std::bind(MyMethodRC, stdx::bind_forward<MyClassC>(MyClassC()))();
	
	std::cout << "\tLvalue\n";

	std::bind(MyMethod, stdx::bind_forward<MyClassL>(MyObject))(); // Copy
	std::bind(MyMethodL, stdx::bind_forward<MyClassL>(MyObject))(); // Modifies MyObject
//	std::bind(MyMethodR, stdx::bind_forward<MyClassL>(MyObject))(); // Error

	std::bind(MyMethod, stdx::bind_forward<MyClassLC>(MyObjectC))(); // Copy
//	std::bind(MyMethodL, stdx::bind_forward<MyClassLC>(MyObjectC))(); // Error
//	std::bind(MyMethodR, stdx::bind_forward<MyClassLC>(MyObjectC))(); // Error

	std::bind(MyMethodC, stdx::bind_forward<MyClassLC>(MyObjectC))(); // Copy
	std::bind(MyMethodLC, stdx::bind_forward<MyClassLC>(MyObjectC))();
//	std::bind(MyMethodRC, stdx::bind_forward<MyClassLC>(MyObjectC))(); // Error

	std::cout << "\t\t\ti: " << MyObject.i << "\n\tXvalue\n";

	std::bind(MyMethod, stdx::bind_forward<MyClassR>(std::move(MyObject)))(); // Move
//	std::bind(MyMethodL, stdx::bind_forward<MyClassR>(std::move(MyObject)))(); // Error
	std::bind(MyMethodR, stdx::bind_forward<MyClassR>(std::move(MyObject)))(); // Modifies MyObject
	
	std::bind(MyMethod, stdx::bind_forward<MyClassRC>(std::move(MyObjectC)))(); // Copy
//	std::bind(MyMethodL, stdx::bind_forward<MyClassRC>(std::move(MyObjectC)))(); // Error
//	std::bind(MyMethodR, stdx::bind_forward<MyClassRC>(std::move(MyObjectC)))(); // Error

	std::bind(MyMethodC, stdx::bind_forward<MyClassRC>(std::move(MyObjectC)))(); // Copy
	std::bind(MyMethodLC, stdx::bind_forward<MyClassRC>(std::move(MyObjectC)))();
	std::bind(MyMethodRC, stdx::bind_forward<MyClassRC>(std::move(MyObjectC)))();

	std::cout << "\t\t\ti: " << MyObject.i << "\nEnd" << std::endl;
	
	return 0;
}
