#include <iostream> 
#include <iomanip>
#include <vector>
#include <string>
#include <algorithm>
#define DEBUG
//#define ofile
#ifdef ofile
#include <fstream>	
std::ofstream outfile;
#endif // ofile
//////////////////////////////////////////////////////////////////////
typedef int64_t ll;
class PCB {
public:
	std::string PID;//进程标识符
	ll priority;//进程优先数\轮转时间片
	ll CPUtime;//进程所占用的CPU时间
	ll needtime;//进程所需时间片数
	ll State;//进程状态：1运行 2就绪 3已完成
	void set(std::string a, ll b, ll c, ll d, ll e) {
		PID = a;
		priority = b;
		CPUtime = c;
		needtime = d;
		State = e;
	}
	void ouput(std::ostream& os) {
		std::string string_state;
		if (this->State == 1)
			string_state = "运行中";
		else if(this->State == 2)
			string_state = "就绪";
		else
			string_state = "已完成";
		
		os << std::left << std::setw(10) << this->PID << std::setw(10) << string_state << std::setw(10) << this->priority << std::setw(10) << this->CPUtime << std::setw(10) << this->needtime  << std::endl;
	}
};
class RRPCB {
public:
	std::string PID;//进程标识符
	ll CPUtime;//进程所占用的CPU时间
	ll needtime;//进程所需时间片数
	ll State;//进程状态：1运行 2就绪 3已完成
	void set(std::string a, ll c, ll d, ll e) {
		PID = a;
		CPUtime = c;
		needtime = d;
		State = e;
	}
	void ouput(std::ostream& os) {
		std::string string_state;
		if (this->State == 1)
			string_state = "运行中";
		else if (this->State == 2)
			string_state = "就绪";
		else
			string_state = "已完成";
		os << std::left << std::setw(10) << this->PID << std::setw(10) << string_state << std::setw(10) << this->CPUtime << std::setw(10) << this->needtime  << std::endl;
	}
};
bool priority_sort_higher(const PCB &m, const PCB &n)
{
	if (m.priority == n.priority) 
		return m.PID < n.PID;
	return m.priority > n.priority;
}
std::vector <PCB> Priority_scheduling_algorithm(std::vector <PCB> wait)//优先数调度算法
{
	std::vector <PCB> pcb;
	sort(wait.begin(), wait.end(), priority_sort_higher);
	PCB running;
	while (!wait.empty())
	{
		running = wait.at(0);
		wait.erase(wait.begin());
		std::cout << std::left << std::setw(10) << "进程名" << std::setw(10) << "进程状态" << std::setw(10) << "优先级" << std::setw(10) << "CPU时间" << std::setw(10) << "需要时间" << std::endl;
		running.State = 1;//运行
		running.ouput(std::cout);
		running.priority -= 3;
		running.CPUtime += 1;
		running.needtime -= 1;
		if (running.needtime == 0)
		{
			running.State = 3;
			for (PCB& x : wait)
				x.ouput(std::cout);
			for (PCB& x : pcb)
				x.ouput(std::cout);
			pcb.push_back(running);
		}
		else
		{
			for (PCB& x : wait)
				x.ouput(std::cout);
			for (PCB& x : pcb)
				x.ouput(std::cout);
			running.State = 2;
			wait.push_back(running);
			sort(wait.begin(), wait.end(), priority_sort_higher);
		}
	}
	std::cout << std::left << std::setw(10) << "进程名" << std::setw(10) << "进程状态" << std::setw(10) << "优先级" << std::setw(10) << "CPU时间" << std::setw(10) << "需要时间" << std::endl;
	for (PCB& x : pcb)
		x.ouput(std::cout);
	return pcb;
}
std::vector<RRPCB> round_robin_algorithm(std::vector <RRPCB> wait)//循环轮转调度算法
{
	std::vector<RRPCB> rrpcb;
	RRPCB running;
	while (!wait.empty())
	{
		running = wait.at(0);
		wait.erase(wait.begin());
		std::cout << std::left << std::setw(10) << "进程名" << std::setw(10) << "进程状态" << std::setw(10) << "CPU时间" << std::setw(10) << "需要时间" << std::endl;
		running.State = 1;//运行
		running.ouput(std::cout);
		for (RRPCB& x : wait)
			x.ouput(std::cout);
		for (RRPCB& x : rrpcb)
			x.ouput(std::cout);

		if (running.needtime <= 2)
		{
			running.CPUtime += running.needtime;
			running.needtime = 0;
			running.State = 3;
			rrpcb.push_back(running);
		}
		else
		{
			running.CPUtime += 2;
			running.needtime -= 2;
			running.State = 2;//就绪
			wait.push_back(running);
		}
	}
	std::cout << std::left << std::setw(10) << "进程名" << std::setw(10) << "进程状态" << std::setw(10) << "CPU时间" << std::setw(10) << "需要时间" << std::endl;
	for (RRPCB& x : rrpcb)
		x.ouput(std::cout);
	return rrpcb;
}
int main(int argc, char *argv[])
{
	std::ios::sync_with_stdio(false);
	setlocale(LC_ALL, "");
#ifdef ofile	
	outfile.open("C:\\Users\\Bruce Wayne\\Desktop\\out.txt", std::ios::trunc);
	std::cout.rdbuf(outfile.rdbuf());
#endif
	//////////////////////////////////////////////////////////////////////
	std::string al;
	register size_t i;
	PCB temp;
	RRPCB rrtmp;
	std::string name[5];
	ll needtime[5];
	std::cout << "TYPE THE ALGORITHM(PRIORITY/ROUNDROBIN)：";
	std::cin >> al;
	std::cout << "INPUT NAME AND NEEDTIME:" << std::endl;
	if (al == "PRIORITY" || al == "P" || al == "p") {
		std::vector<PCB> pcb;
		for (i = 0; i < 5; ++i) {
			std::cin >> name[i] >> needtime[i];
			temp.set(name[i], 50 - needtime[i], 0, needtime[i], 2);
			pcb.push_back(temp);
		}
		std::cout << "进程调度运行情况如下：" << std::endl;
		
		Priority_scheduling_algorithm(pcb);
	}
	else if (al == "ROUNDROBIN" || al == "R" || al == "r") {
		std::vector<RRPCB> rrpcb;
		for (i = 0; i < 5; ++i) {
			std::cin >> name[i] >> needtime[i];
			rrtmp.set(name[i], 0, needtime[i], 2);
			rrpcb.push_back(rrtmp);
		}
		std::cout << "进程调度运行情况如下：" << std::endl;
		
		round_robin_algorithm(rrpcb);
	}
	//////////////////////////////////////////////////////////////////////
#ifdef ofile
	outfile.close();
#endif
#ifdef DEBUG
	system("pause");
#endif
	return 0;
}