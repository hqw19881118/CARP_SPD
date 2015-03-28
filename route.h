#ifndef ROUTE_H
#define ROUTE_H

#include "problem.h"

class TSingleRoute 
{
protected:

    //route中存放一辆车走过的所有服务边，但不包括车场边
	list<int> route;
    //tour中存放一辆车走过的所有边， 不包括车场边 
	list<int> tour; 
	
	double tour_cost;
	double tour_delivery;
	double tour_pick_up;

	double max_load;
	double min_load;
	double start_load;
	//bool bFeasible;
	
public:
	
	TSingleRoute() {};	//只用于生成vector用 
	
	//拷贝构造函数 
	TSingleRoute(TSingleRoute const& oSingleRoute)
	{
		route = oSingleRoute.route;
		tour = oSingleRoute.tour;
		tour_cost = oSingleRoute.tour_cost;
		tour_delivery = oSingleRoute.tour_delivery;
		tour_pick_up = oSingleRoute.tour_pick_up;
		max_load = oSingleRoute.max_load;
		min_load = oSingleRoute.min_load;
		start_load = oSingleRoute.start_load;
	}
	TSingleRoute& operator=(const TSingleRoute & oSingleRoute)
	{
		route = oSingleRoute.route;
		tour = oSingleRoute.tour;
		tour_cost = oSingleRoute.tour_cost;
		tour_delivery = oSingleRoute.tour_delivery;
		tour_pick_up = oSingleRoute.tour_pick_up;
		max_load = oSingleRoute.max_load;
		min_load = oSingleRoute.min_load;
		start_load = oSingleRoute.start_load;
	}
	void GetRoute(list<int>& gotRoute)
	{
		gotRoute = route;
	}
	
	void GetTour(list<int>& gotTour)
	{
		gotTour = tour;
	}
	double GetTourCost()
	{
		return tour_cost;
	}
	
	double GetTourDelivery()
	{
		return tour_delivery;
	}

	double GetTourPick_up()
	{
		return tour_pick_up;
	}
	double GetMaxLoad()
	{
		return max_load;
	}
	double GetMinLoad()
	{	
		return min_load;
	}
	double GetStartLoad()
	{
		return start_load;
	}
	void SetTourCost(double c)
	{
		tour_cost=c;
	}
	void SetTourDelivery(double d)
	{
		tour_delivery = d;
	}
	void SetTourPick_up(double p)
	{
		tour_pick_up = p;
	}
	void SetStartLoad(double s_load)
	{
		start_load = s_load;
	}
	void SetRoute(list<int>& oRoute)
	{
		route = oRoute;
	}
	void CalculateMin_MaxLoad();

	bool ConvertSegment(list<int>& segment);
	
	void ConvertOneRoute();
	
	//由route生成完整的tour.并计算相应的值
	void CreatTour();
		
	void EvaluateSegment(int previousEdge, int succesiveEdge, 
                        list<int>& segment, double& delivery, double& pick_up, double& cost);
	void EvaluateRoute()
	{
		EvaluateSegment(Problem.depotEdge,Problem.depotEdge, 
                                route, tour_delivery, tour_pick_up, tour_cost);
		start_load = tour_delivery;
		CalculateMin_MaxLoad();
	}
};

//计算整条segment的cost、demand 
inline void TSingleRoute::EvaluateSegment(int previousEdge, int succesiveEdge, 
                            list<int>& segment, double& delivery, double& pick_up, double& cost)
{
	delivery = 0;
	pick_up = 0;
	cost = 0;
	if(segment.empty())
		return;
	int preEdge = previousEdge;
	int sucEdge;//sucEdge用来保存当前边 
	list<int>::iterator liIter;
	
	//计算整个route 
	for(liIter = segment.begin(); liIter != segment.end(); liIter++)
	{
		sucEdge = *liIter;
		
		delivery += Problem.edges[sucEdge]->delivery;
		pick_up += Problem.edges[sucEdge]->pick_up;
		//cost见论文定义式(3)
		cost += Problem.pathsDistanceMatrix[preEdge][sucEdge] + Problem.edges[sucEdge]->cost; 
		//改变preEdge,后边变为前边 
		preEdge = sucEdge;	
	}
	sucEdge = succesiveEdge;
	cost += Problem.pathsDistanceMatrix[preEdge][sucEdge];
}


#endif
