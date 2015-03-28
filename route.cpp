#include "route.h"

using namespace std;
////////////////////////////////////////////////////////////////////////////////
///////////////////      TSingleRoute ��Ա����                    //////////////
////////////////////////////////////////////////////////////////////////////////
//��route����������tour.��������Ӧ��ֵ
//list<int>route �д�ŵĽ��Ǹ�route�ķ���ߣ�����Ҫ��������������һ��·���������tour�С� 
void TSingleRoute::CreatTour()
{
	int anteEdge;
	int postEdge;
	list<int>::iterator liIter1, liIter2;
	list<int> liTmp;

	tour_cost = 0;
	tour_delivery = 0;
	tour_pick_up = 0;
	tour.clear();
	
	//��depot�߳��� 
	anteEdge = Problem.depotEdge; 
	
	//��������route 
	for(liIter1 = route.begin(); liIter1 != route.end(); liIter1++)
	{
		postEdge = *liIter1;
		
		liTmp.clear();	//��һ������Ҫ!	
		//�õ�anteEdge��postEdge֮��(���������˵�)���������·��,����liTmp��	
		Problem.GetShortestPath(anteEdge,postEdge,liTmp);// (anteEdge,postEdge)
		
		//�������liTmp�е����·�� 
		for(liIter2 = liTmp.begin(); liIter2 != liTmp.end(); liIter2++)
		{
			int edgeNO = *liIter2;
			tour.push_back(edgeNO);	//�������ı߼�¼���� 
			tour_cost += Problem.edges[edgeNO]->cost;
		}
		
		//�����һ����postEdge,����Ҫ����,����,tour�е�ÿһ��������ΪpostEdgeʱ������һ�� 
		tour.push_back(postEdge); //��¼ 
		tour_delivery += Problem.edges[postEdge]->delivery;
		tour_pick_up += Problem.edges[postEdge]->pick_up;
		tour_cost += Problem.edges[postEdge]->cost;
		
		anteEdge = postEdge;	//�ı�anteEdge,��߱�Ϊǰ�� 
	}
	//�������ڷ���depotEdge 
	postEdge = Problem.depotEdge;
	liTmp.clear();	
	//���������һ���߷���depot�����·��	
	Problem.GetShortestPath(anteEdge,postEdge,liTmp);
	
	for(liIter2 = liTmp.begin(); liIter2 != liTmp.end(); liIter2++)
	{
		int edgeNO = *liIter2;
		tour.push_back(edgeNO);	//�������ı߼�¼���� 
		tour_cost += Problem.edges[edgeNO]->cost;
	}
	//depot�����账��,���� 
}

///////////////////////////////////////////////////////////////////////////
void TSingleRoute::CalculateMin_MaxLoad()
{
	double total_load = start_load;
	max_load = 0;
	min_load = numeric_limits<double>::max();
	for(list<int>::iterator it = route.begin(); it != route.end(); ++it)
	{
		total_load += Problem.edges[*it]->pick_up - Problem.edges[*it]->delivery;
		//cout<<"t_load: "<<total_load;
		//cout<<"\t-d: "<<Problem.edges[*it]->delivery<<"\t+p: "<<Problem.edges[*it]->pick_up<<endl;
		if(max_load < total_load)
		{
			max_load = total_load;
		}
		if(total_load < min_load)
		{
			min_load = total_load;
		}
	}
}
///////////////////////////////////////////////////////////////////////////
bool TSingleRoute::ConvertSegment(list < int > & segment)
{
	list<int> lag_edges;
	lag_edges.clear();
	double total_load = start_load;
	//cout<<"t_load: "<<total_load;
	for(list<int>::iterator it = segment.begin(); it != segment.end(); ++it)
	{
		total_load += Problem.edges[*it]->pick_up - Problem.edges[*it]->delivery;
		
		//cout<<"\t-d: "<<Problem.edges[*it]->delivery<<"\t+p: "<<Problem.edges[*it]->pick_up;
		//cout<<"\tt_load: "<<total_load<<endl;
		if(total_load > Problem.capacity)
		{
			total_load -= Problem.edges[*it]->pick_up - Problem.edges[*it]->delivery;
			lag_edges.push_back(*it);
		}
	}
	if(lag_edges.empty())//show that segment is already strong-feasible
	{
		return true;
	}
	for(list<int>::iterator it = lag_edges.begin(); it != lag_edges.end(); ++it)
	{
		list<int>::iterator seg_iter = find(segment.begin(), segment.end(), *it);
		if(seg_iter == segment.end())
		{
			cout<<"error in convertSegment!\n";
			system("pause");
		}
		segment.erase(seg_iter);
	}
	segment.insert(segment.end(), lag_edges.begin(), lag_edges.end());
	//cout<<"lag_edges.size = "<<lag_edges.size();
	//system("pause");
	return false;
}
///////////////////////////////////////////////////////////////////////////
void TSingleRoute::ConvertOneRoute()
{
	if(route.empty())
	{
		cout<<"error: route is empty!"<<endl;
		system("pause");
	}
	CalculateMin_MaxLoad();
	//if it is feasible originally, return
	if(max_load <= Problem.capacity)
	{
		//cout<<"route_num: "<<route_num<<" is feasible"<<endl;
		return;
	}
	bool b_feasible = false;
	while(!b_feasible)
	{
		b_feasible = ConvertSegment(route);
	}
	
	CalculateMin_MaxLoad();
	//test:
	if(max_load > Problem.capacity)
	{
		cout<<"error at the end of ConvertOneRoute!\n";
		system("pause");
	}
}
