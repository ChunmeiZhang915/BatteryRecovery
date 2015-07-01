#pragma once
#include<iostream>
struct Packet{
	Packet(int s, int t):source(s), TTL(t){}
	int source;
	int TTL;
};
