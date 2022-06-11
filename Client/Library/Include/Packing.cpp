#include "Packing.h"

void ErrorCode(const char* str)
{
	int errCode = ::WSAGetLastError();
	cout << str << " ErrorCode : " << errCode << endl;
}

void PackCpy(char** ptr, void* val, int size)
{
	::memcpy(*ptr, val, size);
	*ptr += size;
}

void UnPackCpy(void* val, char** ptr, int size)
{
	::memcpy(val, *ptr, size);
	*ptr += size;
}

int Packing(char** ptr)
{
	return 0;
}

void UnPacking(char** ptr)
{
	return;
}