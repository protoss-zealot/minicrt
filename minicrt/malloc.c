#include "minicrt.h"

/*空闲链表中节点，即每块用于malloc的存储区的头部信息*/
typedef struct _heap_header
{
	enum{
		HEAP_BLOCK_FREE = 0xABABABAB,	//magic number of free block
		HEAP_BLOCK_USED = 0xCDCDCDCD,	//magic number of used block
	} type;

	unsigned size;
	struct _heap_header *next;
	struct _heap_header *prev;

} heap_header;

#define ADDR_ADD(a,o)	(((char *)(a)) + o)	
#define HEADER_SIZE		(sizeof(heap_header))

static heap_header* list_head = NULL;

void free(void *ptr)
{
	/*因为ptr其实纯粹是指向一块使用过的内存空间的，这块内存空间前面就保存的是heap_header*/
	heap_header *header = (heap_header *)ADDR_ADD(ptr,-HEADER_SIZE);

	if(header->type!=HEAP_BLOCK_USED)
		return;
	/*空间释放流程*/
	header->type = HEAP_BLOCK_FREE;									/*STEP 1: 先将类型标志改为FREE，个人觉得应该在所有操作完成之后再更改*/
	if(header->prev!=NULL && header->prev->type==HEAP_BLOCK_FREE)	/*STEP 2: 判断前驱是否为空，不为空就合并*/
	{
		header->prev->next = header->next;
		if(header->next!=NULL)
			header->next->prev = header->prev;
		header->prev->size +=header->size;

		header = header->prev;
	}

	if(header->next!=NULL && header->next->type==HEAP_BLOCK_FREE)
	{
		header->size += header->next->size;
		header->next = header->next->next;
		if(header->next!=NULL)			/*i think it's needed*/
			header->next->prev = header;
	}
}


void *malloc(unsigned size)
{
	heap_header *header;

	if(size==0)
		return NULL;

	header = list_head;
	while(header!=0)
	{
		if(header->type==HEAP_BLOCK_USED)	/*当前块已经被使用，则查找下一个节点*/
		{
			header = header->next;
			continue;
		}

		if(header->size > size + HEADER_SIZE && header->size <= size + HEADER_SIZE *2)
		{
			header->type = HEAP_BLOCK_USED;
		}
		
		if(header->size > size + HEADER_SIZE *2)
		{
			heap_header *next = (heap_header *)ADDR_ADD(header,size + HEADER_SIZE);

			next->prev = header;
			next->next = header->next;
			next->type = HEAP_BLOCK_FREE;
			next->size = header->size - size - HEADER_SIZE;
			header->next = next;
			header->size = size + HEADER_SIZE;
			header->type = HEAP_BLOCK_USED;

			return ADDR_ADD(header,HEADER_SIZE);
		}
		header = header->next;
	}
	return NULL;
}

#ifndef WIN32

/*Implementation of linux system call : brk*/

static int brk(void *end_data_segment)
{
	int ret = 0;

	asm( "movl $45, %%eax	\n\t"
		"movl %1,%%ebx		\n\t"
		"int $0x80			\n\t"
		"movl %%eax, %0     \n\t"
		: "=r"(ret): "m"(end_data_segment) );
}

#endif

int mini_crt_heap_init()
{
	void *base = NULL;
	heap_header *header = NULL;
	unsigned heap_size = 1024*1024*32;		//32Mb space

	base = (void *)brk(0);
	void *end = ADDR_ADD(base,heap_size);
	end = (void *)brk(end);
	if(!end)
		return 0;

	header = (heap_header *)base;

	header->size = heap_size;
	header->type = HEAP_BLOCK_FREE;
	header->next = NULL;
	header->prev = NULL;

	list_head = header;
	return 1;
}
