#include <Windows.h>

#define getCurPlayerInfoAddress		0x6fb1cc10;
#define getMaxPlayerHpAddress		0x6fabc662;
#define chatOutAddress				0x6FB61CE0;

#define playerPointer				0x6fbcd050;
#define lastChatPointer				0x6fbcd650;

enum INFOTYPE { Life = 6, Mana = 8, Stamina = 0xA };

typedef struct 
{
	DWORD address;
	DWORD newVal;
	DWORD size;
} Patch;