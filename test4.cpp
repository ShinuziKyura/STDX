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

int main()
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

	stdx::binder::bind(MyMethod, std::forward<MyClass>(MyClass()))(); // Move
//	stdx::binder::bind(MyMethodL, std::forward<MyClass>(MyClass()))(); // Error
	stdx::binder::bind(MyMethodR, std::forward<MyClass>(MyClass()))();

	stdx::binder::bind(MyMethod, std::forward<MyClassC>(MyClassC()))(); // Copy
//	stdx::binder::bind(MyMethodL, std::forward<MyClassC>(MyClassC()))(); // Error
//	stdx::binder::bind(MyMethodR, std::forward<MyClassC>(MyClassC()))(); // Error

	stdx::binder::bind(MyMethodC, std::forward<MyClassC>(MyClassC()))(); // Copy
	stdx::binder::bind(MyMethodLC, std::forward<MyClassC>(MyClassC()))();
	stdx::binder::bind(MyMethodRC, std::forward<MyClassC>(MyClassC()))();
	
	std::cout << "\tLvalue\n";

	stdx::binder::bind(MyMethod, std::forward<MyClassL>(MyObject))(); // Copy
	stdx::binder::bind(MyMethodL, std::forward<MyClassL>(MyObject))(); // Modifies MyObject
//	stdx::binder::bind(MyMethodR, std::forward<MyClassL>(MyObject))(); // Error

	stdx::binder::bind(MyMethod, std::forward<MyClassLC>(MyObjectC))(); // Copy
//	stdx::binder::bind(MyMethodL, std::forward<MyClassLC>(MyObjectC))(); // Error
//	stdx::binder::bind(MyMethodR, std::forward<MyClassLC>(MyObjectC))(); // Error

	stdx::binder::bind(MyMethodC, std::forward<MyClassLC>(MyObjectC))(); // Copy
	stdx::binder::bind(MyMethodLC, std::forward<MyClassLC>(MyObjectC))();
//	stdx::binder::bind(MyMethodRC, std::forward<MyClassLC>(MyObjectC))(); // Error

	std::cout << "\t\t\ti: " << MyObject.i << "\n\tXvalue\n";

	stdx::binder::bind(MyMethod, std::forward<MyClassR>(std::move(MyObject)))(); // Move
//	stdx::binder::bind(MyMethodL, std::forward<MyClassR>(std::move(MyObject)))(); // Error
	stdx::binder::bind(MyMethodR, std::forward<MyClassR>(std::move(MyObject)))(); // Modifies MyObject
	
	stdx::binder::bind(MyMethod, std::forward<MyClassRC>(std::move(MyObjectC)))(); // Copy
//	stdx::binder::bind(MyMethodL, std::forward<MyClassRC>(std::move(MyObjectC)))(); // Error
//	stdx::binder::bind(MyMethodR, std::forward<MyClassRC>(std::move(MyObjectC)))(); // Error

	stdx::binder::bind(MyMethodC, std::forward<MyClassRC>(std::move(MyObjectC)))(); // Copy
	stdx::binder::bind(MyMethodLC, std::forward<MyClassRC>(std::move(MyObjectC)))();
	stdx::binder::bind(MyMethodRC, std::forward<MyClassRC>(std::move(MyObjectC)))();

	std::cout << "\t\t\ti: " << MyObject.i << "\nEnd" << std::endl;
	
	return 0;
}
