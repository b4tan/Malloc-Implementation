#include "vm.h"
#include "vmlib.h"
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

/**
 * The vmfree() function frees the memory space pointed to by ptr,
 * which must have been returned by a previous call to vmmalloc().
 * Otherwise, or if free(ptr) has already been called before,
 * undefined behavior occurs.
 * If ptr is NULL, no operation is performed.
 * vmfree() asserts that ptr is 8-byte aligned.
 */
void vmfree(void *ptr)
{   
    assert(ptr != NULL); // cannot free an address that points to nothing
    assert((uint32_t) ptr % 8 == 0); // address has to satisfy alignment requirements
    // ptr is address of first block of payload
    // header is only 4 bytes
    /* TODO: PA 6 */
    struct block_header* current = ptr - 4;
    struct block_header* prev, *next;
    struct block_header* pointer = heapstart;

    // find addresses of left and right blocks
    prev = heapstart;
    while (pointer != current) {
        prev = pointer;
        pointer = (struct block_header*) ((char *)pointer + BLKSZ(pointer));
    }
    next = (struct block_header*) ((char *)current + BLKSZ(current));
    // check if blocks are not 0
    if (BLKSZ(next) == 0) { // no block on the right of ptr
        next = NULL;
    }
    if (current == heapstart) { // left of ptr is the first 4 bytes of the heap
        prev = NULL;
    }
    // unset allocation Bit
    // when freeing, we must consider coalescence with other blocks
    // if other blocks are coallesced, then header of current should be removed
    // if no other blocks are being coallesced then header should still be current
    struct block_footer* footer;
    // set status as occupied for the momment
    int prev_stat = 1;
    int next_stat = 1;
    if (next != NULL) { // if there is a next block, we can check its status
        next_stat = next->size_status & VM_BUSY;
    }
    if (prev != NULL) { // if there is a prev block, we can check its status
       prev_stat = current->size_status & VM_PREVBUSY;
    }
    // RELEASE FOOTERS? No, they are garbage value
    // case 1 : left and right blocks are free and need to be coalesced
    if (prev_stat == 0 && next_stat == 0) { 
        printf("a");
        prev->size_status = prev->size_status + BLKSZ(current) + BLKSZ(next); // prev becomes block header
        current->size_status = 0;
        next->size_status = 0;
        ptr = (struct block_header*) ((char*)prev + BLKSZ(prev) - 4);
        footer = (struct block_footer*) ptr;
        footer->size = BLKSZ(prev);
    // case 2: only left block is free
    } else if (prev_stat == 0) {
        printf("b");
        prev->size_status = prev->size_status + BLKSZ(current);
        current->size_status = 0;
        if (next != NULL) {
            next->size_status = next->size_status - 0x00000002; // set prev not busy
        }
        ptr = (struct block_header*) ((char*)prev + BLKSZ(prev) - 4);
        footer = (struct block_footer*) ptr;
        footer->size = BLKSZ(prev);
    // case 3: only right block is free
    } else if (next_stat == 0) { // correct for when we are freeing heapstart
        printf("c");
        current->size_status = current->size_status + BLKSZ(next);
        current->size_status = current->size_status - 0x00000001;
        next->size_status = 0;
        ptr = (struct block_header*) ((char*)current + BLKSZ(current) - 4);
        footer = (struct block_footer*) ptr;
        footer->size = BLKSZ(current);
    // case 4: none is free
    } else if (next_stat != 0 && prev_stat != 0) {
        printf("d");
        current->size_status = current->size_status - 0x00000001;
        if (next != NULL) {
            next->size_status = next->size_status - 0x00000002; // set prev not busy
        }
        ptr = (struct block_header*) ((char*)current + BLKSZ(current) - 4);
        footer = (struct block_footer*) ptr;
        footer->size = BLKSZ(current);
    }
}
