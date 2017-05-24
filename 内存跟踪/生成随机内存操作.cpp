#include <fstream>
#include <random>
#include <algorithm>

struct operation
{
	size_t time;       //起始时间
	size_t block;      //内存页数
	size_t operate;    //操作
	size_t protection; //权限
};
class RandomINT {
private:
	std::random_device rd;
	std::mt19937 gen;
public:
	RandomINT():gen(rd()) {};
	template<class T = long long>
	T GetRandomInteger(T a, T b) {///return random integer in [a,b]
		if (a>b)
			std::swap(a, b);
		std::uniform_int_distribution<T> dis(a, b);
		return dis(gen);
	}
};
int main()
{
	std::ofstream outfile(R"(C:\Users\Bruce Wayne\Desktop\MemoryOperation.dat)", std::ios::trunc|std::ios::binary);
	operation op;
	RandomINT r;
	for (auto j = 0; j<6; ++j)    //0--保留，1--提交，2--锁，3--解锁，4--回收，5--释放
		for (auto i = 0; i<5; ++i)
			//0-PAGE_READONLY;
			//1-PAGE_READWRITE;
			//2-PAGE_EXECUTE;
			//3-PAGE_EXECUTE_READ;
			//4-PAGE_EXECUTE_READWRITE;
		{
			op.time = r.GetRandomInteger<size_t>(0,1000);//随机生成等待时间
			op.block = r.GetRandomInteger<size_t>(1, 5);//随机生成块大小
			op.operate = j;
			op.protection = i;
			outfile.write(reinterpret_cast<char*>(&op), sizeof(op));
		}
	outfile.close();
	return 0;
}