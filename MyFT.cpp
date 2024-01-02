#include "MyFt.h"



// struct Process {
//     int id;
//     std::vector<int> pages;
// };

// 位图类
class BitMap {
private:
    vector<bool> bits; 
    int free_count; 
    mutex mtx; 
public:

    BitMap(int size) {
        bits.resize(size, false); 
        free_count = size; 
    }


    int get_free_count() {
        lock_guard<mutex> lock(mtx);
        return free_count;
    }

   
    int allocate_page() {
        lock_guard<mutex> lock(mtx); 
        if (free_count == 0) { 
            return -1;
        }
        for (int i = 0; i < bits.size(); i++) { 
            if (!bits[i]) {
                bits[i] = true; 
                free_count--; 
                return i; 
            }
        }
        return -1; 
    }


    void free_page(int page) {
        lock_guard<mutex> lock(mtx); // 上锁
        if (page >= 0 && page < bits.size() && bits[page]) { 
            bits[page] = false; //false，表示空闲
            free_count++; 
        }
    }
};

// 页面类
class Page {
private:
    int job_id; 
    int page_id; 
public:
    Page(int job_id = 0, int page_id = 0) : job_id(job_id), page_id(page_id) {}


    int get_job_id() const {
        return job_id;
    }

    int get_page_id() const {
        return page_id;
    }


    friend std::ostream& operator<<(std::ostream& os, const Page& page) {
        os << "<" << page.job_id << ", " << page.page_id << ">";
        return os;
    }


    friend std::istream& operator>>(std::istream& is, Page& page) {
        is >> page.job_id >> page.page_id;
        return is;
    }
};


class Memory {
private:
    vector<Page> pages; //页面向量存储内存中的页面
    BitMap bitmap; // 位图记录内存页面的分配状态
    mutex mtx; //互斥锁保证线程安全
public:
    Memory(int size) : bitmap(size / PAGE_SIZE) {
        pages.resize(size / PAGE_SIZE); 
    }

    // 获取空闲页面的数量
    int get_free_count() {
        return bitmap.get_free_count(); 
    }

    //没有空闲页面，返回 -1
    int allocate_page() {
        return bitmap.allocate_page(); 
    }


    void free_page(int page) {
        bitmap.free_page(page); 
    }

   
    Page read_page(int page) {
        lock_guard<mutex> lock(mtx); // 上锁
        if (page >= 0 && page < pages.size()) { 
            return pages[page]; 
        }
        return Page(-1, -1); 
    }

    //传入页面号和页面对象
    void write_page(int page, Page p) {
        lock_guard<mutex> lock(mtx); // 上锁
        if (page >= 0 && page < pages.size()) { 
            pages[page] = p; 
        }
    }
};



class File {
private:
    string file_name; 
    vector<Page> pages;
public:
    File(int job_id) {
        file_name = FILE_PREFIX + to_string(job_id) + FILE_SUFFIX; // 根据作业号生成文件名
        pages.resize(VIRTUAL_PAGE_NUM); 
        for (int i = 0; i < VIRTUAL_PAGE_NUM; i++) { 
            pages[i] = Page(job_id, i); 
        }
        write_to_disk(); 
    }

    // 将文件写入磁盘
    void write_to_disk() {
        ofstream ofs(file_name);
        if (ofs.is_open()) { 
            for (int i = 0; i < pages.size(); i++) { 
                ofs << pages[i] << endl; 
            }
            ofs.close(); 
        }
    }

   
    void read_from_disk() {
        ifstream ifs(file_name); 
        if (ifs.is_open()) { 
            for (int i = 0; i < pages.size(); i++) { 
                ifs >> pages[i];
            }
            ifs.close(); 
        }
    }

    // 读取一个页面的内容，传入页面号，返回页面对象
    Page read_page(int page) {
        if (page >= 0 && page < pages.size()) { 
            return pages[page]; 
        }
        return Page(-1, -1); 
    }

  
    void write_page(int page, Page p) {
        if (page >= 0 && page < pages.size()) { 
            pages[page] = p; 
            write_to_disk(); 
        }
    }
};



class Process {
private:
    int job_id; // 作业号
    int page_table_base; // 页表的基地址
    vector<int> page_table; // 页表
    vector<int> access_list; // 访问列表
    int page_faults; // 缺页中断次数
    Memory* memory; // 内存指针
    File* file; // 文件指针
    queue<int> fifo_queue; // FIFO
    unordered_map<int, int> lru_map; // LRU
    string algorithm; 
public:
    Process(int job_id, Memory* memory, string algorithm) {
        this->job_id = job_id; 
        this->memory = memory;
        this->algorithm = algorithm; 
        file = new File(job_id);
        page_table.resize(VIRTUAL_PAGE_NUM, -1); 
        page_faults = 0; // 将缺页中断次数初始化为 0
        generate_access_list(); // 生成访问列表
        allocate_memory(); // 分配内存
    }

 
    void generate_access_list() {
        random_device rd; 
        mt19937 gen(rd());
        discrete_distribution<> dist({0.5, 0.25, 0.125, 0.0625, 0.03125, 0.015625, 0.0078125, 0.00390625, 0.001953125}); // 离散分布，每个页面的访问概率正比于 1/(i+1)1/2
        for (int i = 0; i < ACCESS_NUM; i++) { // 生成 ACCESS_NUM 个逻辑地址
            int page = dist(gen); // 根据分布生成页面号
            int offset = rand() % PAGE_SIZE; // 随机生成偏移量
            int address = page * PAGE_SIZE + offset; // 计算逻辑地址
            access_list.push_back(address); // 将逻辑地址加入访问列表
        }
    }


    void allocate_memory() {
        while (memory->get_free_count() < PROCESS_PAGE_NUM + 1) { // 如果空闲页面数不足
            cout << "Job " << job_id << " is waiting for memory resources." << endl; // 输出等待信息
            this_thread::sleep_for(chrono::milliseconds(100)); // 休眠 100 ms
        }
        page_table_base = memory->allocate_page(); 
        for (int i = 0; i < PROCESS_PAGE_NUM; i++) { 
            int page = memory->allocate_page(); 
            page_table[i] = page; // 更新页表
            memory->write_page(page, file->read_page(i)); 
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
        for (int i = 0; i < access_list.size(); i++) {
            int address = access_list[i];
            int page = address / PAGE_SIZE; 
            int offset = address % PAGE_SIZE; 
            int frame = page_table[page];
            if (frame == -1) { 
                page_faults++; 
                cout << "Page fault occurs when job " << job_id << " accesses address " << address << endl; // 输出缺页中断信息
                frame = page_replace(page); 
            }
            else { 
                update_algorithm(page, frame); 
            }
            int physical_address = frame * PAGE_SIZE + offset; 
            Page p = memory->read_page(frame); 
            cout << "Job " << job_id << " accesses address " << address << ", which is page " << p << ", at physical address " << physical_address << endl; // 输出访问信息
            this_thread::sleep_for(chrono::milliseconds(rand() % MAX_SLEEP_TIME)); // 随机休眠一段时间
        }
    }

    // 页面置换算法
    int page_replace(int page) {
        int frame; 
        if (algorithm == "FIFO") { // FIFO 
            frame = fifo_queue.front(); //队首的物理页面号
            fifo_queue.pop(); 
            fifo_queue.push(frame); 
        }
        else if (algorithm == "LRU") { 
            int max_time = 0; 
            for (auto it : lru_map) { // 遍历 LRU 映射
                if (it.second > max_time) { 
                    max_time = it.second; 
                    frame = it.first; // 获取对应的物理页面号
                }
            }
            lru_map.erase(frame); // 从 LRU 映射中删除该物理页面号
        }
        else { 
            frame = memory->allocate_page(); // 分配一个空闲的物理页面号
        }
        Page p = memory->read_page(frame); 
        int old_page = p.get_page_id(); // 获取旧的页面号
        if (old_page != -1) { 
            page_table[old_page] = -1; // 将对应的页表项置为无效
        }
        page_table[page] = frame; 
        memory->write_page(frame, file->read_page(page)); // 从文件中读取新的页面内容，写入内存
        cout << "Page " << p << " in frame " << frame << " is replaced by page <" << job_id << ", " << page << ">" << endl; // 输出置换信息
        return frame; 
    }


    void update_algorithm(int page, int frame) {
        if (algorithm == "FIFO") { //FIFO
            
        }
        else if (algorithm == "LRU") { //LRU
            static int time = 0;
            time++;
            lru_map[frame] = time;
        }
        else { // 其他算法
            ///
        }
    }

    //缺页中断率
    void print_page_fault_rate() {
        double rate = (double)page_faults / ACCESS_NUM; 
        cout << "The page fault rate of job " << job_id << " is " << rate << endl; 
    }
};



// 模拟作业创建运行
class Job {
private:
    int job_id; 
    Memory* memory;
    string algorithm;
public:
    Job(int job_id, Memory* memory, string algorithm) {
        this->job_id = job_id;
        this->memory = memory;
        this->algorithm = algorithm;
    }

    // 创建并运行进程
    void run() {
        Process* process = new Process(job_id, memory, algorithm); 
        process->access_memory(); // 模拟进程访问
        process->print_page_fault_rate(); 
        process->free_memory(); 
        delete process; // 删除进程对象
    }
};

//运行多个作业
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




