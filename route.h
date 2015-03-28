#ifndef ROUTE_H
#define ROUTE_H

#include "problem.h"

class TSingleRoute 
{
protected:

    //route�д��һ�����߹������з���ߣ���������������
	list<int> route;
    //tour�д��һ�����߹������бߣ� ������������ 
	list<int> tour; 
	
	double tour_cost;
	double tour_delivery;
	double tour_pick_up;

	double max_load;
	double min_load;
	double start_load;
	//bool bFeasible;
	
public:
	
	TSingleRoute() {};	//ֻ��������vector�� 
	
	//�������캯�� 
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
	
	//��route����������tour.��������Ӧ��ֵ
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

//��������segment��cost��demand 
inline void TSingleRoute::EvaluateSegment(int previousEdge, int succesiveEdge, 
                            list<int>& segment, double& delivery, double& pick_up, double& cost)
{
	delivery = 0;
	pick_up = 0;
	cost = 0;
	if(segment.empty())
		return;
	int preEdge = previousEdge;
	int sucEdge;//sucEdge�������浱ǰ�� 
	list<int>::iterator liIter;
	
	//��������route 
	for(liIter = segment.begin(); liIter != segment.end(); liIter++)
	{
		sucEdge = *liIter;
		
		delivery += Problem.edges[sucEdge]->delivery;
		pick_up += Problem.edges[sucEdge]->pick_up;
		//cost�����Ķ���ʽ(3)
		cost += Problem.pathsDistanceMatrix[preEdge][sucEdge] + Problem.edges[sucEdge]->cost; 
		//�ı�preEdge,��߱�Ϊǰ�� 
		preEdge = sucEdge;	
	}
	sucEdge = succesiveEdge;
	cost += Problem.pathsDistanceMatrix[preEdge][sucEdge];
}


#endif
