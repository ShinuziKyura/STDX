#define INVOKE(function) \
int function();\
return function()

int main()
{
	INVOKE(example_4);
}