#include "SYS_Memory.h"

HeapList* _heaplist;
void SYS_MemoryHEAPCall (u32 size,u8 ID, HeapList* heaplist)
{
	void* TEMP;
	TEMP=heaplist->PNextHapeList;
	heaplist->PNextHapeList=malloc (size);//ÄÚ´æÉêÇë±ê¼Ç
	
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
	_heaplist=malloc (SYS_MEMSIZE);//ÄÚ´æÉêÇë±ê¼Ç
	_heaplist->HeapID=SYS_MENID | 0x80;
	_heaplist->Size=SYS_MEMSIZE-sizeof (HeapList);
	_heaplist->PHeapNodeHead=(HeapNode*)((uint32_t)_heaplist+sizeof (HeapList));
	
	_heaplist->PNextHapeList=NULL;
	
	_heaplist->PHeapNodeHead->USEState=NOTUSED;
	_heaplist->PHeapNodeHead->PBlock=(void *)( (uint32_t) _heaplist->PHeapNodeHead+sizeof (HeapNode) );
	_heaplist->PHeapNodeHead->Size=_heaplist->Size-sizeof (HeapNode);
	_heaplist->PHeapNodeHead->PFrontHeapNode=NULL;
	_heaplist->PHeapNodeHead->PNextHeapNode=NULL;
}
u32 SYS_MemorySizeChoice(u32 size)
{
	if (SYS_NEW_MEMSIZE<size) return SYS_NEW_MEMSIZE+SYS_MemorySizeChoice(size-SYS_NEW_MEMSIZE);
	else return SYS_NEW_MEMSIZE;
}
void* SYS_CallMem (u32 size,uint8_t ID)//Íâ²¿µ÷ÓÃ
{
	HeapNode* PN;//µ±Ç°½Úµã
	HeapList* PL;//µ±Ç°¶Ñ
	for (PL=_heaplist;PL->HeapID!=ID && PL->HeapID!=(ID | 0x80);PL=PL->PNextHapeList);//³õÊ¼»¯ÎªÊ×¶Ñ
	while (1)
	{
		for (PN=PL->PHeapNodeHead;PN->USEState==USED;PN=PN->PNextHeapNode)//Ö¸ÏòµÚÒ»¸ö¶ÑµÄµÚÒ»¸ö½Úµã£¬Õâ¸ö½Úµã±»Ê¹ÓÃÔò½øÈëÏÂÒ»¸ö½Úµã£¬Ã»±»Ê¹ÓÃ¾ÍÍË³ö
		{
			if (PN->PNextHeapNode);                                        //ÏÂÒ»¸ö½ÚµãÖ¸Õë²»Îª¿Õ¾Í¼ÌÐø
			else                                                           //Îª¿Õ¾Í»»ÏÂÒ»¸ö¶Ñ²¢ÍË³ö
			{
				if (PL->PNextHapeList==NULL)                                       //Èç¹ûÏÂÒ»¸ö¶Ñ²»´æÔÚ¾ÍÐÂ½¨Ò»¸ö¶Ñ
				{
					SYS_MemoryHEAPCall (SYS_MemorySizeChoice(size),0,PL);
				}
				PL=PL->PNextHapeList;                                        //µ±Ç°¶ÑÇÐ»»ÎªÐÂµÄ¶Ñ
				PN=PL->PHeapNodeHead;                                       //µ±Ç°½ÚµãÇÐ»»ÎªÐÂµÄ¶ÑµÄ½Úµã
			}
		}
		if (PN->Size<size+sizeof (HeapNode) )                            //ÅÐ¶Ï´óÐ¡²»ºÏ¸ñ
		{
			if (PN->PNextHeapNode==NULL)            //ÅÐ¶ÏÏÂÒ»¸ö½Úµã²»´æÔÚ¾ÍÇÐ»»ÏÂÒ»¸ö¶Ñ
			{
				if (PL->PNextHapeList==NULL)        //Èç¹ûÏÂÒ»¸ö¶Ñ²»´æÔÚ¾ÍÐÂ½¨Ò»¸ö¶Ñ
				{
					SYS_MemoryHEAPCall (SYS_MemorySizeChoice(size),ID,PL);
				}
				PL=PL->PNextHapeList; //µ±Ç°¶ÑÇÐ»»ÎªÐÂµÄ¶Ñ
				PN=PL->PHeapNodeHead;	//µ±Ç°½ÚµãÇÐ»»ÎªÐÂµÄ¶ÑµÄ½Úµã
			}
			else		                        	       //´æÔÚ¾Í»»ÏÂÒ»¸ö½Úµã
			{
				PN=PN->PNextHeapNode;
			}
		}
		else break;                                                      //ºÏ¸ñÔòÍË³ö
	}
	/*½¨Á¢ÐÂ½Úµã*/
	((HeapNode*)((uint32_t)(PN->PBlock)+size))->USEState=NOTUSED;
	((HeapNode*)((uint32_t)(PN->PBlock)+size))->Size=PN->Size-sizeof (HeapNode)-size;
	((HeapNode*)((uint32_t)(PN->PBlock)+size))->PBlock=(void *)( (uint32_t) ((HeapNode*)((uint32_t)(PN->PBlock)+size))+sizeof (HeapNode) );
	((HeapNode*)((uint32_t)(PN->PBlock)+size))->PFrontHeapNode=PN;
	((HeapNode*)((uint32_t)(PN->PBlock)+size))->PNextHeapNode=NULL;
	/*ÀÏ½ÚµãÉèÖÃ*/
	PN->USEState=USED;
	PN->Size=size;
	PN->PNextHeapNode=((HeapNode*)((uint32_t)(PN->PBlock)+size));
	return PN->PBlock;
}
void  SYS_FreeMem (void * Addr)
{
	HeapNode* TEMP;
	HeapNode* FTEMP;
	HeapNode* NTEMP;
	HeapList* HTEMP;
	HeapList* FHTEMP;
	TEMP=((HeapNode*)((uint32_t)Addr-sizeof (HeapNode)));
	for (FTEMP=TEMP->PFrontHeapNode;FTEMP && FTEMP->USEState==NOTUSED;FTEMP=FTEMP->PFrontHeapNode);
	for (NTEMP=TEMP->PNextHeapNode;NTEMP && NTEMP->USEState==NOTUSED;NTEMP=NTEMP->PNextHeapNode);
	if (FTEMP && NTEMP)
	{
		FTEMP->PNextHeapNode->PNextHeapNode=NTEMP;
		NTEMP->PFrontHeapNode=FTEMP->PNextHeapNode;
		FTEMP->PNextHeapNode->USEState=NOTUSED;
		FTEMP->PNextHeapNode->Size=(uint32_t)NTEMP-(uint32_t)(FTEMP->PNextHeapNode)-sizeof (HeapNode);
	}
	else if (FTEMP)
	{
		for (;TEMP->PNextHeapNode;TEMP=TEMP->PNextHeapNode);
		FTEMP->PNextHeapNode->PNextHeapNode=NULL;
		FTEMP->PNextHeapNode->USEState=NOTUSED;
		FTEMP->PNextHeapNode->Size=(uint32_t)TEMP+TEMP->Size-(uint32_t)(FTEMP->PNextHeapNode);	
	}
	else if (NTEMP)
	{
		for (;TEMP->PFrontHeapNode;TEMP=TEMP->PFrontHeapNode);
		TEMP->PNextHeapNode=NTEMP;
		TEMP->USEState=NOTUSED;
		TEMP->Size=(uint32_t)NTEMP-(uint32_t)TEMP-sizeof (HeapNode);
		NTEMP->PFrontHeapNode=TEMP;
	}
	else
	{
		for (;TEMP->PFrontHeapNode;TEMP=TEMP->PFrontHeapNode);
		HTEMP=(HeapList*)((uint32_t)TEMP-sizeof (HeapList));
		if (HTEMP->HeapID & 0x80)//Èç¹ûIDÊÇÊ×¶Ñ
		{
			TEMP->PFrontHeapNode=NULL;
			TEMP->PNextHeapNode=NULL;
			TEMP->USEState=NOTUSED;
			TEMP->Size=HTEMP->Size-sizeof (HeapNode);
		}
		else                       //Èç¹ûID²»ÊÇÊ×¶Ñ
		{
			for (FHTEMP=_heaplist;FHTEMP->PNextHapeList!=HTEMP && FHTEMP->PNextHapeList!=NULL;FHTEMP=FHTEMP->PNextHapeList);
			FHTEMP->PNextHapeList=HTEMP->PNextHapeList;
			free (HTEMP);  //ÊÍ·Å±ê¼Ç
		}
	}
}
void SYS_CallHEAP (uint32_t Size,uint8_t ID)
{
	HeapList* TEMP;
	for (TEMP=_heaplist;TEMP->PNextHapeList!=NULL;TEMP=TEMP->PNextHapeList);
	SYS_MemoryHEAPCall (Size,ID | 0x80,TEMP);
}
void SYS_FreeHEAP (uint8_t ID)
{
	HeapList* TEMP;
	for (TEMP=_heaplist;TEMP->HeapID!=(0x80 | ID) && TEMP->PNextHapeList;TEMP=TEMP->PNextHapeList);
	while (1)
	{
		if (TEMP->HeapID==(0x80 | ID) || TEMP->HeapID==ID)
		{
			free (TEMP);
		}
		else return;
		if (TEMP->PNextHapeList==NULL) return;
		else TEMP=TEMP->PNextHapeList;
	}
}

