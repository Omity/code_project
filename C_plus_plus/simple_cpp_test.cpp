#include <iostream>
#include <string>

namespace Diy{
	using namespace std;
	class Student{
		public:
			const char *name;
			int        age;
			float      score;
		public:
			void print(){
				cout << name << " is " << age << " years old and get " << score << endl;
				}
	};
}

int main()
{
	using namespace std;
	Diy::Student stu1;
	string str;
	stu1.name = "星星";
	stu1.age = 12;
	stu1.score = 87.5;
	stu1.print();
	cout << "请输入一个数字:";
	cin >> str;
	cout << "你输入了:" << str << endl;
}
