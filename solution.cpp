#include <iostream>
#include "solution.h"

void PrintRoute(list<int>& route) {
    for(list<int>::iterator it = route.begin(); it != route.end(); ++it)
        cout<<*it<<" ";
    cout<<endl;
}
#if 0
TCARPSolution::TCARPSolution() {
    large_tour.clear();

    //生成边的有序列,以相同概率初始化每条dual边的方向
    for(int edgeNo = 0; edgeNo < Problem.nReqEdges; edgeNo++) {
        if( Problem.edges[edgeNo]->bOneWay ) {
            large_tour.push_back(edgeNo);
        } else {
            if( rand() > RAND_MAX/2 ) {
                large_tour.push_back(edgeNo);
            } else {
                large_tour.push_back(Problem.dualEdgeMap[edgeNo]);	//取边的反向
            }
        }
    }
    //随机重排得到初始染色体表示
    random_shuffle(large_tour.begin(), large_tour.end());
    //for(vector<int>::iterator it = large_tour.begin(); it != large_tour.end(); ++it)
    //	cout<<*it<<" ";
    //cout<<endl;
}
#endif
//////////////////////////////////////////////////////////////////////////////
#if 1
TCARPSolution::TCARPSolution() {
    large_tour.clear();

    list<int> customer_set;
    for(int i=0; i < Problem.nReqEdges; customer_set.push_back(i++));
    //PrintRoute(customer_set);
    int pre_edge = Problem.depotEdge;
    while(!customer_set.empty()) {
        double best_cost = numeric_limits<double>::max();
        liIter it, best_iter;
        int edge_num = -1;
        for(it = customer_set.begin(); it != customer_set.end(); ++it) {
            int edge = *it;
            if(Problem.pathsDistanceMatrix[pre_edge][edge] < best_cost) {
                best_cost = Problem.pathsDistanceMatrix[pre_edge][edge];
                best_iter = it;
                edge_num = edge;
            }
            int edge_rev = Problem.dualEdgeMap[edge];
            if(Problem.pathsDistanceMatrix[pre_edge][edge_rev] < best_cost) {
                best_cost = Problem.pathsDistanceMatrix[pre_edge][edge_rev];
                best_iter = it;
                edge_num = edge_rev;
            }
        }
        customer_set.erase(best_iter);
        large_tour.push_back(edge_num);
        pre_edge = edge_num;
    }

}
#endif
//////////////////////////////////////////////////////////////////////////////
void TCARPSolution::Split() {
    double max_delivery = 0;
    double max_pick = 0;
    Problem.capacities.clear();
    vector<int> split_point;

    for(int index = 0; index != Problem.nReqEdges; ++index) {
        int edgeNO = large_tour[index];
        if(max_delivery + Problem.edges[edgeNO]->delivery > Problem.capacity
                || max_pick + Problem.edges[edgeNO]->pick_up > Problem.capacity) {
            //one route split finished
            Problem.capacities.push_back(max_delivery);
            split_point.push_back(index);
            max_delivery = 0;
            max_pick = 0;
        }
        max_delivery += Problem.edges[edgeNO]->delivery;
        max_pick += Problem.edges[edgeNO]->pick_up;
    }
    //deal with the last route
    if(max_delivery == 0) {
        cout<<"error in split procedure!\n";
        system("pause");
    }
    Problem.capacities.push_back(max_delivery);
    split_point.push_back(-1);
    //construct routes
    int begin_index = 0;
    int end_index = split_point[0];
    int route_size = split_point.size();
    allRoutes.resize(route_size);
    list<int> tmp_route;
    int routeNO = 0;
    for(int index = 0; index != Problem.nReqEdges; ++index) {
        int edgeNO = large_tour[index];
        if(index != end_index) {
            tmp_route.push_back(edgeNO);
            continue;
        }
        ///
        allRoutes[routeNO].SetRoute(tmp_route);
        allRoutes[routeNO].EvaluateRoute();
        allRoutes[routeNO].CalculateMin_MaxLoad();
        ++routeNO;
        tmp_route.clear();
        tmp_route.push_back(edgeNO);
        end_index = split_point[routeNO];
    }
    allRoutes[routeNO].SetRoute(tmp_route);
    tmp_route.clear();
    allRoutes[routeNO].EvaluateRoute();
    allRoutes[routeNO].CalculateMin_MaxLoad();
    EvaluateSolution();
}

/////////////////////////////////////////////////////////////////////////////
void TCARPSolution::EvaluateSolution() {
    cost = 0;
    delivery = 0;
    pick_up = 0;
    int route_size = allRoutes.size();
    for(int i =0; i != route_size; ++i) {
        cost += allRoutes[i].GetTourCost();
        delivery += allRoutes[i].GetTourDelivery();
        pick_up += allRoutes[i].GetTourPick_up();
    }
}

//////////////////////////////////////////////////////////////////////////////
void TCARPSolution::ShiftTwoRoutes(list < int > & route1, int route_num1,
                                   list < int > & route2, int route_num2) {
    if(route1.empty()) { //route2 can be empty
        cout<<"error : route1 is empty"<<endl;
        system("pause");
    }

    double delivery1, delivery2;
    double pick1, pick2;
    double delete_cost, insert_cost, varied_cost;
    double best_cost = numeric_limits<double>::max();
    bool flag, b_feasible = false;
    liIter iter1, iter2, best_iter1, best_iter2;

    for(iter1= route1.begin(); iter1 != route1.end(); ++iter1) {
        int insert_edge = *iter1;
        CalculateDeletion(route1, iter1, delete_cost);
        for(iter2 = route2.begin(), flag = true; flag; ++iter2) {
            if(iter2 == route2.end()) {
                flag = false;
            }
            b_feasible = CalculateInsertion(insert_edge, route2, route_num2, iter2, insert_cost);
            if(!b_feasible) {
                continue;
            }
            varied_cost = insert_cost - delete_cost;
            if(varied_cost < best_cost) {
                best_cost = varied_cost;
                best_iter1 = iter1;
                best_iter2 = iter2;
            }
        }
        //may do shift here
    }
    //do shift here
    if(b_feasible) {
        route2.insert(best_iter2, *best_iter1);
        route1.erase(best_iter1);
        allRoutes[route_num1].SetRoute(route1);
        allRoutes[route_num2].SetRoute(route2);
        allRoutes[route_num1].EvaluateRoute();
        allRoutes[route_num2].EvaluateRoute();
    }
    //PrintRoute(route1);
    //PrintRoute(route2);
    //system("pause");
}
//////////////////////////////////////////////////////////////////////////////
void TCARPSolution::Shift() {
    int route_size = allRoutes.size();;
    int route_num1, route_num2;
    list<int> route1, route2;
    for(route_num1 = 0; route_num1 != route_size; ++route_num1) {
        for(route_num2 = 0; route_num2 != route_size; ++route_num2) {
            if(route_num1 == route_num2) {
                continue;
            }
            allRoutes[route_num1].GetRoute(route1);
            allRoutes[route_num2].GetRoute(route2);
            //do the best: delete one edge from route1, then insert it into route2
            ShiftTwoRoutes(route1, route_num1, route2, route_num2);
        }
    }
    EvaluateSolution();
}
/////////////////////////////////////////////////////////////////////////////
void TCARPSolution::Convert() {
    int route_size = allRoutes.size();
    list<int> tmp_route;
    for(int route_num = 0; route_num != route_size; ++route_num) {
        allRoutes[route_num].ConvertOneRoute();
        allRoutes[route_num].EvaluateRoute();
    }
    EvaluateSolution();
}

/////////////////////////////////////////////////////////////////////////////
void TCARPSolution::ShakOneRoute(int k, int route_num) {
    while(1) {
        list<int> route;
        allRoutes[route_num].GetRoute(route);
        double start_load = allRoutes[route_num].GetTourDelivery();
        int max_length = min((int)route.size(), k);
        int length = 1 + rand()%max_length;
        //cout<<"length1="<<length1<<endl;
        int position;
        if(route.size() == length) {
            position = 0;
        } else {
            position = rand()%(route.size() - length);
        }

        liIter it_begin = route.begin();
        advance(it_begin, position);
        liIter it_end = it_begin;
        advance(it_end, length);
        list<int> middle(it_begin, it_end);

        list<int> tail(it_end, route.end());
        list<int> tail_rev;
        for(liIter it = tail.begin(); it != tail.end(); ++it)
            tail_rev.push_front(Problem.dualEdgeMap[*it]);
        route.resize(position);
        route.insert(route.end(), tail_rev.begin(), tail_rev.end());
        route.insert(route.end(), middle.begin(), middle.end());
        if(!(CheckFeasibility(route, start_load))) {
            continue;
        }
        allRoutes[route_num].SetRoute(route);
        allRoutes[route_num].EvaluateRoute();
        break;
    }
}
/////////////////////////////////////////////////////////////////////////////
TCARPSolution* TCARPSolution::Shaking(int k) {
    TCARPSolution* shake_solution = new TCARPSolution(*this);
    assert(NULL != shake_solution);
    int route_size = shake_solution->allRoutes.size();
    //随机shake两条route
    int route_num1 = rand()%route_size;
    shake_solution->ShakOneRoute(k, route_num1);
    int route_num2;
    do {
        route_num2= rand()%route_size;
    } while(route_num1 == route_num2);
    shake_solution->ShakOneRoute(k, route_num2);

    shake_solution->EvaluateSolution();
    return shake_solution;
#if 0
    TCARPSolution* shake_solution = new TCARPSolution(*this);
    assert(NULL != shake_solution);
    //shake_solution->PrintSolution();
    int route_size = shake_solution->allRoutes.size();
    //case1: route_size == 1
    if(route_size == 1) {
        list<int> route;
        shake_solution->allRoutes[0].GetRoute(route);
        int length = route.size();
        liIter depart_iter = route.begin();
        advance(depart_iter, length/2);

        list<int> new_route(depart_iter, route.end());
        route.resize(length/2);

        shake_solution->allRoutes[0].SetRoute(route);
        TSingleRoute single_route;
        shake_solution->allRoutes.push_back(single_route);
        shake_solution->allRoutes[1].SetRoute(new_route);
        shake_solution->allRoutes[0].EvaluateRoute();
        shake_solution->allRoutes[1].EvaluateRoute();
        shake_solution->EvaluateSolution();
        shake_solution->PrintSolution();
        //system("pause");
        return shake_solution;
    }
    bool b_feasible = false;
    list<int>route1, route2;
    int route_num1, route_num2;

    int position1, position2;
    liIter it1_begin, it1_end, it2_begin, it2_end;

    int length1, length2;//
    int max_length1, max_length2;
    while(!b_feasible) {
        //cout<<"while shaking!!\n";
        route_num1 = rand()%route_size;
        do {
            route_num2= rand()%route_size;
        } while(route_num1 == route_num2);
        //cout<<"here!\n";
        ///////////////////
        shake_solution->allRoutes[route_num1].GetRoute(route1);
        //PrintRoute(route1);
        max_length1 = min((int)route1.size(), k);
        length1 = 1 + rand()%max_length1;
        //cout<<"length1="<<length1<<endl;
        if(route1.size() == length1) {
            position1 = 0;
        } else {
            position1 = rand()%(route1.size() - length1);
        }

        it1_begin = route1.begin();
        advance(it1_begin, position1);
        it1_end = it1_begin;
        advance(it1_end, length1);

        ////////////////////
        shake_solution->allRoutes[route_num2].GetRoute(route2);
        //PrintRoute(route2);
        max_length2 = min((int)route2.size(), k);
        length2 = 1 + rand()%max_length2;
        //cout<<"length2="<<length2<<endl;
        if(route2.size() == length2) {
            position2 = 0;
        } else {
            position2 = rand()%(route2.size() - length2);
        }

        it2_begin = route2.begin();
        advance(it2_begin, position2);
        it2_end = it2_begin;
        advance(it2_end, length2);//it2_end may equal to end()

        ////////////////
        double start_load1, start_load2;
        list<int> segment1(it1_begin, it1_end);
        list<int> segment2(it2_begin, it2_end);
        list<int> end1(it1_end, route1.end());
        list<int> end2(it2_end, route2.end());
        route1.resize(position1);
        route2.resize(position2);
        route1.insert(route1.end(), segment2.begin(), segment2.end());
        route1.insert(route1.end(), end1.begin(), end1.end());
        start_load1 = CalculateDelivery(route1);
        if(!(CheckFeasibility(route1, start_load1))) {
            continue;
        }

        route2.insert(route2.end(), segment1.begin(), segment1.end());
        route2.insert(route2.end(), end2.begin(), end2.end());
        start_load2 = CalculateDelivery(route2);
        if(!(CheckFeasibility(route2, start_load2))) {
            continue;
        }
        shake_solution->allRoutes[route_num1].SetRoute(route1);
        shake_solution->allRoutes[route_num1].EvaluateRoute();

        shake_solution->allRoutes[route_num2].SetRoute(route2);
        shake_solution->allRoutes[route_num2].EvaluateRoute();
        b_feasible = true;
    }
    shake_solution->EvaluateSolution();
    return shake_solution;
#endif
}
/////////////////////////////////////////////////////////////////////////////
bool TCARPSolution::TestBestExchange(list < int > & route1, int route_num1, liIter& best_iter1,
                                     list < int > & route2, int route_num2, liIter& best_iter2) {
    bool b_exchanged = false;
    bool b_feasible1, b_feasible2;

    int edge1, edge2;
    double delivery_edge1, delivery_edge2;
    double pick_edge1, pick_edge2;
    int pre_edge1, pre_edge2, suc_edge1, suc_edge2;
    double varied_cost, best_cost = 0;
    double start_load1, start_load2;
    for(liIter it1 = route1.begin(); it1 != route1.end(); ++it1) {
        edge1 = *it1;
        delivery_edge1 = Problem.edges[edge1]->delivery;
        pick_edge1 = Problem.edges[edge1]->pick_up;

        for(liIter it2 = route2.begin(); it2 != route2.end(); ++it2) {
            edge2 = *it2;
            delivery_edge2 = Problem.edges[edge2]->delivery;
            pick_edge2 = Problem.edges[edge2]->pick_up;
            //先判断弱可行性
            if(allRoutes[route_num2].GetTourDelivery() - delivery_edge2 + delivery_edge1 > Problem.capacity
                    ||allRoutes[route_num2].GetTourPick_up() - pick_edge2 + pick_edge1 > Problem.capacity
                    ||allRoutes[route_num1].GetTourDelivery() - delivery_edge1 + delivery_edge2 > Problem.capacity
                    ||allRoutes[route_num1].GetTourPick_up() - pick_edge1 + pick_edge2 > Problem.capacity) {
                continue;
            }
            //exchange
            *it2 = edge1;
            start_load2 = allRoutes[route_num2].GetTourDelivery() - delivery_edge2 + delivery_edge1;
            //再判断强可行性
            if(!(CheckFeasibility(route2, start_load2))) {
                *it2 = edge2;//recovery
                continue;
            }
            *it2 = edge2;//recovery
            //exchange
            *it1 = edge2;
            start_load1 = allRoutes[route_num1].GetTourDelivery() - delivery_edge1 + delivery_edge2;
            if(!(CheckFeasibility(route1, start_load1))) {
                *it1 = edge1;//recovery
                continue;
            }
            *it1 = edge1;//recovery
            //calculate the exchanged cost
            pre_edge1 = GetPreviousEdge(route1, it1);
            suc_edge1 = GetSuccesiveEdge(route1, it1);

            pre_edge2 = GetPreviousEdge(route2, it2);
            suc_edge2 = GetSuccesiveEdge(route2, it2);
            varied_cost = (Problem.pathsDistanceMatrix[pre_edge1][edge2]
                           +Problem.pathsDistanceMatrix[edge2][suc_edge1]
                           +Problem.pathsDistanceMatrix[pre_edge2][edge1]
                           +Problem.pathsDistanceMatrix[edge1][suc_edge2])
                          -(Problem.pathsDistanceMatrix[pre_edge1][edge1]
                            +Problem.pathsDistanceMatrix[edge1][suc_edge1]
                            +Problem.pathsDistanceMatrix[pre_edge2][edge2]
                            +Problem.pathsDistanceMatrix[edge2][suc_edge2]);
            if(varied_cost < best_cost) {
                best_cost = varied_cost;
                best_iter1 = it1;
                best_iter2 = it2;
                b_exchanged = true;
                //cout<<"best_iter1="<<*best_iter1<<"\tbest_iter2="<<*best_iter2<<endl;
            }
        }
    }
    //cout<<"at the end of TestBestExchange\n";
    return b_exchanged;
}
/////////////////////////////////////////////////////////////////////////////
bool TCARPSolution::DoBestExchange(list < int > & route1, int route_num1,
                                   list < int > & route2, int route_num2) {
    liIter iter1,iter2;
    //cout<<"before test!!!!!!!!!!!!!!!!\n";
    bool b_exchanged =
        TestBestExchange(route1, route_num1, iter1, route2, route_num2, iter2);
    //cout<<"after test!!!!!!!!!!!!!!!!\n";

    if(b_exchanged) { //do exchange
        //cout<<"*iter1="<<*iter1<<"\t*iter2="<<*iter2<<endl;
        int edge1 = *iter1;
        int edge2 = *iter2;
        *iter1 = edge2;
        *iter2 = edge1;
    }
    return b_exchanged;
}
/////////////////////////////////////////////////////////////////////////////
void TCARPSolution::Exchange() {
    int route_size = allRoutes.size();
    if(route_size == 0) {
        cout<<"error at Exchange!\n";
        system("pause");
    }
    int route_num1, route_num2;
    list<int> route1, route2;
    bool b_exchanged;
    for(route_num1 = 0; route_num1 != route_size - 1; ++route_num1) {
        for(route_num2 = route_num1 + 1; route_num2 != route_size; ++route_num2) {
            allRoutes[route_num1].GetRoute(route1);
            allRoutes[route_num2].GetRoute(route2);
            //cout<<"before do best exchange\n";
            b_exchanged = DoBestExchange(route1, route_num1, route2, route_num2);
            if(!b_exchanged) {
                continue;
            }
            //update routes
            allRoutes[route_num1].SetRoute(route1);
            allRoutes[route_num2].SetRoute(route2);
            allRoutes[route_num1].EvaluateRoute();
            allRoutes[route_num2].EvaluateRoute();
        }
    }
    EvaluateSolution();
    //cout<<"##########end of Exchange()"<<endl;
}
/////////////////////////////////////////////////////////////////////////////
bool TCARPSolution::TestBestReverse(list < int > & route, int route_num,
                                    liIter	& it_begin_best, liIter & it_end_best) {
    bool b_reversed = false;
    liIter it_begin, it_end;
    bool flag;//
    double start_load = allRoutes[route_num].GetTourDelivery();
    double varied_cost, best_cost = 0;
    for(it_begin = route.begin(); it_begin != route.end(); ++it_begin) {
        it_end = it_begin;
        advance(it_end, 1);
        int pre_edge = GetPreviousEdge(route, it_begin);
        for(flag = true; flag; ++it_end) {
            if(it_end == route.end()) {
                flag = false;
            }
            int suc_edge = (it_end == route.end() ? Problem.depotEdge : *it_end);
            list<int> segment(it_begin, it_end);
            list<int> segment_rev;
            for(liIter it = segment.begin(); it != segment.end(); ++it)
                segment_rev.push_front(Problem.dualEdgeMap[*it]);
            list<int> tmp_route(route.begin(), it_begin);
            tmp_route.insert(tmp_route.end(), segment_rev.begin(), segment_rev.end());
            tmp_route.insert(tmp_route.end(), it_end, route.end());
            if(!(CheckFeasibility(tmp_route, start_load))) {
                continue;
            }
            varied_cost = CalcuateCost(pre_edge, suc_edge, segment_rev)
                          - CalcuateCost(pre_edge, suc_edge, segment);
            if(varied_cost < best_cost) {
                //cout<<"reverse can lead improvement!\n";
                best_cost = varied_cost;
                it_begin_best = it_begin;
                it_end_best = it_end;
                b_reversed = true;
            }
        }
    }
    return b_reversed;
}
/////////////////////////////////////////////////////////////////////////////
void TCARPSolution::Reverse() {
    int route_size = allRoutes.size();
    list<int> route;
    bool b_reversed;
    list<int> segment_rev;
    for(int route_num = 0; route_num != route_size; ++route_num) {
        allRoutes[route_num].GetRoute(route);
        liIter it_begin, it_end;
        b_reversed = TestBestReverse(route, route_num, it_begin, it_end);
        if(!b_reversed) {
            continue;
        }
        segment_rev.clear();
        for(liIter it = it_begin; it != it_end; ++it) {
            segment_rev.push_front(Problem.dualEdgeMap[*it]);
        }
        liIter it1, it2;
        for(it1 = it_begin, it2 = segment_rev.begin(); it1 != it_end && it2 != segment_rev.end(); ++it1, ++it2) {
            *it1 = *it2;
        }
        allRoutes[route_num].SetRoute(route);
        allRoutes[route_num].EvaluateRoute();
    }
    EvaluateSolution();
}
/////////////////////////////////////////////////////////////////////////////
bool TCARPSolution::TestBestCross(list < int > & route1, int route_num1, liIter & it1,
                                  list < int > & route2, int route_num2, liIter & it2) {
    bool b_crossed = false;
    double start_load1 = allRoutes[route_num1].GetTourDelivery();
    double start_load2 = allRoutes[route_num2].GetTourDelivery();

    double head_delivery1 = 0;
    double head_delivery2 = 0;
    double tail_delivery1 = start_load1;
    double tail_delivery2 = start_load2;

    double varied_cost, best_cost = 0;
    liIter it_begin1, it_begin2;
    //list<int> tmp_route1;
    //list<int> tmp_route2;
    list<int> head1, head2;
    head1.resize(0);
    head2.resize(0);
    list<int> tail1(route1);
    list<int> tail2;
    int pre_edge1, pre_edge2;
    //cout<<"here1"<<endl;
    for(it_begin1 = route1.begin(); it_begin1 != route1.end(); ++it_begin1) {
        pre_edge1 = GetPreviousEdge(route1, it_begin1);
        tail2 = route2;
        //cout<<"here2"<<endl;
        for(it_begin2 = route2.begin(); it_begin2 != route2.end(); ++it_begin2) {
            //cout<<"here3"<<endl;
            if(it_begin1 == route1.begin() && it_begin2 == route2.begin()) {
                //cout<<"here4"<<endl;
                tail_delivery2 -= Problem.edges[*it_begin2]->delivery;
                tail2.erase(tail2.begin());

                head2.push_back(*it_begin2);
                head_delivery2 += Problem.edges[*it_begin2]->delivery;
                continue;
            }
            //cout<<"here4"<<endl;
            list<int> tmp_route1(head1);
            tmp_route1.insert(tmp_route1.end(), tail2.begin(), tail2.end());

            list<int> tmp_route2(head2);
            tmp_route2.insert(tmp_route2.end(), tail1.begin(), tail1.end());

            if(!(CheckFeasibility(tmp_route1, head_delivery1 + tail_delivery2)
                    && CheckFeasibility(tmp_route2, head_delivery2 + tail_delivery1)) ) {
                //cout<<"here5"<<endl;
                tail_delivery2 -= Problem.edges[*it_begin2]->delivery;
                tail2.erase(tail2.begin());

                head2.push_back(*it_begin2);
                head_delivery2 += Problem.edges[*it_begin2]->delivery;
                continue;
            }
            //cout<<"here5"<<endl;
            pre_edge2 = GetPreviousEdge(route2, it_begin2);
            varied_cost = Problem.pathsDistanceMatrix[pre_edge1][*it_begin2]
                          + Problem.pathsDistanceMatrix[pre_edge2][*it_begin1]
                          - Problem.pathsDistanceMatrix[pre_edge1][*it_begin1]
                          - Problem.pathsDistanceMatrix[pre_edge2][*it_begin2];
            //cout<<"here5"<<endl;
            if(varied_cost < best_cost) {
                best_cost = varied_cost;
                it1 = it_begin1;
                it2 = it_begin2;
                b_crossed = true;
            }
            //cout<<"here6"<<endl;
            tail_delivery2 -= Problem.edges[*it_begin2]->delivery;
            tail2.erase(tail2.begin());

            head2.push_back(*it_begin2);
            head_delivery2 += Problem.edges[*it_begin2]->delivery;
            //cout<<"here7"<<endl;
        }
        tail_delivery1 -= Problem.edges[*it_begin1]->delivery;
        tail1.erase(tail1.begin());

        head_delivery1 += Problem.edges[*it_begin1]->delivery;
        head1.push_back(*it_begin1);
    }
    //deal with the it_begin1 = end()
    it_begin1 = route1.end();
    pre_edge1 = *(route1.rbegin());
    head_delivery1 =start_load1;
    tail_delivery2 = start_load2;
    //cout<<"here8"<<endl;
    tail2 = route2;
    int size1 = route1.size();
    list<int> tmp_route(route1);
    for(it_begin2 = route2.begin(); it_begin2 != route2.end(); ++it_begin2) {
        //cout<<"here9"<<endl;
        if(head_delivery1 + tail_delivery2> Problem.capacity) {
            tail_delivery2 -= Problem.edges[*it_begin2]->delivery;
            tail2.erase(tail2.begin());
            continue;
        }
        tmp_route.resize(size1);
        tmp_route.insert(tmp_route.end(), tail2.begin(), tail2.end());
        if(!(CheckFeasibility(tmp_route, head_delivery1 + tail_delivery2))) {
            tail_delivery2 -= Problem.edges[*it_begin2]->delivery;
            tail2.erase(tail2.begin());
            continue;
        }
        pre_edge2 = GetPreviousEdge(route2, it_begin2);
        varied_cost = Problem.pathsDistanceMatrix[pre_edge1][*it_begin2]
                      + Problem.pathsDistanceMatrix[pre_edge2][Problem.depotEdge]
                      - Problem.pathsDistanceMatrix[pre_edge1][Problem.depotEdge]
                      - Problem.pathsDistanceMatrix[pre_edge2][*it_begin2];
        if(varied_cost < best_cost) {
            best_cost = varied_cost;
            it1 = it_begin1;
            it2 = it_begin2;
            b_crossed = true;
        }
        //cout<<"here10"<<endl;
        tail_delivery2 -= Problem.edges[*it_begin2]->delivery;
        tail2.erase(tail2.begin());
    }
    //deal with the it_begin2 = end()
    it_begin2 = route2.end();
    pre_edge2 = *(route2.rbegin());
    head_delivery2 = start_load2;
    tail_delivery1 = start_load1;
    //cout<<"here11"<<endl;
    tail1 = route1;
    int size2 = route2.size();
    tmp_route = route2;
    for(it_begin1= route1.begin(); it_begin1 != route1.end(); ++it_begin1) {
        //cout<<"here12"<<endl;
        if(head_delivery2 + tail_delivery1 > Problem.capacity) {
            //cout<<"here130"<<endl;
            tail_delivery1 -= Problem.edges[*it_begin1]->delivery;
            tail1.erase(tail1.begin());
            continue;
        }
        //cout<<"here13"<<endl;
        tmp_route.resize(size2);
        tmp_route.insert(tmp_route.end(), tail1.begin(), tail1.end());
        if(!(CheckFeasibility(tmp_route, head_delivery2 + tail_delivery1))) {
            //cout<<"here140"<<endl;
            tail_delivery1 -= Problem.edges[*it_begin1]->delivery;
            tail1.erase(tail1.begin());
            continue;
        }
        pre_edge1 = GetPreviousEdge(route1, it_begin1);
        //cout<<"here14"<<endl;
        varied_cost = Problem.pathsDistanceMatrix[pre_edge2][*it_begin1]
                      + Problem.pathsDistanceMatrix[pre_edge1][Problem.depotEdge]
                      - Problem.pathsDistanceMatrix[pre_edge2][Problem.depotEdge]
                      - Problem.pathsDistanceMatrix[pre_edge1][*it_begin1];
        //cout<<"here150"<<endl;
        if(varied_cost < best_cost) {
            best_cost = varied_cost;
            it1 = it_begin1;
            it2 = it_begin2;
            b_crossed = true;
        }
        //cout<<"here15"<<endl;
        tail_delivery1 -= Problem.edges[*it_begin1]->delivery;
        tail1.erase(tail1.begin());
    }
    return b_crossed;
}
/////////////////////////////////////////////////////////////////////////////
bool TCARPSolution::CrossTwoRoutes(list < int > & route1, int route_num1,
                                   list < int > & route2, int route_num2) {

    liIter it_begin1 = route1.begin();
    liIter it_begin2 = route2.begin();
    //cout<<"befor test corss two routes\n";
    bool b_crossed =
        TestBestCross(route1, route_num1,  it_begin1, route2, route_num2, it_begin2);
    //cout<<"after test corss two routes\n";
    if(b_crossed) {
        list<int> tail1(it_begin1, route1.end());
        route1.erase(it_begin1, route1.end());

        list<int> tail2(it_begin2, route2.end());
        route2.erase(it_begin2, route2.end());

        route1.insert(route1.end(), tail2.begin(), tail2.end());
        route2.insert(route2.end(), tail1.begin(), tail1.end());
    }
    return b_crossed;
}
/////////////////////////////////////////////////////////////////////////////
void TCARPSolution::Cross() {
    int route_size = allRoutes.size();
    int route_num1, route_num2;
    bool b_crossed = false;
    list<int> route1, route2;
    for(route_num1 = 0; route_num1 != route_size - 1; ++route_num1) {
        for(route_num2 = route_num1 + 1; route_num2 != route_size; ++route_num2) {
            allRoutes[route_num1].GetRoute(route1);
            allRoutes[route_num2].GetRoute(route2);
            b_crossed = CrossTwoRoutes(route1, route_num1, route2, route_num2);
            if(b_crossed) {
                //cout<<"********here is improved by Cross!\n";
                allRoutes[route_num1].SetRoute(route1);
                allRoutes[route_num2].SetRoute(route2);
                allRoutes[route_num1].EvaluateRoute();
                allRoutes[route_num2].EvaluateRoute();
            }
        }
    }
    //cout<<"end of cross!\n";
    EvaluateSolution();
}
/////////////////////////////////////////////////////////////////////////////
//将it1及其之后的一条边从route1中删除，并插入到route2中it2之前
bool TCARPSolution::TestBestSwap(list < int > & route1, int route_num1, liIter & it1,
                                 list < int > & route2, int route_num2, liIter & it2) {
    bool b_swaped = false;

    liIter it_begin1, it_begin2;
    int edge1, edge2;
    int pre_edge1, pre_edge2, suc_edge1, suc_edge2;
    double varied_cost, best_cost = 0;
    double insert_delivery = 0;
    double start_load2 = allRoutes[route_num2].GetTourDelivery();

    if(route1.size() < 2) {
        cout<<"the route is too small!\n";
        return b_swaped;
    }
    list<int> head2;
    list<int> tail2;
    list<int> tmp_route;
    for(it_begin1 = route1.begin(); it_begin1 != --(route1.end()); ++it_begin1) {
        pre_edge1 = GetPreviousEdge(route1, it_begin1);
        liIter edge1_iter = it_begin1;
        edge1 = *edge1_iter;
        edge2 = *(++edge1_iter);
        suc_edge1 = GetSuccesiveEdge(route1, edge1_iter);
        insert_delivery = Problem.edges[edge1]->delivery + Problem.edges[edge2]->delivery;
        double start_load =start_load2 + insert_delivery;

        head2.clear();
        tail2 = route2;
        for(it_begin2 = route2.begin(); it_begin2 != route2.end(); ++it_begin2) {
            tmp_route = head2;
            tmp_route.push_back(edge1);
            tmp_route.push_back(edge2);
            tmp_route.insert(tmp_route.end(), tail2.begin(), tail2.end());

            if(!(CheckFeasibility(tmp_route, start_load))) {
                head2.push_back(*it_begin2);
                tail2.erase(tail2.begin());
                continue;
            }
            pre_edge2 = GetPreviousEdge(route2, it_begin2);
            suc_edge2 = *it_begin2;
            varied_cost = Problem.pathsDistanceMatrix[pre_edge1][suc_edge1]
                          + Problem.pathsDistanceMatrix[pre_edge2][edge1]
                          + Problem.pathsDistanceMatrix[edge2][suc_edge2]
                          - Problem.pathsDistanceMatrix[pre_edge1][edge1]
                          - Problem.pathsDistanceMatrix[edge2][suc_edge1]
                          - Problem.pathsDistanceMatrix[pre_edge2][suc_edge2];
            if(varied_cost < best_cost) {
                best_cost = varied_cost;
                it1 = it_begin1;
                it2 = it_begin2;
                b_swaped = true;
            }
            head2.push_back(*it_begin2);
            tail2.erase(tail2.begin());
        }
        //when it_begin2 = end()
        it_begin2 = route2.end();
        pre_edge2 = *(route2.rbegin());
        suc_edge2 = Problem.depotEdge;
        list<int> segment(route2);
        segment.push_back(edge1);
        segment.push_back(edge2);

        if(CheckFeasibility(segment, start_load)) {
            varied_cost = Problem.pathsDistanceMatrix[pre_edge1][suc_edge1]
                          + Problem.pathsDistanceMatrix[pre_edge2][edge1]
                          + Problem.pathsDistanceMatrix[edge2][suc_edge2]
                          - Problem.pathsDistanceMatrix[pre_edge1][edge1]
                          - Problem.pathsDistanceMatrix[edge2][suc_edge1]
                          - Problem.pathsDistanceMatrix[pre_edge2][suc_edge2];
            if(varied_cost < best_cost) {
                best_cost = varied_cost;
                it1 = it_begin1;
                it2 = it_begin2;
                b_swaped = true;
            }
        }
    }
    return b_swaped;
}
/////////////////////////////////////////////////////////////////////////////
bool TCARPSolution::SwapTwoRoutes(list < int > & route1, int route_num1,
                                  list < int > & route2, int route_num2) {
    liIter it_begin1, it_begin2;
    bool b_swaped =
        TestBestSwap(route1, route_num1, it_begin1, route2, route_num2, it_begin2);
    if(b_swaped) {
        route2.insert(it_begin2, *it_begin1);
        ++it_begin1;
        route2.insert(it_begin2, *it_begin1);
        --it_begin1;
        liIter tmp_iter = route1.erase(it_begin1);
        route1.erase(tmp_iter);
    }
    return b_swaped;
}
/////////////////////////////////////////////////////////////////////////////
void TCARPSolution::Swap() {
    int route_size = allRoutes.size();
    int route_num1, route_num2;
    bool b_swaped = false;
    list<int> route1, route2;
    for(route_num1 = 0; route_num1 != route_size; ++route_num1) {
        for(route_num2 = 0; route_num2 != route_size; ++route_num2) {
            if(route_num1 == route_num2) {
                continue;
            }
            allRoutes[route_num1].GetRoute(route1);
            allRoutes[route_num2].GetRoute(route2);
            if(route1.size() < 2) {
                continue;
            }
            b_swaped = SwapTwoRoutes(route1, route_num1, route2, route_num2);
            if(b_swaped) {
                allRoutes[route_num1].SetRoute(route1);
                allRoutes[route_num2].SetRoute(route2);
                allRoutes[route_num1].EvaluateRoute();
                allRoutes[route_num2].EvaluateRoute();
            }
        }
    }
    EvaluateSolution();
}

/////////////////////////////////////////////////////////////////////////////
bool TCARPSolution::TestBestInsertion(list<int>& route, list<int>& segment, double& best_cost,
                                      double old_cost, double start_load, int& insert_position) {
    bool b_inserted = false;
    best_cost = 0;
    double varied_cost;
    list<int> head;
    head.clear();
    list<int> tail(route);
    liIter it;
    for(it = route.begin(); it != route.end(); ++it) {
        list<int> tmp_route(head);
        tmp_route.insert(tmp_route.end(), segment.begin(), segment.end());
        tmp_route.insert(tmp_route.end(), tail.begin(), tail.end());
        if(!(CheckFeasibility(tmp_route, start_load))) {
            head.push_back(*it);
            tail.erase(tail.begin());
            continue;
        }
        varied_cost = CalcuateCost(Problem.depotEdge, Problem.depotEdge, tmp_route) - old_cost;
        if(varied_cost < best_cost) {
            best_cost = varied_cost;
            insert_position = *it;
            b_inserted = true;
        }
        head.push_back(*it);
        tail.erase(tail.begin());
    }
    //it = route.end()
    it = route.end();
    list<int> tmp_route(route);
    tmp_route.insert(tmp_route.end(), segment.begin(), segment.end());
    if(CheckFeasibility(tmp_route, start_load)) {
        varied_cost = CalcuateCost(Problem.depotEdge, Problem.depotEdge, tmp_route) - old_cost;
        if(varied_cost < best_cost) {
            best_cost = varied_cost;
            insert_position = -1;//-1 means: push back to the end
            b_inserted = true;
        }
    }
    return b_inserted;
}
/////////////////////////////////////////////////////////////////////////////
bool TCARPSolution::RelocateSegment(list < int > & route, int route_num, int length) {
    bool b_relocated = false;
    if(route.size() <= length) {
        return b_relocated;
    }
    double old_cost = allRoutes[route_num].GetTourCost();
    double start_load = allRoutes[route_num].GetTourDelivery();
    double varied_cost, best_cost = 0;
    int insert_position, best_position;
    liIter best_begin;
    liIter whister_iter = route.end();
    advance(whister_iter, 1-length);

    liIter it_begin, it_end;
    for(it_begin = route.begin(); it_begin != whister_iter; ++it_begin) {
        it_end = it_begin;
        advance(it_end, length);
        list<int> segment(it_begin, it_end);
        list<int> tmp_route(route.begin(), it_begin);
        tmp_route.insert(tmp_route.end(), it_end, route.end());
        b_relocated =
            TestBestInsertion(tmp_route, segment, varied_cost, old_cost, start_load, insert_position);
        if(!b_relocated) {

            continue;
        }
        if(varied_cost < best_cost) {
            best_begin = it_begin;
            best_position = insert_position;
            best_cost = varied_cost;
        }
    }

    if(!b_relocated) {
        return b_relocated;
    }
    //now, do the relocation!
    it_end = best_begin;
    advance(it_end, length);
    list<int> seg(best_begin, it_end);
    route.erase(best_begin, it_end);
    if(insert_position == -1) {
        route.insert(route.end(), seg.begin(), seg.end());
    } else {
        liIter insert_iter = find(route.begin(), route.end(), insert_position);
        route.insert(insert_iter, seg.begin(), seg.end());
    }

    return b_relocated;
}
/////////////////////////////////////////////////////////////////////////////
void TCARPSolution::Relocate() {
    int route_size = allRoutes.size();
    list<int> route;
    bool b_reversed;
    list<int> segment_rev;
    for(int route_num = 0; route_num != route_size; ++route_num) {
        for(int length = 1; length <= 3; ++length) {
            allRoutes[route_num].GetRoute(route);
            if(RelocateSegment(route, route_num, length)) {
                allRoutes[route_num].SetRoute(route);
                allRoutes[route_num].EvaluateRoute();
            }
        }
    }
    EvaluateSolution();
}
/////////////////////////////////////////////////////////////////////////////
void TCARPSolution::LocalSearch() {
    //cout<<"local search start!\n";
#if 1
    for(int k=0; k<3; ++k) {
        Exchange();
        Reverse();
        Cross();
        Swap();
        Relocate();
    }
#endif
#if 0
    int local_search_num = 5;
    vector<int> sequence;
    for(int i=0; i<local_search_num; ++i) {
        sequence.push_back(i+1);
    }
    random_shuffle(sequence.begin(), sequence.end());
    for(int k=0; k<10; ++k) {
        for(int i=0; i<5; ++i) {
            int num = sequence[i];
            switch(num) {
            case 1:
                Exchange();
                break;
            case 2:
                Reverse();
                break;
            case 3:
                Cross();
                break;
            case 4:
                Swap();
                break;
            case 5:
                Relocate();
                break;
            default:
                break;

            }
        }
    }
#endif
    //cout<<"local search finished!\n";
}
/////////////////////////////////////////////////////////////////////////////
void TCARPSolution::VNS(int k_max) {
    int max_iteration = 50000;
    int k=1;
    int r=0;
    int sigma = 8;
    double theta = 0.99;
    TCARPSolution* ptr_best = new TCARPSolution;
    ptr_best->Split();
    ptr_best->Shift();
    ptr_best->Convert();
    //ptr_best->PrintSolution();
    while(k <= k_max && r++<max_iteration) {
        cout<<k<<" ";
        TCARPSolution* ptr_solution = ptr_best->Shaking(k);
        ptr_solution->LocalSearch();
        if(ptr_solution->GetSolutionCost() < ptr_best->GetSolutionCost()) {
            *ptr_best = *ptr_solution;
            k=1;
        } else if(k%sigma == 0
                  && theta *(ptr_solution->GetSolutionCost()) < ptr_best->GetSolutionCost()) {
            *ptr_best = *ptr_solution;
            k=1;
        } else {
            ++k;
        }
        delete ptr_solution;
    }
    ptr_best->PrintSolution();
    delete ptr_best;
}
/////////////////////////////////////////////////////////////////////////////
void TCARPSolution::PrintSolution() {
    cout <<endl<<"cost= " <<cost<<"\ttotal_d = "<<delivery<<"\ttotal_p = "<<pick_up<<endl;
    double s_load, route_cost, route_delivery, route_pick_up;
    double ma_load, mi_load;
    int route_size = allRoutes.size();
    for(int i = 0; i < route_size; ++i) {
        list<int> liTmp;
        allRoutes[i].GetRoute(liTmp);

        route_cost = allRoutes[i].GetTourCost();
        route_delivery= allRoutes[i].GetTourDelivery();
        route_pick_up = allRoutes[i].GetTourPick_up();
        s_load = allRoutes[i].GetStartLoad();
        //allRoutes[i].CalculateMin_MaxLoad(i);
        ma_load = allRoutes[i].GetMaxLoad();
        mi_load = allRoutes[i].GetMinLoad();
        cout <<endl<<"route " << i << ":   ";
        for(liIter iter = liTmp.begin(); iter != liTmp.end(); iter++) {
            //if(*iter > Problem.nReqEdges)
            //{
            //	cout << "-" << Problem.dualEdgeMap[*iter] << "  ";
            //}
            //else
            //{
            cout << *iter << "  ";
            //}
        }
        cout<<"\nstart_load = "<<s_load
            <<"\troute_c="<<route_cost<<"\troute_d="<<route_delivery
            <<"\troute_p = "<<route_pick_up<<endl;
        cout<<"min_load = "<<mi_load<<"\tmax_load = "<<ma_load<<endl;
    }
}
