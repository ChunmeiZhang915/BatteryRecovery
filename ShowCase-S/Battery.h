#pragma once
#include<iostream>
#include<vector>
#include<stdio.h> 
#include<stdlib.h>
#include<time.h>
#include<fstream>
#include<queue>
#include<string>
#include<sstream>
#define INITIAL_NORM_ENERGY (double)25200//��������������������ʵ�����ڵ�Ҳ���Բ�ͬ�ģ�
#define INTIAL_MAX_RECOVER_ENERGY (double)7200//�������
//#define INITIAL_NORM_ENERGY (double)2520000//��������������������ʵ�����ڵ�Ҳ���Բ�ͬ�ģ�
//#define INTIAL_MAX_RECOVER_ENERGY (double)720000//�������
#define SEND_PACKET_ENERGY 1 //����һ�����ݰ���Ҫ������
#define RECEIVE_PACKET_ENERGY 0.5 //����һ�����ݰ���Ҫ������
#define BATTERY_RECOVERY_ENERGY 0.3 //�����Ϣһ��ʱ�䵥λ�ָ��ĵ���
#define WORK_CURRENT 24.8//in receive mode, the current is 24.8mA
#define IDLE_CURRENT  1.821 // in idle mode, the current is 1.821mA
#define SLEEP_CURRENT 0.0061//in sleep mode, the current is 6.1 uA
#define SLEEP_CONSUME 2.4//��Ϣ��ʱ��ҲҪ����һ�������� 2400 uA
#define ENERGEY_GRADE (double)100 //���������ֳ�һ���ȼ�������
using namespace std;
enum NodeWorkState{Receiving, Sleeping};//Idling:ֻ�ǹص�ͨ���豸�� sleeping��ָ���ֹͣ���
extern vector<double> recoveryCurrent;
class Battery{
public:
	Battery():m_currentCapacity(m_normCapacity),m_maxRecoverCapacity(0){}
	Battery(double c):m_currentCapacity(c),m_maxRecoverCapacity(0){}
	void Run(NodeWorkState status, int relaxingTime = 0);
	int GetEnergyGrade(){return (int) (m_currentCapacity / ENERGEY_GRADE);}
	double GetCapacity(){return m_currentCapacity;}
	void SetCapacity(double c){m_currentCapacity = c;}
	
private:
	static const double m_normCapacity;
	double m_maxRecoverCapacity;
	double m_currentCapacity;


};

void CaculateCurrent();
