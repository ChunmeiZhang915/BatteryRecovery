#include"stdafx.h"
#include"Network.h"
Network::Network( ){
}
Network::~Network(){
}

void Network::ConstructNetwork(){//����һ�����磬�ڵ���ȷֲ�
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
void Network::ConstructNetworkWithFixedPositon(){//�̶�λ�õĽڵ㣬�����
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
void Network::GatherInformation(){//�ռ��ھӽڵ����Ϣ�����ھӵ��ھ���Ϣ������������ʱ�䣩
	for(int i = 0; i < NODE_NUMBER; ++i){//sink�ڵ�Ҳ��Ҫ�ռ��ھ���Ϣ
		//�ڵ㷢��һ�����ݰ���Ҫ���ĵ�����
		//nodes[i]->AddEnergy(-SEND_PACKET_ENERGY);
		for(int j = 0; j < NODE_NUMBER; ++j){
			if(i == j){
				continue;
			}
			if(DIS(nodes[i]->GetXLoc(), nodes[i]->GetYLoc(), nodes[j]->GetXLoc(), nodes[j]->GetYLoc()) < nodes[i]->GetTransDis()){
				//����ʵ��ͨ�ž���Ĳ��Գ�
				nodes[i]->AddNeighbors(j);
				//�����ھӵĽڵ�����ݰ�����Ҫ��������
				//nodes[i]->AddEnergy(-RECEIVE_PACKET_ENERGY);
			}
		}
	}
}
void Network::MarkingProcess(){//���ݽڵ������������ʱ�����Ϣ��ǽڵ��Ƿ���Gateway Node
	nodes[0]->SetNodeStatus(NonGatewayNode);
	for(int i = 1; i < NODE_NUMBER; ++i){//sink�ڵ㲻��Ҫ��һֱ���ڹ���״̬
		nodes[i]->MakeSureIsGatewayNodeOrNot();
	}
}
void Network::AddRules(const int & time){//����dominating set�Ĵ�С
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
	for(int n = 0; n < NODE_NUMBER; ++n){//sink�ڵ㲻�ƶ�
		nodes[n]->AddTime();
		nodes[n]->UpdateRoutingTable();						
	}
	for(int i = 1; ; ++i){
		if(i % UPDATE_DURATION == 0){
			nodes[0]->ClearInfo();		
			for(int n = 1; n < NODE_NUMBER; ++n){//sink�ڵ㲻�ƶ�
				if(nodes[n]->GetBattery()->GetCapacity() <= 0){
					return;
				}
				nodes[n]->ClearInfo();			
			}
			GatherInformation();
			MarkingProcess();
			AddRules(i);
			for(int n = 0; n < NODE_NUMBER; ++n){//sink�ڵ㲻�ƶ�				
				nodes[n]->UpdateRoutingTable();						
			}
		}else{
			for(int n = 1; n < NODE_NUMBER; ++n){//sink�ڵ㲻�ƶ�
				if(nodes[n]->GetBattery()->GetCapacity() <= 0){
					return;
				}
				//nodes[n]->MovementSimulation();
				nodes[n]->PacketGenerating(i);			
			}
		}
		//�������ݰ�
		//·�ɵ��µĽڵ������仯
		//�ж������Ƿ�̱����������
		

		
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