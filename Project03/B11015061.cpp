#include "lib.h"
#include "manager.h"

int main(int argc, char* argv[])
{
	Manager manager;
	manager.ReadFile("two_level_blif/e64.blif");
	manager.Simplify();
	manager.WriteFile();
}