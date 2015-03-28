/*
  Name: 
  Copyright: 
  Author: 
  Date: 01-06-12 19:04
  Description: 
*/
#if !defined(AFX_TCARPPROBLEM_H)
#define AFX_TCARPPROBLEM_H

#include <vector>
#include <list>
#include <set>
#include <string>

#include <algorithm>
#include <limits>	
#include <utility>	
	
#include <iostream>		
#include <fstream>		

#include "edge.h"
using namespace std;

class Edge
{
public:
		
	int ID; //ID编号顺序为： nEdges(包含服务与非服务边),nVehicles
	int from;
	int to;
	double cost;				//
	double demand;				//服务弧的需求 
	double delivery;           //in SPD
	double pick_up;            //in SPD 
	double deadHeadingTime;		//非服务弧的空载时间 
	double treatmentTime;		//
	bool bCulDeSac;
	bool bOneWay;				//是否为单向弧 
	bool bServed;				//是否已服务过 
	//specialTurnForm specialTurn;
	//specificRoadType roadType;
	bool bexmain;		//examined or not
	vector<int> NeighborList;
	double vCurrentDemand;//某条route中直到此边为止已经有的需求
	int routeNoBelong;//route number belonging of each edge
};
/** Instance of the CARP problem */
class TCARPProblem
{

// TSingleSolution and TCARPSolution are granted full access to memebers of TCARPProblem */
friend class TSingleRoute;
friend class TCARPSolution;
friend class VNS;
protected:
    //程序双向处理后的各边编号依次是：服务边、车辆、非服务边，然后是双向处理后的边 
	int depot;         // 车场结点的个数node n、编号umber of the depot 
	int depotEdge;	   // 车场边的编号the depot edge number
	int nVertices;     // 顶点的个数the number of Vertices 
	int nReqEdges;     // 服务边的条数the number of Requested Edges
	int nUnreqEdges;   // 非服务边的条数the number of Unreqested Edges
	int nEdges;		   // 转化前该网络的所有边数the number of all edges in the network
	int nVehicles;     // 车辆的数量the number of Vehicles
	int nOneWayEdges;  // 所有的单向边条数the number of all one-way edges
	int nReqOnewayEdges; // 有需求的单向边条数 the number of requied one-way edges
	int nEdgAdVeh;	   //总边数+车辆数the number of all edgs plus number of vehicles
	int nRqedgAdVeh;	//需求边数+车辆数the number of requied edgs plus number of vehicles
	int nTotalEdges;    //所有的边 ,包括双重边的总边数 
	double totalReq; 	//网络的总需求 
	double capacity;	//齐次车队车辆的统一容量,	
	vector<double> capacities; // 对非齐次车队使用,记录不同容量,目前未用; 
	
	vector<Edge *> edges;	//所有边,包括双重边 
	
	int  **pathMatrix;	  	//最短路径矩阵，即P(u,v) 
	
	double **pathsDistanceMatrix;	//最短距离矩阵，即D(u,v) 
	double **deadHeadingTime;	//两条边之间的空载行驶时间 即第一个定义式 
	
	vector<list<int>* > inNeighbor;		//每个顶点的进边集	
	vector<list<int>* > outNeighbor;	//每个顶点的出边集 
	vector<list<int>* > pathNeighbor;   //每条边的出边集
	vector<pair<int,int> > noTurns;	    //禁止转弯集合no-turn集合 
	vector<pair<int,int> > mandatoryTurns;//强制转弯集合mandatory-turn集合
	//vector<int> mandatoryTurnStartingEnds;
	
	vector<int> eEdgeRMap;	//从现有编号到原始数据中边的编号的映射 
	vector<int> dualEdgeMap;	// 双向边匹配，可正反互查quickly find the mapping between each dual edges

	vector<int> activeEdgeList;
	
   	void AllocateMemory(); 	//为各种量分配内存 
	
	virtual void ReadEachEdge(ifstream &Source, int edgeNO);//读入某条边 
			            
	virtual void ReadEdgeData(ifstream &Source, bool bRequiredEdges);//读入所有边 
	     
	virtual void ReadTurns(istream &Source, const string& turnType);//读入turn 
		
	virtual void SetVehicleEdges();	//对所有vehicle edge设置 
	
	// Loads the instance data from a file with the given name 
	virtual bool Load(char* fileName);//read the file line by line 

	/*consider inwards and outwards edge sepearatly for each node i, 
	  and store their numbers in inNeighbor[] and outNeighbor[] arraies */
    void SetNodeNeighbors();
	
	//for each path, find the outwards paths  
	void SetPathNeighbors();
			
	//calculate the distance between each pair of paths by using Dijkstras algorithm  	  
	void dijkstrasAlgForPaths(fstream&, fstream&);
	//virtual void dijkstrasAlgForPaths();
	
	//read the distance and path information from files 	
	void ReadMatrix(fstream&, fstream&);
	//virtual void ReadMatrix();
    	
public:
    //TCARPProblem();
    //TCARPProblem& TCARPProblem(const TCARPProblem& problem);
  	//将dual edge的大编号转换为原号.  
 	//06-04-24,25 
	int SetEdgeNO(int edgeNO)
	{	
		return ( edgeNO >= nEdgAdVeh ) ? dualEdgeMap[edgeNO] : edgeNO;
	}
	int GetVehicles()
	{
		return nVehicles;
	}
    //从最短路径矩阵中读出edgeA和edgeB之间的所有边，存入list容器 
	void GetShortestPath(int edgeA, int edgeB, list<int>& shortestPath);
	
	//从最短路径矩阵中读出edgeA和edgeB之间所有的服务边，并存入list容器 
	void GetShortestRequiredPath(int edgeA, int edgeB, list<int>& shortestRequiredPath);
	
	//计算出 edgeA与edgeB之间的空载时间，与论文中定义式(1)类似 
	double GetDeadheadingTime(int edgeA, int edgeB);

	//返回
	void GetNeighborListofEdge();
	//产生具体carp例子		
	void CreateCARPInstance(char *fileName);
};


/** Declaration of external Problem variable (defined in TMOKPProblem.cpp)
*	makes it accessible to all modules that include this header */
//将Problem申明为外部的，所有包含此头文件的程序均可使用此对象 
extern TCARPProblem Problem;


////////////////////////////////////////////////////////////////////////////////
///////////////////////        内联函数定义           //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 得到edgeA与edgeB之间的最短路径(不包括这两条边)，并将其存放在shortestPath中 
//06-04-24, 25
inline void TCARPProblem::GetShortestPath(int edgeA, int edgeB, list<int>& shortestPath)
{
	double distance = 0;
	
	//用pathMatrix,从edgeB开始反读出最短路径,直到读到edgeA
	int iTmp = pathMatrix[edgeA][edgeB];	//读出pathMatrix中的记录
	//cout << edgeA << edgeB << iTmp <<endl;

    //后一个判断条件是为了避免在有多个车辆边的情况下,出现两个车辆边被认为不等的情况
    //while( (iTmp!= edgeA) && !( (edgeA >= nReqEdges) && (edgeA < nRqedgAdVeh) 
	//&& (iTmp >= nReqEdges) && (iTmp < nRqedgAdVeh) ) )
	while((iTmp!=edgeA)&& /*edgeA及iTmp都不是车辆边*/
	((edgeA<nReqEdges)||(edgeA>=nRqedgAdVeh)||(iTmp<nReqEdges)||(iTmp>=nRqedgAdVeh)))
	{
		if( -1 == iTmp ) 
		{
			cout << "wrong!! No path between " << edgeA << "  and  " << edgeB;
			system("PAUSE");
		}
		
		/*
		//测试用 
		if( edgeA == 51 && edgeB == 4 )
			cout<< iTmp << '\t';
		//测试结束 
		*/
		//cout << "i1=  " << iTmp << "   ";
		
		shortestPath.push_front(iTmp);	
		
		//system("PAUSE");   
		distance +=  edges[iTmp]->cost;	   	   	   	   	   
		iTmp = pathMatrix[edgeA][iTmp];	//沿着path反向依次读出pathMatrix中的记录
		
		//cout << "i2=  " << iTmp << "   ";
	}
	
	//debug, 测试读出的路径正确否 
	if( (int)distance != (int)pathsDistanceMatrix[edgeA][edgeB])
	{
		cout << "  wrong distance  " << endl;
		cout << distance << "   " << pathsDistanceMatrix[edgeA][edgeB] << endl;
		//cout << SetEdgeNO(edgeA) << '\t' << SetEdgeNO(edgeB) << endl;
		cout << edgeA << '\t' << edgeB << endl;
	}
}


////////////////////////////////////////////////////////////////////////////////
//求两条任务边之间的空载时间 ,和上面程序完全类似,只是不返回具体的路径
//本程序将不再使用,直接从矩阵读出 
//06-04-25
//定义成内联函数的好处是：将该函数在每个调用点上“内联地展开”，从而消除了把它写成函数的开销 
inline double TCARPProblem::GetDeadheadingTime(int edgeA, int edgeB)
{
	double deadHeadingTime = 0;
	
	int iTmp = pathMatrix[edgeA][edgeB];
	//cout << edgeA << edgeB << iTmp <<endl;
	while( ( iTmp != edgeA )  && !( (edgeA >= nReqEdges) && (edgeA < nRqedgAdVeh) 
	                             && (iTmp >= nReqEdges) && (iTmp < nRqedgAdVeh) )  )
	{
		if( -1 == iTmp ) 
		{
			cout << "wrong!! No path between " << edgeA << "  and  " << edgeB;
			system("PAUSE");
		}
		
		//cout << "i1=  " << iTmp << "   ";
		deadHeadingTime += edges[iTmp]->deadHeadingTime;
		iTmp = pathMatrix[edgeA][iTmp];
	}
	return deadHeadingTime;
}


////////////////////////////////////////////////////////////////////////////////
// 得到edgeA与edgeB之间的最短路径中所有要服务的边,不包括这两条边 
//06-04-25
inline void TCARPProblem::GetShortestRequiredPath(int edgeA, int edgeB, list<int>& shortestRequiredPath)
{
	int iTmp;

	iTmp = pathMatrix[edgeA][edgeB];
	
	while(iTmp != edgeA)
	{
		//
		if(iTmp > numeric_limits<double>::max()/2) 
			printf("wrong!! No path at all\n");
		//仅保存服务边 
		if(iTmp < nReqEdges)
			shortestRequiredPath.push_front(iTmp);
		
		iTmp = pathMatrix[edgeA][iTmp];
	}
}

#endif 

