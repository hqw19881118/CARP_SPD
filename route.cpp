#include "route.h"

using namespace std;
////////////////////////////////////////////////////////////////////////////////
///////////////////      TSingleRoute 成员函数                    //////////////
////////////////////////////////////////////////////////////////////////////////
//由route生成完整的tour.并计算相应的值
//list<int>route 中存放的仅是该route的服务边，下面要依此生成完整的一条路径，存放在tour中。 
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
	
	//从depot边出发 
	anteEdge = Problem.depotEdge; 
	
	//经过整个route 
	for(liIter1 = route.begin(); liIter1 != route.end(); liIter1++)
	{
		postEdge = *liIter1;
		
		liTmp.clear();	//这一步很重要!	
		//得到anteEdge和postEdge之间(不包括两端点)的整个最短路径,放在liTmp中	
		Problem.GetShortestPath(anteEdge,postEdge,liTmp);// (anteEdge,postEdge)
		
		//处理放在liTmp中的最短路径 
		for(liIter2 = liTmp.begin(); liIter2 != liTmp.end(); liIter2++)
		{
			int edgeNO = *liIter2;
			tour.push_back(edgeNO);	//将经过的边记录下来 
			tour_cost += Problem.edges[edgeNO]->cost;
		}
		
		//到达后一条边postEdge,它需要处理,这里,tour中的每一条边在作为postEdge时被处理一次 
		tour.push_back(postEdge); //记录 
		tour_delivery += Problem.edges[postEdge]->delivery;
		tour_pick_up += Problem.edges[postEdge]->pick_up;
		tour_cost += Problem.edges[postEdge]->cost;
		
		anteEdge = postEdge;	//改变anteEdge,后边变为前边 
	}
	//车辆现在返回depotEdge 
	postEdge = Problem.depotEdge;
	liTmp.clear();	
	//车辆从最后一条边返回depot的最短路径	
	Problem.GetShortestPath(anteEdge,postEdge,liTmp);
	
	for(liIter2 = liTmp.begin(); liIter2 != liTmp.end(); liIter2++)
	{
		int edgeNO = *liIter2;
		tour.push_back(edgeNO);	//将经过的边记录下来 
		tour_cost += Problem.edges[edgeNO]->cost;
	}
	//depot边无需处理,结束 
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
