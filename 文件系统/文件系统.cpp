#include <iostream>
#include <string>
#include <sstream>
#include <new>
#include <ctime>

#define MaxUser 100          //定义最大MDF主目录文件   
#define MaxDisk 512*1024     //模拟最大磁盘空间   
#define commandAmount 12     //对文件操作的指令数   
//存储空间管理有关结构体和变量   
char disk[MaxDisk];     //模拟512K的磁盘存储空间

template<class T>
T StringToNum(const std::string& s)//字符串转数字
{
	T num;
	std::stringstream ss(s);
	ss >> num;
	return num;
}

typedef struct distTable  //磁盘块结构体   
{
	int maxlength;
	int start;
	int useFlag;
	distTable *next;
}diskNode;
diskNode *diskHead;
struct fileTable    //文件块结构体   
{
	std::string fileName;
	int strat;          //文件在磁盘存储空间的起始地址   
	int length;         //文件内容长度   
	int maxlength;      //文件的最大长度   
	std::string fileKind;   //文件的属性——读写方式   
	struct tm *timeinfo;
	bool openFlag;       //判断是否有进程打开了该文件   
						 //fileTable *next;   
};

//两级目录结构体   
typedef struct user_file_directory  //用户文件目录文件UFD   
{  
	fileTable *file;
	user_file_directory *next;
}UFD;
//UFD *headFile;   
typedef struct master_file_directory  //主文件目录MFD   
{
	std::string userName;
	std::string password;
	UFD *user;
}MFD;
MFD userTable[MaxUser];
int used = 0;//定义MFD目录中用已有的用户数   
int userID = -1;//用户登录的ID号，值为-1时表示没有用户登录 

void userCreate()
{
	std::string userName;
	if (used<MaxUser)
	{
		std::cout << "请输入用户名：";
		getline(std::cin, userName, '\n');
		for (int i = 0; i<used; ++i)
		{
			if (userTable[i].userName == userName)
			{
				std::cout << std::endl << "该用户名已存在,创建用户失败" << std::endl;
				system("pause");
				return;
			}
		}
		userTable[used].userName = userName;
		std::cout << std::endl << "请输入密码：";
		getline(std::cin, userTable[used].password, '\n');
		std::cout << std::endl << "创建用户成功" << std::endl;
		++used;
		system("pause");
	}
	else
	{
		std::cout << "创建用户失败，用户已达到上限" << std::endl;;
		system("pause");
	}
}

int login()
{
	std::string name, psw;
	int i;
	std::cout << "请输入用户名:";
	getline(std::cin, name, '\n');
	for (i = 0; i<used; i++)
	{
		if (userTable[i].userName == name)
			break;
	}
	if (i == used)
	{
		std::cout << std::endl << "您输入的用户名不存在" << std::endl;
		system("pause");
		return -1;
	}
	for (int times = 0; times<3; ++times)
	{
		std::cout << std::endl << "请输入密码:";
		getline(std::cin, psw, '\n');
		std::cout << std::endl;

		for (i = 0; i<used; ++i)
		{
			if (psw == userTable[i].password)
			{
				std::cout << "用户登录成功" << std::endl;
				system("pause");
				break;
			}
		}
		if (i == used)
		{
			std::cout << "您输入的密码错误，您还有" << 2 - times << "次输入机会" << std::endl;
			if (times == 2)
				exit(0);
		}
		else
			break;
	}
	return i;
}

void initDisk()
{
	diskHead = new diskNode;
	diskHead->maxlength = MaxDisk;
	diskHead->useFlag = 0;
	diskHead->start = 0;
	diskHead->next = nullptr;
}

//磁盘分配查询
int requestDist(int &startPostion, int maxLength)
{
	int flag = 0;//标记是否分配成功   
	diskNode *p = diskHead;
	while (p)
	{
		if (p->useFlag == 0 && p->maxlength>maxLength)
		{
			startPostion = p->start;
			diskNode *q = new diskNode;
			q->start = p->start;
			q->maxlength = maxLength;
			q->useFlag = 1;
			q->next = nullptr;
			diskHead->start = p->start + maxLength;
			diskHead->maxlength = p->maxlength - maxLength;
			flag = 1;
			diskNode *temp = p;
			if (diskHead->next == nullptr) diskHead->next = q;
			else
			{
				while (temp->next) temp = temp->next;
				temp->next = q;
			}
			break;
		}
		p = p->next;
	}
	return flag;
}

void fileCreate(std::string fileName, int length, std::string fileKind)
{  
	time_t rawtime;
	int startPos;
	UFD* p;
	for (p = userTable[userID].user->next; p != nullptr; p = p->next)
	{
		if (p->file->fileName==fileName)
		{
			std::cout<<"文件重名，创建文件失败"<<std::endl;
			system("pause");
			return;
		}
	}
	if (requestDist(startPos, length))
	{
		UFD *fileNode = new UFD;
		fileNode->file =new fileTable; 
		fileNode->file->fileName=fileName;
		fileNode->file->fileKind=fileKind;
		fileNode->file->maxlength = length;
		fileNode->file->strat = startPos;
		fileNode->file->openFlag = false;
		time(&rawtime);
		fileNode->file->timeinfo = localtime(&rawtime);
		fileNode->next = nullptr;
		if (userTable[userID].user->next == nullptr)
			userTable[userID].user->next = fileNode;
		else
		{
			p = userTable[userID].user->next;
			while (p->next) p = p->next;
			p->next = fileNode;
		}
		std::cout << "创建文件成功" << std::endl;
		system("pause");
	}
	else
	{
		std::cout << "磁盘空间已满或所创建文件超出磁盘空闲容量，磁盘空间分配失败"<< std::endl;
		system("pause");
	}
}

void freeDisk(int startPostion)
{
	diskNode *p;
	for (p = diskHead; p != nullptr; p = p->next)
	{
		if (p->start == startPostion)
			break;
	}
	p->useFlag = false;
}

void fileDel(std::string fileName)
{
	UFD *q = userTable[userID].user;
	UFD *p = q->next;
	while (p)
	{
		if (p->file->fileName==fileName) 
			break;
		p = p->next;
		q = q->next;
	}
	if (p)
	{
		if (p->file->openFlag != true)//先判断是否有进程打开该文件   
		{
			UFD *temp = p;
			q->next = p->next;
			freeDisk(temp->file->strat);//磁盘空间回收   
			free(temp);
			std::cout<<"文件删除成功"<<std::endl;
			system("pause");
		}
		else
		{
			std::cout << "该文件已被进程打开,删除失败" << std::endl;
			system("pause");
		}
	}
	else
	{
		std::cout << "没有找到该文件,请检查输入的文件名是否正确" << std::endl;
		system("pause");
	}
}

void fileCat(std::string fileName)
{
	UFD *p;
	UFD *q = userTable[userID].user;
	for (p = q->next; p != nullptr; p = p->next)
	{
		if (p->file->fileName==fileName)
			break;
	}
	if (p)
	{
		int startPos = p->file->strat;
		int length = p->file->length;
		p->file->openFlag = true;//文件打开标记   
		std::cout << "*****************************************************"<<std::endl;;
		for (int i = startPos,k=0; k<length; ++i, ++k)
		{
			if (i % 50 == 0)
				std::cout << std::endl;  //一行大于50个字符换行   
			std::cout<<disk[i];
		}
		std::cout<<std::endl<< std::endl << "*****************************************************" << std::endl;
		std::cout << p->file->fileName << "已被read进程打开,请用close命令将其关闭" << std::endl;
		system("pause");
	}
	else
	{
		std::cout << "没有找到该文件,请检查输入的文件名是否正确" << std::endl;
		system("pause");
	}
}

void fileWrite(std::string fileName)
{
	UFD *p;
	UFD *q = userTable[userID].user;
	int i, k;
	for (p = q->next; p != nullptr; p = p->next)
	{
		if (p->file->fileName==fileName)
			break;
	}
	if (p)
	{
		if (p->file->fileKind=="r")//判断文件类型   
		{
			std::cout << "该文件是只读文件,写入失败" << std::endl;
			system("pause");
			return;
		}
		std::string str;
		std::cout << "请输入内容：" << std::endl;
		getline(std::cin, str, '\n');
		int startPos = p->file->strat;
		p->file->openFlag = true;    //文件打开标记   
		p->file->length = str.size();
		if (p->file->length>p->file->maxlength)
		{
			std::cout << "写入字符串长度大于该文件的总长度,写入失败" << std::endl;
			system("pause");
			return;
		}
		for (i = startPos, k = 0; k<int(str.size()); ++i, ++k)
			disk[i] = str[k];
		std::cout << "文件写入成功,请用close命令将该文件关闭" << std::endl;
		system("pause");
	}
	else
	{
		std::cout << "没有找到该文件,请检查输入的文件名是否正确" << std::endl;
		system("pause");
	}
}

void fileFine(std::string fileName)
{
	UFD *p;
	UFD *q = userTable[userID].user;
	for (p = q->next; p != nullptr; p = p->next)
	{
		if (p->file->fileName==fileName)
			break;
	}
	if (p)
	{
		std::cout
			<< "********************************************" << std::endl
			<< "文件名：" << p->file->fileName << std::endl
			<< "文件长度：" << p->file->maxlength << std::endl
			<< "文件在存储空间的起始地址：" << p->file->strat << std::endl
			<< "文件类型：" << p->file->fileKind << std::endl
			<< "创建时间：" << asctime(p->file->timeinfo) << std::endl
			<< "********************************************" << std::endl;
		system("pause");
	}
	else
	{
		std::cout<<"没有找到该文件,请检查输入的文件名是否正确"<<std::endl;
		system("pause");
	}
}

void chmod(std::string fileName, std::string kind)
{
	UFD *p;
	UFD *q = userTable[userID].user;
	for (p = q->next; p != nullptr; p = p->next)
	{
		if (p->file->fileName==fileName)
			break;
	}
	if (p)
	{
		p->file->fileKind=kind;
		std::cout<<"修改文件类型成功"<<std::endl;
		system("pause");
	}
	else
	{
		std::cout << "没有找到该文件,请检查输入的文件名是否正确" << std::endl;
		system("pause");
	}
}

void fileRen(std::string fileName, std::string name)
{
	UFD *p;
	UFD *q = userTable[userID].user;
	for (p = q->next; p != nullptr; p = p->next)
	{
		if (p->file->fileName==fileName)
			break;
	}
	if (p)
	{
		while (q->next)
		{
			if (q->next->file->fileName==name)
			{
				std::cout<<"您输入的文件名已存在,重命名失败"<<std::endl;
				system("pause");
				return;
			}
			q = q->next;
		}
		p->file->fileName=name;
		std::cout << "重命名成功" << std::endl;
		system("pause");
	}
	else
	{
		std::cout << "没有找到该文件,请检查输入的文件名是否正确" << std::endl;
		system("pause");
	}
}

void fileDir(std::string userName)
{
	int i, k = 0;
	for (i = 0; i<MaxUser; ++i)
	{
		if (userTable[i].userName==userName)
		{
			k = i;
			break;
		}
	}
	if (i == MaxUser)
	{
		std::cout<<"没有找到该用户，请检查输入用户名是否正确"<<std::endl;
		system("pause");
		return;
	}
	UFD *p = userTable[k].user->next;
	std::cout << "********************************************************************************" << std::endl;
	std::cout << "文件名  文件长度  文件在磁盘的起始地址  文件类型  创建时间" << std::endl;
	for (; p != nullptr; p = p->next)
		std::cout << p->file->fileName << '\t' << p->file->maxlength << '\t' << p->file->strat << '\t' << p->file->fileKind << '\t' << asctime(p->file->timeinfo) << std::endl;
	std::cout << "********************************************************************************" << std::endl;
	system("pause");
}

void diskShow()
{
	int i = 0, unusedDisk = 0;
	std::cout<<"***************************************************************************" << std::endl;
	std::cout <<"  盘块号    起始地址       容量(bit)   是否已被使用" << std::endl;
	for (diskNode *p = diskHead; p != nullptr; p = p->next, ++i)
	{
		if (p->useFlag == false) 
			unusedDisk += p->maxlength;
		std::cout << "   " << i << "         " << p->start << "              " << p->maxlength << "          " << p->useFlag << std::endl;
	}
	std::cout << "***************************************************************************" << std::endl;
	std::cout <<"磁盘空间总容量：512*1024bit  已使用："<< MaxDisk - unusedDisk<<"bit   末使用："<< unusedDisk<<"bit" << std::endl << std::endl;
	system("pause");
}

void fileClose(std::string fileName)
{
	UFD *p;
	UFD *q = userTable[userID].user;
	for (p = q->next; p != nullptr; p = p->next)
	{
		if (p->file->fileName==fileName)
			break;
	}
	if (p)
	{
		p->file->openFlag = false;
		std::cout<<p->file->fileName<<"文件已关闭" << std::endl;
		system("pause");
	}
	else
	{
		std::cout << "没有找到该文件,请检查输入的文件名是否正确" << std::endl;
		system("pause");
	}
}
  

int main()
{
	_wsetlocale(LC_ALL, L"");
	std::string order[commandAmount];
	order[0] = "create";
	order[1] = "rm";
	order[2] = "cat";
	order[3] = "write";
	order[4] = "fine";
	order[5] = "chmod";
	order[6] = "ren";
	order[7] = "dir";
	order[8] = "close";
	order[9] = "return";
	order[10] = "exit";
	order[11] = "df";
	initDisk();//初始化磁盘   
	for (auto n = 0; n<MaxUser;++n)//初始化用户UFD目录文件的头指针   
	{
		userTable[n].user = new UFD;
		userTable[n].user->next = nullptr;
	}
	while (true)
	{
		std::cout << R"(
********************************************
1、创建用户
2、登录
********************************************
请输入数字:>)";
		int choice;
		std::cin>>choice;
		if (choice == 1) 
			userCreate();
		else if (choice == 2) 
			userID = login();
		else 
			std::cout<<"您的输入有误，请重新选择"<<std::endl;
		while (userID != -1)
		{
			std::cout << R"(
——————————————————————————————————————————————————————————————————————————————
 create-创建 格式：create a1 1000 rw,将创建名为a1,长度为1000字节可读可写的文件
 rm-删除 格式：rm a1,将删除名为a1的文件
 cat-查看文件内容 格式：cat a1,显示a1的内容
 write-写入  格式：write a1
 fine-查询 格式：fine a1 ,将显示文件 a1的属性
 chmod-修改 格式：chmod a1 r,将文件a1的权限改为只读方式
 ren-重命名 格式：ren a1 b1 ,将a1改名为b1
 dir-显示文件 格式：dir aaa,将显示aaa用户的所有文件
 df-显示磁盘空间使用情况 格式：df
 close-关闭文件 格式：close a1,将关闭文件a1
 return-退出用户，返回登录界面
 exit-退出程序
———————————————————————————————————————————————————————————————————————————————
请输入命令:>)";
			std::string command, command_str1, command_str2, command_str3, command_str4;
			getline(std::cin, command, '\n');
			int select = 0;
			int i,j;
			for (i = 0; command[i] != ' '&&i<command.size(); ++i)//command_str1字符串存储命令的操作类型   
				command_str1.push_back(command[i]);
			for (j = 0; j<commandAmount; ++j)
			{
				if (command_str1==order[j])
				{
					select = j;
					break;
				}
			}
			if (j == commandAmount)
			{
				std::cout << "您输入的命令有误，请重新输入" << std::endl;
				continue;
			}
			for (++i; command[i] != ' '&&i<command.size(); ++i)//commmand_str2字符串存储文件名或用户名   
				command_str2.push_back(command[i]);
			switch (select)
			{
			case 0:
				for (++i; command[i] != ' '; ++i)
					command_str3.push_back(command[i]); 
				for (++i; command[i] != ' '&&i<command.size();++i)
					command_str4.push_back(command[i]);
				fileCreate(command_str2, StringToNum<int>(command_str3), command_str4);
				break;
			case 1:
				fileDel(command_str2);
				break;
			case 2:
				fileCat(command_str2);
				break;
			case 3:
				fileWrite(command_str2);
				break;
			case 4:
				fileFine(command_str2);
				break;
			case 5:
				for (++i; command[i] != ' '&&i<command.size(); ++i)
					command_str3.push_back(command[i]);
				chmod(command_str2, command_str3);
				break;
			case 6:
				for (++i; command[i] != ' '&&i<command.size(); ++i)
					command_str3.push_back(command[i]);
				fileRen(command_str2, command_str3);
				break;
			case 7:
				fileDir(command_str2);
				break;
			case 8:
				fileClose(command_str2);
				break;
			case 9:
				for (UFD *p = userTable[userID].user->next; p != nullptr; p = p->next)//退出用户之前关闭所有打的文件   
					if (p->file->openFlag)
						p->file->openFlag = false;
				system("cls");
				userID = -1;
				break;
			case 10:
				exit(0);
				break;
			case 11:
				diskShow();
				break;
			default: ;
			}
		}
	}
	return 0;
}