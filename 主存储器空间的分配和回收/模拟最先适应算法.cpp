#include <iostream>
#include <list>
#include <string>
using namespace std;
struct Node {
	int start=0; // 起址
	int length = 0; // 长度大小
	Node(int a,int b):start(a), length(b){}
	Node(){}
};
list<Node> mem;
void DispSpace() { // 显示内存空间分配情况
	cout << R"(
空闲区:
----起址----长度
)" << endl;
	if (!mem.empty())
	{
		for (auto i = mem.begin(); i != mem.end(); ++i)
			cout << '\t' << i->start << '\t' << i->length << endl;
		cout << endl;
	}
}

void Initial() { // 初始化
	mem.push_back(Node(14, 12));
	mem.push_back(Node(32, 96));
	DispSpace();
}

bool Allocation(int len) { // 分配内存给新作业
	if(mem.empty())
		return false;
	for (auto i = mem.begin(); i != mem.end(); ++i)
	{
		if (i->length < len)
			continue;
		if(i->length > len)
		{
			i->start += len;
			i->length-=len;			
			return true;
		}
		mem.erase(i);
		return true;
	}
	return false;
}

void CallBack(int address, int len) { // Recycle memory
	if (mem.empty())
	{
		mem.push_back(Node(address, len));
		return;
	}
	auto isRecycled = false;
	auto x = mem.begin();
	for (auto i = mem.begin(); i != mem.end(); ++i)
	{
		if(i->start>address)
		{
			x=mem.insert(i, Node(address, len));
			isRecycled = true;
			break;
		}
	}
	if(!isRecycled)
	{
		mem.push_back(Node(address, len));

		auto i = mem.end(), j = mem.end(); --i, --j;
		--i;
		if ((i->start + i->length) == address)
		{
			i->length += len;
			mem.erase(j);
		}
		return;
	}
	//合并下邻区
	auto i = x; ++i;
	if(x->start+x->length==i->start)
	{
		x->length += i->length;
		mem.erase(i);
	}
	if (x == mem.begin())
		return;
	//合并上邻区
	i = x; --i;
	if ((i->start + i->length) == x->start)
	{
		i->length += x->length;
		mem.erase(x);
	}
}
void main() {
	setlocale(LC_ALL, "");
	Initial();

	cout << "现在分配大小为 6K 的作业 4 申请装入主存: ";
	if(Allocation(6))
		cout << "分配成功！"<<endl;
	else
		cout << "分配失败！" << endl;
	DispSpace();

	cout << "现回收作业 3 (起址10,长度4)\n";
	CallBack(10, 4);
	DispSpace();

	cout << "现回收作业 2 (起址26,长度6)\n";
	CallBack(26, 6);
	DispSpace();
	//---------------演示结束-------------
	system("pause");
}
