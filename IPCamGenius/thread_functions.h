#ifndef _THREAD_FUNCTIONS_H
#define _THREAD_FUNCTIONS_H

DWORD WINAPI t_gateway_startup(LPVOID lpParam);
DWORD WINAPI t_send_packets(LPVOID lpParam);
DWORD WINAPI t_recv_packets(LPVOID lpParam);

#endif