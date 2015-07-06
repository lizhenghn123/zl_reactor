#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include "base/Timestamp.h"
using namespace zl::base;

void test_usage()
{
	Timestamp now(Timestamp::now());
	printf("Time : %s\n", now.toString().c_str());
	
	now += 3;
	printf("Time : %s\n", now.toString().c_str());

	tm t = now.getTm();
	printf("%s\n", asctime(&t));
}

void test_bench1()
{
	const int max_count = 1000*1000;

	std::vector<Timestamp> stamps;
	stamps.reserve(max_count);
	for (int i = 0; i < max_count; ++i)
	{
		stamps.push_back(Timestamp::now());
	}
	
	printf("%s\n", stamps.front().toString().c_str());
	printf("%s\n", stamps.back().toString().c_str());
	printf("elapsed %f s\n", Timestamp::timeDiff(stamps.back(), stamps.front()));

	int increments[100] = { 0 };
	int64_t start = stamps.front().microSeconds();
	for (int i = 1; i < max_count; ++i)
	{
		int64_t next = stamps[i].microSeconds();
		int inc = next - start;
		start = next;
		if (inc < 0)
		{
			printf("reverse!\n");
		}
		else if (inc < 100)
		{
			++increments[inc];
		}
		else
		{
			printf("big gap %d\n", inc);
		}
	}
    
	for (int i = 0; i < 100; ++i)
	{
		printf("%2d: %d\n", i, increments[i]);
	}
}

double time_diff(struct timeval* high, struct timeval* low)
{
  return (high->tv_sec-low->tv_sec)+(high->tv_usec-low->tv_usec)/1000000.0;
}

void test_bench2(int flag = 0)
{
	const int max_count = 1000 * 10;
	
	const char* name = "unknown";

	struct timeval start;
	gettimeofday(&start, NULL);
	if (flag == 0)
	{
		name = "gettimeofday";
		struct timeval tv;
		for (int i = 0; i < max_count; ++i)
		{
			gettimeofday(&tv, NULL);
		}
	}
	else if (flag == 1)
	{
		name = "time";
		for (int i = 0; i < max_count; ++i)
		{
		    time(NULL);
		}
	}
	else if (flag == 2)
	{
		name = "clock_gettime";
		for (int i = 0; i < max_count; ++i)
		{
			struct timespec ts;
			clock_gettime(CLOCK_REALTIME, &ts);
		}
	}
	else
	{
		name = "getppid";
		for (int i = 0; i < max_count; ++i)
		{
		    getppid();
		}
	}

	struct timeval end;
	gettimeofday(&end, NULL);

	double d = time_diff(&end, &start);
	printf("%f seconds, %f micro seconds per %s\n", d, d * 1000000 / max_count, name);	
}

void passByValue(Timestamp t)
{
	printf("%s\n", t.toString().c_str());
}

void passByConstReference(const Timestamp& t)
{
	printf("%s\n", t.toString().c_str());
}

// 结论： 传值或传引用基本没有区别
void test_bench3(int flag = 0)
{
	const static int max_count = 1000 * 10;
	const static int max_loop  = 100;
	vector<int64_t> times;
	times.reserve(max_loop);
	
	Timestamp now(Timestamp::now());	
	
	if(flag == 0)
	{
		for(int i = 0 ; i < max_loop; i++)
		{
			Timestamp start(Timestamp::now());
			for(int i = 0; i < max_count; ++i)
			{
				passByValue(now);
			}
			Timestamp end(Timestamp::now());

			times.push_back(end - start);
		}
	}
	else
	{
		for(int i = 0 ; i < max_loop; i++)
		{
			Timestamp start(Timestamp::now());
			for(int i = 0; i < max_count; ++i)
			{
				passByConstReference(now);
			}
			Timestamp end(Timestamp::now());

			times.push_back(end - start);
		}
	}
	
	int sum = 0;
	for(int i = 0 ; i < max_loop; i++)
	{
		sum += times[i];
		printf("%" PRId64 " ", times[i]);
	}
	printf("\naverage : %f\n", sum * 1.0 / (max_loop*max_count));
}

int main(int argc, char* argv[])
{
	printf("##########################################\n");
	test_usage();
	
	printf("##########################################\n");
    test_bench1();

    if(argc < 2)
    {
        printf("exit....\n");
        return 0;
    }
    
	int flag = atoi(argv[1]);

	printf("##########################################\n");
    test_bench2(flag);
	
	printf("##########################################\n");
	//test_bench3(flag);
}
