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
		
	int ID; //ID���˳��Ϊ�� nEdges(����������Ƿ����),nVehicles
	int from;
	int to;
	double cost;				//
	double demand;				//���񻡵����� 
	double delivery;           //in SPD
	double pick_up;            //in SPD 
	double deadHeadingTime;		//�Ƿ��񻡵Ŀ���ʱ�� 
	double treatmentTime;		//
	bool bCulDeSac;
	bool bOneWay;				//�Ƿ�Ϊ���� 
	bool bServed;				//�Ƿ��ѷ���� 
	//specialTurnForm specialTurn;
	//specificRoadType roadType;
	bool bexmain;		//examined or not
	vector<int> NeighborList;
	double vCurrentDemand;//ĳ��route��ֱ���˱�Ϊֹ�Ѿ��е�����
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
    //����˫�����ĸ��߱�������ǣ�����ߡ��������Ƿ���ߣ�Ȼ����˫�����ı� 
	int depot;         // �������ĸ���node n�����umber of the depot 
	int depotEdge;	   // �����ߵı��the depot edge number
	int nVertices;     // ����ĸ���the number of Vertices 
	int nReqEdges;     // ����ߵ�����the number of Requested Edges
	int nUnreqEdges;   // �Ƿ���ߵ�����the number of Unreqested Edges
	int nEdges;		   // ת��ǰ����������б���the number of all edges in the network
	int nVehicles;     // ����������the number of Vehicles
	int nOneWayEdges;  // ���еĵ��������the number of all one-way edges
	int nReqOnewayEdges; // ������ĵ�������� the number of requied one-way edges
	int nEdgAdVeh;	   //�ܱ���+������the number of all edgs plus number of vehicles
	int nRqedgAdVeh;	//�������+������the number of requied edgs plus number of vehicles
	int nTotalEdges;    //���еı� ,����˫�رߵ��ܱ��� 
	double totalReq; 	//����������� 
	double capacity;	//��γ��ӳ�����ͳһ����,	
	vector<double> capacities; // �Է���γ���ʹ��,��¼��ͬ����,Ŀǰδ��; 
	
	vector<Edge *> edges;	//���б�,����˫�ر� 
	
	int  **pathMatrix;	  	//���·�����󣬼�P(u,v) 
	
	double **pathsDistanceMatrix;	//��̾�����󣬼�D(u,v) 
	double **deadHeadingTime;	//������֮��Ŀ�����ʻʱ�� ����һ������ʽ 
	
	vector<list<int>* > inNeighbor;		//ÿ������Ľ��߼�	
	vector<list<int>* > outNeighbor;	//ÿ������ĳ��߼� 
	vector<list<int>* > pathNeighbor;   //ÿ���ߵĳ��߼�
	vector<pair<int,int> > noTurns;	    //��ֹת�伯��no-turn���� 
	vector<pair<int,int> > mandatoryTurns;//ǿ��ת�伯��mandatory-turn����
	//vector<int> mandatoryTurnStartingEnds;
	
	vector<int> eEdgeRMap;	//�����б�ŵ�ԭʼ�����бߵı�ŵ�ӳ�� 
	vector<int> dualEdgeMap;	// ˫���ƥ�䣬����������quickly find the mapping between each dual edges

	vector<int> activeEdgeList;
	
   	void AllocateMemory(); 	//Ϊ�����������ڴ� 
	
	virtual void ReadEachEdge(ifstream &Source, int edgeNO);//����ĳ���� 
			            
	virtual void ReadEdgeData(ifstream &Source, bool bRequiredEdges);//�������б� 
	     
	virtual void ReadTurns(istream &Source, const string& turnType);//����turn 
		
	virtual void SetVehicleEdges();	//������vehicle edge���� 
	
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
  	//��dual edge�Ĵ���ת��Ϊԭ��.  
 	//06-04-24,25 
	int SetEdgeNO(int edgeNO)
	{	
		return ( edgeNO >= nEdgAdVeh ) ? dualEdgeMap[edgeNO] : edgeNO;
	}
	int GetVehicles()
	{
		return nVehicles;
	}
    //�����·�������ж���edgeA��edgeB֮������бߣ�����list���� 
	void GetShortestPath(int edgeA, int edgeB, list<int>& shortestPath);
	
	//�����·�������ж���edgeA��edgeB֮�����еķ���ߣ�������list���� 
	void GetShortestRequiredPath(int edgeA, int edgeB, list<int>& shortestRequiredPath);
	
	//����� edgeA��edgeB֮��Ŀ���ʱ�䣬�������ж���ʽ(1)���� 
	double GetDeadheadingTime(int edgeA, int edgeB);

	//����
	void GetNeighborListofEdge();
	//��������carp����		
	void CreateCARPInstance(char *fileName);
};


/** Declaration of external Problem variable (defined in TMOKPProblem.cpp)
*	makes it accessible to all modules that include this header */
//��Problem����Ϊ�ⲿ�ģ����а�����ͷ�ļ��ĳ������ʹ�ô˶��� 
extern TCARPProblem Problem;


////////////////////////////////////////////////////////////////////////////////
///////////////////////        ������������           //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// �õ�edgeA��edgeB֮������·��(��������������)������������shortestPath�� 
//06-04-24, 25
inline void TCARPProblem::GetShortestPath(int edgeA, int edgeB, list<int>& shortestPath)
{
	double distance = 0;
	
	//��pathMatrix,��edgeB��ʼ���������·��,ֱ������edgeA
	int iTmp = pathMatrix[edgeA][edgeB];	//����pathMatrix�еļ�¼
	//cout << edgeA << edgeB << iTmp <<endl;

    //��һ���ж�������Ϊ�˱������ж�������ߵ������,�������������߱���Ϊ���ȵ����
    //while( (iTmp!= edgeA) && !( (edgeA >= nReqEdges) && (edgeA < nRqedgAdVeh) 
	//&& (iTmp >= nReqEdges) && (iTmp < nRqedgAdVeh) ) )
	while((iTmp!=edgeA)&& /*edgeA��iTmp�����ǳ�����*/
	((edgeA<nReqEdges)||(edgeA>=nRqedgAdVeh)||(iTmp<nReqEdges)||(iTmp>=nRqedgAdVeh)))
	{
		if( -1 == iTmp ) 
		{
			cout << "wrong!! No path between " << edgeA << "  and  " << edgeB;
			system("PAUSE");
		}
		
		/*
		//������ 
		if( edgeA == 51 && edgeB == 4 )
			cout<< iTmp << '\t';
		//���Խ��� 
		*/
		//cout << "i1=  " << iTmp << "   ";
		
		shortestPath.push_front(iTmp);	
		
		//system("PAUSE");   
		distance +=  edges[iTmp]->cost;	   	   	   	   	   
		iTmp = pathMatrix[edgeA][iTmp];	//����path�������ζ���pathMatrix�еļ�¼
		
		//cout << "i2=  " << iTmp << "   ";
	}
	
	//debug, ���Զ�����·����ȷ�� 
	if( (int)distance != (int)pathsDistanceMatrix[edgeA][edgeB])
	{
		cout << "  wrong distance  " << endl;
		cout << distance << "   " << pathsDistanceMatrix[edgeA][edgeB] << endl;
		//cout << SetEdgeNO(edgeA) << '\t' << SetEdgeNO(edgeB) << endl;
		cout << edgeA << '\t' << edgeB << endl;
	}
}


////////////////////////////////////////////////////////////////////////////////
//�����������֮��Ŀ���ʱ�� ,�����������ȫ����,ֻ�ǲ����ؾ����·��
//�����򽫲���ʹ��,ֱ�ӴӾ������ 
//06-04-25
//��������������ĺô��ǣ����ú�����ÿ�����õ��ϡ�������չ�������Ӷ������˰���д�ɺ����Ŀ��� 
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
// �õ�edgeA��edgeB֮������·��������Ҫ����ı�,�������������� 
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
		//���������� 
		if(iTmp < nReqEdges)
			shortestRequiredPath.push_front(iTmp);
		
		iTmp = pathMatrix[edgeA][iTmp];
	}
}

#endif 

