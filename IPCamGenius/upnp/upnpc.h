#ifndef _UPNPC_H
#define _UPNPC_H
#include <map>
#include <string>

typedef struct{
	char protocol[10];
	char extPort[5];
	char intClient[16];
	char intPort[5];
	char desc[20];//upnp×¢²áÃû
	char rHost[20];
	char duration[20];
}UpnpPacket;

using namespace std;

int SetUpnp(char *pIpaddress,char *pInternalport,char *pExternalPort,char *pExIpaddress, multimap<string, UpnpPacket> &mmap);
int upnpSetup(char *pExIpaddress,int argc, char ** argv, multimap<string, UpnpPacket> &mmap);
int test(map<long long, long> &stlmap);

#endif