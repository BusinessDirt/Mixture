#include "Core.h"

#include <iostream>

namespace Core {

	void PrintHelloWorld()
	{
#ifdef DEBUG
		std::cout << "Running in Debug Configuration!\n";
#endif

#ifdef RELEASE
		std::cout << "Running in Release Configuration!\n";
#endif

#ifdef DIST
		std::cout << "Running in Dist Configuration!\n";
#endif

#ifdef WINDOWS
		std::cout << "Hello World from Windows!\n";
		std::cin.get();
#endif

#ifdef LINUX
		std::cout << "Hello World from Linux!\n";
		std::cin.get();
#endif

#ifdef MACOSX
		std::cout << "Hello World from MacOSX!\n";
		std::cin.get();
#endif
		
	}

}