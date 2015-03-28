#include "problem.h"

// Global variable declared external in TCARPProblem.h /
TCARPProblem Problem;

//edgeNo���˳��:����ߵ����������񳵵��������Ƿ���ߵ����� 
////////////////////////////////////////////////////////////////////////////////
//�ڴ����
//06-04-24, 25

void TCARPProblem::AllocateMemory()
{
	//nEdges = nReqEdges + nUnreqEdges;
	//int itmp = nEdges+nVehicles;
	//pair<int,int> pairTmp(-1,-1);   
	//edges.resize(nEdgAdVeh);
	edges.resize(2*nEdges+nVehicles);	// allocate space for dual roads
	capacities.resize(nVehicles);
	
	//eEdgeRMap = new int[nEdgAdVeh];    
	dualEdgeMap.resize(2*nEdgAdVeh);	 
	inNeighbor.resize(nVertices);
	outNeighbor.resize(nVertices);	
}

//����һ���� 
void TCARPProblem::ReadEachEdge(ifstream &Source, int edgeNO)
{
	int iTmp;
	char cTmp;
	int from, to;	
	string sTmp;
					
	edges[edgeNO] = new Edge ; 
	Source >> cTmp >> from >> cTmp >> to;
	//Ĭ���������ݾ�Ϊ����ͼ
	edges[edgeNO]->bOneWay = false;/////caution!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//if(edgeNO == 7)
	//	edges[edgeNO]->bOneWay = true;//one-way !!!!!!!!!!!!!!!!!!!!!!!!
	edges[edgeNO]->from = from - 1;//��Ŵӣ���ʼ 
	edges[edgeNO]->to = to - 1; //
	
	//����˱߻��� 
	Source >> cTmp >> sTmp >> edges[edgeNO]->cost;

    	if( edgeNO < nReqEdges )//�������������� 
   	{
	   	Source >> sTmp >> edges[edgeNO]->demand;
	}
}
//������ѡ���Եض�ȡ�������Ƿ���ߵ�������� 
void TCARPProblem::ReadEdgeData(ifstream &Source, bool bRequiredEdges)
{//һ���Զ�ȡ���бߵ����� 
    char cTmp;
	int iTmp;
	string sTmp;
	double dtmp;
	int from, to;
	int locIndex;   
	
	//read required edges		
	if( true == bRequiredEdges )	
	{	
		locIndex = nEdgAdVeh;//��������䷴�����߱��֮��ļ�� 
		nOneWayEdges = nReqOnewayEdges = 0;
		totalReq = 0;
		
		for(int edgeNO = 0; edgeNO < nReqEdges; edgeNO++)
		{
			ReadEachEdge(Source, edgeNO);
			
			//eEdgeRMap[edgeNO] = edges[edgeNO]->ID;
			totalReq += edges[edgeNO]->demand;
            		double percent = (double)rand()/(double)RAND_MAX;
			edges[edgeNO]->delivery = (edges[edgeNO]->demand)*percent;
			edges[edgeNO]->pick_up = (edges[edgeNO]->demand)*(1 - percent);
			if ( !edges[edgeNO]->bOneWay )
			{	//�Է���߽���˫����������Map�����������nEdgAdVeh 
               		dualEdgeMap[edgeNO] = locIndex;
				dualEdgeMap[locIndex] = edgeNO;
				//������edges[edgeNO]��ͬ��һ������ֻ���޸���from��to���ݼ���ɷ��� 
				edges[locIndex] = new Edge(*edges[edgeNO]);//create the edge
				int iTmp = edges[locIndex]->from;//swap 'from' and 'to'
				edges[locIndex]->from = edges[locIndex]->to;
				edges[locIndex]->to = iTmp;
				
				++locIndex;
			}  
            		else
            		{
				nOneWayEdges++;
				nReqOnewayEdges++;  
			}
	    }
	    cout << endl << "totalReq(������)= "<<totalReq << endl;
		cout<<"��������:"<<capacity<<endl;
	}
	//read unrequired edges
	else 
	{
		//����Ϊ������߽���˫������Ժ��ܱ��� ����Ϊ����Ƿ���߱�ŵĿ�ʼ 
		locIndex = nEdgAdVeh + (nReqEdges - nReqOnewayEdges);
		for(int edgeNO = nRqedgAdVeh; edgeNO < nEdgAdVeh; edgeNO++)
		{
		    ReadEachEdge(Source, edgeNO);
			
			//eEdgeRMap[edgeNO] = edges[edgeNO]->ID;
            
           	if ( !edges[edgeNO]->bOneWay )
			{	//�ԷǷ���߽���˫����������Map 
               	dualEdgeMap[edgeNO] = locIndex;
				dualEdgeMap[locIndex] = edgeNO;
							
				edges[locIndex] = new Edge(*edges[edgeNO]);//create the edge
				int iTmp = edges[locIndex]->from;//swap 'from' and 'to'
				edges[locIndex]->from = edges[locIndex]->to;
				edges[locIndex]->to = iTmp;
				
				++locIndex;
	        } 
			else
				++nOneWayEdges; 
		}
	}
}
//��ת��Ĵ���������δʹ�� 
void TCARPProblem::ReadTurns(istream& Source, const string& turnType)
{
	char cTmp;
	int edge1, edge2;
	
	Source >> cTmp; 
	cout << endl << "input  " << cTmp << endl;
	
	while( '(' == cTmp )
	{
		Source >> edge1 >> cTmp >> edge2 >> cTmp;
		
		if( "NOTURN" == turnType )
		{
			noTurns.push_back(make_pair(edge1,edge2) );
		}
		else
		{
			mandatoryTurns.push_back(make_pair(edge1,edge2) );
		}
		
		Source >> cTmp; 
	}
	
	Source.putback(cTmp);
	cout << endl << noTurns.size() << endl;
	cout << endl << mandatoryTurns.size() << endl;
	
}
	
//���⻡�ı��	
void TCARPProblem::SetVehicleEdges()
{	//�Գ����߽��б�Ŵ��� 
	for(int edgeNO = nReqEdges; edgeNO < nRqedgAdVeh; edgeNO++)
    {
	    edges[edgeNO] = new Edge ;
	    //ÿ�����ӳ�������ʱ����һ���Լ��ıߣ���ID�����ŷ���ߡ��Ƿ���߽��б�� 
	    edges[edgeNO]->ID = nEdges  + (edgeNO - nReqEdges);
		//eEdgeRMap[edgeNO] = nEdges  + (edgeNO - nReqEdges); 
	    
	    edges[edgeNO]->from = depot ;
	    edges[edgeNO]->to = depot;
        edges[edgeNO]->cost = 0;
	    edges[edgeNO]->demand = 0;
	    edges[edgeNO]->bOneWay = false;
	    dualEdgeMap[edgeNO] = edgeNO;	
	}
}
	

bool TCARPProblem::Load(char* fname)//add the file line by line 
{
	ifstream Source;
	Source.open(fname);
	cout << endl << "open data file: " <<fname << endl;
	
	//file open error message
	if ( Source.fail() )
	{   // error check
		cerr << "Cannot open " << fname  << "!\n";
		system("PAUSE");
		exit(-1);
	}
				
	nVertices = nReqEdges = nUnreqEdges = nVehicles = nEdgAdVeh = nRqedgAdVeh = -1;
	nOneWayEdges = nReqOnewayEdges = 0;  
	
    //double capacity = -1.0;	    
    	
	char cTmp;
	string sTmp;
	int iTmp;
	bool bRequired;
    
    //read the file����while ���ƣ�һ��һ���ض�ȡ��ֱ����ȡʧ�� 
 	while( !Source.eof() )//eof()Ϊ true����������ڴ���״̬
	{
		Source >> sTmp;
		//cout << "  read   "<< sTmp << endl;
		if ( "VERTICES" == sTmp )    //input the first line
		{
			Source >> cTmp >> nVertices;
			//cout << endl << sTmp << endl << cTmp <<endl <<nVertices<<endl;
		}
		else if ( "ARISTAS_REQ" == sTmp )
		{
			Source >> cTmp >> nReqEdges;
			//cout << endl << sTmp << endl << cTmp <<endl <<nReqEdges<<endl;
		}
		else if ( "ARISTAS_NOREQ" == sTmp )
		{
			Source >> cTmp >> nUnreqEdges;
			//cout << endl << sTmp << endl << cTmp <<endl <<nUnreqEdges<<endl;
		}
		else if ( "VEHICULOS" == sTmp )
		{
			Source >> cTmp >> nVehicles;
			//cout << endl << sTmp << endl << cTmp <<endl << nVehicles<<endl;
		}
		else if ( "CAPACIDAD" == sTmp )
		{
			Source >> cTmp >> capacity;
		
			//cout << endl << sTmp << endl << cTmp <<endl << capacity<<endl;
		}
		//begin to read edge data
		else if ( "LISTA_ARISTAS_REQ" == sTmp )
		{
			//cout << sTmp << endl;
		    Source >> cTmp;
		    
		    nEdges = nReqEdges + nUnreqEdges;
			nEdgAdVeh = nEdges + nVehicles;//dxl
			nRqedgAdVeh = nReqEdges + nVehicles;
			depotEdge = nReqEdges;//��Ϊ���бߵı��˳��Ϊ����ߡ������ߡ��Ƿ���ߣ�
							   	//�ұ�Ŵӣ���ʼ�����ԾͿ���nReqEdges��ֵ��Ϊ�����ߵı�� 
    		        		
    		cout << endl << "nEdges(���б�����)= " << nEdges << "\nnReqEdges(���������)= " << nReqEdges
			     << "\nnUnreqEdges(�Ƿ��������)= " << nUnreqEdges << "\nnEdgAdVeh(�ܱ���+�ܳ�����)= "
				 << nEdgAdVeh << "\nnRqedgAdVeh(�������+������)= " << nRqedgAdVeh;
				 				                 
			AllocateMemory();
			
			bRequired = true;
			ReadEdgeData(Source, bRequired);
		}
		//���зǷ���ߣ����ȡ�Ƿ���ߵ����� 
		else if ( ("LISTA_ARISTAS_NOREQ" == sTmp) && (nUnreqEdges > 0) )
		{
			//cout << sTmp << endl;
		    Source >> cTmp;
			
			bRequired = false;
			ReadEdgeData(Source, bRequired);
		}
		else if( "DEPOSITO" == sTmp )
		{
			//cout << cTmp << endl;
		    Source >> cTmp;
			Source >> depot;
			depot -= 1;
			//break;		///////////////// caution  !!!!!!!!!!!!!!!!!!!!!

			Source >> sTmp;/*�ٴζ�ȡ��sTmpΪ�λ���DEPOSITO�� */
			cout << endl << "no1  " <<sTmp;
			//is there any turn constraint
			if( "MANDATORY" == sTmp )//ǿ��ת�� 
			{
				cout << endl << sTmp << endl;
				Source >> sTmp;
				
				cout << endl << sTmp << endl;
				ReadTurns(Source, "MANDATORY");
				
				Source >> sTmp;
				cout << endl << "no2  " <<sTmp;
				if( "NOTURN" == sTmp )
				{
					//cout << sTmp << endl;
					Source >> cTmp;
					ReadTurns(Source, "NOTURN");
				
					break;		
				}
				else
				{
					break;
				}
			}
			else if( "NOTURN" == sTmp )
			{
				//cout << sTmp << endl;
				Source >> cTmp;
				ReadTurns(Source, "NOTURN");
				
				break;		
			}
			else
			{
				break;
		    }
		}
		else // skip line,��ȡ���з� 
		{
			if( !Source.eof() )
			{
				Source.get(cTmp);
				while( cTmp != '\n')
				{
					Source.get(cTmp);
				}
			}
		}
	}
	
	//system("PAUSE");
	SetVehicleEdges();
	
	nTotalEdges = nEdgAdVeh + (nEdges - nOneWayEdges);
	
	cout << endl << "\nnOneWayEdges(���������)=" << nOneWayEdges
		 << "\nnOneWayReqEdges(������������)=" << nReqOnewayEdges
		 << "\ntotal edges(ת�����ܱ���)=" << nTotalEdges;
		 	
	return true;
	
 }

////////////////////////////////////////////////////////////////////////////////
//////// calculate the relating quantatity for network    //////////////////////


////////////////////////////////////////////////////////////////////////////////
// consider inwards and outwards edge sepearatly for each node i, 
//and store their numbers in inNeighbor[] and outNeighbor[] arraies
//

void TCARPProblem::SetNodeNeighbors()
{
	//cout << endl << "in setNeighbor" << endl;
	for(int i = 0; i < nVertices; i++)
	{
		 //neighbor[i] = new list<int>;
		 inNeighbor[i] = new list<int>;	//dxl
		outNeighbor[i] = new list<int>;	//dxl
	}
	
	for(int edgeNO = 0; edgeNO < nEdgAdVeh; edgeNO++)
	{
		//for all no-depot edges
		if( edgeNO < nReqEdges || edgeNO >= nRqedgAdVeh )
		{	
			//neighbor[edges[edgeNO]->from]->push_back(edgeNO);	//hxh
			//neighbor[edges[edgeNO]->to]->push_back(edgeNO);	//hxh
			
			inNeighbor[edges[edgeNO]->to]->push_back(edgeNO);
			outNeighbor[edges[edgeNO]->from]->push_back(edgeNO);
			
			if( !edges[edgeNO]->bOneWay )
			{
				outNeighbor[edges[edgeNO]->to]->push_back(dualEdgeMap[edgeNO]);
				inNeighbor[edges[edgeNO]->from]->push_back(dualEdgeMap[edgeNO]);
			}
		}	//ensure that the edges returing to depot can turn to the vehicle edge
		else if( nRqedgAdVeh-1 == edgeNO )
		{
			//for the vehicle edge (start-1), omit all other vehicle edges
			//depot�ڶ�ȡ��ʱ���Ѿ����ˣ�
			outNeighbor[depot]->push_back(edgeNO); //dxl
		}
	}
	
	//������,������� 
	/*
	for(int edgeNO=0; edgeNO <5; edgeNO++)
	{
		cout<<endl<<"neighbors"<<endl;
		for(list<int>::iterator it=(*outNeighbor[edgeNO]).begin(); 
		    it!=(*outNeighbor[edgeNO]).end(); it++)
		{
			cout<<"  "<< *it ;
		}
		cout << endl;
	}
	*/	
		
}


//for each path, find the outwards paths  
void TCARPProblem::SetPathNeighbors()
{
	
	list<int>::iterator itListInt;	
	list<int> *pListInt;
	vector<pair<int,int> >::iterator itVectorPair;
	
	bool bMandatoryTurn = mandatoryTurns.size();
	bool bNoTurn = noTurns.size();
	
	pathNeighbor.resize(nTotalEdges);  //allocate memory
	//for each edge, find the outwards neighbors		
	for(int edgeNO = 0; edgeNO < nTotalEdges; edgeNO++)  //initialization
	{
		pathNeighbor[edgeNO] = new list<int>;
		
		if( 0 == (*outNeighbor[edges[edgeNO]->to]).size() )
		{
			 cout << "outneighbors don't exist for edge" << edgeNO;
			 exit(-1);
		}
		
		*pathNeighbor[edgeNO] = *outNeighbor[edges[edgeNO]->to];
	}
	
	if( bMandatoryTurn )
	{	//find if mandatory turn
		for(itVectorPair = mandatoryTurns.begin(); itVectorPair != mandatoryTurns.end(); 
		    itVectorPair++)
		{
			int edgeNO1 = (*itVectorPair).first;
			int edgeNO2 = (*itVectorPair).second;
			  
			(*pathNeighbor[edgeNO1]).clear();
			(*pathNeighbor[edgeNO1]).push_back(edgeNO2);
		}
	}
	
	if( bNoTurn )
	{
		//printf("edgeNO=%d   ",edgeNO);  
		//if(prTmp->size() < 3) 
		// printf("i=%d netighnum=%d    ",i,prTmp->size());	
				
		for(itVectorPair = noTurns.begin(); itVectorPair != noTurns.end(); 
	        itVectorPair++)
		{	
			int edgeNO1 = (*itVectorPair).first;
			int edgeNO2 = (*itVectorPair).second;
			
			(*pathNeighbor[edgeNO1]).remove(edgeNO2);
		}
	}
	//��Ų��Խ�� 
	//ofstream arcNeighbour("arcneighbor.txt", ios_base::out);
				
	for(int edgeNO = 0; edgeNO < nTotalEdges; edgeNO++)  //initialization
	{
		//int iTmp = SetEdgeNO(edgeNO);
		edges[edgeNO]->bCulDeSac = false;
			
		list<int> listInt(1, dualEdgeMap[edgeNO]);
									
		if( 0 == (*pathNeighbor[edgeNO]).size() )
		{
			cout << endl << "wrong! edge" << edgeNO << "has no turns!" << endl;
			exit(-1);
		}
		else if( listInt == *pathNeighbor[edgeNO] )
		{
			edges[edgeNO]->bCulDeSac = true;//culDeSac arc
			//cout << endl << "cul de sac edge  " << edgeNO << endl;
	    }
	    //������ȥ������U����,��ʱ�Ȳ�����
	 	/*
	    else if( edges[edgeNO]->to != depot )
	    {	//delete all U-turns
			(*pathNeighbor[edgeNO]).remove(dualEdgeMap[edgeNO]);
		}
		*/
		
		//������				
		//cout << "after conitue";
		//system("PAUSE");
		
		//cout << endl << "  edge " << edgeNO << "  " << pathNeighbor[edgeNO]->size(); 
		/*
		//������ 
		if(edgeNO == 61)
		{
			cout<<endl<<"neighbors"<<endl;
			for(list<int>::iterator it=(*pathNeighbor[edgeNO]).begin(); 
			    it!=(*pathNeighbor[edgeNO]).end(); it++)
			{
				cout<<"  "<< *it ;
			}
			cout << endl;
		}
		//���Խ���
		*/
		 
		//if(i%100==0)
		//	printf("i=%d size=%d    ",i,pathNeighbor[i]->size());
			//if(isFrom(i) == 390 && isTo(i) == 227)
		
		//arcNeighbour<<endl<<edgeNO;
		/*for(list<int>::iterator it=(*pathNeighbor[edgeNO]).begin(); 
		    it!=(*pathNeighbor[edgeNO]).end(); it++)
		{
			arcNeighbour<<"  "<< *it;
		}*/
		//���Խ���	
		 //arcNeighbour.close();
	    //system("PAUSE");
	}
		
}
	
//////////////////////////////////////////////////////////////////////////////////	
///////////////calculate the distance between each pair of paths by using/////////
/////////////////////////////Dijkstras algorithm//////////////////////////////////
 
void TCARPProblem::dijkstrasAlgForPaths(fstream& pathDist, fstream& pathMatrixFile) 	  
//void TCARPProblem::dijkstrasAlgForPaths()
{
	int u, v;
	list<int>::iterator il;
	vector<bool> fix(nTotalEdges);
	//vector<pair<int,int> >::iterator iterV;	
	
	//memory allocation
	pathsDistanceMatrix = new double*[nTotalEdges];	  
	pathMatrix = new int*[nTotalEdges];	//pathMatirx stores each arc's preceding arc
	deadHeadingTime = new double*[nTotalEdges];	  
	
	for(int i = 0; i < nTotalEdges; i++)
	{
		//shotstPaths[i].resize(nTotalEdges);
		//shotstRePaths[i].resize(nTotalEdges);
		pathsDistanceMatrix[i] = new double[nTotalEdges];
		pathMatrix[i] = new int[nTotalEdges];
		deadHeadingTime[i] = new double[nTotalEdges];
	}
			
	//calculate each entry (u,v)of the pathsDistaceMatrix
	for(u = 0; u < nTotalEdges; u++)
	{
		//if(u%100==0) 	cout << endl << "u= " << u;
		//only calculate one depot edge, omit all other ones
		if( (nReqEdges <= u) && (u < nRqedgAdVeh-1) )
		{//���ڵ�����������,���г����߶�����Ϊͬһ���ߣ���ֻ�����һ�� 
			continue;
		}
		
		//initilize pathsDistanceMatrix[u][v] and fix[v]
		for(v = 0; v < nTotalEdges; v++)
		{
			pathsDistanceMatrix[u][v] = numeric_limits<double>::max()-1;
			pathMatrix[u][v] = -1;
			deadHeadingTime[u][v] = numeric_limits<double>::max()-1;
			fix[v] = false;
		}
		
		//fix[u] = true;
		//Dijkstra�㷨��һ��,�ó�ʼֵ	 	 
		//reinitialize  pathsDistanceMatrix(u,v) by seting (u,v) = cost of the edg pair (u,v) for adjacent u,v.
		for(il = pathNeighbor[u]->begin(); il != pathNeighbor[u]->end(); il++)
		{
			pathsDistanceMatrix[u][*il] = 0;	     //edges[*il]->cost;
			pathMatrix[u][*il] = u;
			deadHeadingTime[u][*il] = 0;
			//if( (u < nReqEdges) || (u >= nEdgesAdVeh) )
				//shotstRePaths[u][*il].push_back(u);
		}
		
		//�㷨��Ҫ��������	
		//for given node u, find its shortest path to each other node,by fixing one node each time
		for(int cnt = 0; cnt < nTotalEdges; cnt++)
		{
			double min = numeric_limits<double>::max();
			int minV = -1;
			
			//printf("cnt=%d\n",cnt);	//dxl			
			//find the next node to be fixed, which is with the minimum cost
			for(v = 0; v < nTotalEdges; v++)
			{
				//���v��vehicle edge,����,ֻ����v=nRqdgAdVeh-1��������� 
				if( (nReqEdges <= v) && (v < nRqedgAdVeh-1) )
				{
					continue;
				}
				//�Ҵ����·��(u����minV)
				if( (false == fix[v]) && (pathsDistanceMatrix[u][v] < min) )
				{
					min = pathsDistanceMatrix[u][v];
					minV = v;
				}
			}
			
					
			if( -1 == minV )    //all edges (except depot edges) have been fixed
			{
				continue;
			}
			//cout<<"minV"<<minV<<" hewer"<<endl;
			fix[minV] = true;//fix the minV node
			
			// update all pending nodes adjacent to the just fixed node
			//�Ըչ̶��ı߶���,�����䵽�ڱ߶���ľ����ʱ�� 
			for(il = pathNeighbor[minV]->begin(); il != pathNeighbor[minV]->end(); il++)
			{
				//if(*il == u)
				//	  continue;
					
				//printf("\nsize=%d  \n",(*pathNeighbor[minV]).size());
				//printf("*il=%d  ",*il);
				if( pathsDistanceMatrix[u][*il] > (pathsDistanceMatrix[u][minV] + edges[minV]->cost) )
				{   //if need to update the cost
					pathsDistanceMatrix[u][*il] = pathsDistanceMatrix[u][minV] + edges[minV]->cost;//new cost value
					pathMatrix[u][*il] = minV;
					deadHeadingTime[u][*il] = deadHeadingTime[u][minV] + edges[minV]->deadHeadingTime; 
					//cout << "dist="<<pathsDistanceMatrixNew[u][*il] << endl;
				}
			}
		}
	}
	
	//printf("here1\n");	
   	//����������vehicle edge 
	for(u = nReqEdges; u < nRqedgAdVeh-1; u++)
	{
		for(v = 0; v < nTotalEdges; v++)
		{
			pathsDistanceMatrix[u][v] = pathsDistanceMatrix[nRqedgAdVeh-1][v];
			pathMatrix[u][v] = pathMatrix[nRqedgAdVeh-1][v];
			deadHeadingTime[u][v] = deadHeadingTime[nRqedgAdVeh-1][v];
		}
	}
	//printf("here2\n");	
	#if 1
	for(v = nReqEdges; v < nRqedgAdVeh-1; v++)
	{
		for(u = 0; u < nTotalEdges; u++)
		{
			pathsDistanceMatrix[u][v] = pathsDistanceMatrix[u][nRqedgAdVeh-1];
			pathMatrix[u][v] = pathMatrix[u][nRqedgAdVeh-1];
			deadHeadingTime[u][v] = deadHeadingTime[u][nRqedgAdVeh-1];
		}
	}
	#endif
	//printf("here3\n");	
	//cout<<endl;
	for(u = 0; u < nTotalEdges; u++)
	{
		for(v = 0; v < nTotalEdges; v++)
		{//������·������ 
            //cout<<(int)pathsDistanceMatrix[u][v] << " ";
            //cout<<  pathMatrix[u][v] << " ";
			pathDist << (int)pathsDistanceMatrix[u][v] << " ";
			pathMatrixFile <<  pathMatrix[u][v] << " ";
			//if(v==nTotalEdges-1)
				//fprintf(pathDistance,"\n");
		}
		//cout<<endl;
	}
	
	//������ 
	//printf("here4\n");	//dxl						
	//fclose(pathDist);	//dxl
    //fclose(pathMatrixFile);	//dxl
	//for(u = 0; u < nTotalEdges; u++)
		//{
	/*	  u=nEdgAdVeh - 1;
			for(v = 0; v < nTotalEdges; v++)
			{
				
				printf(" v=%d %lf",v,pathsDistanceMatrixNew[u][v]);
			}
	//	  }
	printf("\n*************************888888888888\n");*/
	//printf("here5\n");	//dxl
    /*for(u=0; u<nTotalEdges;u++)
	{
			  	     
		for(v = 0; v < nTotalEdges; v++)
		{
			if(pathsDistanceMatrix[u][v] > 0)
			{
				printf("u=%d v=%d   %lf  ",u,v,pathsDistanceMatrix[u][v]);
				//printf("    %lf\n",distanceMatrix[edge[u]->to][edge[v]->from]);
			}
		}
	}*/	 
	//printf("here6\n");	//dxl	
	//���Խ���										
}


//read the distance and path information from files 
void TCARPProblem::ReadMatrix(fstream& pathDist, fstream& pathMatrixFile)
{
	int u, v;
	list<int>::iterator il;
	vector<bool> fix(nTotalEdges);
	vector<pair<int,int> >::iterator iterV;
		
	//fstream pathDist("pathDist.out");	
    //fstream pathMatrixFile("path.out");	  
    
	//memory allocation
	//shotstPaths.resize(nTotalEdges);
	//shotstRePaths.resize(nTotalEdges);
	pathsDistanceMatrix = new double*[nTotalEdges];	  
	pathMatrix = new int*[nTotalEdges];	//pathMatirx stores each arc's preceding arc
	
	for(int i = 0; i < nTotalEdges; i++)
	{
		//shotstPaths[i].resize(nTotalEdges);
		//shotstRePaths[i].resize(nTotalEdges);
		pathsDistanceMatrix[i] = new double[nTotalEdges];
		pathMatrix[i] = new int[nTotalEdges];
	}
			    
	    //pathDist = fopen("pathDist.out","r");	//dxl
	    //pathMatrixFile = fopen("path.out","r");	  //dxl
	    
		//if( (pathDist == 0) || (pathMatrixFile == 0) )
		//{
		///	cout << "\n open file failure\n";
		//}
		
		for(u = 0; u < nTotalEdges; u++)
		{
			for(v = 0; v < nTotalEdges; v++)
			{
				//fscanf(pathDist,"%lf",&pathsDistanceMatrix[u][v]);
				//fscanf(pathMatrixFile,"%d",&pathMatrix[u][v]);
				pathDist >> pathsDistanceMatrix[u][v]  ;
				pathMatrixFile >> pathMatrix[u][v];
            }
		}
}
#if 0
//ð������
template<typename T>
void Bubble_Sort(vector<T>& array, vector<int>& auxiliaryIndex)
{
	int n=array.size();
	if(n==0)
		return;
	bool change;
	for(int i=n-1,change=true; i>=1 && change; --i)
	{
		change=false;
		T tmpT;
		int tmpI;
		for(int j=0;j<i;++j)
			if(array[j] > array[j+1])
			{
				tmpT=array[j+1];	tmpI=auxiliaryIndex[j+1]; 
				array[j+1]=array[j];	auxiliaryIndex[j+1]=auxiliaryIndex[j];
				array[j]=tmpT;	auxiliaryIndex[j]=tmpI;
				change=true;
			}
	}
} 
//����ĸ����е���С����
double g_min(double a, double b, double c, double d)
{
	double m = numeric_limits<double>::max();
	if(a < m)
		m = a;
	if(b < m)
		m = b;
	if(c < m)
		m = c;
	if(d < m)
		m = d;
	return m;
}
//���edgeNo���ڽӱ߼�����СΪlen(�ߵĴ洢����Ϊ��edgeNo�ľ���Զ�����ɽ���Զ)
void TCARPProblem::GetNeighborListofEdge()
{
    //������ÿ���ڽӱ߼��Ĵ�С                                      
	int len= Problem.nReqEdges;
	if(len >= 50)
		len /= 2;
	if(Problem.edges.size() != Problem.nTotalEdges)
	{
		cout<<"here error edge size"<<endl;
		return;
	}
	vector<double> tmpCost;
	vector<int > tmpList;
	for(int i=0; i<Problem.nTotalEdges; ++i)
		tmpList.push_back(i);

	for(int i=0; i<Problem.nTotalEdges; ++i)
	{
		tmpCost.assign(Problem.pathsDistanceMatrix[i], Problem.pathsDistanceMatrix[i] + Problem.nTotalEdges);
		Bubble_Sort(tmpCost, tmpList);
		for(int k=0; k != Problem.nTotalEdges; ++k)
		{
			if(find(Problem.edges[i]->NeighborList.begin(), Problem.edges[i]->NeighborList.end(), Problem.SetEdgeNO(tmpList[k]))
				!= Problem.edges[i]->NeighborList.end() || tmpList[k] >= Problem.nReqEdges)
				continue;
			Problem.edges[i]->NeighborList.push_back(Problem.SetEdgeNO(tmpList[k]));
			//Problem.edges[Problem.SetEdgeNO(tmpList[k])]->bexmain=UNEXAMINED;
			if(k >= len && Problem.edges[i]->NeighborList.size() == len)
				break;
		}
		
		tmpCost.clear();
		tmpList.clear();
	}

	#if 0
	//test begin
	ofstream ffo("neighorlist.txt");
	for(int i=0; i<Problem.nTotalEdges; ++i)
	{
		ffo<<i<<": ";
		for(vector<int>::iterator it=Problem.edges[i]->NeighborList.begin(); it != Problem.edges[i]->NeighborList.end(); ++it)
			ffo<<*it<<" ";
		ffo<<"\n";
	}
	ffo.close();
	//test finished
	#endif
}
#endif
////////////////////////////////////////////////////////////////////////////////
// ���ɾ���CARP����
// 06-04-18, 24
void TCARPProblem::CreateCARPInstance(char *fname) 
{
     //�����ļ���Ϊ�˼����ṩ�������ļ� 
	ofstream result("result.out");
	fstream pathDist("pathDist.out", ios::in | ios::out);	
	fstream pathMatrixFile("path.out", ios::in | ios::out);	 
	
	Load(fname);

	if (!fname)
		cerr << "error: unable to open input file: "<<fname << endl;
             
	SetNodeNeighbors();
	
	SetPathNeighbors();    
	//cout << endl << " finish setpathNeighborS" << endl;
	
	//���ظ�����ͬһ��carp����ʱ,��һ������ʡ�� 
	dijkstrasAlgForPaths(pathDist, pathMatrixFile);	 
	
	//basic_istream& seekg(pos_type _Pos);��ֱ�Ӷ�λλ��_Pos��;
	//����seekg(0)���������ֱ�Ӱ�ָ�붨λ���ļ��ײ�
	//pathDist.seekg(0);
	//pathMatrixFile.seekg(0);
	//ReadMatrix(pathDist, pathMatrixFile);
		
	result.close();	
	//GetNeighborListofEdge();
	
	cout << endl << "\nfunction CreateCARPInstance finished" << endl;
	//system("PAUSE");
}

