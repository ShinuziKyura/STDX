#define FUNCTION(function) \
int function();\
int(&main_function)() = function;

FUNCTION(example_9);

int main()
{
	return main_function();
}