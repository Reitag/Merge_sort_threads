#include <iostream>
#include <future>
#include <chrono>
#include <random>
#include <mutex>

int count = 0;
bool make_thread = true;
std::mutex mutex;

void Merge(int* arr, int l, int m, int r) 
{
	int nl = m - l + 1;
	int nr = r - m;

	int* left = new int[nl];
	int* right = new int[nr];

	for (int i = 0; i < nl; ++i)
	{
		left[i] = arr[l + i];
	}
	for (int j = 0; j < nr; ++j)
	{
		right[j] = arr[m + j + 1];
	}

	int i = 0, j = 0;
	int k = l;

	while (i < nl && j < nr)
	{
		if (left[i] < right[j])
		{
			arr[k] = left[i];
			++i;
		}
		else
		{
			arr[k] = right[j];
			++j;
		}
		++k;
	}

	while (i < nl)
	{
		arr[k] = left[i];
		++i;
		++k;
	}
	while (j < nr)
	{
		arr[k] = right[j];
		++j;
		++k;
	}

	delete[] left;
	delete[] right;
}

void MergeSort(int* arr, int l, int r) 
{
	if (l >= r)
	{
		return;
	}

	int m = (l + r - 1) / 2;

	if (make_thread == true)
	{
		if (count < 5)
		{
			mutex.lock();
			++count;
			mutex.unlock();
			std::future<void> f = std::async(std::launch::async, [&]()
				{
					MergeSort(arr, l, m);
				});
			MergeSort(arr, m + 1, r);
			f.wait();
		}
		else
		{
			MergeSort(arr, l, m);
			MergeSort(arr, m + 1, r);
		}
	}
	else
	{
		MergeSort(arr, l, m);
		MergeSort(arr, m + 1, r);
	}
	
	Merge(arr, l, m, r);
}


int main()
{
	srand(0);
	long arr_size = 1000000;
	int* arr = new int[arr_size];
	for (long i = 0; i < arr_size; i++) 
	{
		arr[i] = rand() % 500000;
	}
	
	time_t start, end;
	double seconds;

	time(&start);
	MergeSort(arr, 0, arr_size - 1);
	time(&end);
	seconds = difftime(end, start);
	printf("Time with threads is: %f seconds\n", seconds);

	make_thread = false;

	time(&start);
	MergeSort(arr, 0, arr_size - 1);
	time(&end);
	seconds = difftime(end, start);
	printf("Time without threads is: %f seconds\n", seconds);

	return 0;
}
