#include "Queue.h"

int main(void)
{
	// Pushing 
	Queue<int> intQueue(3);

	// Currently have a memory leak somewhere at the time of sumbitting. Just a heads up!
	intQueue.Push(10);
	intQueue.Push(30);
	intQueue.Push(35);

	// Print Queue
	intQueue.Print();

	// Pop element
	intQueue.Pop();

	// Push More
	intQueue.Push(100);
	intQueue.Push(200);
	intQueue.Push(5);

	// Print Queue
	intQueue.Print();
	getchar();
	return 0;
}