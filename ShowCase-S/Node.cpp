#include"stdafx.h"
#include"Node.h"

vector<Node*> nodes;//�����еĽڵ�

Node::Node(int nodeId){
	id = nodeId;
	xLoc = ((double)(rand() + 1) / (double)(RAND_MAX)) * LENGTH;
	yLoc = ((double)(rand() + 1) / (double)(RAND_MAX)) * WIDTH;
	workingTime = 0;
	relaxingTime = 0;
	totalWorkTime = 0;
	transDis = COMMUNICATION_RANGE;
	currentState = NonGatewayNode;
	currentWorkState = Receiving;
	hop = 0;
	m_Battery = new Battery();
}
Node::~Node(){
	delete m_Battery;
}
void Node::CopyNode(const int& nodeId, const double& x, const double& y, const NodeState& isGateway,const NodeWorkState&  workState, const double& capacity, const int& totWorkTime, const int& relaTime, const int& workTime, const double& tranRange, const string& str){
	id = nodeId;
	xLoc = x;
	yLoc = y;
	workingTime = workTime;
	relaxingTime = relaTime;
	totalWorkTime = totWorkTime;
	if(isGateway == GatewayNode){
		currentState = GatewayNode;
	}else{
		currentState = NonGatewayNode;
	}
	
	if(workState == Receiving){
		currentWorkState = Receiving;
	}else{
		currentWorkState = Sleeping;
	}
	
	transDis = tranRange;
	m_Battery = new Battery(capacity);
	neis.clear();
	neis = str;
}
void Node::BroadcastPhase(){//�㲥�׶�
	assert(currentWorkState == Receiving);
	for(int i = 0; i < NODE_NUMBER; ++i){
		if(i == GetId()){
			continue;
		}
		if(DIS(xLoc, yLoc,nodes[i]->GetXLoc(), nodes[i]->GetYLoc()) < transDis){
			neighbors.push_back(i);
		}
	}
}
void Node::MakeSureIsGatewayNodeOrNot(){//ȷ���Լ��Ƿ���gateway node,ֻ�ǳ���ȷ������
	assert(currentWorkState == Receiving);
	//assert(relaxingTime == 0);
	if(m_Battery->GetEnergyGrade() <= 0){
		ofstream outLogFile("log.txt", ios_base::out | ios_base::app);
		assert(!outLogFile.fail());
		outLogFile<<__TIME__<<"�ڵ�"<<id<<"��غľ���><"<<endl;
		outLogFile.close();
		return;
	}
	if(neighbors.size() == 0){
		ofstream outLogFile("log.txt", ios_base::out | ios_base::app);
		assert(!outLogFile.fail());
		outLogFile<<__TIME__<<"�ڵ�"<<id<< "����"<<endl;
		outLogFile.close();
		return;//��һ���ڵ��ǲ���ͨ��fail to get a dominating set,����̱��
	}
	if(neighbors.size() == 1){//��ʹ���Ψһ���ھ��Ѿ���gateway�ڵ�����������furtherdecision�н��бȽ�ѡ��ȽϺ����һ��
		currentState = GatewayNode;
		return;
	}
	currentState = NonGatewayNode;	
	for(size_t i = 0; i < neighbors.size(); ++i){//Node 0��sink�ڵ㣬����Լ��sink�ڵ�
		size_t j = 0;
		for(; j < neighbors.size(); ++j){
			if(neighbors[i] == neighbors[j]){
				continue;
			}
			if(DIS(nodes[neighbors[i]]->GetXLoc(), nodes[neighbors[i]]->GetYLoc(),nodes[neighbors[j]]->GetXLoc(), nodes[neighbors[j]]->GetYLoc()) >= nodes[neighbors[i]]->GetTransDis()){
				//ʵ��ͨ�ž���Ĳ��Գƣ� ����ýڵ�������ڵ�û��ֱ��ͨ�ţ���ýڵ��Ӧ�ñ��Ϊgateway node
				currentState = GatewayNode;
				break;
			}
		}
		if(j < neighbors.size()){
			break;
		}
	}
}
void Rule1(int id1, int id2){//�����ڵ�֮��ıȽϣ�����id1�ǲ���gateway�ڵ�
	/*if(nodes[id1]->IsGatewayNodeOrNot () == NonGatewayNode || nodes[id2]->IsGatewayNodeOrNot() == NonGatewayNode){
		return;
	}
	*/	
	//���Ϲ���ʱ��ıȽϣ��Ǿ�����һ����Ϣ��ʱ�䣬���Ǵ�һ��ʼ������ʱ��	
	//if(nodes[id1]->GetWorkingTime() >= MAX_WORK_TIME || (nodes[id1]->GetRelaxingTime() > 0 && nodes[id1]->GetRelaxingTime() < THRESHOLD)){
	//	nodes[id1]->SetNodeStatus( NonGatewayNode);//
	//	return;
	//}
		if(nodes[id1]->GetWorkingTime() > nodes[id2]->GetWorkingTime()){//�ýڵ�����Ĵﵽǿ����Ϣ�Ĵ��㣬�������ھ�ֹͣ
			nodes[id1]->SetNodeStatus(NonGatewayNode);
		}else{
			if(nodes[id1]->GetWorkingTime() == nodes[id2]->GetWorkingTime()){
				if(nodes[id1]->GetRelaxingTime() < nodes[id2]->GetRelaxingTime()){
					nodes[id1]->SetNodeStatus(NonGatewayNode);
				}else{
					if(nodes[id1]->GetRelaxingTime() == nodes[id2]->GetRelaxingTime()){
						if(nodes[id1]->GetBattery()->GetEnergyGrade() < nodes[id2]->GetBattery()->GetEnergyGrade()){//�����ȼ��Ƚ�
							nodes[id1]->SetNodeStatus(NonGatewayNode);
						}else{
							if(nodes[id1]->GetBattery()->GetEnergyGrade() == nodes[id2]->GetBattery()->GetEnergyGrade()){
								if(nodes[id1]->GetNeighbors().size() < nodes[id2]->GetNeighbors().size()){//�ڵ�Ķ�
									nodes[id1]->SetNodeStatus(NonGatewayNode);
								}else{
									if(nodes[id1]->GetNeighbors().size() == nodes[id2]->GetNeighbors().size()){//
										if(nodes[id1]->GetId() < nodes[id2]->GetId()){//�ڵ�Id break a tie
											nodes[id1]->SetNodeStatus(NonGatewayNode);
										}
									}
								}
							}
						}
					}
			}
		}
						
	}
}
void Rule11(int id1, int id2){//Wu Jie's method
	if(nodes[id1]->GetBattery()->GetEnergyGrade() < nodes[id2]->GetBattery()->GetEnergyGrade()){//�����ȼ��Ƚ�
		nodes[id1]->SetNodeStatus(NonGatewayNode);
	}else{
		if(nodes[id1]->GetBattery()->GetEnergyGrade() == nodes[id2]->GetBattery()->GetEnergyGrade()){
			if(nodes[id1]->GetNeighbors().size() < nodes[id2]->GetNeighbors().size()){//�ڵ�Ķ�
				nodes[id1]->SetNodeStatus(NonGatewayNode);
			}else{
				if(nodes[id1]->GetNeighbors().size() == nodes[id2]->GetNeighbors().size()){//
					if(nodes[id1]->GetId() < nodes[id2]->GetId()){//�ڵ�Id break a tie
						nodes[id1]->SetNodeStatus(NonGatewayNode);
					}
				}
			}
		}
	}
}
void Node::SetNodeStatus(NodeState i){
		//workingTime++; 
		//relaxingTime = 0; 
		currentState = i;
}
void Rule2(int id1, int id2, int id3){
	//if(nodes[id1]->GetWorkingTime() >= MAX_WORK_TIME || (nodes[id1]->GetRelaxingTime() > 0 && nodes[id1]->GetRelaxingTime() < THRESHOLD)){
	//	nodes[id1]->SetNodeStatus( NonGatewayNode);//
	//	return;
	//}
	if(nodes[id1]->GetWorkingTime() > nodes[id2]->GetWorkingTime()
		&& nodes[id1]->GetWorkingTime() > nodes[id3]->GetWorkingTime()){
			nodes[id1]->SetNodeStatus(NonGatewayNode);
	}else{
		if(nodes[id1]->GetWorkingTime() == nodes[id2]->GetWorkingTime()
			&& nodes[id1]->GetWorkingTime() > nodes[id3]->GetWorkingTime()){
			if(nodes[id1]->GetRelaxingTime() < nodes[id2]->GetRelaxingTime()){
					nodes[id1]->SetNodeStatus(NonGatewayNode);
				}else{
					if(nodes[id1]->GetRelaxingTime() == nodes[id2]->GetRelaxingTime()){
						if(nodes[id1]->GetBattery()->GetEnergyGrade() < nodes[id2]->GetBattery()->GetEnergyGrade()){//�����ȼ��Ƚ�
							nodes[id1]->SetNodeStatus(NonGatewayNode);
						}else{
							if(nodes[id1]->GetBattery()->GetEnergyGrade() == nodes[id2]->GetBattery()->GetEnergyGrade()){
								if(nodes[id1]->GetNeighbors().size() < nodes[id2]->GetNeighbors().size()){//�ڵ�Ķ�
									nodes[id1]->SetNodeStatus(NonGatewayNode);
								}else{
									if(nodes[id1]->GetNeighbors().size() == nodes[id2]->GetNeighbors().size()){//
										if(nodes[id1]->GetId() < nodes[id2]->GetId()){//�ڵ�Id break a tie
											nodes[id1]->SetNodeStatus(NonGatewayNode);
										}
									}
								}
							}
						}
					}
			}
		}else{
			if(nodes[id1]->GetWorkingTime() == nodes[id2]->GetWorkingTime()
				&& nodes[id1]->GetWorkingTime() == nodes[id3]->GetWorkingTime()){
				if(nodes[id1]->GetRelaxingTime() < nodes[id2]->GetRelaxingTime()
					&& nodes[id1]->GetRelaxingTime() < nodes[id3]->GetRelaxingTime()){
					nodes[id1]->SetNodeStatus(NonGatewayNode);
				}else{
					if(nodes[id1]->GetRelaxingTime() == nodes[id2]->GetRelaxingTime()
						&&nodes[id1]->GetRelaxingTime() < nodes[id3]->GetRelaxingTime()){
						if(nodes[id1]->GetBattery()->GetEnergyGrade() < nodes[id2]->GetBattery()->GetEnergyGrade()){//�����ȼ��Ƚ�
							nodes[id1]->SetNodeStatus(NonGatewayNode);
						}else{
							if(nodes[id1]->GetBattery()->GetEnergyGrade() == nodes[id2]->GetBattery()->GetEnergyGrade()){
								if(nodes[id1]->GetNeighbors().size() < nodes[id2]->GetNeighbors().size()){//�ڵ�Ķ�
									nodes[id1]->SetNodeStatus(NonGatewayNode);
								}else{
									if(nodes[id1]->GetNeighbors().size() == nodes[id2]->GetNeighbors().size()){//
										if(nodes[id1]->GetId() < nodes[id2]->GetId()){//�ڵ�Id break a tie
											nodes[id1]->SetNodeStatus(NonGatewayNode);
										}
									}
								}
							}
						}
					}else{
						if(nodes[id1]->GetRelaxingTime() == nodes[id2]->GetRelaxingTime()
							&&nodes[id1]->GetRelaxingTime() == nodes[id3]->GetRelaxingTime()){
							if(nodes[id1]->GetBattery()->GetEnergyGrade() < nodes[id2]->GetBattery()->GetEnergyGrade()
								&& nodes[id1]->GetBattery()->GetEnergyGrade() < nodes[id3]->GetBattery()->GetEnergyGrade()){//�����ȼ��Ƚ�
								nodes[id1]->SetNodeStatus(NonGatewayNode);
							}else{
								if(nodes[id1]->GetBattery()->GetEnergyGrade() == nodes[id2]->GetBattery()->GetEnergyGrade()
									&& nodes[id1]->GetBattery()->GetEnergyGrade() < nodes[id3]->GetBattery()->GetEnergyGrade()){
									if(nodes[id1]->GetNeighbors().size() < nodes[id2]->GetNeighbors().size()){//�ڵ�Ķ�
										nodes[id1]->SetNodeStatus(NonGatewayNode);
									}else{
										if(nodes[id1]->GetNeighbors().size() == nodes[id2]->GetNeighbors().size()){//
											if(nodes[id1]->GetId() < nodes[id2]->GetId()){//�ڵ�Id break a tie
												nodes[id1]->SetNodeStatus(NonGatewayNode);
											}
										}
									}
								}else{
									if(nodes[id1]->GetBattery()->GetEnergyGrade() == nodes[id2]->GetBattery()->GetEnergyGrade()
										&& nodes[id1]->GetBattery()->GetEnergyGrade() == nodes[id3]->GetBattery()->GetEnergyGrade()){
										if(nodes[id1]->GetNeighbors().size() < nodes[id2]->GetNeighbors().size()
											&& nodes[id1]->GetNeighbors().size() < nodes[id3]->GetNeighbors().size()){//�ڵ�Ķ�
											nodes[id1]->SetNodeStatus(NonGatewayNode);
										}else{
											if(nodes[id1]->GetNeighbors().size() == nodes[id2]->GetNeighbors().size()
												&& nodes[id1]->GetNeighbors().size() < nodes[id3]->GetNeighbors().size()){//
												if(nodes[id1]->GetId() < nodes[id2]->GetId()){//�ڵ�Id break a tie
													nodes[id1]->SetNodeStatus(NonGatewayNode);
												}
											}else{
												if(nodes[id1]->GetNeighbors().size() == nodes[id2]->GetNeighbors().size()
												&& nodes[id1]->GetNeighbors().size() == nodes[id3]->GetNeighbors().size()){//
													if(nodes[id1]->GetId() < nodes[id2]->GetId() && nodes[id1]->GetId() < nodes[id3]->GetId()){//�ڵ�Id break a tie
														nodes[id1]->SetNodeStatus(NonGatewayNode);
													}
												}
											}
										}
									}
								}
							}
						}
					}
			}
			}
		}

	}
}
void Rule22(int id1, int id2, int id3){
	if(nodes[id1]->GetBattery()->GetEnergyGrade() < nodes[id2]->GetBattery()->GetEnergyGrade()
		&& nodes[id1]->GetBattery()->GetEnergyGrade() < nodes[id3]->GetBattery()->GetEnergyGrade()){//�����ȼ��Ƚ�
		nodes[id1]->SetNodeStatus(NonGatewayNode);
	}else{
		if(nodes[id1]->GetBattery()->GetEnergyGrade() == nodes[id2]->GetBattery()->GetEnergyGrade()
			&&nodes[id1]->GetBattery()->GetEnergyGrade() < nodes[id3]->GetBattery()->GetEnergyGrade()){
				
				
			if(nodes[id1]->GetNeighbors().size() < nodes[id2]->GetNeighbors().size()){//�ڵ�Ķ�
				nodes[id1]->SetNodeStatus(NonGatewayNode);
			}else{
				if(nodes[id1]->GetNeighbors().size() == nodes[id2]->GetNeighbors().size()){//
					if(nodes[id1]->GetId() < nodes[id2]->GetId()){//�ڵ�Id break a tie
						nodes[id1]->SetNodeStatus(NonGatewayNode);
					}
				}
			}
					
				
		}else{
			if(nodes[id1]->GetBattery()->GetEnergyGrade() == nodes[id2]->GetBattery()->GetEnergyGrade()
				&&nodes[id1]->GetBattery()->GetEnergyGrade() == nodes[id3]->GetBattery()->GetEnergyGrade()){
				
				if(nodes[id1]->GetNeighbors().size() < nodes[id2]->GetNeighbors().size()
							&& nodes[id1]->GetNeighbors().size() < nodes[id3]->GetNeighbors().size()){//�ڵ�Ķ�
							nodes[id1]->SetNodeStatus(NonGatewayNode);
				}else{
					if(nodes[id1]->GetNeighbors().size() == nodes[id2]->GetNeighbors().size()
						&& nodes[id1]->GetNeighbors().size() < nodes[id3]->GetNeighbors().size()){//
						if(nodes[id1]->GetId() < nodes[id2]->GetId()){//�ڵ�Id break a tie
							nodes[id1]->SetNodeStatus(NonGatewayNode);
						}
					}else{
						if(nodes[id1]->GetNeighbors().size() == nodes[id2]->GetNeighbors().size()
						&& nodes[id1]->GetNeighbors().size() == nodes[id3]->GetNeighbors().size()){//
							if(nodes[id1]->GetId() < nodes[id2]->GetId() && nodes[id1]->GetId() < nodes[id3]->GetId()){//�ڵ�Id break a tie
								nodes[id1]->SetNodeStatus(NonGatewayNode);
							}
						}
					}
				}
					
				}
					
			}
		}
}
void Node::FurtherDecision(const int& time){//��һ��ȷ���Լ��ǲ���gateway node���Ӷ�����dominating set�Ĵ�С
	assert(currentState == GatewayNode);
	assert(currentWorkState == Receiving);
	/*if(currentWorkState == Sleeping){
		ofstream outLogFile("log.txt", ios_base::out | ios_base::app);
		assert(!outLogFile.fail());
		outLogFile<<__TIME__<<"�ڵ�"<<id<<"��˯���ˣ�����ô��һ���ж��Լ��ǲ���gateway������><"<<endl;
		outLogFile.close();
		return;
	}*/
	if(m_Battery->GetEnergyGrade() <= 0){
		ofstream outLogFile("log.txt", ios_base::out | ios_base::app);
		assert(!outLogFile.fail());
		outLogFile<<__TIME__<<"�ڵ�"<<id<<"��غľ���><"<<endl;
		outLogFile.close();
		return;
	}
	//Rule 1
	for(size_t i = 0; i < neighbors.size(); ++i){
		if(nodes[neighbors[i]]->IsGatewayNodeOrNot() == GatewayNode){
			if(VectorBelong2Vector(neighbors,id, nodes[neighbors[i]]->GetNeighbors(), neighbors[i])){
				//�����ǰ�ڵ�������ھ�Ҳ�����ھ�gateway�ڵ�neighbor[i]���ھ�
				Rule1(id, neighbors[i]);
				//Rule11(id, neighbors[i]);
			}
		}
	}
	if(currentState == NonGatewayNode){
		return;
	}
	//Rule 2
	for(size_t i = 0; i < neighbors.size(); ++i){
		for(size_t j = 0; j < neighbors.size(); ++j){
			if(neighbors[i] == neighbors[j]){
				continue;
			}
			if(nodes[neighbors[i]]->IsGatewayNodeOrNot() == GatewayNode && nodes[neighbors[j]]->IsGatewayNodeOrNot() == GatewayNode){
				if(VectorBelong2VectorAndVector(neighbors, nodes[neighbors[i]]->GetNeighbors(), nodes[neighbors[j]]->GetNeighbors())
					&& !VectorBelong2VectorAndVector(nodes[neighbors[i]]->GetNeighbors(),neighbors, nodes[neighbors[j]]->GetNeighbors())
					&& !VectorBelong2VectorAndVector(nodes[neighbors[j]]->GetNeighbors(),neighbors, nodes[neighbors[i]]->GetNeighbors())
					){
					currentState = NonGatewayNode;
				}else{
					if(VectorBelong2VectorAndVector(neighbors, nodes[neighbors[i]]->GetNeighbors(), nodes[neighbors[j]]->GetNeighbors())
						&& VectorBelong2VectorAndVector(nodes[neighbors[i]]->GetNeighbors(),neighbors, nodes[neighbors[j]]->GetNeighbors())
						&& !VectorBelong2VectorAndVector(nodes[neighbors[j]]->GetNeighbors(),neighbors, nodes[neighbors[i]]->GetNeighbors())
						){
							Rule1(id, neighbors[i]);
							//Rule11(id, neighbors[i]);
					}else{
						if(VectorBelong2VectorAndVector(neighbors, nodes[neighbors[i]]->GetNeighbors(), nodes[neighbors[j]]->GetNeighbors())
							&& VectorBelong2VectorAndVector(nodes[neighbors[i]]->GetNeighbors(),neighbors, nodes[neighbors[j]]->GetNeighbors())
							&& VectorBelong2VectorAndVector(nodes[neighbors[j]]->GetNeighbors(),neighbors, nodes[neighbors[i]]->GetNeighbors())
							){
								Rule2(id, neighbors[i], neighbors[j]);
								//Rule22(id, neighbors[i], neighbors[j]);
						}
					}
				}
			}
		}
	}
	/*if(time != -1 && currentState == GatewayNode){
		double pos = ((double)(totalWorkTime) / (double)(time));
		double randN = ((double) (rand() + 1) / (double)(RAND_MAX));
		if( totalWorkTime >= MAX_WORK_TIME &&  randN < pos){
			size_t i = 0;
			for(; i < neighbors.size(); ++i){
				if(nodes[neighbors[i]]->GetNodeWorkState() == Sleeping){
					continue;
				}
				size_t j = 0;
				for(; j < nodes[neighbors[i]]->neighbors.size(); ++j){
					if(nodes[nodes[neighbors[i]]->neighbors[j]]->IsGatewayNodeOrNot() == GatewayNode && id != nodes[neighbors[i]]->neighbors[j]){
						break;
					}
				}
				if(j == nodes[neighbors[i]]->neighbors.size()){
					break;
				}
			}
			if(i == neighbors.size()){
				currentState = NonGatewayNode;
			}
		}
	}*/
	//Rule 3
	/*
	size_t i = 0;
	for(; i < neighbors.size(); ++i){
		if(nodes[neighbors[i]]->IsGatewayNodeOrNot() == NonGatewayNode){
			size_t n = 0;
			for(; n < nodes[neighbors[i]]->GetNeighbors().size(); ++n){
				if(nodes[nodes[neighbors[i]]->GetNeighbors()[n]]->IsGatewayNodeOrNot() == GatewayNode
					&& nodes[neighbors[i]]->GetNeighbors()[n] != id){
					break;
				}
			}
			if(n == nodes[neighbors[i]]->GetNeighbors().size()){
				//��ǰ�ھӵ�һ��NonGateway�ڵ�û������Gateway�ڵ��ھ��ˣ�ֻ�иýڵ�һ�����ÿ�����TQT,��ǰ�ڵ㲻�ܸ�ΪnongatwayNode��
				return;
			}
		}else{
			size_t m = 0;
			for(; m < neighbors.size(); ++m){
				if(neighbors[i] == neighbors[m]){
					continue;
				}else{
					if(nodes[neighbors[m]]->IsGatewayNodeOrNot() == GatewayNode){
						size_t x = 0;
						size_t y = 0;
						for(x = 0; x < nodes[neighbors[i]]->GetNeighbors().size(); ++x){						
							for(y = 0; y < nodes[neighbors[m]]->GetNeighbors().size(); ++y){
								if(nodes[neighbors[i]]->GetNeighbors()[x] == nodes[neighbors[m]]->GetNeighbors()[y]
								&& nodes[nodes[neighbors[i]]->GetNeighbors()[x]]->IsGatewayNodeOrNot() == GatewayNode
									&& nodes[neighbors[i]]->GetNeighbors()[x] != id){
									//��ǰ�ڵ������gateway�ھӽڵ�ͨ������һ���ھӽڵ�����
									//����������Ҫ��һ�㣬���������ȽϷ�ʽ
										Rule1(id, nodes[neighbors[i]]->GetNeighbors()[x]);
										if(currentState == NonGatewayNode){
											currentState = GatewayNode;//ֻ����������gateway�ڵ��������Ķ��ѣ����������е�
											break;
										}
								}
							}
							if(y < nodes[neighbors[m]]->GetNeighbors().size()){
								break;
							}
						}
						if(y < nodes[neighbors[m]]->GetNeighbors().size()){
							break;
						}
					}
				}
			}
			if(m == neighbors.size()){//��ǰ�ھӵ�����Gateway�ڵ�û������Gateway�ڵ��ھ��ˣ�ֻ�иýڵ�һ����Ҳ�ȽϿ���
				return;
			}
		}
	}
	if( i == neighbors.size()){
		currentState = NonGatewayNode;
	}
	
	//relaxingTime = 0;
	*/
}
bool Node::ForcedSleep(){
	assert(currentWorkState == Receiving);
	//assert(relaxingTime == 0);
	if(currentState == GatewayNode){
		
		//���gateway�ڵ㣬Ҫ�����ھӽڵ�����ѡ����
		for(size_t i = 0; i < neighbors.size(); ++i){
			if(nodes[neighbors[i]]->IsGatewayNodeOrNot() == NonGatewayNode){
				//
				/*if(!nodes[neighbors[i]]->routingTable.size()){
					nodes[neighbors[i]]->UpdateRoutingTable();
				}*/
				assert(nodes[neighbors[i]]->routingTable.size());
				
				if(nodes[neighbors[i]]->routingTable[0].destination != id && nodes[nodes[neighbors[i]]->routingTable[0].destination]->GetNodeWorkState() == Receiving){
					continue;
					//����ھӽڵ㱾���Ͳ����Լ���ת���ڵ㣬�Լ�˯��˯���˼�һ�㶼������
				}
				double bestGatewayNodeCondition = 0.0;
				RoutingEntry entry;
				for(size_t j = 0; j < nodes[neighbors[i]]->GetNeighbors().size(); ++j){
					if(nodes[ nodes[neighbors[i]]->GetNeighbors()[j]]->IsGatewayNodeOrNot() == GatewayNode
						&&nodes[ nodes[neighbors[i]]->GetNeighbors()[j]]->GetId() != id
						&& nodes[nodes[neighbors[i]]->GetNeighbors()[j]]->GetNodeWorkState() == Receiving){
						if(nodes[nodes[neighbors[i]]->GetNeighbors()[j]]->GetBattery()->GetEnergyGrade() > bestGatewayNodeCondition){
							bestGatewayNodeCondition = nodes[nodes[neighbors[i]]->GetNeighbors()[j]]->GetBattery()->GetEnergyGrade();//��ʱ����ѡ��һ����
							entry.destination = nodes[neighbors[i]]->GetNeighbors()[j];
							entry.nextHop = entry.destination;
							entry.distance = 1;
						}
				
					}
				}
				if(bestGatewayNodeCondition == double(0)){
					
					ofstream outLogFile("log.txt", ios_base::out | ios_base::app);
					assert(!outLogFile.fail());
					outLogFile<<__TIME__<<"NonGateway�ڵ�"<<neighbors[i]<< "���ھӽڵ�"<<id<<" Sleep�����"<<endl;
					outLogFile.close();
					return false;//��һ���ڵ��ǲ���ͨ��fail to get a dominating set,����̱��
				}else{
					if(routingTable.size()){
						routingTable[0].destination = entry.destination;
						routingTable[0].nextHop = entry.nextHop;
					}else{
						routingTable.push_back(entry);
					}
				}
			}
			//else����£����ܷ���������ǣ�
			//�ھӵ�gateway�ڵ������Ҫ��ǰ�ڵ�ת����Ϣ�����ǵ�ǰ�ڵ㴦��˯��״̬�����ܲ���ת��,����û�з�����
		}
	}
	//else����£���ʵ�ʵ������У��ڵ���˯֮ǰ֪ͨһ���ھӽڵ㣬ʹ���ھӽڵ��ڽ���״̬�����ʱ��Ҫ���Լ�������
	//�ڵ���sleep�׶δ������Ϣ�����ڽڵ�������ʱ�������ռ�һ��
	return true;
}
void Node::UpdateRoutingTable(){//����·�ɱ��domain nodes
//�����Լ���ǰ��״̬ȷ���Լ���routing table����Ҫ�Ļ� nodes in its domain
	assert(currentWorkState == Receiving);//��Ϊ��Ҫ���ھӽڵ�Ľ���·�ɱ�
	//assert(relaxingTime == 0);
	if(id == 0){
		id = 0;
	}
	if(id != 0 && m_Battery->GetEnergyGrade() <= 0){
		ofstream outLogFile("log.txt", ios_base::out | ios_base::app);
		assert(!outLogFile.fail());
		outLogFile<<__TIME__<<"�ڵ�"<<id<<"��غľ���><"<<endl;
		outLogFile.close();
		return;
	}

	RoutingEntry entry;
	if(currentState == NonGatewayNode){
		//has no domain
		//routing table ֻ��һ�����Լ�ѡ��õ�������ߵ�gateway node���ɲ����Զ�ѡ�񼸸���
		//�ٸ�����Ϣʱ��ʲô��ȷ������Ϊ����С���Ǹ�˵��������Ϣ���Ҳ�������������������������ߵ��Ǹ��ڵ�˵���������Լ�����������
		double bestGatewayNodeCondition = 0.0;
		if(routingTable.size()){
			bestGatewayNodeCondition = nodes[routingTable[0].nextHop]->GetBattery()->GetEnergyGrade();
		}
		for(size_t i = 0; i < neighbors.size(); ++i){
			if(nodes[neighbors[i]]->IsGatewayNodeOrNot() == GatewayNode 
				&& nodes[neighbors[i]]->GetNodeWorkState() == Receiving){
				if(nodes[neighbors[i]]->GetBattery()->GetEnergyGrade() > bestGatewayNodeCondition){
					bestGatewayNodeCondition = nodes[neighbors[i]]->GetBattery()->GetEnergyGrade();//��ʱ����ѡ��һ����
					entry.destination = neighbors[i];
					entry.nextHop = entry.destination;
					entry.distance = 1;
				}
				
			}
		}
		if(bestGatewayNodeCondition == double(0)){
			ofstream outLogFile("log.txt", ios_base::out | ios_base::app);
			assert(!outLogFile.fail());
			outLogFile<<__TIME__<<"NonGateway�ڵ�"<<id<< "����"<<endl;
			outLogFile.close();
			return;//��һ���ڵ��ǲ���ͨ��fail to get a dominating set,����̱��
		}else{
			if(routingTable.size()){
				if(bestGatewayNodeCondition != nodes[routingTable[0].nextHop]->GetBattery()->GetEnergyGrade()){
					routingTable[0].destination = entry.destination;
					routingTable[0].nextHop = entry.nextHop;
				}
			}else{
				routingTable.push_back(entry);
			}
		}
		
	}else{//Gateway�ڵ�
		for(size_t i = 0; i < neighbors.size(); ++i){
			if(nodes[neighbors[i]]->IsGatewayNodeOrNot() == GatewayNode){
				size_t j = 0;
				for(; j < routingTable.size(); ++j){
					if(neighbors[i] == routingTable[j].nextHop){//�Ѿ�����
						break;
					}
				}
				if(j == routingTable.size()){
					entry.destination = neighbors[i];
					entry.nextHop = entry.destination;
					entry.distance = 1;
					routingTable.push_back(entry);
				}
			}else{
				size_t m = 0;
				for(; m < domainMembers.size(); ++m){
					if(i == domainMembers[m]){//�Ѿ�����
						break;
					}
				}
				if(m == domainMembers.size()){
					domainMembers.push_back(neighbors[i]);
				}
				
			}
		}
		if(!routingTable.size()){
			ofstream outLogFile("log.txt", ios_base::out | ios_base::app);
			assert(!outLogFile.fail());
			outLogFile<<__TIME__<<"Gateway�ڵ�"<<id<< "����"<<endl;
			outLogFile.close();
		}
		//�����ھӽڵ��routing table�ٸ���һ��
		for(size_t i = 0; i < neighbors.size(); ++i){
			if(nodes[neighbors[i]]->IsGatewayNodeOrNot() == GatewayNode){
				for(size_t j = 0; j < nodes[neighbors[i]]->routingTable.size(); ++j){
					size_t k = 0;
					for(; k < routingTable.size(); ++k){
						if(nodes[neighbors[i]]->routingTable[j].destination == routingTable[k].destination 
							|| nodes[neighbors[i]]->routingTable[j].destination == id){
							break;
						}
					}
					if(k == routingTable.size()){
						entry.destination = nodes[neighbors[i]]->routingTable[j].destination;
						entry.nextHop = neighbors[i];
						entry.distance = nodes[neighbors[i]]->routingTable[j].distance + 1;
						routingTable.push_back(entry);
					}
				}
			}
		}
	}
}
void Node::PacketGenerating(int time){//�ڵ��Ƿ�������ݰ����Լ���������
	if(m_Battery->GetEnergyGrade() <= 0){
		ofstream outLogFile("log.txt", ios_base::out | ios_base::app);
		assert(!outLogFile.fail());
		outLogFile<<__TIME__<<"�ڵ�"<<id<<"��غľ���><"<<endl;
		outLogFile.close();
		return;
	}
	double pos = (double) (rand() + 1) / (double)RAND_MAX;
	if(time % SENSE_DURATION == 1){//��Ϊ�Ǵӵ�һ�뿪ʼ�����ô
		if(pos <= POSIBILITY_PACKET_GENERATING){//�������ݰ�
			bufferedPackets.push(Packet(id, MAX_TIME));
		}
		if(currentWorkState == Sleeping){
			return;
		}
		
		int i = 0;
		while(!bufferedPackets.empty() && i < MAX_PACKETS_PER_SLOT){
			//RoutingWithBufferedPackets();
			bufferedPackets.pop();
			++i;
		}
		if(bufferedPackets.empty() && currentState == NonGatewayNode){
			pos = (double) (rand() + 1) / (double)RAND_MAX;
			if(pos <= POSIBILITY_PACKET_GENERATING){
				currentWorkState = Sleeping;
				workingTime = 0;
				relaxingTime = 0;
			}
		}
	}
}
void Node::PushPackets(Packet p){
	bufferedPackets.push(p);
}
void Node::RoutingWithBufferedPackets(){
	
	assert(bufferedPackets.size());
	
	assert(currentWorkState == Receiving);
	if(currentWorkState == Sleeping){
		ofstream outLogFile("log.txt", ios_base::out | ios_base::app);
		assert(!outLogFile.fail());
		outLogFile<<__TIME__<<"�ڵ�"<<id<<"����˯��״̬��ô��·����,һ��������Ū����><"<<endl;
		outLogFile.close();
		return;
	}
	if(m_Battery->GetEnergyGrade() <= 0){
		ofstream outLogFile("log.txt", ios_base::out | ios_base::app);
		assert(!outLogFile.fail());
		outLogFile<<__TIME__<<"�ڵ�"<<id<<"��ض��ľ�������·��ʲô��><"<<endl;
		outLogFile.close();
		return;
	}
	if(bufferedPackets.empty()){
		return;//No Packets in the queue
	}
	--(bufferedPackets.front().TTL);
	if(bufferedPackets.front().TTL >= 0){
		if(id == 0){
			assert(bufferedPackets.size());
			bufferedPackets.pop();
			return;
		}
		if(currentState == NonGatewayNode){
			if(!routingTable.size() || nodes[routingTable[0].nextHop]->GetNodeWorkState() == Sleeping){
				double bestGatewayNodeCondition = 0.0;
				RoutingEntry entry;
				for(size_t i = 0; i < neighbors.size(); ++i){
					if(nodes[neighbors[i]]->IsGatewayNodeOrNot() == GatewayNode 
						&& nodes[neighbors[i]]->GetNodeWorkState() == Receiving){
						if(nodes[neighbors[i]]->GetBattery()->GetEnergyGrade() > bestGatewayNodeCondition){
							bestGatewayNodeCondition = nodes[neighbors[i]]->GetBattery()->GetEnergyGrade();//��ʱ����ѡ��һ����
							entry.destination = neighbors[i];
							entry.nextHop = entry.destination;
							entry.distance = 1;
						}
				
					}
				}
				if(bestGatewayNodeCondition == double(0)){
					ofstream outLogFile("log.txt", ios_base::out | ios_base::app);
					assert(!outLogFile.fail());
					outLogFile<<__TIME__<<"NonGateway�ڵ�"<<id<< "����,�޷�·����"<<endl;
					outLogFile.close();
					return;//��һ���ڵ��ǲ���ͨ��fail to get a dominating set,����̱��
				}else{
					if(routingTable.size()){
						routingTable[0].destination = entry.destination;
						routingTable[0].nextHop = entry.nextHop;
					}else{
						routingTable.push_back(entry);
					}
				}
			}
			assert(bufferedPackets.size());
			nodes[routingTable[0].nextHop]->PushPackets(bufferedPackets.front());
			
			bufferedPackets.pop();
			return;
		}
		size_t d = 0;
		size_t j = 0;
		for(d = 0; d < domainMembers.size(); ++d){
			if(domainMembers[d] == 0){//�ݶ�sink�ڵ�IdΪ0��0�ڵ�һֱ�������ŵİ�����
				assert(bufferedPackets.size());
				nodes[0]->PushPackets(bufferedPackets.front());
				
				bufferedPackets.pop();
				//�ҵ���sink�ڵ�
				//nodes[currentNode->domainMembers[i]]->AddEnergy( -(SEND_PACKET_ENERGY + RECEIVE_PACKET_ENERGY));
				//nodes[0]->AddEnergy( -(RECEIVE_PACKET_ENERGY));
				break;
			}
		}
		if(d == domainMembers.size()){
			size_t i = 0;
			for(i = 0; i < routingTable.size(); ++i){
				j = 0;
				for(; j < nodes[routingTable[i].destination]->GetDomainMembers().size()
					&& nodes[routingTable[i].nextHop]->GetNodeWorkState() == Receiving; ++j){
					if(nodes[routingTable[i].destination]->GetDomainMembers()[j] == 0){//�ݶ�sink�ڵ�IdΪ0
						assert(bufferedPackets.size());
						nodes[routingTable[i].nextHop]->PushPackets(bufferedPackets.front());
						
						bufferedPackets.pop();
						break;
					}
				}
				if(j < nodes[routingTable[i].destination]->GetDomainMembers().size()){
					break;
				}	
			}
			if(i == routingTable.size()){//û���ֳɵ�·�ɣ�ֻ�й㲥����
				/*if(currentState == NonGatewayNode){
					double bestGatewayNodeCondition = 0.0;
					RoutingEntry entry;
					for(size_t i = 0; i < neighbors.size(); ++i){
						if(nodes[neighbors[i]]->IsGatewayNodeOrNot() == GatewayNode && nodes[neighbors[i]]->GetNodeWorkState() == Receiving){
							if(nodes[neighbors[i]]->GetBattery()->GetEnergyGrade() > bestGatewayNodeCondition){
								bestGatewayNodeCondition = nodes[neighbors[i]]->GetBattery()->GetEnergyGrade();//��ʱ����ѡ��һ����
								entry.destination = neighbors[i];
								entry.nextHop = entry.destination;
								entry.distance = 1;
							}
				
						}
					}
					if(bestGatewayNodeCondition == double(0)){
						ofstream outLogFile("log.txt", ios_base::out | ios_base::app);
						assert(!outLogFile.fail());
						outLogFile<<__TIME__<<"NonGateway�ڵ�"<<id<< "����"<<endl;
						outLogFile.close();
						return;//��һ���ڵ��ǲ���ͨ��fail to get a dominating set,����̱��
					}else{
						if(routingTable.size()){
							routingTable[0].destination = entry.destination;
							routingTable[0].nextHop = entry.nextHop;
						}else{
							routingTable.push_back(entry);
						}
					}

				}else{
				*/
					bool isOut = false;
					for(size_t m = 0; m < routingTable.size() 
						&& nodes[routingTable[m].nextHop]->GetNodeWorkState() == Receiving; ++m){
						if(routingTable[m].distance == 1){//��Ȼֻ�㲥һ�����ھ���
							
							assert(bufferedPackets.size());
							
							nodes[routingTable[m].nextHop]->PushPackets(bufferedPackets.front());
							isOut = true;
						}
					}
					if(isOut){
						assert(bufferedPackets.size());
						bufferedPackets.pop();
					}
				//}
			}
		}
	}
	
}
void Node::Routing(){//·�ɣ�ȷ�����ݰ���·�ɽ׶ξ�����Щ�ڵ㣬��Ӧ�ڵ����Ҫ��������
	if(currentWorkState == Sleeping){
		ofstream outLogFile("log.txt", ios_base::out | ios_base::app);
		assert(!outLogFile.fail());
		outLogFile<<__TIME__<<"�ڵ�"<<id<<"����˯��״̬��ô��·����><"<<endl;
		outLogFile.close();
		return;
	}
	if(m_Battery->GetEnergyGrade() <= 0){
		ofstream outLogFile("log.txt", ios_base::out | ios_base::app);
		assert(!outLogFile.fail());
		outLogFile<<__TIME__<<"�ڵ�"<<id<<"��غľ���><"<<endl;
		outLogFile.close();
		return;
	}
	allNodesOfForwardedPacket.push(id);
	allNodesOfForwardedPacket.push(-1);
	Node* currentNode = NULL;
	while(allNodesOfForwardedPacket.front() != -1){
		if(hop > MAX_TIME){
			//TTL expired
			ofstream outLogFile("log.txt", ios_base::out | ios_base::app);
			assert(!outLogFile.fail());
			outLogFile<<__TIME__<<"���ݰ�TTL����"<<id<<endl;
			outLogFile.close();
			return;
		}
		currentNode = nodes[allNodesOfForwardedPacket.front()];
		//currentNode->SetNodeWorkStatus(Receiving);
		assert(currentNode->GetNodeWorkState() == Receiving);
		size_t i = 0;
		size_t j = 0;
		for(i = 0; i < currentNode->domainMembers.size(); ++i){
			if(currentNode->GetDomainMembers()[i] == 0){//�ݶ�sink�ڵ�IdΪ0
				//�ҵ���sink�ڵ�
				//nodes[currentNode->domainMembers[i]]->AddEnergy( -(SEND_PACKET_ENERGY + RECEIVE_PACKET_ENERGY));
				nodes[0]->AddEnergy( -(RECEIVE_PACKET_ENERGY));
				break;
			}
		}
		if(i == currentNode->domainMembers.size()){
			for(i = 0; i < currentNode->routingTable.size(); ++i){
				j = 0;
				for(; j < nodes[currentNode->routingTable[i].destination]->GetDomainMembers().size(); ++j){
					if(nodes[currentNode->routingTable[i].destination]->GetDomainMembers()[j] == 0){//�ݶ�sink�ڵ�IdΪ0
						//�ҵ���sink�ڵ�
						if(currentNode->routingTable[i].destination == currentNode->routingTable[i].nextHop){
							nodes[currentNode->routingTable[i].destination]->AddEnergy( -(SEND_PACKET_ENERGY + RECEIVE_PACKET_ENERGY));
							nodes[0]->AddEnergy( -(RECEIVE_PACKET_ENERGY));
						}else{
							nodes[currentNode->routingTable[i].nextHop]->AddEnergy( -(SEND_PACKET_ENERGY + RECEIVE_PACKET_ENERGY));
							allNodesOfForwardedPacket.push(currentNode->routingTable[i].nextHop);
						}
						
						break;
					}
				}
				if(j < nodes[currentNode->routingTable[i].destination]->GetDomainMembers().size()){
					break;
				}	
			}
			if(i == currentNode->routingTable.size()){//û���ֳɵ�·�ɣ�ֻ�й㲥����
				for(size_t m = 0; m < currentNode->routingTable.size(); ++m){
					if(currentNode->routingTable[m].distance == 1){//��Ȼֻ�㲥һ�����ھ���
						allNodesOfForwardedPacket.push(currentNode->routingTable[m].destination);
						nodes[currentNode->routingTable[m].destination]->AddEnergy( -(SEND_PACKET_ENERGY + RECEIVE_PACKET_ENERGY));
					}
				}
			}
		}
		allNodesOfForwardedPacket.pop();
		if(allNodesOfForwardedPacket.front() == -1){
			++hop;
			allNodesOfForwardedPacket.pop();
			allNodesOfForwardedPacket.push(-1);
		}
	}
}
void Node::AddEnergy(double energy){//�����ļӼ�
	//this->energyStatus += energy;
	/*if(this->energyStatus <= 0){
		cout<<"�ڵ�"<<this->id<<"��غľ���><"<<endl;
		return;
	}*/
}

void Node::AddTime(){//���Ӿ����ϴ���Ϊgateway node��������Ϣʱ�䣬
	if(id == 0){
		workingTime++;
		totalWorkTime++;
		return;//��Ϊsink�ڵ㣬Ҫһֱ����
	}
	switch(currentWorkState){
	case Receiving:
		workingTime++;
		totalWorkTime++;
		if(currentState == NonGatewayNode && workingTime >= MAX_WORK_TIME){
			//�����������ھ�
			/*if(ForcedSleep()){
				currentWorkState = Sleeping;
				currentState = WhoKnows;
				//��˯��֮ǰ�����е��ھӽڵ㷢����Ϣ��
			
				relaxingTime = 0;
				workingTime = 0;
			}
			*/
			currentWorkState = Sleeping;
			currentState = WhoKnows;
			//��˯��֮ǰ�����е��ھӽڵ㷢����Ϣ��
			
			relaxingTime = 0;
			workingTime = 0;
		}
		
		break;
	case Sleeping:
		relaxingTime++;
		if(relaxingTime >= THRESHOLD){
			//ȷ���費��Ҫ�ѹ�����
			relaxingTime = 0;
			workingTime = 0;
			currentWorkState = Receiving;
			WakeupAfterSleep();
		}
		break;
	default:
		ofstream outLogFile("log.txt", ios_base::out | ios_base::app);
		assert(!outLogFile.fail());
		outLogFile<<__TIME__<<"CurrentWorkStatus Error"<<endl;
		outLogFile.close();
	}
}
void Node::MovementSimulation(){//ȷ���Լ��Ƿ��ƶ������ƶ�����false���ƶ��Ļ����������Ϣ
	//����ͨ�����ļ��ж�ȡλ����Ϣ
	double movePos = (double) (rand() + 1) / (double)RAND_MAX;
	double x = 0.0, y = 0.0;
	if(movePos <= POSIBILITY_MOVEMENT){//�ƶ�
		x = DISTANCE_MOVEMENT * cos( 2 * PI * (double)(rand() + 1) / (double)(RAND_MAX)) * (double)(rand() + 1) / (double)(RAND_MAX);
		y = DISTANCE_MOVEMENT * sin( 2 * PI * (double)(rand() + 1) / (double)(RAND_MAX)) * (double)(rand() + 1) / (double)(RAND_MAX);
		xLoc = xLoc + x > 0?xLoc + x:xLoc;
		yLoc = yLoc + y > 0?yLoc + y:yLoc;
		if(currentWorkState == Receiving){
			WakeupAfterSleep();//ֻ��active�Ľڵ�Ż�֪ͨ�ھ����ƶ����������ǰֻ��˯�ţ��Ͳ�����֪ͨ��
		}
	}
	
}
void Node::WakeupAfterSleep(){
	//�����Ϣ�޸�
		//�ƶ����¾��ھ�Ҳ��Ҫ����
		for(vector<int>::iterator first = neighbors.begin(); first != neighbors.end(); ++first){
			if(DIS(xLoc, yLoc, nodes[*first]->GetXLoc(), nodes[*first]->GetYLoc()) >= nodes[*first]->GetTransDis()){//���¾��ھ��Ѿ����������Լ����ھ���><
				//�Ӿ��ھӵ��ھӱ���ɾ���Լ�
				//��Щ�ھ���Ҫ���¼����Լ����ǲ���dominating set��
				//�������ھ�֪���˵�ǰ�ڵ��ƶ�����Ϣ
				//��Щ˯�ŵĽڵ���ô�죬oh����֪�����������ѹ�����ʱ���Լ��㲥һ����Ϣ�����»��һ���Լ����ھ�
				
				nodes[*first]->DeleteNeighborAndUpdateRoutingTable(id);
			}
			if(DIS(xLoc, yLoc, nodes[*first]->GetXLoc(), nodes[*first]->GetYLoc()) >= transDis){
				//����ʵ��ͨ�ž���Ĳ��Գ�
				
				neighbors.erase(first);
				break;
				
			}
		}
		//������ھ��Լ����ھӸ���
		for(int i = 0; i < NODE_NUMBER  && nodes[i]->GetNodeWorkState() == Receiving; ++i){
			if(id == i){
				continue;
			}
			if(DIS(xLoc, yLoc, nodes[i]->GetXLoc(), nodes[i]->GetYLoc()) < transDis){
				//����ʵ��ͨ�ž���Ĳ��Գ�
				vector<int>::iterator first = neighbors.begin();
				for(; first != neighbors.end(); ++first){
					if(i == *first){
						break;
					}
				}
				if(first == neighbors.end()){
					neighbors.push_back(i);
				}
			}
			if(DIS(xLoc, yLoc, nodes[i]->GetXLoc(), nodes[i]->GetYLoc()) < nodes[i]->transDis){
				//����ʵ��ͨ�ž���Ĳ��Գ�	
				nodes[i]->AddNeighborAndUpdateRoutingTable(id);
			}
		}
		//��ǰ�ڵ����¼����Լ��Ƿ���gateway�ڵ�
		MakeSureIsGatewayNodeOrNot();
		if(currentState == GatewayNode){
			FurtherDecision(-1);
		}
		UpdateRoutingTable();
}
void Node::AddNeighbors(int n){
	if(id == 7){
		id = 7;
	}
	for(size_t i = 0; i < neighbors.size(); ++i){
		if(n == neighbors[i]){
			return;
		}
	}
	neighbors.push_back(n);
}
void Node::AddNeighborAndUpdateRoutingTable(int n){//���һ���ھ�,������·��
	if(id == 7){
		id = 7;
	}
	for(size_t i = 0; i < neighbors.size(); ++i){
		if(n == neighbors[i]){
			return;
		}
	}
	neighbors.push_back(n);
	if(currentState == NonGatewayNode){//������ھӣ��������ƶ������Ļ����ѹ����ģ���ǰ�ڵ㶼��Ҫ�ж��Լ���״̬
		MakeSureIsGatewayNodeOrNot();
		if(currentState == GatewayNode){
			FurtherDecision(-1);
		}
		UpdateRoutingTable();
	}else{
		RoutingEntry entry;
		
		if(nodes[n]->IsGatewayNodeOrNot() == GatewayNode){
			size_t j = 0;
			for(; j < routingTable.size(); ++j){
				if(n == routingTable[j].destination){
					break;
				}
			}
			if(j == routingTable.size()){
				entry.destination = nodes[n]->GetId();
				entry.nextHop = entry.destination;
				entry.distance = 1;
				routingTable.push_back(entry);
			}
		}else{
			size_t m = 0;
			for(; m < domainMembers.size(); ++m){
				if(n == domainMembers[m]){
					break;
				}
			}
			if(m == domainMembers.size()){
				domainMembers.push_back(n);
			}
				
		}
	}
}
int Node::GetId(){//��ýڵ�Id
	return this->id;
}
double Node::GetXLoc(){//���x������
	return this->xLoc;
}
double Node::GetYLoc(){//���y������
	return this->yLoc;
}
double Node::GetTransDis(){//���ͨ�ž���
	return this->transDis;
}
int Node::GetWorkingTime(){//����Ѿ�������ʱ��
	return this->workingTime;
}
NodeState Node::IsGatewayNodeOrNot(){//����Լ��Ƿ���gateway node
	return this->currentState;
}
vector<int> Node::GetNeighbors(){//����Լ����ھӽڵ�
	return this->neighbors;
}
void Node::DeleteNeighborAndUpdateRoutingTable(int n){//ɾ��һ���Լ����ھӽڵ�By id��˳��ɾ��·�ɱ�������Ϣ
	for(vector<int>::iterator first = neighbors.begin(); first != neighbors.end(); ++first){
		if(n == *first){
			neighbors.erase(first);
			break;
		}
	}
	if(currentState == GatewayNode && nodes[n]->IsGatewayNodeOrNot() == NonGatewayNode){
		for(vector<int>::iterator iPtr = domainMembers.begin(); iPtr != domainMembers.end(); ++iPtr){
			if(n == *iPtr){
				domainMembers.erase(iPtr);
				break;
			}
		}
	}
	if(this->currentState == GatewayNode && nodes[n]->IsGatewayNodeOrNot() == GatewayNode){
		for(vector<RoutingEntry>::iterator iPtr = this->routingTable.begin(); iPtr != this->routingTable.end(); ++iPtr){
			if(n == iPtr->destination){
				routingTable.erase(iPtr);
				break;
			}
		}
	}
	if(currentState == NonGatewayNode && nodes[n]->IsGatewayNodeOrNot() == GatewayNode){
		//ɾ�����ھ���һ��gateway�ڵ㣬����Ҫ�����Լ��ǲ�����Ҫ����·�ɱ�
		MakeSureIsGatewayNodeOrNot();
		if(currentState == GatewayNode){
			FurtherDecision(-1);
		}
		UpdateRoutingTable();
	}
}
vector<int> Node::GetDomainMembers(){//����Լ�domain nodes
	return domainMembers;
}
void Node::ClearInfo(){
	routingTable.clear();
	domainMembers.clear();
	neighbors.clear();
	//totalWorkTime ++;
	//relaxingTime = 0;
	//workingTime = 0;
	currentState = NonGatewayNode;
	currentWorkState = Receiving;
}
bool VectorBelong2Vector(vector<int> first, int v, vector<int> second, int u){//�жϵ�һ�������Ƿ��ǵڶ����������Ӽ�,�ռ�
	size_t j = 0;
	first.push_back(v);
	second.push_back(u);
	for(size_t i = 0; i < first.size(); ++i){
		j = 0;
		for(; j < second.size(); ++j){
			if(first[i] == second[j]){
				break;
			}
		}
		if(j == second.size()){
			return false;
		}
	}
	return true;
}

bool VectorBelong2VectorAndVector(vector<int> first, vector<int> second, vector<int> third){//�жϵ�һ�������Ƿ������ں����������Ĳ���
	size_t j = 0;
	for(size_t i = 0; i < third.size(); ++i){
		second.push_back(third[i]);
	}
	for(size_t i = 0; i < first.size(); ++i){
		j = 0;
		for(; j < second.size(); ++j){
			if(first[i] == second[i]){
				break;
			}
		}
		if(j == second.size()){
			return false;
		}
	}
	return true;
}

