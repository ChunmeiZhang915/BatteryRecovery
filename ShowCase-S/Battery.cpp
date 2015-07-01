#include "stdafx.h"
#include"Battery.h"
const double Battery::m_normCapacity = INITIAL_NORM_ENERGY;
vector<double> recoveryCurrent;
void Battery::Run(NodeWorkState nodeStatus, int relaxingTime ){
	if(m_currentCapacity <= 0){
		ofstream outLogFile("log.txt", ios_base::out | ios_base::app);
		assert(!outLogFile.fail());
		outLogFile<<__TIME__<<"Battery Run Out"<<endl;
		outLogFile.close();
		return;
	}
	double tempRe = 0.0;
	switch(nodeStatus){
	case Sleeping:
		//如果允许休息更久的时间的话，恢复电流为0
		//m_currentCapacity -= (SLEEP_CONSUME * ((double)(1) / (double)(60)));
		assert(relaxingTime < 15);
		tempRe = recoveryCurrent[relaxingTime];
		if(m_maxRecoverCapacity >= INTIAL_MAX_RECOVER_ENERGY){
			break;
		}
		m_maxRecoverCapacity += tempRe;
		m_currentCapacity += (tempRe - max((m_maxRecoverCapacity - INTIAL_MAX_RECOVER_ENERGY), 0));
		
		break;
	case Receiving:
		//m_currentCapacity -= RECEIVE_CONSUME * t;
		m_currentCapacity -= WORK_CURRENT;
		break;
	default:
		ofstream outLogFile("log.txt", ios_base::out | ios_base::app);
		assert(!outLogFile.fail());
		outLogFile<<__TIME__<<"Battery Run Error"<<endl;
		outLogFile.close();
	}
}

void CaculateCurrent(){
	double workCurrent = WORK_CURRENT;//24.8mA
	double sleepCurrent = SLEEP_CURRENT;//6.1 uA
	vector<double> workTime;
	ifstream  dataFile("data.txt", ios_base::in);
	if(dataFile.fail()){
		ofstream outLogFile("log.txt", ios_base::out | ios_base::app);
		assert(!outLogFile.fail());
		outLogFile<<__TIME__<<"Error to open the input file:data.txt"<<endl;
		outLogFile.close();
		return;
	}
	double temp = 0.0;
	while(dataFile.good()){
		dataFile>>temp;
		workTime.push_back(temp /(double)(60));
	}
	dataFile.close();
	//time slot is 1 second
	recoveryCurrent.push_back(0.0);
	double activeCapacity = 0.0;
	double sleepCapacity = 0.0;
	double preTotalCapacity = workCurrent * workTime[0];
	double curTotalCapacity = 0.0;
	double timePhase = 0.0;
	for(size_t i = 1; i < workTime.size(); ++i){
		activeCapacity = workCurrent * workTime[i];
		sleepCapacity = sleepCurrent * workTime[i] * (double)(i) / (double)(10);
		curTotalCapacity = activeCapacity + sleepCapacity;
		preTotalCapacity =  curTotalCapacity - preTotalCapacity;
		timePhase  = (workTime[i] * (double)(i) / (double)(10)) - (workTime[i - 1] * (double)(i - 1) / (double)(10));
		recoveryCurrent.push_back(preTotalCapacity / timePhase);
		preTotalCapacity = curTotalCapacity;
	}
	ofstream recoveryFile("current.txt", ios_base::out);
	if(recoveryFile.fail()){
		ofstream outLogFile("log.txt", ios_base::out | ios_base::app);
		assert(!outLogFile.fail());
		outLogFile<<__TIME__<<"Error to open the output file:current.txt"<<endl;
		outLogFile.close();
		return;
	}
	for(size_t i = 0; i < recoveryCurrent.size(); ++i){
		recoveryFile<<recoveryCurrent[i]<<endl;
	}
	recoveryFile.close();
	return;
}
