#ifndef SOLUTION_H
#define SOLUTION_H
#include "problem.h"
#include "route.h"

class TCARPSolution
{
	private:
		vector<int> large_tour;
		typedef list<int>::iterator liIter;   

	protected:
		double cost;
		double delivery;
		double pick_up;
		
	public:
		vector<TSingleRoute> allRoutes;
		/** Constructs new random solution */
		TCARPSolution();
	
		TCARPSolution(TCARPSolution const& oCARPSolution)
		{
			cost = oCARPSolution.cost;
			delivery = oCARPSolution.delivery;
			pick_up = oCARPSolution.pick_up;
			allRoutes = oCARPSolution.allRoutes;
    		}
		TCARPSolution &operator=(const TCARPSolution &oCARPSolution)
		{
			cost = oCARPSolution.cost;
			delivery = oCARPSolution.delivery;
			pick_up = oCARPSolution.pick_up;
			allRoutes = oCARPSolution.allRoutes;
			return *this;
		}

		int GetPreviousEdge(list<int>& route, liIter iter);

		int GetSuccesiveEdge(list<int>& route, liIter iter);

		double GetSolutionCost()
		{
			return cost;
		}
		double CalcuateCost(int pre_edge, int suc_edge, list<int>& segment);
		double CalculateDelivery(list<int>& segment);
		double CalculatePick_up(list<int>& segment);
		void EvaluateSegment(list<int>& segment, double& delivery, double& pick);
		void Split();
		
		void EvaluateSolution();

		void CalculateDeletion(list<int>& route, liIter iter, double& delete_cost);
		
		bool CalculateInsertion(int insert_edge, list<int>&route, int route_num, 
									liIter iter, double& cost);
		void ShiftTwoRoutes(list<int>& route1, int route_num1, 
							list<int>& route2, int route_num2);
		void Shift();
		
		void Convert();

		bool CheckFeasibility(list<int>& route, double start_load);

		void CheckSolution();

		void ShakOneRoute(int k, int route_num);
		
		TCARPSolution* Shaking(int k);
		
		bool TestBestExchange(list<int>& route1, int route_num1, liIter& iter1, 
						list<int>& route2, int route_num2, liIter& iter2);
		bool DoBestExchange(list<int>& route1, int route_num1,
						list<int>& route2, int route_num2);

		void Exchange();

		bool TestBestReverse(list<int>& route, int route_num, liIter& it_begin, liIter& it_end);
		
		void Reverse();
		
		bool TestBestCross(list<int>& route1, int route_num1, liIter& it1, 
								list<int>& route2, int route_num2, liIter& it2);
		bool CrossTwoRoutes(list<int>& route1, int route_num1, 
								list<int>& route2, int route_num2);
		void Cross();

		bool TestBestSwap(list<int>& route1, int route_num1, liIter& it1, 
								list<int>& route2, int route_num2, liIter& it2);
		bool SwapTwoRoutes(list<int>& route1, int route_num1, 
								list<int>& route2, int route_num2);
		void Swap();
		void TwoOpt();

		bool TestBestInsertion(list<int>& route, list<int>& segment, double& best_cost,
								double old_cost, double start_load, int& insert_position);
		
		bool RelocateSegment(list<int>& route, int route_num,  int length);

		void Relocate();
		
		void LocalSearch();

		void VNS(int k_max);
		
		void PrintSolution();
		
};
//////////////////////////////////////////////////////////////////////////////
inline int TCARPSolution::GetPreviousEdge(list<int>& route, liIter iter)
{
	int preEdge;
	return preEdge = ( *(route.begin()) == *iter ) ? Problem.depotEdge : *(--iter);
}
//////////////////////////////////////////////////////////////////////////////
inline int TCARPSolution::GetSuccesiveEdge(list<int>& route, liIter iter)
{
	int sucEdge;
	return  sucEdge = ( *(route.rbegin() ) == *iter ) ? Problem.depotEdge : *(++iter);
}
//////////////////////////////////////////////////////////////////////////////
inline double TCARPSolution::CalcuateCost(int previousEdge, int succesiveEdge, list<int>& segment)
{
	double seg_cost = 0;
	for(liIter iter = segment.begin(); iter != segment.end(); previousEdge = *(iter++) )
		seg_cost += Problem.pathsDistanceMatrix[previousEdge][*iter] + Problem.edges[*iter]->cost;		
	seg_cost += Problem.pathsDistanceMatrix[previousEdge][succesiveEdge];
	return seg_cost;
}

//////////////////////////////////////////////////////////////////////////////
inline double TCARPSolution::CalculateDelivery(list < int > & segment)
{
	double tmp_delivery = 0;
	for(liIter it = segment.begin(); it !=segment.end(); ++it)
	{
		tmp_delivery += Problem.edges[*it]->delivery;
	}
	return tmp_delivery;
}

//////////////////////////////////////////////////////////////////////////////
inline double TCARPSolution::CalculatePick_up(list < int > & segment)
{
	double tmp_pick = 0;
	for(liIter it = segment.begin(); it !=segment.end(); ++it)
	{
		tmp_pick += Problem.edges[*it]->pick_up;
	}
	return tmp_pick;
}

//////////////////////////////////////////////////////////////////////////////
inline void TCARPSolution::EvaluateSegment(list < int > & segment, double & delivery, double & pick)
{
	delivery = 0;
	pick = 0;
	for(liIter it = segment.begin(); it !=segment.end(); ++it)
	{
		delivery += Problem.edges[*it]->delivery;
		pick += Problem.edges[*it]->pick_up;
	}
}
//////////////////////////////////////////////////////////////////////////////
inline void TCARPSolution::CalculateDeletion(list < int > & route, liIter iter, double & delete_cost)
{
	if(*iter >= Problem.nTotalEdges)
	{
		cout<<"error in calculateDeletion!\n";
		system("pause");
	}
	int pre_edge = GetPreviousEdge(route, iter);
	int suc_edge = GetSuccesiveEdge(route, iter);
	delete_cost = Problem.pathsDistanceMatrix[pre_edge][*iter]
				+ Problem.edges[*iter]->cost
				+ Problem.pathsDistanceMatrix[*iter][suc_edge]
				- Problem.pathsDistanceMatrix[pre_edge][suc_edge];
}

//////////////////////////////////////////////////////////////////////////////
inline bool TCARPSolution::CalculateInsertion(int insert_edge, list < int > & route, 
								int routeNO, liIter iter, double & cost)
{
	cost = 0;
	bool b_feasible;
	
	double old_delivery = allRoutes[routeNO].GetTourDelivery();
	double old_pick = allRoutes[routeNO].GetTourPick_up();
	double old_cost = allRoutes[routeNO].GetTourCost();
	if(old_delivery + Problem.edges[insert_edge]->delivery > Problem.capacity
		|| old_pick + Problem.edges[insert_edge]->pick_up > Problem.capacity)
	{
		b_feasible = false;
		return b_feasible;
	}
	b_feasible = true;
	int pre_edge = GetPreviousEdge(route, iter);
	int suc_edge = (iter == route.end() ? Problem.depotEdge : *iter);
	cost = Problem.pathsDistanceMatrix[pre_edge][insert_edge] 
		+ Problem.edges[insert_edge]->cost
		+ Problem.pathsDistanceMatrix[insert_edge][suc_edge]
		- Problem.pathsDistanceMatrix[pre_edge][suc_edge];
	return b_feasible;
}

//////////////////////////////////////////////////////////////////////////////
inline bool TCARPSolution::CheckFeasibility(list < int > & route, double start_load)
{
	double total_load = start_load;
	for(liIter it = route.begin(); it != route.end(); ++it)
	{
		total_load += Problem.edges[*it]->pick_up - Problem.edges[*it]->delivery;
		if(total_load > Problem.capacity)
		{
			return false;
		}
	}
	return true;
}
//////////////////////////////////////////////////////////////////////////////
inline void TCARPSolution::CheckSolution()
{
	int route_size  = allRoutes.size();
	list<int> route;
	//bool b_feasible;
	for(int i = 0; i != route_size; ++i)
	{
		allRoutes[i].GetRoute(route);
		if(!(CheckFeasibility(route, allRoutes[i].GetTourDelivery())))
		{
			cout<<"error: this solution is not strongly feasible\n";
			system("pause");
		}
	}
}
#endif 
