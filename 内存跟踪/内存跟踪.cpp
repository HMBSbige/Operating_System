#include <iostream>
#include <fstream>
#include <windows.h>
#include <vector>
using namespace std;


struct operation
{
	size_t time;       //起始时间
	size_t block;      //内存页数
	size_t operate;    //操作
	size_t protection; //权限
};

struct trace  //跟踪每一次分配活动的数据结构
{
	LPVOID start;  //起始地址
	size_t size;     //分配的大小
};

HANDLE Allocator_Semaphore, Tracker_Semaphore;  //信号量句柄

DWORD WINAPI Tracker(LPVOID param) //跟踪Allocator线程的内存行为
{
	wofstream outfile(R"(C:\Users\Bruce Wayne\Desktop\Tracker.txt)", ios::trunc);
	///如果直接输出宽字符L""或读入UTF-8文本，需要加上下面的代码
	/*
	#include <codecvt>
	try {
		outfile.imbue(locale("zh_CN.UTF8"));
	}
	catch (std::runtime_error) {
		outfile.imbue(locale(locale::classic(), new codecvt_utf8<wchar_t>));
	}
	*/

	//系统信息
	SYSTEM_INFO info;
	GetSystemInfo(&info);
	outfile << u8"系统信息：" << endl;
	outfile << u8"处理器掩码: " << info.dwActiveProcessorMask << endl;
	outfile << u8"虚拟内存空间的粒度: " << info.dwAllocationGranularity << endl;
	outfile << u8"处理器个数: " << info.dwNumberOfProcessors << endl;
	outfile << u8"OEM ID: " << info.dwOemId << endl;
	outfile << u8"内存分页大小: " << info.dwPageSize << endl;
	outfile << u8"处理器类型: " << info.dwProcessorType << endl;
	outfile << u8"最大寻址单元: " << info.lpMaximumApplicationAddress << endl;
	outfile << u8"最小寻址单元: " << info.lpMinimumApplicationAddress << endl;
	outfile << u8"处理器的体系结构: " << info.wProcessorArchitecture << endl;
	outfile << u8"处理器架构: ";
	switch (info.wProcessorLevel)
	{
	case PROCESSOR_ARCHITECTURE_AMD64:
		outfile << u8"x64 (AMD or Intel)";
		break;
	case PROCESSOR_ARCHITECTURE_ARM:
		outfile << u8"ARM";
		break;
	case PROCESSOR_ARCHITECTURE_IA64:
		outfile << u8"Intel Itanium-based";
		break;
	case PROCESSOR_ARCHITECTURE_INTEL:
		outfile << u8"x86";
		break;
	case PROCESSOR_ARCHITECTURE_UNKNOWN:
		outfile << u8"未知架构";
		break;
	default:
		outfile << u8"未知架构";
		break;
	}
	outfile << endl;
	outfile << u8"处理器版本: " << info.wProcessorRevision << endl;
	outfile << u8"保留: " << info.wReserved << endl;
	outfile << u8"----------------------------------------------------------" << endl;

	//内存基本信息
	MEMORY_BASIC_INFORMATION mem;
	//调用进程虚拟空间中的页面信息
	VirtualQuery(info.lpMinimumApplicationAddress, &mem, sizeof(MEMORY_BASIC_INFORMATION));
	outfile << u8"分配基地址:" << u8"\t" << mem.AllocationBase << endl;
	outfile << u8"地址空间区域被初次保留时赋予的保护属性:" << u8"\t" << mem.AllocationProtect << endl;
	outfile << u8"区域基地址:" << u8"\t" << mem.BaseAddress << endl;
	outfile << u8"保护属性:" << u8"\t" << mem.Protect << endl;
	outfile << u8"区域大小(字节)" << u8"\t" << mem.RegionSize << endl;
	outfile << u8"状态:" << u8"\t" << mem.State << endl;
	outfile << u8"类型:" << u8"\t" << mem.Type << endl;
	outfile << u8"----------------------------------------------------------" << endl;

	for (auto i = 0; i <= 30; ++i)
	{
		//等待Allocator一次内存分配活动结束
		WaitForSingleObject(Tracker_Semaphore, INFINITE);

		outfile << i << endl;
		
		//内存状态
		MEMORYSTATUS status;
		GlobalMemoryStatus(&status);
		outfile << u8"内存状态：" << endl;
		outfile << u8"可用的页面文件大小(byte):" << u8"\t" << status.dwAvailPageFile << endl;
		outfile << u8"可用的物理内存大小(byte):" << u8"\t" << status.dwAvailPhys << endl;
		outfile << u8"当前空闲的地址空间(byte):" << u8"\t" << status.dwAvailVirtual << endl;
		outfile << u8"本结构的长度:" << u8"\t" << status.dwLength << endl;
		outfile << u8"当前系统内存的使用率" << u8"\t" << status.dwMemoryLoad << "%" << endl;
		outfile << u8"可以存在的页面文件大小(byte):" << u8"\t" << status.dwTotalPageFile << endl;
		outfile << u8"物理内存总量(byte):" << u8"\t" << status.dwTotalPhys << endl;
		outfile << u8"可用虚拟地址空间大小(byte):" << u8"\t" << status.dwTotalVirtual << endl;
		outfile << u8"----------------------------------------------------------" << endl;

		//释放信号量通知Allocator可以执行下一次内存分配活动
		ReleaseSemaphore(Allocator_Semaphore, 1, nullptr);
	}
	outfile.close();
	return 0;
}

DWORD WINAPI Allocator(LPVOID param) //模拟内存分配活动的线程
{
	trace traceArray[5];
	auto index = 0;
	ifstream file_MemoryOperation(R"(C:\Users\Bruce Wayne\Desktop\MemoryOperation.dat)", ios::binary);
	operation op;
	SYSTEM_INFO info;
	DWORD permission;

	GetSystemInfo(&info);
	for (auto i = 0; i<30; ++i)
	{
		//等待Tracker打印结束
		WaitForSingleObject(Allocator_Semaphore, INFINITE);
		cout << i << ":";
		file_MemoryOperation.read(reinterpret_cast<char*>(&op), sizeof(op));

		//Sleep(op.time);
		GetSystemInfo(&info);

		//访问权限
		switch (op.protection)
		{
			case 0:
				index = 0;
				permission = PAGE_READONLY;
				break;
			case 1:
				permission = PAGE_READWRITE;
				break;
			case 2:
				permission = PAGE_EXECUTE;
				break;
			case 3:
				permission = PAGE_EXECUTE_READ;
				break;
			case 4:
				permission = PAGE_EXECUTE_READWRITE;
				break;
			default:
				permission = PAGE_READONLY;
		}

		//操作
		switch (op.operate)
		{
			case 0://保留(reserve)
			{
				cout << "保留..." << endl;
				traceArray[index].start = VirtualAlloc(nullptr, op.block*info.dwPageSize,MEM_RESERVE, PAGE_NOACCESS);
				traceArray[index++].size = op.block*info.dwPageSize;
				cout << "开始地址:" << traceArray[index - 1].start << "\t"
					<< "大小:" << traceArray[index - 1].size << endl;
				break;
			}
			case 1://提交(commit)
			{
				cout << "提交..." << endl;
				traceArray[index].start = VirtualAlloc(traceArray[index].start,traceArray[index].size, MEM_COMMIT, permission);
				++index;
				cout << "开始地址:" << traceArray[index - 1].start << "\t" 
					 << "大小:"<< traceArray[index - 1].size << endl;
				break;
			}
			case 2: //锁住(lock)
			{
				cout << "锁住..." << endl;
				cout << "开始地址:" << traceArray[index].start << "\t"
					 << "大小:" <<traceArray[index].size << endl;
				if (!VirtualLock(traceArray[index].start, traceArray[index++].size))
					cout << GetLastError() << endl;
				break;
			}
			case 3: //解锁(unlock)
			{
				cout << "解锁..." << endl;
				cout << "开始地址:" << traceArray[index].start << "\t" 
					 << "大小:"<< traceArray[index].size << endl;
				if (!VirtualUnlock(traceArray[index].start, traceArray[index++].size))
					cout << GetLastError() << endl;
				break;
			}
			case 4: //回收(decommit)
			{
				cout << "回收..." << endl;
				cout << "开始地址:" << traceArray[index].start << "\t"
					 << "大小:" << traceArray[index].size << endl;
				if (!VirtualFree(traceArray[index].start, traceArray[index++].size,MEM_DECOMMIT))
					cout << GetLastError() << endl;
				break;
			}
			case 5: //释放(release)
			{
				cout << "释放..." << endl;
				cout << "开始地址:" << traceArray[index].start << "\t"
					 <<"大小:" << traceArray[index].size << endl;
				if (!VirtualFree(traceArray[index++].start, 0, MEM_RELEASE))
					cout << GetLastError() << endl;
				break;
			}
			default:
				cout << "error" << endl;
		}
		//释放信号量通知Tracker可以打印信息
		ReleaseSemaphore(Tracker_Semaphore, 1, nullptr);
	}
	file_MemoryOperation.close();
	return 0;
}

int main() {
	_wsetlocale(LC_ALL, L"");
	ios::sync_with_stdio(false);

	DWORD ThreadId[2];
	HANDLE ThreadHandle[2];

	//生成两个线程
	ThreadHandle[0] = CreateThread(nullptr, 0, Tracker, nullptr, 0, &ThreadId[0]);
	ThreadHandle[1] = CreateThread(nullptr, 0, Allocator, nullptr, 0, &ThreadId[1]);

	//生成两个信号量
	Allocator_Semaphore = CreateSemaphore(nullptr, 0, 1, L"分配");
	Tracker_Semaphore = CreateSemaphore(nullptr, 1, 1, L"跟踪");

	//等待线程执行结束后再退出
	WaitForMultipleObjects(2, ThreadHandle, TRUE, INFINITE);

	system("pause");
	return 0;
}