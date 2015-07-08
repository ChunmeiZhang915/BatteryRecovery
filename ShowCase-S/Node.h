#pragma once

#include "Battery.h"
#include"Packet.h"
#define DIS(a1, b1, a2, b2) sqrt((((double)(a1) - (double)(a2)) * ((double)(a1) - (double)(a2)))+ (((double)(b1) - (double)(b2)) * ((double)(b1) - (double)(b2)))) //求两个点之间的距离
#define NODE_NUMBER 50//节点数量
#define SIMULATION_TIME 220000//仿真总时长 500s
#define LENGTH (double)900;//区域长度
#define WIDTH (double)650;//区域宽度


#define SENSE_DURATION 5 //采样频率

#define POSIBILITY_PACKET_GENERATING 0.5//产生数据包的概率
#define POSIBILITY_MOVEMENT 0.2//节点移动的概率
#define DISTANCE_MOVEMENT 10//节点每一次（s）的最大移动距离
#define PI 3.1415926

#define COMMUNICATION_RANGE (double)600 //通信距离，各个节点其实也可以不同的
#define RADIUS 7 //画点用的数值
#define SHIFT_MOVE 10//用于画点，正方型的中心移动到左顶点
#define MAX_TIME 100//数据包的TTL

#define THRESHOLD 14//电池恢复模型的数据值
#define MAX_WORK_TIME 10//最大工作时间
#define UPDATE_DURATION 11//更新周期
#define MAX_PACKETS_PER_SLOT 3 ////从数据包产生到下一跳收到的时间间隔内可以发送的最大数目的数据包
using namespace std;
enum NodeState{GatewayNode, NonGatewayNode, WhoKnows};//GatewayNode:属于dominating set； NonGatewayNode：不属于dominating set,sleeping状态下属于WhoKnows

struct RoutingEntry{//路由表中的条目
	int destination;
	int nextHop;
	int distance;
};
class Node{
public:
	Node(int id);//width表示为0的时候，可能会用到圆形的网络，
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
	void BroadcastPhase();//广播阶段
	void MakeSureIsGatewayNodeOrNot();//确定自己是否是gateway node
	void FurtherDecision(const int &);//进一步确定自己是不是gateway node，从而精简dominating set的大小
	void MovementSimulation();//确定自己是否移动，不移动返回false，移动的话重置相关信息,导致主干集的变化说不定
	void WakeupAfterSleep();//醒来之后需要干的事情
	bool ForcedSleep();//强制休息
	void AddNeighbors(int i);//初始化阶段添加邻居，收集邻居的信息
	void AddNeighborAndUpdateRoutingTable(int);//增加一个neighbor,并更新路由表
	void PacketGenerating(int time);//是否产生数据包，以及能量消耗
	void CopyNode(const int& id, const double& x, const double& y, const NodeState& isGateway, const NodeWorkState&  workState, const double& capacity, const int& totWorkTime, const int& relaTime, const int& workTime, const double& tranRange, const string& str);
	void AddTime();//增加距离上次作为gateway node工作的休息时间，
	int GetId();//获得节点ID
	double GetXLoc();//获得x坐标轴
	double GetYLoc();//获得y坐标轴
	double GetTransDis();//获得通信距离
	int GetWorkingTime();//获得已经工作的时间
	int GetTotalWorkTime(){return totalWorkTime;}//获得总的工作时间
	int GetRelaxingTime(){return relaxingTime;}
	NodeState IsGatewayNodeOrNot();//获得自己是否是gateway node
	void SetNodeStatus(NodeState i);
	vector<int> GetNeighbors();//获得自己的邻居节点
	void DeleteNeighborAndUpdateRoutingTable(int);//删除一个自己的邻居节点By id,顺便删掉路由表的相关信息
	void UpdateRoutingTable();//更新路由表和domain nodes
	vector<int> GetDomainMembers();//获得自己domain nodes
	void Routing();//路由，确定数据包在路由阶段经过哪些节点，相应节点就需要能量减少
	void RoutingWithBufferedPackets();
	void AddEnergy(double);//能量的加减
	void PushPackets(Packet p);
	Battery* GetBattery(){return m_Battery;}
	NodeWorkState GetNodeWorkState(){return currentWorkState;}
	void SetNodeWorkStatus(NodeWorkState s){currentWorkState = s;}
	string GetAllNeighborsText(){return neis;}
	void ClearInfo();//清空路由表什么的，每一次竞争都是
private:
	int id;//ID
	double xLoc;//x坐标
	double yLoc;//y坐标
	double transDis;//通信距离
	int workingTime;//距离上次休息已经工作的时间，以秒为单位
	int relaxingTime;//距离上次作为gateway node工作的休息时间，
	int hop;//数据包经过的跳数
	int totalWorkTime;//节点总共工作的时间
	NodeState currentState;//表示该节点是否属于dominating set
	NodeWorkState currentWorkState;//表示该节点的当前工作状态，work，idle，or sleep
	vector<int> neighbors;//该节点的所有邻居
	vector<int> domainMembers;//如果是gateway节点，则表示该节点周围的非gateway节点
	vector<RoutingEntry> routingTable;//如果是gateway节点，则表示该节点的路由表，如果是非gateway节点，则表示与该节点直接相连的gateway节点
	queue<int> allNodesOfForwardedPacket;//所有参与数据包转发的节点，方便能量的加减
	string neis;//for neighbors to be displayed int the screen
	queue<Packet> bufferedPackets;//在sink节点记得delete
	Battery* m_Battery;//节点中的电池
};

extern vector<Node*> nodes;//网络中的节点
bool VectorBelong2Vector(vector<int>, int, vector<int>, int);//判断第一个向量是否是第二个向量的子集,闭集
bool VectorBelong2VectorAndVector(vector<int>, vector<int>, vector<int>);//判断第一个向量是否是属于后两个向量的并集
void Rule1(int id1, int id2);//两个节点之间的比较，决定id1是不是gateway节点
void Rule2(int id1, int id2, int id3);
void Rule11(int id1, int id2);//两个节点之间的比较，决定id1是不是gateway节点
void Rule22(int id1, int id2, int id3);