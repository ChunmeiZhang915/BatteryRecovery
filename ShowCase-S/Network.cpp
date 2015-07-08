#include"stdafx.h"
#include"Network.h"
Network::Network( ){
}
Network::~Network(){
}

void Network::ConstructNetwork(){//构建一个网络，节点均匀分布
	ofstream initialPosFile("IniPos.txt", ios_base::out | ios_base::trunc);
	if(initialPosFile.fail()){
		ofstream outLogFile("log.txt", ios_base::out | ios_base::app);
		assert(!outLogFile.fail());
		outLogFile<<__TIME__<<"ConstructNetwork: Error opening this file:fixPos"<<endl;
		outLogFile.close();
		return;
	}
	for(int i = 0; i < NODE_NUMBER; ++i){
		nodes.push_back(new Node(i));
		initialPosFile<<nodes.back()->GetId()<<"\t"<<nodes.back()->GetXLoc()<<"\t"<<nodes.back()->GetYLoc()<<endl;
	}
	initialPosFile.close();
}
void Network::ConstructNetworkWithFixedPositon(){//固定位置的节点，搭建网络
	ifstream inFile("IniPos.txt", ios_base::in);
	//ifstream inFile("FixPos.txt", ios_base::in);
	if(inFile.fail()){
		ofstream outLogFile("log.txt", ios_base::out | ios_base::app);
		assert(!outLogFile.fail());
		outLogFile<<__TIME__<<"ConstructNetworkWithFixedPositon: Error opening this file:fixPos"<<endl;
		outLogFile.close();
		return;
	}
	int id;
	double x, y;
	while(inFile.good()){
		inFile>>id>>x>>y;
		nodes.push_back(new Node(id, x, y));
	}
	inFile.close();
}
void Network::GatherInformation(){//收集邻居节点的信息：其邻居的邻居信息（能量，工作时间）
	for(int i = 0; i < NODE_NUMBER; ++i){//sink节点也需要收集邻居信息
		//节点发送一个数据包需要消耗的能量
		//nodes[i]->AddEnergy(-SEND_PACKET_ENERGY);
		for(int j = 0; j < NODE_NUMBER; ++j){
			if(i == j){
				continue;
			}
			if(DIS(nodes[i]->GetXLoc(), nodes[i]->GetYLoc(), nodes[j]->GetXLoc(), nodes[j]->GetYLoc()) < nodes[i]->GetTransDis()){
				//可以实现通信距离的不对称
				nodes[i]->AddNeighbors(j);
				//接收邻居的节点的数据包，需要消耗能量
				//nodes[i]->AddEnergy(-RECEIVE_PACKET_ENERGY);
			}
		}
	}
}
void Network::MarkingProcess(){//根据节点的能量，工作时间等信息标记节点是否是Gateway Node
	nodes[0]->SetNodeStatus(NonGatewayNode);
	for(int i = 1; i < NODE_NUMBER; ++i){//sink节点不需要，一直处于工作状态
		nodes[i]->MakeSureIsGatewayNodeOrNot();
	}
}
void Network::AddRules(const int & time){//精简dominating set的大小
	for(int i = 1; i < NODE_NUMBER; ++i){
		if(nodes[i]->IsGatewayNodeOrNot() == GatewayNode){
			nodes[i]->FurtherDecision(time);
		}
	}
}
void Network::PacketDelivery(){
	for(int i = 1; i < NODE_NUMBER; ++i){
		nodes[i]->PacketGenerating(0);		
	}
}
void Network::AdjustPostion(){
	stringstream ss;
	string s;
	s.clear();
	ss.str("");
	ss<<0;
	s = "da/";
	s += ss.str();
	s += ".txt";
	ofstream outPutSnapshot(s, ios_base::in | ios_base::trunc);
	if(outPutSnapshot.fail()){
		ofstream outLogFile("log.txt", ios_base::out | ios_base::app);
		assert(!outLogFile.fail());
		outLogFile<<__TIME__<<"Network Run: Error opening this file:snapshot"<<endl;
		outLogFile.close();
		return;
	}
		
	for(int n = 0; n < NODE_NUMBER; ++n){
		string re(",");
		for(size_t i = 0; i < nodes[n]->GetNeighbors().size(); ++i){
			stringstream neis;
			neis<<nodes[n]->GetNeighbors()[i];
			re += neis.str();
			re += ",";
		}
		outPutSnapshot<<nodes[n]->GetId()<<"\t"
			<<(nodes[n]->GetXLoc() + SHIFT_MOVE - RADIUS)<<"\t"
			<<(nodes[n]->GetYLoc() + SHIFT_MOVE - RADIUS)<<"\t"
			<<nodes[n]->IsGatewayNodeOrNot()<<"\t"
			<<nodes[n]->GetNodeWorkState()<<"\t"
			<<nodes[n]->GetBattery()->GetCapacity()<<"\t"
			<<nodes[n]->GetRelaxingTime()<<"\t"
			<<nodes[n]->GetWorkingTime()<<"\t"
			<<nodes[n]->GetTotalWorkTime()<<"\t"
			<<nodes[n]->GetTransDis()<<"\t"
			<<re
			<<endl;
	}
	outPutSnapshot.close();
}
void Network::Run(){
	stringstream ss;
	string s;

	GatherInformation();
	MarkingProcess();
	AddRules(-1);
	for(int n = 0; n < NODE_NUMBER; ++n){//sink节点不移动
		nodes[n]->AddTime();
		nodes[n]->UpdateRoutingTable();						
	}
	for(int i = 1; ; ++i){
		if(i % UPDATE_DURATION == 0){
			nodes[0]->ClearInfo();		
			for(int n = 1; n < NODE_NUMBER; ++n){//sink节点不移动
				if(nodes[n]->GetBattery()->GetCapacity() <= 0){
					return;
				}
				nodes[n]->ClearInfo();			
			}
			GatherInformation();
			MarkingProcess();
			AddRules(i);
			for(int n = 0; n < NODE_NUMBER; ++n){//sink节点不移动				
				nodes[n]->UpdateRoutingTable();						
			}
		}else{
			for(int n = 1; n < NODE_NUMBER; ++n){//sink节点不移动
				if(nodes[n]->GetBattery()->GetCapacity() <= 0){
					return;
				}
				//nodes[n]->MovementSimulation();
				nodes[n]->PacketGenerating(i);			
			}
		}
		//产生数据包
		//路由导致的节点能量变化
		//判断网络是否瘫痪啦啦啦啦
		

		
		s.clear();
		ss.str("");
		ss<<i;
		s = "da/";
		s += ss.str();
		s += ".txt";
		ofstream outPutSnapshot(s, ios_base::in | ios_base::trunc);
		if(outPutSnapshot.fail()){
			ofstream outLogFile("log.txt", ios_base::out | ios_base::app);
			assert(!outLogFile.fail());
			outLogFile<<__TIME__<<"Network Run: Error opening this file:snapshot"<<endl;
			outLogFile.close();
			return;
		}
		
		for(int n = 1; n < NODE_NUMBER; ++n){
			
			nodes[n]->GetBattery()->Run(nodes[n]->GetNodeWorkState(), nodes[n]->GetRelaxingTime());
			nodes[n]->AddTime();
			string re(",");
			for(size_t i = 0; i < nodes[n]->GetNeighbors().size(); ++i){
				stringstream neis;
				neis<<nodes[n]->GetNeighbors()[i];
				re += neis.str();
				re += ",";
			}
			outPutSnapshot<<nodes[n]->GetId()<<"\t"
				<<(nodes[n]->GetXLoc() + SHIFT_MOVE - RADIUS)<<"\t"
				<<(nodes[n]->GetYLoc() + SHIFT_MOVE - RADIUS)<<"\t"
				<<nodes[n]->IsGatewayNodeOrNot()<<"\t"
				<<nodes[n]->GetNodeWorkState()<<"\t"
				<<nodes[n]->GetBattery()->GetCapacity()<<"\t"
				<<nodes[n]->GetRelaxingTime()<<"\t"
				<<nodes[n]->GetWorkingTime()<<"\t"
				<<nodes[n]->GetTotalWorkTime()<<"\t"
				<<nodes[n]->GetTransDis()<<"\t"
				<<re
				<<endl;
		}
		outPutSnapshot.close();
        
	}
}