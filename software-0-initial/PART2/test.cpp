#include <cstring>
#include <cmath>
#include <vector>
#include <cassert>
#include <iostream>
#include <sstream>
#include <thread>
#include "threadpool_test.cpp"


int main(int argc, char *argv[])
{
    ThreadPool pool;
    pool.Start();
    return 0;
}
