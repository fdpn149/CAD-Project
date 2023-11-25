#include "lib.h"
#include "manager.h"

int main(int argc, char* argv[])
{
	Manager manager;
	manager.ReadFile(argv[1]);
	//manager.ReadFile("two_level_blif/sample.blif");
	manager.Simplify();
	manager.WriteFile();
}