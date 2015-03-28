#include "problem.h"
#include "solution.h"
#include <cstdlib>
#include <iostream>
#include <string>
#include <ctime>
using namespace std;

int main(int argc, char** argv)
{

	string no = "5";
	cout<<"请输入数据文件名：\n";
	cin>>no;
	string s = "./dataSet/gdb" + no + ".dat";
	char* fileName = const_cast<char *>(s.c_str() ); 
	unsigned int seed  = 100;
	
	//ofstream result("result.out", ios::app);
	srand(seed);	
	Problem.CreateCARPInstance(fileName);	//生成具体的carp问题	
   
	TCARPSolution *ptr_solution = new TCARPSolution;
	//ptr_solution->Split();
	//ptr_solution->PrintSolution();
	//cout<<endl<<"@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"<<endl;
	//ptr_solution->Shift();
	//ptr_solution->PrintSolution();
	//cout<<endl<<"##################################################"<<endl;
	//ptr_solution->Convert();
	//ptr_solution->PrintSolution();
	//cout<<endl<<"$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$"<<endl;
	//TCARPSolution shake;
	//shake = ptr_solution->Shaking(5);//参数k
	//shake.PrintSolution();
	//ptr_solution->Exchange();
	//ptr_solution->PrintSolution();
	//cout<<endl<<"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"<<endl;
	//ptr_solution->Reverse();
	//ptr_solution->PrintSolution();
	//cout<<endl<<"^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^"<<endl;
	//ptr_solution->Cross();
	//ptr_solution->CheckSolution();
	//ptr_solution->PrintSolution();
	//cout<<endl<<"&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"<<endl;
	//ptr_solution->Swap();
	//ptr_solution->CheckSolution();
	//ptr_solution->PrintSolution();
	//cout<<endl<<"********************************************************************"<<endl;
	//ptr_solution->Relocate();
	//ptr_solution->PrintSolution();
	ptr_solution->VNS(25);
	delete ptr_solution;
	system("pause");
	return 0;
}
