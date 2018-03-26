#include "SYS_Memory.h"
#include <SYS_Config.h>
static HeapList* _heaplist;
void SYS_Error (uint8_t ErrorNum)
{
	uint8_t Error;
	while (1)
	{
		Error=ErrorNum;
	}
}
void * SYS_Memory_MainMEN_Init ()
{
	static uint8_t men[MEMSIZE];
	return men;
}
void SYS_MemoryHEAPCall (u32 size,u8 ID, HeapList* heaplist)
{
	void* TEMP;
	uint32_t ALLsize=0;
	
	vTaskSuspendAll ();
	
	TEMP=heaplist->PNextHapeList;
	/**/
	HeapList* Plist=_heaplist;
	for (;Plist!=NULL;Plist=Plist->PNextHapeList)
	{
		ALLsize+=Plist->Size + sizeof (HeapList);   // 对所有已分配内存大小求和
	}
	if (ALLsize+size+sizeof (HeapList)>MEMSIZE)   // 判断还有没有空间可以分配
		for (;;); //给定内存空间太小
	/**/
	heaplist->PNextHapeList=(void *)((uint32_t)_heaplist + ALLsize);
	if (heaplist->PNextHapeList==NULL) SYS_Error(1);
	heaplist=heaplist->PNextHapeList;
	
	heaplist->PNextHapeList=TEMP;
	heaplist->HeapID=ID;
	heaplist->Size=size-sizeof (HeapList);
	heaplist->PHeapNodeHead=(HeapNode*)((uint32_t)heaplist+sizeof (HeapList));
	
	heaplist->PHeapNodeHead->USEState=NOTUSED;
	heaplist->PHeapNodeHead->PBlock=(void *)( (uint32_t) heaplist->PHeapNodeHead+sizeof (HeapNode) );
	heaplist->PHeapNodeHead->Size=heaplist->Size-sizeof (HeapNode);
	heaplist->PHeapNodeHead->PFrontHeapNode=NULL;
	heaplist->PHeapNodeHead->PNextHeapNode=NULL;
	
	xTaskResumeAll ();
}
void SYS_MemoryInit ()
{
	_heaplist=SYS_Memory_MainMEN_Init ();
	if (_heaplist==NULL) SYS_Error(0);
	_heaplist->HeapID=0;
	_heaplist->Size=SYS_MEMSIZE-sizeof (HeapList);
	_heaplist->PNextHapeList=NULL;
	
	_heaplist->PHeapNodeHead=(HeapNode*)((uint32_t)_heaplist+sizeof (HeapList));
	_heaplist->PHeapNodeHead->USEState=NOTUSED;
	_heaplist->PHeapNodeHead->PBlock=(void *)( (uint32_t) _heaplist->PHeapNodeHead+sizeof (HeapNode) );
	_heaplist->PHeapNodeHead->Size=_heaplist->Size-sizeof (HeapNode);
	_heaplist->PHeapNodeHead->PFrontHeapNode=NULL;
	_heaplist->PHeapNodeHead->PNextHeapNode=NULL;
}
uint8_t SYS_MemoryIDChoose ()
{
	uint8_t TEMPID;
	HeapIDList * IDList, *TEMPIDList;
	HeapList * TEMP;
	IDList=SYS_CallMem (sizeof (HeapIDList),NULL);
	IDList->HeapID=0;
	IDList->PHeapIDListL=NULL;
	IDList->PHeapIDListR=NULL;
	/*排序*/
	for (TEMP=_heaplist;TEMP;TEMP=TEMP->PNextHapeList)
	{
		TEMPID=TEMP->HeapID;
		while (1)
		{
			if (TEMPID==IDList->HeapID) break;
			
			if (TEMPID>IDList->HeapID);
			else
			{
				IDList->PHeapIDListL->PHeapIDListR=SYS_CallMem (sizeof (HeapIDList),NULL);
				IDList->PHeapIDListL->PHeapIDListR->PHeapIDListR=IDList;
				IDList->PHeapIDListL->PHeapIDListR->PHeapIDListL=IDList->PHeapIDListL;
				IDList->PHeapIDListL=IDList->PHeapIDListL->PHeapIDListR;
			}
			
			if (IDList->PHeapIDListR==NULL)
			{
				IDList->PHeapIDListR=SYS_CallMem (sizeof (HeapIDList),NULL);
				IDList->PHeapIDListR->HeapID=TEMPID;
				IDList->PHeapIDListR->PHeapIDListL=IDList;
				IDList->PHeapIDListR->PHeapIDListR=NULL;
				break;
			}
			
			IDList=IDList->PHeapIDListR;
		}
	}
	for (;IDList->PHeapIDListL;IDList=IDList->PHeapIDListL);//向左移动
	/*找出合适的ID*/
	TEMPIDList=IDList;
	for (TEMPID=IDList->HeapID;IDList;IDList=IDList->PHeapIDListR)
	{
		if ((IDList->HeapID-TEMPID)>1)
		{
			break;
		}
		else TEMPID=IDList->HeapID;
	}
	++TEMPID;
	IDList=TEMPIDList;
	/*释放内存*/
	for (;IDList->PHeapIDListL;IDList=IDList->PHeapIDListL);//向左移动
	while (IDList->PHeapIDListL)
	{
		TEMPIDList=IDList;
		IDList=IDList->PHeapIDListR;
		SYS_FreeMem (TEMPIDList);
	}
	SYS_FreeMem (IDList);
	
	return TEMPID;
}
void* SYS_CallMem (u32 size,uint8_t ID)//外部调用
{
	HeapNode* PN;//当前节点
	HeapList* PL;//当前堆
	for (PL=_heaplist;PL->HeapID!=ID;PL=PL->PNextHapeList);//找到堆
	PN=PL->PHeapNodeHead;//指向第一个堆的第一个节点
	while (1)
	{
		for (;PN->USEState==USED;PN=PN->PNextHeapNode)//这个节点被使用则进入下一个节点，没被使用就退出
		{
			if (PN->PNextHeapNode);                                        //下一个节点指针不为空就继续
			else                                                           //为空就退出
			{
				return NULL;
			}
		}
		
		if (PN->Size<=size )                            //判断大小不合格
		{
			if (PN->PNextHeapNode==NULL)            //判断下一个节点不存在就退出
			{
				return NULL;
			}
			else		                        	       //存在就换下一个节点
			{
				PN=PN->PNextHeapNode;
			}
		}
		else                                       //合格则计算大小并退出
		{
			break;
		}
	}
		
	/*建立新节点*/
	if (PN->Size-size>sizeof (HeapNode)+1)
	{
		((HeapNode*)((uint32_t)(PN->PBlock)+size))->USEState=NOTUSED;
		((HeapNode*)((uint32_t)(PN->PBlock)+size))->Size=PN->Size-sizeof (HeapNode)-size;
		((HeapNode*)((uint32_t)(PN->PBlock)+size))->PBlock=(void *)( (uint32_t) ((HeapNode*)((uint32_t)(PN->PBlock)+size))+sizeof (HeapNode) );
		((HeapNode*)((uint32_t)(PN->PBlock)+size))->PFrontHeapNode=PN;
		((HeapNode*)((uint32_t)(PN->PBlock)+size))->PNextHeapNode=PN->PNextHeapNode;
		if (PN->PNextHeapNode!=NULL) PN->PNextHeapNode->PFrontHeapNode=((HeapNode*)((uint32_t)(PN->PBlock)+size));
		/*老节点设置*/
		PN->USEState=USED;
		PN->Size=size;
		PN->PNextHeapNode=((HeapNode*)((uint32_t)(PN->PBlock)+size));
	}
	else
	{
		PN->USEState=USED;
	}
	return PN->PBlock;
}
void  SYS_FreeMem (void * Addr)
{
	HeapNode* TEMP;
	HeapNode* FTEMP;
	HeapNode* NTEMP;
	HeapList* HTEMP;
	TEMP=((HeapNode*)((uint32_t)Addr-sizeof (HeapNode)));
	for (FTEMP=TEMP->PFrontHeapNode;FTEMP && FTEMP->USEState==NOTUSED;FTEMP=FTEMP->PFrontHeapNode);//找到节点头
	for (NTEMP=TEMP->PNextHeapNode;NTEMP && NTEMP->USEState==NOTUSED;NTEMP=NTEMP->PNextHeapNode);//找到节点尾
	if (FTEMP && NTEMP)//有头有尾
	{
		FTEMP->PNextHeapNode->PNextHeapNode=NTEMP;
		NTEMP->PFrontHeapNode=FTEMP->PNextHeapNode;
		FTEMP->PNextHeapNode->USEState=NOTUSED;
		FTEMP->PNextHeapNode->Size=(uint32_t)NTEMP-(uint32_t)(FTEMP->PNextHeapNode)-sizeof (HeapNode);
	}
	else if (FTEMP)//有头
	{
		for (;TEMP->PNextHeapNode;TEMP=TEMP->PNextHeapNode);
		FTEMP->PNextHeapNode->PNextHeapNode=NULL;
		FTEMP->PNextHeapNode->USEState=NOTUSED;
		FTEMP->PNextHeapNode->Size=(uint32_t)TEMP-(uint32_t)FTEMP->PNextHeapNode->PBlock+TEMP->Size+sizeof (HeapNode);
		if (TEMP->PNextHeapNode) FTEMP->PNextHeapNode->Size+= TEMP->PNextHeapNode->Size + sizeof (HeapNode);
	}
	else if (NTEMP)//有尾
	{
		for (;TEMP->PFrontHeapNode;TEMP=TEMP->PFrontHeapNode);
		TEMP->PNextHeapNode=NTEMP;
		TEMP->USEState=NOTUSED;
		TEMP->Size=(uint32_t)NTEMP-(uint32_t)TEMP-sizeof (HeapNode);
		NTEMP->PFrontHeapNode=TEMP;
	}
	else//没头没尾
	{
		for (;TEMP->PFrontHeapNode;TEMP=TEMP->PFrontHeapNode);
		HTEMP=(HeapList*)((uint32_t)TEMP-sizeof (HeapList));
		TEMP->PFrontHeapNode=NULL;
		TEMP->PNextHeapNode=NULL;
		TEMP->USEState=NOTUSED;
		TEMP->Size=HTEMP->Size-sizeof (HeapNode);
	}
}
uint8_t SYS_CallHEAP (uint32_t Size)
{
	HeapList* TEMP;
	uint8_t ID;
	ID=SYS_MemoryIDChoose ();
	for (TEMP=_heaplist;TEMP->PNextHapeList!=NULL;TEMP=TEMP->PNextHapeList);
	SYS_MemoryHEAPCall (Size,ID,TEMP);
	return ID;
}
void SYS_FreeHEAP (uint8_t ID)
{
	HeapList* TEMP;
	for (TEMP=_heaplist;TEMP->HeapID!=ID && TEMP->PNextHapeList;TEMP=TEMP->PNextHapeList);
	while (1)
	{
		if (TEMP->HeapID==ID)
		{
			free (TEMP);
		}
		if (TEMP->PNextHapeList==NULL) return;
		else TEMP=TEMP->PNextHapeList;
	}
}
uint16_t SYS_HeapFreeSize (uint8_t ID)
{
	HeapNode* PN;//当前节点
	HeapList* PL;//当前堆
	uint16_t size=0;
	for (PL=_heaplist;PL->HeapID!=ID;PL=PL->PNextHapeList);//找到堆
	PN=PL->PHeapNodeHead;
	for (;PN;PN=PN->PNextHeapNode)
		if (PN->USEState==NOTUSED) size += PN->Size;
	return size;
}

