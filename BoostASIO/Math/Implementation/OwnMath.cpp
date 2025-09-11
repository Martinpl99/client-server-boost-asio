#include "../Headers/OwnMath.h"
#include <iostream>

namespace BoostASIO
{
	void OwnMath::Add(int a, int b) noexcept
	{
		std::lock_guard<std::mutex> lock(mtx);
		std::cout << a + b << std::endl;;
	}

	void OwnMath::Substract(int a, int b)  noexcept
	{
		std::lock_guard<std::mutex> lock(mtx);
		std::cout << a - b << std::endl;
	}
}