// Polysim.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include<iostream>
#include <vector>
#include "Driver.h"
using namespace std;



int main(int argc, const char * argv[])
{


	Driver driver(argv[1], argv[2]);

	driver.processRecord();

    return 0;
}
