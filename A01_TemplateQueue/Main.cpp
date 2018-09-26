#include "Main.h"
template <typename T>
void Print(std::deque<T> a_queue)
{
	std::cout << "Begining of queue......." << std::endl;
	uint count = a_queue.size();
	for (uint i = 0; i < count; ++i)
	{
		std::cout << a_queue[i];
		std::cout << std::endl;
	}
	std::cout << "End of queue............" << std::endl;
}


//int main(void)
//{
//	std::deque<int> myqueue;
//	//push elements
//	myqueue.push_back(0);
//	std::cout << "	just pushed +++++" << std::endl;
//	myqueue.push_back(1);
//	std::cout << "	just pushed +++++" << std::endl;
//	myqueue.push_back(2);
//	std::cout << "	just pushed +++++" << std::endl;
//	//print queue
//	print<int>(myqueue);
//	//pop an element
//	myqueue.pop_front();
//	std::cout << "		just popped -----" << std::endl;
//	//add elements
//	myqueue.push_back(3);
//	std::cout << "	just pushed +++++" << std::endl;
//	myqueue.push_back(4);
//	std::cout << "	just pushed +++++" << std::endl;
//	myqueue.push_back(5);
//	std::cout << "	just pushed +++++" << std::endl;
//	//print queue
//	print<int>(myqueue);
//	myqueue.pop_front();
//	std::cout << "		just popped -----" << std::endl;
//	myqueue.pop_front();
//	std::cout << "		just popped -----" << std::endl;
//	//print queue
//	print<int>(myqueue);
//	//clear queue
//	uint count = myqueue.size();
//	for (uint i = 0; i < count; ++i)
//	{
//		myqueue.pop_front();
//		std::cout << "		just popped -----" << std::endl;
//	}
//	//print queue
//	print<int>(myqueue);
//
//	//complex data usage
//	std::deque<foo> myfooqueue;
//	for (uint i = 0; i < 10; ++i)
//	{
//		foo foo(i);
//		myfooqueue.push_back(foo);
//		std::cout << "	just pushed +++++" << std::endl;
//		if (i % 2 == 0)
//		{
//			myfooqueue.pop_front();
//			std::cout << "		just popped -----" << std::endl;
//		}
//	}
//	print<foo>(myfooqueue);
//	std::cout << "press enter to finish." << std::endl;
//	getchar();
//}