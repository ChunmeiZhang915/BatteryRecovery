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
#define INITIAL_NORM_ENERGY (double)25200//额定容量，最初的能量，其实各个节点也可以不同的，
#define INTIAL_MAX_RECOVER_ENERGY (double)7200//最大容量
//#define INITIAL_NORM_ENERGY (double)2520000//额定容量，最初的能量，其实各个节点也可以不同的，
//#define INTIAL_MAX_RECOVER_ENERGY (double)720000//最大容量
#define SEND_PACKET_ENERGY 1 //发送一个数据包需要的能量
#define RECEIVE_PACKET_ENERGY 0.5 //接收一个数据包需要的能量
#define BATTERY_RECOVERY_ENERGY 0.3 //电池休息一个时间单位恢复的电量
#define WORK_CURRENT 24.8//in receive mode, the current is 24.8mA
#define IDLE_CURRENT  1.821 // in idle mode, the current is 1.821mA
#define SLEEP_CURRENT 0.0061//in sleep mode, the current is 6.1 uA
#define SLEEP_CONSUME 2.4//休息的时候也要消耗一定的能量 2400 uA
#define ENERGEY_GRADE (double)100 //能量被划分成一个等级的容量
using namespace std;
enum NodeWorkState{Receiving, Sleeping};//Idling:只是关掉通信设备， sleeping是指电池停止输出
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
