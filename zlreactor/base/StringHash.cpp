#include "StringHash.h"

//SDBM Hash Function
unsigned int SDBMHash(const char *str)
{
	unsigned int hash = 0;
	while( *str )
	{
		//equivalent to: hash = 65599*hash+(*str++);
		hash = (*str++) + (hash<<6) + (hash<<16) - hash;
	}

	return (hash&0x7FFFFFFF);
}

//RS Hash Function
unsigned int RSHash(const char *str)
{
	unsigned int b = 378551;
	unsigned int a = 63689;
	unsigned int hash = 0;

	while( *str )
	{		
		hash = hash * a + (*str++);
		a *= b;
	}

	return (hash&0x7FFFFFFF);
}

//JS Hash Function
unsigned int JSHash(const char *str)
{
	unsigned int hash = 1315423911;

	while( *str )
	{		
		hash ^= ( (hash<<5) + (*str++) + (hash>>2) );		
	}

	return (hash&0x7FFFFFFF);
}

//BKDR Hash Function
unsigned int BKDRHash(const char *str)
{
	unsigned int seed = 131; //31 131 1313 13131 131313 
	unsigned int hash = 0;

	while( *str )
	{		
		hash ^= ( (hash<<5) + (*str++) + (hash>>2) );
	}

	return (hash&0x7FFFFFFF);
}

//DJB Hash Function
unsigned int DJBHash(const char *str)
{
	unsigned int hash = 5381;

	while( *str )
	{		
		hash += (hash<<5) + (*str++);		
	}

	return (hash&0x7FFFFFFF);
}

unsigned int FNVHash(const char *str)
{  
	register unsigned int  hash = 2166136261;  
	while (unsigned int  ch = (unsigned int )*str++)  
	{  
		hash *= 16777619;  
		hash ^= ch;  
	}  
	return hash;  
}  