#pragma once

#include <iostream>
#include <vector>
#include <random>
#include <thread>
#include <chrono>
#include <fstream>
#include <string>
#include <unordered_map>
#include <cmath>
#include <mutex>
#include <queue>
using namespace std;

// 定义常量
const int MEMORY_SIZE = 1 << 14; // 模拟内存的大小，2^14 字节
const int PAGE_SIZE = 256; // 系统的页面大小，256 字节
const int PAGE_ENTRY_SIZE = 4; // 每个页表项的大小，4 字节
const int PHYSICAL_PAGE_NUM = 1 << 6; // 系统的物理页面数，2^6
const int VIRTUAL_PAGE_NUM = 1 << 8; // 每个进程的虚拟页面数，2^8
const int PROCESS_PAGE_NUM = 9; // 每个进程分配的页面数，9
const int PROCESS_NUM = 12; // 进程的数量，12
const int ACCESS_NUM = 200; // 每个进程的访问次数，200
const int MAX_SLEEP_TIME = 100; // 每次访问后的最大休眠时间，100 ms
const string FILE_PREFIX = "file_"; // 文件的前缀，file_
const string FILE_SUFFIX = ".txt"; // 文件的后缀，.txt






// // 定义进程结构
// struct Process {
//     int id;
//     std::vector<int> pages;
// };


// // 定义内存
// std::vector<int> memory(PAGE_NUM, -1);
// int free_page_count = PAGE_NUM;