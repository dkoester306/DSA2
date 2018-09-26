#pragma once
using namespace std;


#include <iostream>

template <class T>
class Queue
{
	int count;					// the current number of elements in the queue.
	static const int START_SIZE = 10;		// the starting size of the queue
	int capacity;
	int front = 0;
	int rear = -1;
	T *queue = nullptr;					// pointer to the queue

public:

	Queue();	
	Queue(int size);

#pragma region Rule of Three
	// copy assignment operator
	template <class T>
	Queue<T>& operator=(const Queue<T>& other)
	{
		count = other.count;
		capacity = other.capacity;
		front = other.front;
		rear = other.rear;

		//T* oldQueue = queue;
		
		if (queue != nullptr)
		{
			delete[] queue;
			queue = nullptr;
		}
		queue = new T[capacity];

		for (int i = 0; i<count; i++)
		{
			queue[i] = other.queue[i];
		}

		//delete[] oldQueue;
		return *this;
	}

	// copy constructor
	Queue(const Queue& other)
	{
		count = other.count;
		capacity = other.capacity;
		front = other.front;
		rear = other.rear;
		
		queue = new T[capacity];

		for (int i = 0; i < count; i++)
		{
			queue[i] = other.queue[i];
		}
	}

	// destructor
	~Queue();
#pragma endregion

	void Pop();
	void Push(T);
	void Print();
	int GetSize();
	bool IsEmpty();
	bool IsFull();
};




template <class T>
Queue<T>::Queue()
{
	queue = new T[START_SIZE];
	capacity = START_SIZE;
}

template <class T>
Queue<T>::Queue(int size)
{
	queue = new T[size];
	capacity = size;

}

template <class T>
Queue<T>::~Queue()
{
	if (queue != nullptr)
	{
		delete[] queue;
		queue = nullptr;
	}
}

template <class T>
void Queue<T>::Push(T element)
{
	if (IsFull())
	{
		T* oldQueue = queue;
		queue = new T[2 * count];

		for(int i=0;i<count;i++)
		{ 
			queue[i] = oldQueue[i];
		}
		capacity *= 2;

		delete[] oldQueue;
	
	}
	// add the element to the queue
	//rear = (rear + 1) % capacity;		// makes sure that 
	rear++;
	queue[rear] = element;

	// increase the count
	count++;

}

template <class T>
void Queue<T>::Pop()
{
	// check if there is enough space
	if (IsEmpty())
	{
		std::cout << "There is no element to pop.";
		return;
	}

	// store the last element of the array
	T temp = queue[rear];

	// remove the front element
	for (int i = 0; i < count - 1; i++)
	{
		queue[i] = queue[i + 1];
	}

	
	rear--;
	// reduce the count
	count--;
	// empty the last index 
	queue[count] = T();
}

template <class T>
void Queue<T>::Print()
{
	for (int i = 0; i < count; i++)
	{
		cout << "Index " << i << ": " << queue[i]<<"\n";
	}
	cout << "\n";
}

template <class T>
int Queue<T>::GetSize()
{
	return count;
}

// Checks if the array is full
template <class T>
bool Queue<T>::IsFull()
{
	return (GetSize() == capacity);
}

// checks if the count of the array is 0
template <class T>
bool Queue<T>::IsEmpty()
{

	return (GetSize() == 0);
}
