#include <iostream>
#include <iomanip>
#include <random>
#include <string>
#include <vector>
#include <bitset>

using namespace std;

class RandomNum {
private:
	random_device rd;
public:
	RandomNum();
	int getNum(int a, int b);
};
RandomNum::RandomNum() {};
int RandomNum::getNum(int a, int b) {
	if (a > b)
		swap(a, b);
	mt19937 gen(rd());
	uniform_int_distribution<> dis(a, b);
	return dis(gen);
}
RandomNum rn;
const int maxn = 320;     //序列个数
const int max = maxn + 20;//数组大小
const int maxp = max / 10;//最大页数

vector<int> inst(max);//指令序列
vector<int> page(max);//页地址流
int sizeofpage;       //内存页数

bitset<maxp> in;//该页是否在内存里
vector<int> pin(maxp);  //现在在内存里的页

//通过随机数产生一个指令序列，共320条指令
void produce_inst()
{
	for (auto num = 0;num < maxn;)
	{
		auto m = rn.getNum(0, maxn - 1);
		inst[num++] = (m + 1) % maxn;
		if (num == maxn) 
			break;

		m = (m + 2) % maxn;
		if (m == 0) m = 160;
		auto n = rn.getNum(0, m-1);
		inst[num++] = (n + 1) % maxn;
		if (num == maxn) 
			break;

		n = (n + 2) % maxn;
		m = maxn - n;
		if (m == 0) 
			m = 160;
		m = rn.getNum(0,m-1) + n;
		inst[num++] = m;
	}
}
//将指令序列变换成为页地址流
void turn_page_address()
{
	for (auto i = 0; i < maxn; ++i)
		page[i] = inst[i] / 10;
}
//先进先出的算法
double FIFO_solve()
{
	in.reset();
	auto fault_n = 0;//页面失效次数
	auto i=0;

	//预调页填满空间    
	auto ptr = 0; //下一个要放的位置
	for (; i < maxn && ptr < sizeofpage; ++i)
		if (!in[page[i]])
		{
			pin[ptr++] = page[i];
			in[page[i]] = true;
			++fault_n;
		}

	//继续执行剩下的指令
	ptr = 0;//队列里最先进来的位置，即下一个要被替换的位置
	for (; i < maxn; ++i)
		if (!in[page[i]])
		{
			in[pin[ptr]] = false;
			in[page[i]] = true;
			pin[ptr] = page[i];
			++fault_n;
			ptr = (ptr + 1) % sizeofpage;
		}

	return (1 - static_cast<double>(fault_n) / 320.0);
}
//最近最少使用算法
double LRU_solve()
{
	vector<int> LastTimeUse(maxp,0);
	auto ti = 1;    //模拟时间
	auto fault_n = 0;
	in.reset();
	vector<int>(maxp,1).swap(pin);

	for (auto i = 0; i < maxn; ++i)
	{
		if (!in[page[i]])
		{
			auto min = 1000000; 
			auto ptr = 0;
			for (auto j = 0; j < sizeofpage; ++j)
			{
				if (LastTimeUse[j] < min)
				{
					min = LastTimeUse[j];
					ptr = j;
				}
			}
			//替换或写入 
			if (pin[ptr] != -1)
				in[pin[ptr]] = false;
			in[page[i]] = true;
			pin[ptr] = page[i];
			++fault_n;
			LastTimeUse[ptr] = ti++;
		}
		else//已经在内存里则只需更改最近使用时间    
		{
			for (auto j = 0; j < sizeofpage; ++j)
				if (pin[j] == page[i])
				{
					LastTimeUse[j] = ti++;
					break;
				}
		}
	}

	return (1 - static_cast<double>(fault_n) / 320.0);
}
//最佳淘汰算法
double OPT_solve()
{
	vector<int> NextTimeUse(maxp,1);
	auto fault_n = 0;
	int i;
	in.reset();

	//预调页填满
	auto ptr = 0;
	for (i = 0; i < maxn && fault_n < sizeofpage; ++i)
	{
		if (!in[page[i]])
		{
			in[page[i]] = true;
			pin[ptr] = page[i];
			++fault_n;
			++ptr;
		}
	}

	//初始化NextTimeUse数组
	ptr = 0;
	for (auto j = i; j<maxn && ptr<32; ++j)
	{
		if (NextTimeUse[page[j]] == -1)
		{
			NextTimeUse[page[j]] = j;
			++ptr;
		}
	}

	for (; i < maxn; ++i)
	{
		if (!in[page[i]])
		{
			auto max = 0; 
			ptr = 0;
			for (auto j = 0; j<sizeofpage; ++j)
			{
				if (NextTimeUse[pin[j]] == -1)
				{
					ptr = j;
					break;
				}
				if (NextTimeUse[pin[j]] > max)
				{
					max = NextTimeUse[pin[j]];
					ptr = j;
				}
			}
			in[pin[ptr]] = false;
			in[page[i]] = true;
			pin[ptr] = page[i];
			fault_n++;
		}

		NextTimeUse[page[i]] = -1;
		for (auto j = i + 1; j<maxn; ++j)
			if (page[j] == page[i])
			{
				NextTimeUse[page[i]] = j;
				break;
			}
	}
	return (1 - static_cast<double>(fault_n) / 320.0);
}

int main()
{
	std::ios::sync_with_stdio(false);
	setlocale(LC_ALL, "");

	produce_inst();
	turn_page_address();
	cout << "命中率:	" << endl;
	cout << R"(内存容量    先进先出的算法    最近最少使用算法    最佳淘汰算法
)";
	cout << setiosflags(ios::fixed);
	cout << setprecision(2) << setiosflags(ios::right);
	for(sizeofpage =4; sizeofpage <=32;++sizeofpage)
	{
		
		cout << setw(8)<< sizeofpage << setw(18) << FIFO_solve() << setw(20) << LRU_solve()  << setw(16) << OPT_solve() << endl;
	}

	system("pause");
	return 0;
}