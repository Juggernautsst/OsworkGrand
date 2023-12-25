#include "MyFT.cpp"
#include "MyFt.h"

// 主函数，测试代码
int main() {
    Memory* memory = new Memory(MEMORY_SIZE); // 创建内存对象
    string algorithm; // 定义页面置换算法
    cout << "Please enter the page replacement algorithm (FIFO or LRU): " << endl; 
    cin >> algorithm; 
    run_jobs(PROCESS_NUM, memory, algorithm); 
    delete memory; // 释放内存
    return 0;
}