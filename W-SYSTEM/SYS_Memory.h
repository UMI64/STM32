#ifndef __SYS_MEMORY_H
#define __SYS_MEMORY_H
#include "SYS.h"
#include "SYS_Memory.h"

#define SYS_MEMSIZE (uint32_t) 2048
#define GUI_MEMSIZE (uint32_t) 1024

#define SYS_NEW_MEMSIZE (uint32_t) 1024

typedef struct HeapNode
{
	uint16_t Size;
	enum {NOTUSED=0,USED=!NOTUSED} USEState;
	void* PBlock;
	struct HeapNode* PFrontHeapNode;
	struct HeapNode* PNextHeapNode;
}HeapNode;

typedef struct HeapList
{
	uint32_t Size;
	uint8_t HeapID;
	struct HeapNode* PHeapNodeHead;
	struct HeapList* PNextHapeList;
}HeapList;

typedef struct HeapIDList
{
	uint8_t HeapID;
	struct HeapIDList* PHeapIDListL;
	struct HeapIDList* PHeapIDListR;
}HeapIDList;

void  SYS_MemoryInit (void);
void * SYS_CallMem (u32 size,uint8_t ID);
void  SYS_FreeMem (void * Addr);
uint8_t SYS_CallHEAP (uint32_t Size);
void SYS_FreeHEAP (uint8_t ID);

#endif

