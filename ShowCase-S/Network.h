#pragma once

#include"Node.h"

class Network{
public:
	Network( );
	~Network();
public:
	void ConstructNetwork();//����һ�����磬�ڵ���ȷֲ�
	void ConstructNetworkWithFixedPositon();//�̶�λ�õĽڵ㣬�����
	void AdjustPostion();
	void GatherInformation();//�ռ��ھӽڵ����Ϣ�����ھӵ��ھ���Ϣ������������ʱ��ȣ�
	void MarkingProcess();//���ݽڵ������������ʱ�����Ϣ��ǽڵ��Ƿ���Gateway Node
	void AddRules(const int &);//����dominating set�Ĵ�С
	void PacketDelivery();//���ݲ����׶�
public:
	void Run();
};