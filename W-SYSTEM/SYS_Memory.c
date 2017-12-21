#include "SYS_Memory.h"
#include "FreeRTOS.h"

static HeapList* _heaplist;
void SYS_MemoryHEAPCall (u32 size,u8 ID, HeapList* heaplist)
{
	void* TEMP;
	TEMP=heaplist->PNextHapeList;
	heaplist->PNextHapeList=pvPortMalloc (size);//内存申请标记
	if (heaplist->PNextHapeList==NULL)
		while (1);
	heaplist=heaplist->PNextHapeList;
	
	heaplist->PNextHapeList=TEMP;
	heaplist->HeapID=ID;
	heaplist->Size=size-sizeof (HeapList);
	heaplist->PHeapNodeHead=(HeapNode*)((uint32_t)heaplist+sizeof (HeapList));
	
	heaplist->PHeapNodeHead->USEState=NOTUSED;
	heaplist->PHeapNodeHead->PBlock=(void *)( (uint32_t) heaplist->PHeapNodeHead+sizeof (HeapNode) );
	heaplist->PHeapNodeHead->Size=_heaplist->Size-sizeof (HeapNode);
	heaplist->PHeapNodeHead->PFrontHeapNode=NULL;
	heaplist->PHeapNodeHead->PNextHeapNode=NULL;
}
void SYS_MemoryInit ()
{
	_heaplist=pvPortMalloc (SYS_MEMSIZE);//内存申请标记
	_heaplist->HeapID=0;
	_heaplist->Size=SYS_MEMSIZE-sizeof (HeapList);
	_heaplist->PHeapNodeHead=(HeapNode*)((uint32_t)_heaplist+sizeof (HeapList));
	
	_heaplist->PNextHapeList=NULL;
	
	_heaplist->PHeapNodeHead->USEState=NOTUSED;
	_heaplist->PHeapNodeHead->PBlock=(void *)( (uint32_t) _heaplist->PHeapNodeHead+sizeof (HeapNode) );
	_heaplist->PHeapNodeHead->Size=_heaplist->Size-sizeof (HeapNode);
	_heaplist->PHeapNodeHead->PFrontHeapNode=NULL;
	_heaplist->PHeapNodeHead->PNextHeapNode=NULL;
}
u32 SYS_MemorySizeChoose (u32 size)
{
	if (SYS_NEW_MEMSIZE<size) return SYS_NEW_MEMSIZE+SYS_MemorySizeChoose(size-SYS_NEW_MEMSIZE);
	else return SYS_NEW_MEMSIZE;
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
	for (PL=_heaplist;PL->HeapID!=ID;PL=PL->PNextHapeList);//初始化为首堆
	while (1)
	{
		for (PN=PL->PHeapNodeHead;PN->USEState==USED;PN=PN->PNextHeapNode)//指向第一个堆的第一个节点，这个节点被使用则进入下一个节点，没被使用就退出
		{
			if (PN->PNextHeapNode);                                        //下一个节点指针不为空就继续
			else                                                           //为空就换下一个堆并退出
			{
				if (PL->PNextHapeList==NULL)                                       //如果下一个堆不存在就新建一个堆
				{
					SYS_MemoryHEAPCall (SYS_MemorySizeChoose (size),ID,PL);
				}
				PL=PL->PNextHapeList;                                        //当前堆切换为新的堆
				PN=PL->PHeapNodeHead;                                       //当前节点切换为新的堆的节点
			}
		}
		if (PN->Size<size+sizeof (HeapNode) )                            //判断大小不合格
		{
			if (PN->PNextHeapNode==NULL)            //判断下一个节点不存在就切换下一个堆
			{
				if (PL->PNextHapeList->HeapID!=ID || PL->PNextHapeList==NULL)        //如果下一个堆不存在或ID不对就新建一个堆
				{
					SYS_MemoryHEAPCall (SYS_MemorySizeChoose (size),ID,PL);
				}
				PL=PL->PNextHapeList; //当前堆切换为新的堆
				PN=PL->PHeapNodeHead;	//当前节点切换为新的堆的节点
			}
			else		                        	       //存在就换下一个节点
			{
				PN=PN->PNextHeapNode;
			}
		}
		else break;                                                      //合格则退出
	}
	/*建立新节点*/
	((HeapNode*)((uint32_t)(PN->PBlock)+size))->USEState=NOTUSED;
	((HeapNode*)((uint32_t)(PN->PBlock)+size))->Size=PN->Size-sizeof (HeapNode)-size;
	((HeapNode*)((uint32_t)(PN->PBlock)+size))->PBlock=(void *)( (uint32_t) ((HeapNode*)((uint32_t)(PN->PBlock)+size))+sizeof (HeapNode) );
	((HeapNode*)((uint32_t)(PN->PBlock)+size))->PFrontHeapNode=PN;
	((HeapNode*)((uint32_t)(PN->PBlock)+size))->PNextHeapNode=NULL;
	/*老节点设置*/
	PN->USEState=USED;
	PN->Size=size;
	PN->PNextHeapNode=((HeapNode*)((uint32_t)(PN->PBlock)+size));
	return PN->PBlock;
}
void  SYS_FreeMem (void * Addr)
{
	uint8_t TEMPID;
	HeapNode* TEMP;
	HeapNode* FTEMP;
	HeapNode* NTEMP;
	HeapList* HTEMP;
	HeapList* FHTEMP;
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
		FTEMP->PNextHeapNode->Size=(uint32_t)TEMP+TEMP->Size-(uint32_t)(FTEMP->PNextHeapNode);	
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
		TEMPID=HTEMP->HeapID;
		if (TEMPID==NULL)//如果ID是首堆
		{
			if (_heaplist==HTEMP)
			{
				TEMP->PFrontHeapNode=NULL;
				TEMP->PNextHeapNode=NULL;
				TEMP->USEState=NOTUSED;
				TEMP->Size=HTEMP->Size-sizeof (HeapNode);
			}
			else
			{
				for (FHTEMP=_heaplist;FHTEMP->PNextHapeList!=HTEMP && FHTEMP->PNextHapeList!=NULL;FHTEMP=FHTEMP->PNextHapeList);
				FHTEMP->PNextHapeList=HTEMP->PNextHapeList;
				vPortFree(HTEMP); //释放标记
			}
		}
		else                       //如果ID不是首堆
		{
			for (FHTEMP=_heaplist;FHTEMP->PNextHapeList!=HTEMP && FHTEMP->PNextHapeList!=NULL;FHTEMP=FHTEMP->PNextHapeList);
			FHTEMP->PNextHapeList=HTEMP->PNextHapeList;
			vPortFree(HTEMP); //释放标记
		}
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

