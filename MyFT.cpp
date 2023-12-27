#include "MyFt.h"



// struct Process {
//     int id;
//     std::vector<int> pages;
// };

// 位图类
class BitMap {
private:
    vector<bool> bits; // 存储位图
    int free_count; // 空闲页面的数量
    mutex mtx; // 互斥锁，保证线程安全
public:
    // 构造函数，初始化位图
    BitMap(int size) {
        bits.resize(size, false); // 将所有位初始化为 false，表示空闲
        free_count = size; // 将空闲页面数量初始化为位图的大小
    }

    // 获取空闲页面的数量
    int get_free_count() {
        lock_guard<mutex> lock(mtx); // 上锁
        return free_count; // 返回空闲页面数量
    }

    // 分配一个空闲页面，返回页面号，如果没有空闲页面，返回 -1
    int allocate_page() {
        lock_guard<mutex> lock(mtx); // 上锁
        if (free_count == 0) { // 如果没有空闲页面，返回 -1
            return -1;
        }
        for (int i = 0; i < bits.size(); i++) { // 遍历位图
            if (!bits[i]) { // 找到第一个空闲的位
                bits[i] = true; // 将其置为 true，表示已分配
                free_count--; // 空闲页面数量减一
                return i; // 返回页面号
            }
        }
        return -1; // 不应该执行到这里，如果执行到这里，说明有错误
    }

    // 释放一个已分配的页面，传入页面号
    void free_page(int page) {
        lock_guard<mutex> lock(mtx); // 上锁
        if (page >= 0 && page < bits.size() && bits[page]) { // 如果页面号合法且已分配
            bits[page] = false; // 将其置为 false，表示空闲
            free_count++; // 空闲页面数量加一
        }
    }
};

// 页面类
class Page {
private:
    int job_id; // 作业号
    int page_id; // 页面号
public:
    Page(int job_id = 0, int page_id = 0) : job_id(job_id), page_id(page_id) {}

    // 获取作业号
    int get_job_id() const {
        return job_id;
    }

    // 获取页面号
    int get_page_id() const {
        return page_id;
    }

    // 重载输出
    friend std::ostream& operator<<(std::ostream& os, const Page& page) {
        os << "<" << page.job_id << ", " << page.page_id << ">";
        return os;
    }

    // 重载输入
    friend std::istream& operator>>(std::istream& is, Page& page) {
        is >> page.job_id >> page.page_id;
        return is;
    }
};

// 定义一个内存类，用来模拟内存空间
class Memory {
private:
    vector<Page> pages; // 用一个页面向量存储内存中的页面
    BitMap bitmap; // 用一个位图记录内存页面的分配状态
    mutex mtx; // 用一个互斥锁保证线程安全
public:
    // 构造函数，初始化内存空间
    Memory(int size) : bitmap(size / PAGE_SIZE) {
        pages.resize(size / PAGE_SIZE); // 将页面向量的大小初始化为内存大小除以页面大小
    }

    // 获取空闲页面的数量
    int get_free_count() {
        return bitmap.get_free_count(); // 调用位图的方法
    }

    // 分配一个空闲页面，返回页面号，如果没有空闲页面，返回 -1
    int allocate_page() {
        return bitmap.allocate_page(); // 调用位图的方法
    }

    // 释放一个已分配的页面，传入页面号
    void free_page(int page) {
        bitmap.free_page(page); // 调用位图的方法
    }

    // 读取一个页面的内容，传入页面号，返回页面对象
    Page read_page(int page) {
        lock_guard<mutex> lock(mtx); // 上锁
        if (page >= 0 && page < pages.size()) { // 如果页面号合法
            return pages[page]; // 返回页面对象
        }
        return Page(-1, -1); // 否则返回一个无效的页面对象
    }

    // 写入一个页面的内容，传入页面号和页面对象
    void write_page(int page, Page p) {
        lock_guard<mutex> lock(mtx); // 上锁
        if (page >= 0 && page < pages.size()) { // 如果页面号合法
            pages[page] = p; // 写入页面对象
        }
    }
};

// 定义一个文件类，用来模拟磁盘上的文件
// 定义一个文件类，用来模拟磁盘上的文件
class File {
private:
    string file_name; // 文件名
    vector<Page> pages; // 文件中的页面
public:
    // 构造函数，初始化文件
    File(int job_id) {
        file_name = FILE_PREFIX + to_string(job_id) + FILE_SUFFIX; // 根据作业号生成文件名
        pages.resize(VIRTUAL_PAGE_NUM); // 将页面向量的大小初始化为虚拟页面数
        for (int i = 0; i < VIRTUAL_PAGE_NUM; i++) { // 遍历页面向量
            pages[i] = Page(job_id, i); // 初始化每个页面的作业号和页面号
        }
        write_to_disk(); // 将文件写入磁盘
    }

    // 将文件写入磁盘
    void write_to_disk() {
        ofstream ofs(file_name); // 打开文件输出流
        if (ofs.is_open()) { // 如果打开成功
            for (int i = 0; i < pages.size(); i++) { // 遍历页面向量
                ofs << pages[i] << endl; // 将每个页面的信息写入文件
            }
            ofs.close(); // 关闭文件输出流
        }
    }

    // 从磁盘读取文件
    void read_from_disk() {
        ifstream ifs(file_name); // 打开文件输入流
        if (ifs.is_open()) { // 如果打开成功
            for (int i = 0; i < pages.size(); i++) { // 遍历页面向量
                ifs >> pages[i]; // 从文件中读取页面信息
            }//啊啊啊啊啊啊啊啊啊啊啊啊阿啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊
            /////////////////////都别活了
            ////////////啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊
            //////////怎么改啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊
            ifs.close(); // 关闭文件输入流
        }
    }

    // 读取一个页面的内容，传入页面号，返回页面对象
    Page read_page(int page) {
        if (page >= 0 && page < pages.size()) { // 如果页面号合法
            return pages[page]; // 返回页面对象
        }
        return Page(-1, -1); // 否则返回一个无效的页面对象
    }

    // 写入一个页面的内容，传入页面号和页面对象
    void write_page(int page, Page p) {
        if (page >= 0 && page < pages.size()) { // 如果页面号合法
            pages[page] = p; // 写入页面对象
            write_to_disk(); // 将文件写入磁盘
        }
    }
};

// 定义一个进程类，用来模拟进程的行为
// 定义一个进程类，用来模拟进程的行为
class Process {
private:
    int job_id; // 作业号
    int page_table_base; // 页表的基地址
    vector<int> page_table; // 页表
    vector<int> access_list; // 访问列表
    int page_faults; // 缺页中断次数
    Memory* memory; // 内存指针
    File* file; // 文件指针
    queue<int> fifo_queue; // FIFO 队列
    unordered_map<int, int> lru_map; // LRU 映射
    string algorithm; // 页面置换算法
public:
    // 构造函数，初始化进程
    Process(int job_id, Memory* memory, string algorithm) {
        this->job_id = job_id; // 初始化作业号
        this->memory = memory; // 初始化内存指针
        this->algorithm = algorithm; // 初始化页面置换算法
        file = new File(job_id); // 创建文件对象
        page_table.resize(VIRTUAL_PAGE_NUM, -1); // 将页表的大小初始化为虚拟页面数，每个元素初始化为 -1，表示无效
        page_faults = 0; // 将缺页中断次数初始化为 0
        generate_access_list(); // 生成访问列表
        allocate_memory(); // 分配内存
    }

    // 生成访问列表，根据概率分布随机生成访问的逻辑地址
    void generate_access_list() {
        random_device rd; // 随机设备
        mt19937 gen(rd()); // 随机数生成器
        discrete_distribution<> dist({0.5, 0.25, 0.125, 0.0625, 0.03125, 0.015625, 0.0078125, 0.00390625, 0.001953125}); // 离散分布，每个页面的访问概率正比于 1/(i+1)1/2
        for (int i = 0; i < ACCESS_NUM; i++) { // 生成 ACCESS_NUM 个逻辑地址
            int page = dist(gen); // 根据分布生成页面号
            int offset = rand() % PAGE_SIZE; // 随机生成偏移量
            int address = page * PAGE_SIZE + offset; // 计算逻辑地址
            access_list.push_back(address); // 将逻辑地址加入访问列表
        }
    }

  //写的什么乱七八糟的cjb？
  //没copilot自己看得懂？
    void allocate_memory() {
        while (memory->get_free_count() < PROCESS_PAGE_NUM + 1) { // 如果空闲页面数不足
            cout << "Job " << job_id << " is waiting for memory resources." << endl; // 输出等待信息
            this_thread::sleep_for(chrono::milliseconds(100)); // 休眠 100 ms
        }
        page_table_base = memory->allocate_page(); // 分配一个页面给页表
        for (int i = 0; i < PROCESS_PAGE_NUM; i++) { // 分配 PROCESS_PAGE_NUM 个页面给进程
            int page = memory->allocate_page(); // 分配一个页面
            page_table[i] = page; // 更新页表
            memory->write_page(page, file->read_page(i)); // 从文件中读取页面内容，写入内存
        }
        memory->write_page(page_table_base, Page(job_id, -1)); // 将页表的基地址写入内存
        cout << "Job " << job_id << " has been allocated " << PROCESS_PAGE_NUM + 1 << " pages." << endl; // 输出分配信息
    }

    // 释放内存，将进程占用的内存页面释放
    void free_memory() {
        memory->free_page(page_table_base); // 释放页表占用的页面
        for (int i = 0; i < PROCESS_PAGE_NUM; i++) { // 释放进程占用的页面
            memory->free_page(page_table[i]); // 释放一个页面
        }
        cout << "Job " << job_id << " has freed " << PROCESS_PAGE_NUM + 1 << " pages." << endl; // 输出释放信息
    }

    // 模拟进程的访问行为，根据访问列表访问内存中的页面
    void access_memory() {
        for (int i = 0; i < access_list.size(); i++) { // 遍历访问列表
            int address = access_list[i]; // 获取逻辑地址
            int page = address / PAGE_SIZE; // 计算页面号
            int offset = address % PAGE_SIZE; // 计算偏移量
            int frame = page_table[page]; // 获取页表项，即物理页面号
            if (frame == -1) { // 如果页表项无效，说明页面不在内存中，发生缺页中断
                page_faults++; // 缺页中断次数加一
                cout << "Page fault occurs when job " << job_id << " accesses address " << address << endl; // 输出缺页中断信息
                frame = page_replace(page); // 调用页面置换算法，返回新的物理页面号
            }
            else { // 如果页表项有效，说明页面在内存中，直接访问
                update_algorithm(page, frame); // 根据页面置换算法更新相关的数据结构
            }
            int physical_address = frame * PAGE_SIZE + offset; // 计算物理地址
            Page p = memory->read_page(frame); // 读取内存中的页面内容
            cout << "Job " << job_id << " accesses address " << address << ", which is page " << p << ", at physical address " << physical_address << endl; // 输出访问信息
            this_thread::sleep_for(chrono::milliseconds(rand() % MAX_SLEEP_TIME)); // 随机休眠一段时间
        }
    }

    // 页面置换算法，根据不同的算法替换一个页面，返回新的物理页面号
    int page_replace(int page) {
        int frame; // 物理页面号
        if (algorithm == "FIFO") { // 如果是 FIFO 算法
            frame = fifo_queue.front(); // 获取队首的物理页面号
            fifo_queue.pop(); // 出队
            fifo_queue.push(frame); // 入队
        }
        else if (algorithm == "LRU") { // 如果是 LRU 算法
            int max_time = 0; // 最大的访问时间
            for (auto it : lru_map) { // 遍历 LRU 映射
                if (it.second > max_time) { // 找到最大的访问时间
                    max_time = it.second; // 更新最大的访问时间
                    frame = it.first; // 获取对应的物理页面号
                }
            }
            lru_map.erase(frame); // 从 LRU 映射中删除该物理页面号
        }
        else { // 如果是其他算法
            frame = memory->allocate_page(); // 分配一个空闲的物理页面号
        }
        Page p = memory->read_page(frame); // 读取内存中的页面内容
        int old_page = p.get_page_id(); // 获取旧的页面号
        if (old_page != -1) { // 如果旧的页面号有效
            page_table[old_page] = -1; // 将对应的页表项置为无效
        }
        page_table[page] = frame; // 将新的页面号写入页表项
        memory->write_page(frame, file->read_page(page)); // 从文件中读取新的页面内容，写入内存
        cout << "Page " << p << " in frame " << frame << " is replaced by page <" << job_id << ", " << page << ">" << endl; // 输出置换信息
        return frame; // 返回物理页面号
    }


        // 根据页面置换算法更新相关的数据结构，传入页面号和物理页面号
    void update_algorithm(int page, int frame) {
        if (algorithm == "FIFO") { // 如果是 FIFO 算法
            // 无需更新
        }
        else if (algorithm == "LRU") { // 如果是 LRU 算法
            static int time = 0; // 定义一个静态变量，表示访问时间
            time++; // 每次访问时，访问时间加一
            lru_map[frame] = time; // 将物理页面号和访问时间存入 LRU 映射中
        }
        else { // 如果是其他算法
            // 无需更新
        }
    }

    // 输出进程的缺页中断率
    void print_page_fault_rate() {
        double rate = (double)page_faults / ACCESS_NUM; // 计算缺页中断率
        cout << "The page fault rate of job " << job_id << " is " << rate << endl; // 输出缺页中断率
    }
};



// 定义一个作业类，用来模拟作业的创建和运行
class Job {
private:
    int job_id; // 作业号
    Memory* memory; // 内存指针
    string algorithm; // 页面置换算法
public:
    // 构造函数，初始化作业
    Job(int job_id, Memory* memory, string algorithm) {
        this->job_id = job_id; // 初始化作业号
        this->memory = memory; // 初始化内存指针
        this->algorithm = algorithm; // 初始化页面置换算法
    }

    // 创建并运行进程
    void run() {
        Process* process = new Process(job_id, memory, algorithm); // 创建进程对象
        process->access_memory(); // 模拟进程的访问行为
        process->print_page_fault_rate(); // 输出进程的缺页中断率
        process->free_memory(); // 释放进程占用的内存
        delete process; // 删除进程对象
    }
};

// 定义一个函数，用来创建并运行多个作业
void run_jobs(int n, Memory* memory, string algorithm) {
    vector<Job*> jobs; // 定义一个作业向量
    for (int i = 0; i < n; i++) { // 创建 n 个作业
        Job* job = new Job(i, memory, algorithm); // 创建作业对象
        jobs.push_back(job); // 将作业对象加入作业向量
    }
    for (int i = 0; i < n; i++) { // 运行 n 个作业
        jobs[i]->run(); // 调用作业的运行方法
        delete jobs[i]; // 删除作业对象
    }
}




