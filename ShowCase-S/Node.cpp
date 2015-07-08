#include"stdafx.h"
#include"Node.h"

vector<Node*> nodes;//网络中的节点

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
void Node::BroadcastPhase(){//广播阶段
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
void Node::MakeSureIsGatewayNodeOrNot(){//确定自己是否是gateway node,只是初步确定而已
	assert(currentWorkState == Receiving);
	//assert(relaxingTime == 0);
	if(m_Battery->GetEnergyGrade() <= 0){
		ofstream outLogFile("log.txt", ios_base::out | ios_base::app);
		assert(!outLogFile.fail());
		outLogFile<<__TIME__<<"节点"<<id<<"电池耗尽啦><"<<endl;
		outLogFile.close();
		return;
	}
	if(neighbors.size() == 0){
		ofstream outLogFile("log.txt", ios_base::out | ios_base::app);
		assert(!outLogFile.fail());
		outLogFile<<__TIME__<<"节点"<<id<< "孤立"<<endl;
		outLogFile.close();
		return;//有一个节点是不连通，fail to get a dominating set,网络瘫痪
	}
	if(neighbors.size() == 1){//即使这个唯一的邻居已经是gateway节点啦，可以在furtherdecision中进行比较选择比较合理的一个
		currentState = GatewayNode;
		return;
	}
	currentState = NonGatewayNode;	
	for(size_t i = 0; i < neighbors.size(); ++i){//Node 0是sink节点，我们约定sink节点
		size_t j = 0;
		for(; j < neighbors.size(); ++j){
			if(neighbors[i] == neighbors[j]){
				continue;
			}
			if(DIS(nodes[neighbors[i]]->GetXLoc(), nodes[neighbors[i]]->GetYLoc(),nodes[neighbors[j]]->GetXLoc(), nodes[neighbors[j]]->GetYLoc()) >= nodes[neighbors[i]]->GetTransDis()){
				//实现通信距离的不对称， 如果该节点的两个节点没有直接通信，则该节点就应该标记为gateway node
				currentState = GatewayNode;
				break;
			}
		}
		if(j < neighbors.size()){
			break;
		}
	}
}
void Rule1(int id1, int id2){//两个节点之间的比较，决定id1是不是gateway节点
	/*if(nodes[id1]->IsGatewayNodeOrNot () == NonGatewayNode || nodes[id2]->IsGatewayNodeOrNot() == NonGatewayNode){
		return;
	}
	*/	
	//加上工作时间的比较，是距离上一次休息的时间，还是从一开始工作的时间	
	//if(nodes[id1]->GetWorkingTime() >= MAX_WORK_TIME || (nodes[id1]->GetRelaxingTime() > 0 && nodes[id1]->GetRelaxingTime() < THRESHOLD)){
	//	nodes[id1]->SetNodeStatus( NonGatewayNode);//
	//	return;
	//}
		if(nodes[id1]->GetWorkingTime() > nodes[id2]->GetWorkingTime()){//该节点会较早的达到强制休息的触点，不如现在就停止
			nodes[id1]->SetNodeStatus(NonGatewayNode);
		}else{
			if(nodes[id1]->GetWorkingTime() == nodes[id2]->GetWorkingTime()){
				if(nodes[id1]->GetRelaxingTime() < nodes[id2]->GetRelaxingTime()){
					nodes[id1]->SetNodeStatus(NonGatewayNode);
				}else{
					if(nodes[id1]->GetRelaxingTime() == nodes[id2]->GetRelaxingTime()){
						if(nodes[id1]->GetBattery()->GetEnergyGrade() < nodes[id2]->GetBattery()->GetEnergyGrade()){//能量等级比较
							nodes[id1]->SetNodeStatus(NonGatewayNode);
						}else{
							if(nodes[id1]->GetBattery()->GetEnergyGrade() == nodes[id2]->GetBattery()->GetEnergyGrade()){
								if(nodes[id1]->GetNeighbors().size() < nodes[id2]->GetNeighbors().size()){//节点的度
									nodes[id1]->SetNodeStatus(NonGatewayNode);
								}else{
									if(nodes[id1]->GetNeighbors().size() == nodes[id2]->GetNeighbors().size()){//
										if(nodes[id1]->GetId() < nodes[id2]->GetId()){//节点Id break a tie
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
	if(nodes[id1]->GetBattery()->GetEnergyGrade() < nodes[id2]->GetBattery()->GetEnergyGrade()){//能量等级比较
		nodes[id1]->SetNodeStatus(NonGatewayNode);
	}else{
		if(nodes[id1]->GetBattery()->GetEnergyGrade() == nodes[id2]->GetBattery()->GetEnergyGrade()){
			if(nodes[id1]->GetNeighbors().size() < nodes[id2]->GetNeighbors().size()){//节点的度
				nodes[id1]->SetNodeStatus(NonGatewayNode);
			}else{
				if(nodes[id1]->GetNeighbors().size() == nodes[id2]->GetNeighbors().size()){//
					if(nodes[id1]->GetId() < nodes[id2]->GetId()){//节点Id break a tie
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
						if(nodes[id1]->GetBattery()->GetEnergyGrade() < nodes[id2]->GetBattery()->GetEnergyGrade()){//能量等级比较
							nodes[id1]->SetNodeStatus(NonGatewayNode);
						}else{
							if(nodes[id1]->GetBattery()->GetEnergyGrade() == nodes[id2]->GetBattery()->GetEnergyGrade()){
								if(nodes[id1]->GetNeighbors().size() < nodes[id2]->GetNeighbors().size()){//节点的度
									nodes[id1]->SetNodeStatus(NonGatewayNode);
								}else{
									if(nodes[id1]->GetNeighbors().size() == nodes[id2]->GetNeighbors().size()){//
										if(nodes[id1]->GetId() < nodes[id2]->GetId()){//节点Id break a tie
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
						if(nodes[id1]->GetBattery()->GetEnergyGrade() < nodes[id2]->GetBattery()->GetEnergyGrade()){//能量等级比较
							nodes[id1]->SetNodeStatus(NonGatewayNode);
						}else{
							if(nodes[id1]->GetBattery()->GetEnergyGrade() == nodes[id2]->GetBattery()->GetEnergyGrade()){
								if(nodes[id1]->GetNeighbors().size() < nodes[id2]->GetNeighbors().size()){//节点的度
									nodes[id1]->SetNodeStatus(NonGatewayNode);
								}else{
									if(nodes[id1]->GetNeighbors().size() == nodes[id2]->GetNeighbors().size()){//
										if(nodes[id1]->GetId() < nodes[id2]->GetId()){//节点Id break a tie
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
								&& nodes[id1]->GetBattery()->GetEnergyGrade() < nodes[id3]->GetBattery()->GetEnergyGrade()){//能量等级比较
								nodes[id1]->SetNodeStatus(NonGatewayNode);
							}else{
								if(nodes[id1]->GetBattery()->GetEnergyGrade() == nodes[id2]->GetBattery()->GetEnergyGrade()
									&& nodes[id1]->GetBattery()->GetEnergyGrade() < nodes[id3]->GetBattery()->GetEnergyGrade()){
									if(nodes[id1]->GetNeighbors().size() < nodes[id2]->GetNeighbors().size()){//节点的度
										nodes[id1]->SetNodeStatus(NonGatewayNode);
									}else{
										if(nodes[id1]->GetNeighbors().size() == nodes[id2]->GetNeighbors().size()){//
											if(nodes[id1]->GetId() < nodes[id2]->GetId()){//节点Id break a tie
												nodes[id1]->SetNodeStatus(NonGatewayNode);
											}
										}
									}
								}else{
									if(nodes[id1]->GetBattery()->GetEnergyGrade() == nodes[id2]->GetBattery()->GetEnergyGrade()
										&& nodes[id1]->GetBattery()->GetEnergyGrade() == nodes[id3]->GetBattery()->GetEnergyGrade()){
										if(nodes[id1]->GetNeighbors().size() < nodes[id2]->GetNeighbors().size()
											&& nodes[id1]->GetNeighbors().size() < nodes[id3]->GetNeighbors().size()){//节点的度
											nodes[id1]->SetNodeStatus(NonGatewayNode);
										}else{
											if(nodes[id1]->GetNeighbors().size() == nodes[id2]->GetNeighbors().size()
												&& nodes[id1]->GetNeighbors().size() < nodes[id3]->GetNeighbors().size()){//
												if(nodes[id1]->GetId() < nodes[id2]->GetId()){//节点Id break a tie
													nodes[id1]->SetNodeStatus(NonGatewayNode);
												}
											}else{
												if(nodes[id1]->GetNeighbors().size() == nodes[id2]->GetNeighbors().size()
												&& nodes[id1]->GetNeighbors().size() == nodes[id3]->GetNeighbors().size()){//
													if(nodes[id1]->GetId() < nodes[id2]->GetId() && nodes[id1]->GetId() < nodes[id3]->GetId()){//节点Id break a tie
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
		&& nodes[id1]->GetBattery()->GetEnergyGrade() < nodes[id3]->GetBattery()->GetEnergyGrade()){//能量等级比较
		nodes[id1]->SetNodeStatus(NonGatewayNode);
	}else{
		if(nodes[id1]->GetBattery()->GetEnergyGrade() == nodes[id2]->GetBattery()->GetEnergyGrade()
			&&nodes[id1]->GetBattery()->GetEnergyGrade() < nodes[id3]->GetBattery()->GetEnergyGrade()){
				
				
			if(nodes[id1]->GetNeighbors().size() < nodes[id2]->GetNeighbors().size()){//节点的度
				nodes[id1]->SetNodeStatus(NonGatewayNode);
			}else{
				if(nodes[id1]->GetNeighbors().size() == nodes[id2]->GetNeighbors().size()){//
					if(nodes[id1]->GetId() < nodes[id2]->GetId()){//节点Id break a tie
						nodes[id1]->SetNodeStatus(NonGatewayNode);
					}
				}
			}
					
				
		}else{
			if(nodes[id1]->GetBattery()->GetEnergyGrade() == nodes[id2]->GetBattery()->GetEnergyGrade()
				&&nodes[id1]->GetBattery()->GetEnergyGrade() == nodes[id3]->GetBattery()->GetEnergyGrade()){
				
				if(nodes[id1]->GetNeighbors().size() < nodes[id2]->GetNeighbors().size()
							&& nodes[id1]->GetNeighbors().size() < nodes[id3]->GetNeighbors().size()){//节点的度
							nodes[id1]->SetNodeStatus(NonGatewayNode);
				}else{
					if(nodes[id1]->GetNeighbors().size() == nodes[id2]->GetNeighbors().size()
						&& nodes[id1]->GetNeighbors().size() < nodes[id3]->GetNeighbors().size()){//
						if(nodes[id1]->GetId() < nodes[id2]->GetId()){//节点Id break a tie
							nodes[id1]->SetNodeStatus(NonGatewayNode);
						}
					}else{
						if(nodes[id1]->GetNeighbors().size() == nodes[id2]->GetNeighbors().size()
						&& nodes[id1]->GetNeighbors().size() == nodes[id3]->GetNeighbors().size()){//
							if(nodes[id1]->GetId() < nodes[id2]->GetId() && nodes[id1]->GetId() < nodes[id3]->GetId()){//节点Id break a tie
								nodes[id1]->SetNodeStatus(NonGatewayNode);
							}
						}
					}
				}
					
				}
					
			}
		}
}
void Node::FurtherDecision(const int& time){//进一步确定自己是不是gateway node，从而精简dominating set的大小
	assert(currentState == GatewayNode);
	assert(currentWorkState == Receiving);
	/*if(currentWorkState == Sleeping){
		ofstream outLogFile("log.txt", ios_base::out | ios_base::app);
		assert(!outLogFile.fail());
		outLogFile<<__TIME__<<"节点"<<id<<"都睡着了，还怎么进一步判断自己是不是gateway啊啊啊><"<<endl;
		outLogFile.close();
		return;
	}*/
	if(m_Battery->GetEnergyGrade() <= 0){
		ofstream outLogFile("log.txt", ios_base::out | ios_base::app);
		assert(!outLogFile.fail());
		outLogFile<<__TIME__<<"节点"<<id<<"电池耗尽啦><"<<endl;
		outLogFile.close();
		return;
	}
	//Rule 1
	for(size_t i = 0; i < neighbors.size(); ++i){
		if(nodes[neighbors[i]]->IsGatewayNodeOrNot() == GatewayNode){
			if(VectorBelong2Vector(neighbors,id, nodes[neighbors[i]]->GetNeighbors(), neighbors[i])){
				//如果当前节点的所有邻居也是其邻居gateway节点neighbor[i]的邻居
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
				//当前邻居的一个NonGateway节点没有其他Gateway节点邻居了，只有该节点一个，好可怜啊TQT,当前节点不能改为nongatwayNode啦
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
									//当前节点的两个gateway邻居节点通过另外一个邻居节点相连
									//并且其能量要高一点，或者其他比较方式
										Rule1(id, nodes[neighbors[i]]->GetNeighbors()[x]);
										if(currentState == NonGatewayNode){
											currentState = GatewayNode;//只是其中两个gateway节点有相连的而已，并不是所有的
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
			if(m == neighbors.size()){//当前邻居的两个Gateway节点没有其他Gateway节点邻居了，只有该节点一个，也比较可怜
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
		
		//如果gateway节点，要导致邻居节点重新选择啦
		for(size_t i = 0; i < neighbors.size(); ++i){
			if(nodes[neighbors[i]]->IsGatewayNodeOrNot() == NonGatewayNode){
				//
				/*if(!nodes[neighbors[i]]->routingTable.size()){
					nodes[neighbors[i]]->UpdateRoutingTable();
				}*/
				assert(nodes[neighbors[i]]->routingTable.size());
				
				if(nodes[neighbors[i]]->routingTable[0].destination != id && nodes[nodes[neighbors[i]]->routingTable[0].destination]->GetNodeWorkState() == Receiving){
					continue;
					//这个邻居节点本来就不用自己来转发节点，自己睡不睡，人家一点都不关心
				}
				double bestGatewayNodeCondition = 0.0;
				RoutingEntry entry;
				for(size_t j = 0; j < nodes[neighbors[i]]->GetNeighbors().size(); ++j){
					if(nodes[ nodes[neighbors[i]]->GetNeighbors()[j]]->IsGatewayNodeOrNot() == GatewayNode
						&&nodes[ nodes[neighbors[i]]->GetNeighbors()[j]]->GetId() != id
						&& nodes[nodes[neighbors[i]]->GetNeighbors()[j]]->GetNodeWorkState() == Receiving){
						if(nodes[nodes[neighbors[i]]->GetNeighbors()[j]]->GetBattery()->GetEnergyGrade() > bestGatewayNodeCondition){
							bestGatewayNodeCondition = nodes[nodes[neighbors[i]]->GetNeighbors()[j]]->GetBattery()->GetEnergyGrade();//暂时就先选择一个吧
							entry.destination = nodes[neighbors[i]]->GetNeighbors()[j];
							entry.nextHop = entry.destination;
							entry.distance = 1;
						}
				
					}
				}
				if(bestGatewayNodeCondition == double(0)){
					
					ofstream outLogFile("log.txt", ios_base::out | ios_base::app);
					assert(!outLogFile.fail());
					outLogFile<<__TIME__<<"NonGateway节点"<<neighbors[i]<< "在邻居节点"<<id<<" Sleep后孤立"<<endl;
					outLogFile.close();
					return false;//有一个节点是不连通，fail to get a dominating set,网络瘫痪
				}else{
					if(routingTable.size()){
						routingTable[0].destination = entry.destination;
						routingTable[0].nextHop = entry.nextHop;
					}else{
						routingTable.push_back(entry);
					}
				}
			}
			//else情况下，可能发生的情况是：
			//邻居的gateway节点可能需要当前节点转发消息，可是当前节点处于睡眠状态，不能参与转发,其他没有妨碍吧
		}
	}
	//else情况下，在实际的网络中，节点在睡之前通知一下邻居节点，使得邻居节点在进行状态抉择的时候不要把自己忘记啦
	//节点在sleep阶段错过的消息，会在节点醒来的时候，重新收集一下
	return true;
}
void Node::UpdateRoutingTable(){//更新路由表和domain nodes
//根据自己当前的状态确定自己的routing table，必要的话 nodes in its domain
	assert(currentWorkState == Receiving);//因为需要与邻居节点的交换路由表
	//assert(relaxingTime == 0);
	if(id == 0){
		id = 0;
	}
	if(id != 0 && m_Battery->GetEnergyGrade() <= 0){
		ofstream outLogFile("log.txt", ios_base::out | ios_base::app);
		assert(!outLogFile.fail());
		outLogFile<<__TIME__<<"节点"<<id<<"电池耗尽啦><"<<endl;
		outLogFile.close();
		return;
	}

	RoutingEntry entry;
	if(currentState == NonGatewayNode){
		//has no domain
		//routing table 只有一条，自己选择好的能量最高的gateway node，可不可以多选择几个，
		//再根据休息时间什么的确定，因为能量小的那个说不定再休息多久也不会再增加能量啦，而能量高的那个节点说不定还可以继续增加能量
		double bestGatewayNodeCondition = 0.0;
		if(routingTable.size()){
			bestGatewayNodeCondition = nodes[routingTable[0].nextHop]->GetBattery()->GetEnergyGrade();
		}
		for(size_t i = 0; i < neighbors.size(); ++i){
			if(nodes[neighbors[i]]->IsGatewayNodeOrNot() == GatewayNode 
				&& nodes[neighbors[i]]->GetNodeWorkState() == Receiving){
				if(nodes[neighbors[i]]->GetBattery()->GetEnergyGrade() > bestGatewayNodeCondition){
					bestGatewayNodeCondition = nodes[neighbors[i]]->GetBattery()->GetEnergyGrade();//暂时就先选择一个吧
					entry.destination = neighbors[i];
					entry.nextHop = entry.destination;
					entry.distance = 1;
				}
				
			}
		}
		if(bestGatewayNodeCondition == double(0)){
			ofstream outLogFile("log.txt", ios_base::out | ios_base::app);
			assert(!outLogFile.fail());
			outLogFile<<__TIME__<<"NonGateway节点"<<id<< "孤立"<<endl;
			outLogFile.close();
			return;//有一个节点是不连通，fail to get a dominating set,网络瘫痪
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
		
	}else{//Gateway节点
		for(size_t i = 0; i < neighbors.size(); ++i){
			if(nodes[neighbors[i]]->IsGatewayNodeOrNot() == GatewayNode){
				size_t j = 0;
				for(; j < routingTable.size(); ++j){
					if(neighbors[i] == routingTable[j].nextHop){//已经存在
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
					if(i == domainMembers[m]){//已经存在
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
			outLogFile<<__TIME__<<"Gateway节点"<<id<< "孤立"<<endl;
			outLogFile.close();
		}
		//根据邻居节点的routing table再更新一下
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
void Node::PacketGenerating(int time){//节点是否产生数据包，以及能量消耗
	if(m_Battery->GetEnergyGrade() <= 0){
		ofstream outLogFile("log.txt", ios_base::out | ios_base::app);
		assert(!outLogFile.fail());
		outLogFile<<__TIME__<<"节点"<<id<<"电池耗尽啦><"<<endl;
		outLogFile.close();
		return;
	}
	double pos = (double) (rand() + 1) / (double)RAND_MAX;
	if(time % SENSE_DURATION == 1){//因为是从第一秒开始计算的么
		if(pos <= POSIBILITY_PACKET_GENERATING){//产生数据包
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
		outLogFile<<__TIME__<<"节点"<<id<<"处于睡眠状态怎么能路由呢,一定是哪里弄错了><"<<endl;
		outLogFile.close();
		return;
	}
	if(m_Battery->GetEnergyGrade() <= 0){
		ofstream outLogFile("log.txt", ios_base::out | ios_base::app);
		assert(!outLogFile.fail());
		outLogFile<<__TIME__<<"节点"<<id<<"电池都耗尽啦，还路由什么啊><"<<endl;
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
							bestGatewayNodeCondition = nodes[neighbors[i]]->GetBattery()->GetEnergyGrade();//暂时就先选择一个吧
							entry.destination = neighbors[i];
							entry.nextHop = entry.destination;
							entry.distance = 1;
						}
				
					}
				}
				if(bestGatewayNodeCondition == double(0)){
					ofstream outLogFile("log.txt", ios_base::out | ios_base::app);
					assert(!outLogFile.fail());
					outLogFile<<__TIME__<<"NonGateway节点"<<id<< "孤立,无法路由啦"<<endl;
					outLogFile.close();
					return;//有一个节点是不连通，fail to get a dominating set,网络瘫痪
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
			if(domainMembers[d] == 0){//暂定sink节点Id为0，0节点一直都是醒着的啊啊啊
				assert(bufferedPackets.size());
				nodes[0]->PushPackets(bufferedPackets.front());
				
				bufferedPackets.pop();
				//找到该sink节点
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
					if(nodes[routingTable[i].destination]->GetDomainMembers()[j] == 0){//暂定sink节点Id为0
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
			if(i == routingTable.size()){//没有现成的路由，只有广播啦啦
				/*if(currentState == NonGatewayNode){
					double bestGatewayNodeCondition = 0.0;
					RoutingEntry entry;
					for(size_t i = 0; i < neighbors.size(); ++i){
						if(nodes[neighbors[i]]->IsGatewayNodeOrNot() == GatewayNode && nodes[neighbors[i]]->GetNodeWorkState() == Receiving){
							if(nodes[neighbors[i]]->GetBattery()->GetEnergyGrade() > bestGatewayNodeCondition){
								bestGatewayNodeCondition = nodes[neighbors[i]]->GetBattery()->GetEnergyGrade();//暂时就先选择一个吧
								entry.destination = neighbors[i];
								entry.nextHop = entry.destination;
								entry.distance = 1;
							}
				
						}
					}
					if(bestGatewayNodeCondition == double(0)){
						ofstream outLogFile("log.txt", ios_base::out | ios_base::app);
						assert(!outLogFile.fail());
						outLogFile<<__TIME__<<"NonGateway节点"<<id<< "孤立"<<endl;
						outLogFile.close();
						return;//有一个节点是不连通，fail to get a dominating set,网络瘫痪
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
						if(routingTable[m].distance == 1){//当然只广播一跳的邻居啦
							
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
void Node::Routing(){//路由，确定数据包在路由阶段经过哪些节点，相应节点就需要能量减少
	if(currentWorkState == Sleeping){
		ofstream outLogFile("log.txt", ios_base::out | ios_base::app);
		assert(!outLogFile.fail());
		outLogFile<<__TIME__<<"节点"<<id<<"处于睡眠状态怎么能路由呢><"<<endl;
		outLogFile.close();
		return;
	}
	if(m_Battery->GetEnergyGrade() <= 0){
		ofstream outLogFile("log.txt", ios_base::out | ios_base::app);
		assert(!outLogFile.fail());
		outLogFile<<__TIME__<<"节点"<<id<<"电池耗尽啦><"<<endl;
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
			outLogFile<<__TIME__<<"数据包TTL过期"<<id<<endl;
			outLogFile.close();
			return;
		}
		currentNode = nodes[allNodesOfForwardedPacket.front()];
		//currentNode->SetNodeWorkStatus(Receiving);
		assert(currentNode->GetNodeWorkState() == Receiving);
		size_t i = 0;
		size_t j = 0;
		for(i = 0; i < currentNode->domainMembers.size(); ++i){
			if(currentNode->GetDomainMembers()[i] == 0){//暂定sink节点Id为0
				//找到该sink节点
				//nodes[currentNode->domainMembers[i]]->AddEnergy( -(SEND_PACKET_ENERGY + RECEIVE_PACKET_ENERGY));
				nodes[0]->AddEnergy( -(RECEIVE_PACKET_ENERGY));
				break;
			}
		}
		if(i == currentNode->domainMembers.size()){
			for(i = 0; i < currentNode->routingTable.size(); ++i){
				j = 0;
				for(; j < nodes[currentNode->routingTable[i].destination]->GetDomainMembers().size(); ++j){
					if(nodes[currentNode->routingTable[i].destination]->GetDomainMembers()[j] == 0){//暂定sink节点Id为0
						//找到该sink节点
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
			if(i == currentNode->routingTable.size()){//没有现成的路由，只有广播啦啦
				for(size_t m = 0; m < currentNode->routingTable.size(); ++m){
					if(currentNode->routingTable[m].distance == 1){//当然只广播一跳的邻居啦
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
void Node::AddEnergy(double energy){//能量的加减
	//this->energyStatus += energy;
	/*if(this->energyStatus <= 0){
		cout<<"节点"<<this->id<<"电池耗尽啦><"<<endl;
		return;
	}*/
}

void Node::AddTime(){//增加距离上次作为gateway node工作的休息时间，
	if(id == 0){
		workingTime++;
		totalWorkTime++;
		return;//作为sink节点，要一直工作
	}
	switch(currentWorkState){
	case Receiving:
		workingTime++;
		totalWorkTime++;
		if(currentState == NonGatewayNode && workingTime >= MAX_WORK_TIME){
			//导致其他的邻居
			/*if(ForcedSleep()){
				currentWorkState = Sleeping;
				currentState = WhoKnows;
				//在睡着之前给所有的邻居节点发送信息。
			
				relaxingTime = 0;
				workingTime = 0;
			}
			*/
			currentWorkState = Sleeping;
			currentState = WhoKnows;
			//在睡着之前给所有的邻居节点发送信息。
			
			relaxingTime = 0;
			workingTime = 0;
		}
		
		break;
	case Sleeping:
		relaxingTime++;
		if(relaxingTime >= THRESHOLD){
			//确定需不需要醒过过来
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
void Node::MovementSimulation(){//确定自己是否移动，不移动返回false，移动的话重置相关信息
	//可以通过从文件中读取位置信息
	double movePos = (double) (rand() + 1) / (double)RAND_MAX;
	double x = 0.0, y = 0.0;
	if(movePos <= POSIBILITY_MOVEMENT){//移动
		x = DISTANCE_MOVEMENT * cos( 2 * PI * (double)(rand() + 1) / (double)(RAND_MAX)) * (double)(rand() + 1) / (double)(RAND_MAX);
		y = DISTANCE_MOVEMENT * sin( 2 * PI * (double)(rand() + 1) / (double)(RAND_MAX)) * (double)(rand() + 1) / (double)(RAND_MAX);
		xLoc = xLoc + x > 0?xLoc + x:xLoc;
		yLoc = yLoc + y > 0?yLoc + y:yLoc;
		if(currentWorkState == Receiving){
			WakeupAfterSleep();//只有active的节点才会通知邻居我移动啦，如果当前只是睡着，就不可能通知啦
		}
	}
	
}
void Node::WakeupAfterSleep(){
	//相关信息修改
		//移动导致旧邻居也需要更新
		for(vector<int>::iterator first = neighbors.begin(); first != neighbors.end(); ++first){
			if(DIS(xLoc, yLoc, nodes[*first]->GetXLoc(), nodes[*first]->GetYLoc()) >= nodes[*first]->GetTransDis()){//导致旧邻居已经不会再是自己的邻居啦><
				//从旧邻居的邻居表中删除自己
				//这些邻居需要重新计算自己的是不是dominating set啦
				//工作的邻居知道了当前节点移动的消息
				//那些睡着的节点怎么办，oh，我知道，在它们醒过来的时候，自己广播一条消息，重新获得一下自己的邻居
				
				nodes[*first]->DeleteNeighborAndUpdateRoutingTable(id);
			}
			if(DIS(xLoc, yLoc, nodes[*first]->GetXLoc(), nodes[*first]->GetYLoc()) >= transDis){
				//可以实现通信距离的不对称
				
				neighbors.erase(first);
				break;
				
			}
		}
		//获得新邻居以及新邻居更新
		for(int i = 0; i < NODE_NUMBER  && nodes[i]->GetNodeWorkState() == Receiving; ++i){
			if(id == i){
				continue;
			}
			if(DIS(xLoc, yLoc, nodes[i]->GetXLoc(), nodes[i]->GetYLoc()) < transDis){
				//可以实现通信距离的不对称
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
				//可以实现通信距离的不对称	
				nodes[i]->AddNeighborAndUpdateRoutingTable(id);
			}
		}
		//当前节点重新计算自己是否是gateway节点
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
void Node::AddNeighborAndUpdateRoutingTable(int n){//添加一个邻居,并更新路由
	if(id == 7){
		id = 7;
	}
	for(size_t i = 0; i < neighbors.size(); ++i){
		if(n == neighbors[i]){
			return;
		}
	}
	neighbors.push_back(n);
	if(currentState == NonGatewayNode){//新添的邻居，无论是移动过来的还是醒过来的，当前节点都需要判断自己的状态
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
int Node::GetId(){//获得节点Id
	return this->id;
}
double Node::GetXLoc(){//获得x坐标轴
	return this->xLoc;
}
double Node::GetYLoc(){//获得y坐标轴
	return this->yLoc;
}
double Node::GetTransDis(){//获得通信距离
	return this->transDis;
}
int Node::GetWorkingTime(){//获得已经工作的时间
	return this->workingTime;
}
NodeState Node::IsGatewayNodeOrNot(){//获得自己是否是gateway node
	return this->currentState;
}
vector<int> Node::GetNeighbors(){//获得自己的邻居节点
	return this->neighbors;
}
void Node::DeleteNeighborAndUpdateRoutingTable(int n){//删除一个自己的邻居节点By id，顺便删掉路由表的相关信息
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
		//删掉的邻居是一个gateway节点，就需要考虑自己是不是需要更新路由表
		MakeSureIsGatewayNodeOrNot();
		if(currentState == GatewayNode){
			FurtherDecision(-1);
		}
		UpdateRoutingTable();
	}
}
vector<int> Node::GetDomainMembers(){//获得自己domain nodes
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
bool VectorBelong2Vector(vector<int> first, int v, vector<int> second, int u){//判断第一个向量是否是第二个向量的子集,闭集
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

bool VectorBelong2VectorAndVector(vector<int> first, vector<int> second, vector<int> third){//判断第一个向量是否是属于后两个向量的并集
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

