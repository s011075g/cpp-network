#include <iostream>
#include <thread>

void Thread1()
{
	for (auto i = 1; i <= 10; i++)
		std::cout << "THREAD 1: " << i << std::endl;
}

void Thread2()
{
	for (auto i = 1; i <= 10; i++)
		std::cout << "THREAD 2: " << i << std::endl;
}
int main()
{
	std::thread thread(Thread1);
	std::thread thread2(Thread2);
	if (thread.joinable())
		thread.join();
	if (thread2.joinable())
		thread2.join();
	std::cin.get();
	return 0;
}