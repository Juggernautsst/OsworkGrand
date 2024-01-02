#include "MyFT.cpp"
#include "MyFt.h"

// 主函数，测试代码
int main() {
    Memory* memory = new Memory(MEMORY_SIZE); // 创建内存对象
    string algorithm; 
    cout << "请输入替换算法名称 (FIFO or LRU): " << endl; 
    cin >> algorithm; 
    run_jobs(PROCESS_NUM, memory, algorithm); 
    delete memory; // 释放内存
    return 0;
}