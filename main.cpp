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
	cout<<"�����������ļ�����\n";
	cin>>no;
	string s = "./dataSet/gdb" + no + ".dat";
	char* fileName = const_cast<char *>(s.c_str() ); 
	unsigned int seed  = 100;
	
	//ofstream result("result.out", ios::app);
	srand(seed);	
	Problem.CreateCARPInstance(fileName);	//���ɾ����carp����	
   
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
	//shake = ptr_solution->Shaking(5);//����k
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
