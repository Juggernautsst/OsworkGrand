<!-- // 开辟一块内存空间，作为模拟内存(malloc)
// 空间大小为2^14字节
// 假设系统的页面大小为256字节，每个页表项占4个字节(系统的物理页面数为2^6,每个页表正好占一个页面)
// 用位图刻画内存页面的分配状态，可以用一个辅助的变量来对空闲内存页面计数
// 每个进程的虚拟地址空间也是2^14字节
// 每个进程分配9个页面（连页表一共10个页面）
// 创建12个作业，并模拟作业的运行
// 创建12个文件，模拟磁盘上的代码和数据
//  可以在文件的第i*256字节处写入<作业号，页面号>，以识别相应的页面
// 作业的模拟运行过程
//  如果系统中的空闲页面数<10（因为页表也要占用一个页面，所以至少要有10个空闲页面才行），则作业需要等待空闲页面数
// 否则，为作业分配9个空闲页框和一个存放页表的页框，记录下页表的基地址
// 需要修改位图和空闲页框计数器的值
// 注意互斥访问
// 模拟进程的访问行为
// 每个进程随机生成200次逻辑地址，每次地址访问后休眠（0-100ms）中的一个随机值
// 逻辑地址的生成规则：进程对第i号页面的访问概率正比于1/(i+1)1/2
// 如果对应的逻辑地址已经在内存，则直接访问
// 如果对应的逻辑地址不在内存，则读文件，将文件对应的页面数据载入内存中相应的块，并修改页表项
// 在此过程中，可能会发生页面替换,页面替换采用FIFO和LRU两种替换算法
// 输出进程的本次访问记录（进程号，虚拟地址，虚拟地址页面的内容，物理地址，物理地址中的内容）
// 统计每个进程的缺页中断次数
// 进程运行结束
// 输出进程的缺页中断率
// 唤醒可能等待内存资源的作业
绿色的好看一点 -->
