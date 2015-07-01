#pragma once

#include "Battery.h"
#include"Packet.h"
#define DIS(a1, b1, a2, b2) sqrt((((double)(a1) - (double)(a2)) * ((double)(a1) - (double)(a2)))+ (((double)(b1) - (double)(b2)) * ((double)(b1) - (double)(b2)))) //��������֮��ľ���
#define NODE_NUMBER 50//�ڵ�����
#define SIMULATION_TIME 220000//������ʱ�� 500s
#define LENGTH (double)900;//���򳤶�
#define WIDTH (double)650;//������


#define SENSE_DURATION 5 //����Ƶ��

#define POSIBILITY_PACKET_GENERATING 0.5//�������ݰ��ĸ���
#define POSIBILITY_MOVEMENT 0.2//�ڵ��ƶ��ĸ���
#define DISTANCE_MOVEMENT 10//�ڵ�ÿһ�Σ�s��������ƶ�����
#define PI 3.1415926

#define COMMUNICATION_RANGE (double)600 //ͨ�ž��룬�����ڵ���ʵҲ���Բ�ͬ��
#define RADIUS 7 //�����õ���ֵ
#define SHIFT_MOVE 10//���ڻ��㣬�����͵������ƶ����󶥵�
#define MAX_TIME 100//���ݰ���TTL

#define THRESHOLD 14//��ػָ�ģ�͵�����ֵ
#define MAX_WORK_TIME 10//�����ʱ��
#define UPDATE_DURATION 11//��������
#define MAX_PACKETS_PER_SLOT 3 ////�����ݰ���������һ���յ���ʱ�����ڿ��Է��͵������Ŀ�����ݰ�
using namespace std;
enum NodeState{GatewayNode, NonGatewayNode, WhoKnows};//GatewayNode:����dominating set�� NonGatewayNode��������dominating set,sleeping״̬������WhoKnows

struct RoutingEntry{//·�ɱ��е���Ŀ
	int destination;
	int nextHop;
	int distance;
};
class Node{
public:
	Node(int id);//width��ʾΪ0��ʱ�򣬿��ܻ��õ�Բ�ε����磬
	Node(int id, double x, double y):id(id),xLoc(x),yLoc(y){
		workingTime = 0;
		relaxingTime = 0;
		totalWorkTime = 0;
		transDis = COMMUNICATION_RANGE;
		currentState = NonGatewayNode;
		currentWorkState = Receiving;
		hop = 0;
		m_Battery = new Battery();
	}
	~Node();
public:
	void BroadcastPhase();//�㲥�׶�
	void MakeSureIsGatewayNodeOrNot();//ȷ���Լ��Ƿ���gateway node
	void FurtherDecision(const int &);//��һ��ȷ���Լ��ǲ���gateway node���Ӷ�����dominating set�Ĵ�С
	void MovementSimulation();//ȷ���Լ��Ƿ��ƶ������ƶ�����false���ƶ��Ļ����������Ϣ,�������ɼ��ı仯˵����
	void WakeupAfterSleep();//����֮����Ҫ�ɵ�����
	bool ForcedSleep();//ǿ����Ϣ
	void AddNeighbors(int i);//��ʼ���׶�����ھӣ��ռ��ھӵ���Ϣ
	void AddNeighborAndUpdateRoutingTable(int);//����һ��neighbor,������·�ɱ�
	void PacketGenerating(int time);//�Ƿ�������ݰ����Լ���������
	void CopyNode(const int& id, const double& x, const double& y, const NodeState& isGateway, const NodeWorkState&  workState, const double& capacity, const int& totWorkTime, const int& relaTime, const int& workTime, const double& tranRange, const string& str);
	void AddTime();//���Ӿ����ϴ���Ϊgateway node��������Ϣʱ�䣬
	int GetId();//��ýڵ�ID
	double GetXLoc();//���x������
	double GetYLoc();//���y������
	double GetTransDis();//���ͨ�ž���
	int GetWorkingTime();//����Ѿ�������ʱ��
	int GetTotalWorkTime(){return totalWorkTime;}//����ܵĹ���ʱ��
	int GetRelaxingTime(){return relaxingTime;}
	NodeState IsGatewayNodeOrNot();//����Լ��Ƿ���gateway node
	void SetNodeStatus(NodeState i);
	vector<int> GetNeighbors();//����Լ����ھӽڵ�
	void DeleteNeighborAndUpdateRoutingTable(int);//ɾ��һ���Լ����ھӽڵ�By id,˳��ɾ��·�ɱ�������Ϣ
	void UpdateRoutingTable();//����·�ɱ��domain nodes
	vector<int> GetDomainMembers();//����Լ�domain nodes
	void Routing();//·�ɣ�ȷ�����ݰ���·�ɽ׶ξ�����Щ�ڵ㣬��Ӧ�ڵ����Ҫ��������
	void RoutingWithBufferedPackets();
	void AddEnergy(double);//�����ļӼ�
	void PushPackets(Packet p);
	Battery* GetBattery(){return m_Battery;}
	NodeWorkState GetNodeWorkState(){return currentWorkState;}
	void SetNodeWorkStatus(NodeWorkState s){currentWorkState = s;}
	string GetAllNeighborsText(){return neis;}
	void ClearInfo();//���·�ɱ�ʲô�ģ�ÿһ�ξ�������
private:
	int id;//ID
	double xLoc;//x����
	double yLoc;//y����
	double transDis;//ͨ�ž���
	int workingTime;//�����ϴ���Ϣ�Ѿ�������ʱ�䣬����Ϊ��λ
	int relaxingTime;//�����ϴ���Ϊgateway node��������Ϣʱ�䣬
	int hop;//���ݰ�����������
	int totalWorkTime;//�ڵ��ܹ�������ʱ��
	NodeState currentState;//��ʾ�ýڵ��Ƿ�����dominating set
	NodeWorkState currentWorkState;//��ʾ�ýڵ�ĵ�ǰ����״̬��work��idle��or sleep
	vector<int> neighbors;//�ýڵ�������ھ�
	vector<int> domainMembers;//�����gateway�ڵ㣬���ʾ�ýڵ���Χ�ķ�gateway�ڵ�
	vector<RoutingEntry> routingTable;//�����gateway�ڵ㣬���ʾ�ýڵ��·�ɱ�����Ƿ�gateway�ڵ㣬���ʾ��ýڵ�ֱ��������gateway�ڵ�
	queue<int> allNodesOfForwardedPacket;//���в������ݰ�ת���Ľڵ㣬���������ļӼ�
	string neis;//for neighbors to be displayed int the screen
	queue<Packet> bufferedPackets;//��sink�ڵ�ǵ�delete
	Battery* m_Battery;//�ڵ��еĵ��
};

extern vector<Node*> nodes;//�����еĽڵ�
bool VectorBelong2Vector(vector<int>, int, vector<int>, int);//�жϵ�һ�������Ƿ��ǵڶ����������Ӽ�,�ռ�
bool VectorBelong2VectorAndVector(vector<int>, vector<int>, vector<int>);//�жϵ�һ�������Ƿ������ں����������Ĳ���
void Rule1(int id1, int id2);//�����ڵ�֮��ıȽϣ�����id1�ǲ���gateway�ڵ�
void Rule2(int id1, int id2, int id3);
void Rule11(int id1, int id2);//�����ڵ�֮��ıȽϣ�����id1�ǲ���gateway�ڵ�
void Rule22(int id1, int id2, int id3);