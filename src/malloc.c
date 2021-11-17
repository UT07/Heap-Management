//Utkarsh Singh
//1001722244
#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>

#define ALIGN4(s)         (((((s) - 1) >> 2) << 2) + 4)
#define BLOCK_DATA(b)      ((b) + 1)
#define BLOCK_HEADER(ptr)   ((struct _block *)(ptr) - 1)


static int atexit_registered = 0;
static int num_mallocs       = 0;
static int num_frees         = 0;
static int num_reuses        = 0;
static int num_grows         = 0;
static int num_splits        = 0;
static int num_coalesces     = 0;
static int num_blocks        = 0;
static int num_requested     = 0;
static int max_heap          = 0;

/*
 *  \brief printStatistics
 *
 *  \param none
 *
 *  Prints the heap statistics upon process exit.
 *  Registered via atexit()
 *  \return none
 */
void printStatistics( void )
{
  printf("\nheap management statistics\n");
  printf("mallocs:\t%d\n", num_mallocs );
  printf("frees:\t\t%d\n", num_frees );
  printf("reuses:\t\t%d\n", num_reuses );
  printf("grows:\t\t%d\n", num_grows );
  printf("splits:\t\t%d\n", num_splits );
  printf("coalesces:\t%d\n", num_coalesces );
  printf("blocks:\t\t%d\n", num_blocks );
  printf("requested:\t%d\n", num_requested );
  printf("max heap:\t%d\n", max_heap );
}

struct _block 
{
   size_t  size;         /* Size of the allocated _block of memory in bytes */
   struct _block *prev;  /* Pointer to the previous _block of allcated memory   */
   struct _block *next;  /* Pointer to the next _block of allcated memory   */
   bool   free;          /* Is this _block free?                     */
   char   padding[3];
};


struct _block *heapList = NULL; /* Free list to track the _blocks available */
struct _block *nextFitMemoryBlock=NULL; /*Memory block to implement nextfit algorithm.
/*
 * \brief findFreeBlock
 *
 * \param last pointer to the linked list of free _blocks
 * \param size size of the _block needed in bytes 
 *
 * \return a _block that fits the request or NULL if no free _block matches
 *
 * \TODO Implement Next Fit
 * \TODO Implement Best Fit
 * \TODO Implement Worst Fit
 */
struct _block*findFreeBlock(struct _block **last, size_t size) 
{
   struct _block *curr = heapList;

#if defined FIT && FIT == 0
   /* First fit */
   while (curr && !(curr->free && curr->size >= size)) 
   {
      *last = curr;
      curr  = curr->next;
   }
#endif

#if defined BEST && BEST == 0
   /** \TODO Implement best fit here */
   struct _block *bestMemoryBlock=NULL; //initializing the best memory block as null
   size_t max= 2147483647; //to store the size of bestMemoryBlock, initializing the variable using the value of INT_MAX directly as code is generating some warnings while running on the local machine.
   size_t diff=0; //initializing the difference of the size as 0
   // struct _block *iterator=NULL; //using a iterator to traverse through the memory block. The traversal syntax is similar to that of a singly-linked linked list.
   // for(iterator=curr;iterator!=NULL;iterator=iterator->next){ //iterating until the iterator memory block is not null, and assigned the value of current memory block before beginning the traversal.
   //     *last=iterator; //previous memory block now pointing to the current block
   //     diff=(int)(iterator->size-size); //computing the difference in size
   //     if(iterator->free){ //if the current memory block is free
   //       if((iterator->size >= size) && (diff<max)){ //if the memory block that is free,is greater than the requested size and the difference in size block is smaller than the current saved block
   //          bestMemoryBlock=iterator; //assigning the value of best memory block to the current iterator
   //          max=diff; //similarly assigning the value of size to the difference computed.
   //       }
   //     }
   // }
   while(curr){
      *last=curr; //previous memory block now pointing to the current block
      diff=curr->size-size;//computing the difference in size
      if(diff<max && curr->size>=size){ //if the memory block that is free,is greater than the requested size and the difference in size block is smaller than the current saved block
         if(curr->free){ //if the current memory block is free
            bestMemoryBlock=curr; //assigning the value of best memory block to the current iterator
            max=diff;//similarly assigning the value of size to the difference computed.
         }
      }
      curr=curr->next;
   }
   if(bestMemoryBlock){ //if there exist a memoryblock which meets the above conditions, 
      curr=bestMemoryBlock; //we point the current memory block to the bestMemoryBlock address.
   }
#endif

#if defined WORST && WORST == 0
   /** \TODO Implement worst fit here */
    struct _block *worstMemoryBlock=NULL; //initializing the worst memory block as null
   //  struct _block *iterator=NULL; //using a iterator memory block to traverse through the memory block. The traversal syntax is similar to that of a singly-linked linked list.
    size_t min= -2147483648;// to store the size of worstMemoryBlock
    size_t diff=0;//to compute the difference between available and requested size to allocate enough possible memory
   //  for(iterator=curr;iterator!=NULL;iterator=iterator->next){ //iterating until the iterator memory block is not null, and assigned the value of current memory block before beginning the traversal.
   //      *last=iterator; //previous memory block now pointing to the current block
   //      diff=(int)(iterator->size-size); //computing the difference in size
   //      if(iterator->free){ //if the current memory block is free
   //          if((iterator->size >= size)&&(min<diff)){ //if the memory block that is free,is greater than the requested size and the difference in size block is smaller than the current saved block
   //              worstMemoryBlock=iterator; //assigning the value of worst memory block to the current iterator
   //              min=diff;//similarly assigning the value of size to the difference computed.
   //          }
   //      }
   //  }
   while(curr){ //iterating until the heapList exists
      *last=curr; //previous memory block now pointing to the current block
      diff=curr->size-size; //computing the difference in size
      if((min<diff)&&(curr->size>=size)){//if the memory block that is free,is greater than the requested size and the difference in size block is smaller than the current saved block
         if(curr->free) //if the current memory block is free
         {
            worstMemoryBlock=curr;//assigning the value of worst memory block to the current iterator
            min=diff;//similarly assigning the value of size to the difference computed.
         }
         
      }
      curr=curr->next;
   }
    if(worstMemoryBlock){ //if there exist a memoryblock which meets the above conditions, 
      curr=worstMemoryBlock;//we point the current memory block to the worstMemoryBlock address.

   } 
#endif

#if defined NEXT && NEXT == 0
   /** \TODO Implement next fit here */
   curr=nextFitMemoryBlock;//pointing the current memory block to the last used memory block
   //checking if there is a last used memory block
   if(!nextFitMemoryBlock){
      curr=heapList; //if it is null then we assign a new Free list to track the _blocks available
   }
   //if there is no memory block which was used last, then we will implement first fit to add the beginning of new memory block to the end
   while(curr && !(curr->free & curr->size >= size)){
      *last=curr;
      curr=curr->next;
      nextFitMemoryBlock=curr;
   }
#endif
   return curr;
}

/*
 * \brief growheap
 *
 * Given a requested size of memory, use sbrk() to dynamically 
 * increase the data segment of the calling process.  Updates
 * the free list with the newly allocated memory.
 *
 * \param last tail of the free _block list
 * \param size size in bytes to request from the OS
 *
 * \return returns the newly allocated _block of NULL if failed
 */
struct _block *growHeap(struct _block *last, size_t size) 
{
   /* Request more space from OS */
   struct _block *curr = (struct _block *)sbrk(0);
   struct _block *prev = (struct _block *)sbrk(sizeof(struct _block) + size);

   assert(curr == prev);

   /* OS allocation failed */
   if (curr == (struct _block *)-1) 
   {
      return NULL;
   }

   /* Update heapList if not set */
   if (heapList == NULL) 
   {
      heapList = curr;
   }

   /* Attach new _block to prev _block */
   if (last) 
   {
      last->next = curr;
   }

   /* Update _block metadata */
   curr->size = size;
   curr->next = NULL;
   curr->free = false;
   return curr;
}

/*
 * \brief malloc
 *
 * finds a free _block of heap memory for the calling process.
 * if there is no free _block that satisfies the request then grows the 
 * heap and returns a new _block
 *
 * \param size size of the requested memory in bytes
 *
 * \return returns the requested memory allocation to the calling process 
 * or NULL if failed
 */
void *malloc(size_t size) 
{

   if( atexit_registered == 0 )
   {
      atexit_registered = 1;
      atexit( printStatistics );
   }

   /* Align to multiple of 4 */
   size = ALIGN4(size);
   /* Handle 0 size */
   if (size == 0) 
   {
      return NULL;
   }
   num_mallocs+=1;
   /* Look for free _block */
   struct _block *last = heapList;
   struct _block *next = findFreeBlock(&last, size);
   //adding the requested memory in num_requested
   num_requested+=size;
   /* TODO: Split free _block if possible */
   if(next && (next->size > size)){ //if the free block is not null and it is larger than the requested size
      struct _block *dummyNext=next->next; //keeping the track of old pointer
      int storingSize=next->size;//old size of the memory block.

      next->next=(struct _block*)((void*)next+size+sizeof(struct _block)); //computing and assigning the size of the next memory block
      next->next->size=storingSize-size-sizeof(struct _block); //subtracting the storing size and assign it to the new memory
      next->next->free=true; //assigning the next memory block to free so that it is available for the next allocation
      if(dummyNext!=NULL){ //if the previous pointer is not null then we assign the previous pointer back to the list
         next->next->next=dummyNext;
      }
      else{
         next->next->next=NULL; //otherwise assign it to null
      }
      num_splits+=1; //increase the number of memory splits 
      num_blocks+=1;// the number of blocks will also increase as we have splitted the memory blocks
   }
   /* Could not find free _block, so grow heap */
   if (next == NULL) 
   {
      next = growHeap(last, size);
      num_grows+=1; //adding the number of time we are growing the heap
      max_heap+=size; //computing the new size of heap
      num_blocks+=1;//increase the number of blocks as heap grows
   }
   /* Could not find free _block or grow heap, so just return NULL */
   if (next == NULL) 
   {
      return NULL;
   }
   else{
      num_reuses+=1;//adding the time, reusing the existing block.
   }
   /* Mark _block as in use */
   next->free = false;

   /* Return data address associated with _block */
   return BLOCK_DATA(next);
}
void *calloc(size_t nmemb, size_t size){ //implementing the calloc function
   size_t total= nmemb * size; //computing the size in number of bytes by multiplying size by the number of memory blocks
   void *pointer=malloc(size); //using malloc to dynamically allocate the memory and assign it to pointer
   if (pointer){ //if pointer exists
      return memset(pointer,0,total); //using memset function of C to initialize all the memory blocks with 0
   }
   return NULL; //if pointer is not found then we return NULL
}
void *realloc(void *ptr, size_t size){ //implementing the realloc function
   void *newPointer;
   if (ptr==NULL){ //if the old pointer exists
      if(malloc_size(ptr)>=size){ //and the size of the current pointer is greater than or equal to the requested size
         newPointer=ptr;  //pointing the new pointer to the old address 
      }
      else{
         newPointer=malloc(size); //otherwise allocating new memory for the pointer
         if(!newPointer){ //runs if fails to reallocate memory
            printf("Couldn't reassign the memory block");
            exit(EXIT_FAILURE);
         }
         memcpy(newPointer,ptr,malloc_size(ptr)); //using the inbuilt function to copy the memory contents
         free(ptr); //freeing the pointer after done with the memory
      }
   }
   else{ //if the previous pointer does not exits
      newPointer=malloc(size); //using malloc to allocate the memory for the pointer
      if(!newPointer){ //if couldn't assign the memory blocks
         printf("Couldn't reassign the memory block");
         exit(EXIT_FAILURE);
      }
   }
   return newPointer;
}
/*
 * \brief free
 *
 * frees the memory _block pointed to by pointer. if the _block is adjacent
 * to another _block then coalesces (combines) them
 *
 * \param ptr the heap memory to free
 *
 * \return none
 */
void free(void *ptr) 
{
   if (ptr == NULL) 
   {
      return;
   }

   /* Make _block as free */
   struct _block *curr = BLOCK_HEADER(ptr);
   assert(curr->free == 0);
   curr->free = true;
   /* TODO: Coalesce free _blocks if needed */
   struct _block *storingBlock=heapList;//block to store the coalesced memory blocks
   while(storingBlock){ //while the list exists
      if(storingBlock!=NULL && storingBlock->free && storingBlock->next!=NULL && storingBlock->next->free){ //checking if the current and next block is free
         storingBlock->size=storingBlock->size+storingBlock->next->size+sizeof(struct _block); //computing the size of two free adjacent blocks
         storingBlock->next=storingBlock->next->next; //combining the two adjacent blocks
         num_blocks-=1; //decreasing the number of blocks as they are being merged
         num_coalesces+=1; //increase the number of coalesces
      }
      storingBlock=storingBlock->next; //traversing to the next memory block
   }
num_frees++; //increase the number of free memory blocks
}
