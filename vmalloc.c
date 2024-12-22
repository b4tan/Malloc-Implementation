#include "vm.h"
#include "vmlib.h"
#include <stdio.h>

/**
 * The malloc() function allocates size bytes and returns a pointer
 * to the allocated memory (payload). The memory is not initialized.
 * If size is 0, or if no available heap memory could be allocated,
 * malloc returns NULL.
 */
void *vmalloc(size_t size)
{
    // illegal size = 0
    if (size == 0) {
        return NULL;
    }
    struct block_header *ptr = heapstart; // this is the pointer to header
    struct block_header *next;
    struct block_header *address = NULL;
    struct block_footer *footer = NULL;
    size_t interval = BLKSZ(heapstart);
    // reserve memory for header
    size = size + 4;
    // alignment requirements
    if (size%8 != 0) {
        int mult = size/8;
        int total = mult*8;
        size = total + 8;
    }
    /* TODO: PA 5 */
    if ((heapstart->size_status & VM_BUSY) == 0 && BLKSZ(heapstart) >= size) { // head is not occupied
        // split block
        next = (struct block_header *)((char *)heapstart + size);
        next->size_status = BLKSZ(heapstart) - size + VM_PREVBUSY;
        heapstart->size_status = size + VM_BUSY + VM_PREVBUSY;
        footer = (struct block_footer *) ((char *)next - 4);
        footer->size = BLKSZ(next);
        return (void*) (heapstart + 1);  
    } else {
        size_t best = 0;
        // find first block that is free:
        while(BLKSZ(ptr) != 0) {
            int this_block = ptr->size_status & VM_BUSY;
            // check block if it's free and fits
            if (this_block == 0 && BLKSZ(ptr) >= size) { // if it is, then set values
                best = BLKSZ(ptr);
                address = ptr;
                break; // quit so it is the first free block that is available
            }
            interval = BLKSZ(ptr); // size of current block, so can skip to next
            ptr = (struct block_header *)((char *)ptr + interval); // points to next block splitted 
        }
        // find best fit!
        while(BLKSZ(ptr) != 0) {
            int this_block = ptr->size_status & VM_BUSY;
            // check block if it's free and fits
            if (this_block == 0 && BLKSZ(ptr) >= size) {
                if (BLKSZ(ptr) < best) { // compare which is best size
                    address = ptr;
                    best = BLKSZ(ptr);
                }
            }
            interval = BLKSZ(ptr); // size of current block, so can skip to next
            ptr = (struct block_header *)((char *)ptr + interval); // points to next block splitted 
        }
        if (address != NULL) { // there is a block of such
            next = (struct block_header *)((char *)address + size);
            if (size != BLKSZ(address)) {
                next->size_status = BLKSZ(address) - size + VM_PREVBUSY;
            } else {
                next->size_status += VM_PREVBUSY;
            }
            address->size_status = size + VM_BUSY + VM_PREVBUSY;
            footer = (struct block_footer *) ((char *)next - 4);
            footer->size = BLKSZ(next);
            return (void*) (address + 1);  
        }
    }
    return NULL;
}


