#pragma once

#include"Node.h"

class Network{
public:
	Network( );
	~Network();
public:
	void ConstructNetwork();//构建一个网络，节点均匀分布
	void ConstructNetworkWithFixedPositon();//固定位置的节点，搭建网络
	void AdjustPostion();
	void GatherInformation();//收集邻居节点的信息：其邻居的邻居信息（能量，工作时间等）
	void MarkingProcess();//根据节点的能量，工作时间等信息标记节点是否是Gateway Node
	void AddRules(const int &);//精简dominating set的大小
	void PacketDelivery();//数据产生阶段
public:
	void Run();
};