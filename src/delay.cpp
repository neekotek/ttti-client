#ifdef _WIN32

#include <Windows.h>

void __sleep( unsigned int ms )
{
	Sleep( ms );
}

#else

#include <unistd.h>

void __sleep( unsigned int ms )
{
	usleep( ms * 1000 );
}

#endif
