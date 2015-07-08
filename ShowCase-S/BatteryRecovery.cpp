#include"Network.h"


int main(int argc, char** argv)
{
	Network *nt = new Network();
	nt->ConstructNetwork();
	//nt->ConstructNetworkWithFixedPositon();
	nt->GatherInformation();
	nt->Run();
	delete nt;
	system("pause");
	return 0;
}