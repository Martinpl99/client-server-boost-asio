#pragma once
#include<mutex>
namespace BoostASIO
{
	class OwnMath
	{
	public:
		void Add(int a, int b) noexcept;
		void Substract(int a, int b) noexcept;

	private:
		std::mutex mtx;
	};
}
