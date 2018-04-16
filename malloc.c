#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

# define BLOCK_SIZE sizeof ( struct Block ) //each time heap size increase


struct Block_Node{
    int flag;
    void *ptr;
    char  data [1];
};

struct Block {
    size_t size;
    struct Block *next;
    struct Block *prev;
    struct Block_Node block_n;
};

typedef struct Block * block_t ;//
void *b_ptr=NULL; //Reference to track starting point of the heap


void
*cs550_sbrk(size_t sz)
{
    size_t size=0;
    void* ret;
    size=sz;
    ret= malloc(size);
    return ret;

}
//merge to free chunk
block_t
merge ( block_t block){

    if (block->next && block->next->block_n.flag ){
        block->size += BLOCK_SIZE+ block->next ->size;
        block->next = block->next ->next;
        if (block->next)
            block->next ->prev = block;
    }
    return (block);
}
//block find with reference value
block_t
get_block (void *ptr)
{
    char *temp;
    temp = ptr;
    temp =temp-(BLOCK_SIZE);
    ptr=temp;
    return ptr;
}
void
*cs550_malloc(size_t size) {

//    char msg_buf[100];

    block_t block,last=NULL;
    size_t allign_size,totalsize;
     totalsize = size + BLOCK_SIZE;
        if ((totalsize % 8) != 0)
            totalsize = (8 - (totalsize % 8)) + totalsize;
        allign_size =totalsize;
   // totalsize= BLOCK_SIZE + allign_size;
    if (b_ptr) {
        last = b_ptr;
        block=b_ptr;
        while (block && !(block->block_n.flag && block->size >= size )) //find the block
        {
            last = block;
            block = block->next;
        }
        if (block) {
            if ((block->size - allign_size) >= ( BLOCK_SIZE + 8)) //finding fitting block
            {
                block_t new_block;
                new_block = ( block_t )(block->block_n.data + allign_size);
                new_block ->size = new_block->size - allign_size ;
                new_block ->next = new_block->next;
                new_block ->prev = new_block;
                new_block ->block_n.flag = 1;
                new_block ->block_n.ptr = new_block ->block_n.data;
                block->size = allign_size;
                block->next = new_block;
                if (new_block ->next)
                    new_block ->next ->prev = new_block;

//                sprintf(msg_buf, ">>> Allocated %zu new bytes for %zu byte request at %p.\n", new_block->size, size, block->block_n.data );
//                write(2, msg_buf, strlen(msg_buf));

            }

        } else {  // start with new allocation
            void *nblock;
            block_t temp;
            temp = cs550_sbrk (0);
            nblock = cs550_sbrk ( allign_size);
            if (nblock ==(void*) -1)
                return (NULL );
            temp->size = allign_size;
            temp->next = NULL;
            temp->prev = last;
            temp->block_n.ptr = temp->block_n.data;
            if (last)
                last ->next = temp;
            temp->block_n.flag = 0;

            block=temp;

            if (!block)
                return (NULL );
        }
    } else {
        void* nblock;
        block_t temp;
        temp = cs550_sbrk (0);
        nblock = cs550_sbrk (  allign_size);
        if (nblock ==(void*) -1)
            return (NULL );
        temp->size = allign_size;
        temp->next = NULL;
        temp->prev = last;
        temp->block_n.ptr = temp->block_n.data;
        if (last)
            last ->next = temp;
        temp->block_n.flag = 0;

        block=temp;
        if (!block)
            return (NULL );
    }
 //   dbg_print("<<< Malloc %lu bytes at %lu...\n", block->size, block->block_n.data);
    return (block->block_n.data );

}

void 
*cs550_calloc(size_t nmemb, size_t size)
{
    size_t total_size;
    unsigned char val; // a random value that was written into the block.
    val= rand()%256;
    void* ret;
      total_size=nmemb *size;
    if (size==0){
        return NULL;
    }
    void *vp = cs550_malloc((nmemb*size));
    ret= memset(vp, val, total_size);
    return ret;

}
void
cs550_free(void *ptr) {

    block_t temp;
    int fl=0;
   if (ptr != 0) {
        if (b_ptr)
        {   
        if ( ptr>b_ptr && ptr< cs550_sbrk (0)){
          if (ptr == (get_block(ptr))->block_n.ptr )
                fl=1;
           }
          fl=0;
           }   
        if (fl)
          {
            temp = get_block(ptr);
            temp->block_n.flag = 1;
            if(temp->prev && temp->prev ->block_n.flag)
                temp = merge (temp->prev );
            if (temp->next)
                merge (temp);
            else
            {
                if (temp->prev)
                    temp->prev->next = NULL;
                else
                    b_ptr = NULL;
            }
        }

    }
 
    //   dbg_print("<<< Freed %lu bytes at %lu...\n", block->size);
}

void
*cs550_realloc(void *ptr, size_t size)
{

    block_t block;
    size_t allign_size=0, st=0;
    size_t totalsize = size + BLOCK_SIZE;
       if ((totalsize % 8) != 0)
           totalsize = (8 - (totalsize % 8)) + totalsize;
       allign_size =totalsize;
    void  *ret;
    if(!ptr || !size){
        return cs550_malloc(allign_size);
    }
    block =(struct Block*)ptr - 1;
    st= block->size;
    if( st >= allign_size )
        return block;
    ret =  cs550_malloc(allign_size);
    if(ret){
        memcpy(ret, ptr, block->size);
        cs550_free(block);

    }

    return ret;

}
