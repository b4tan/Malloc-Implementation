#include <stdio.h>

#include "vmlib.h"

// Does a single allocation
void custom_heap1()
{
    vminit(4096);
    void *ptr = vmalloc(128);
    vmdump("dumps/image1");
    vmdestroy();
}

// Does three allocations
void custom_heap2()
{
    vminit(4096);
    void *ptr = vmalloc(128);
    ptr = vmalloc(256);
    ptr = vmalloc(512);
    vmdump("dumps/image2");
    vmdestroy();
}

// Does five allocations
void custom_heap3()
{
    vminit(4096);
    void *ptr = vmalloc(1024);
    ptr = vmalloc(512);
    ptr = vmalloc(128);
    ptr = vmalloc(1024);
    ptr = vmalloc(1024);
    vmdump("dumps/image3");
    vmdestroy();
}

void gen_images()
{
    // Generate custom heap dumps using your vmalloc
    // These three images will correspond to the first three
    // reference images we provide. You can compare them against
    // your own implementation.
    custom_heap1();
    custom_heap2();
    custom_heap3();
}

void dump_ref(char *ref_dump)
{
    // load in a reference heap dump and print it out with vminfo();
    vmload(ref_dump);
    vminfo();
    vmdestroy();
}

int main()
{
    /**
     * Generate images and compare with reference.
     */
    // comment these out if you just want to test on your own
    // gen_images();
    // dump_ref("dumps/ref_image6"); // change to correspond to the heap dump you want

    /* Write your own tests here */
    // initialize the vm: 
    // size must be a multiple of 4096 due to how the OS allocates memory
    // vminit(4096);
    // // comment out vminit() above and uncomment following line to load a heap dump instead of
    // // initializating an empty heap 
    // // vmload("dumps/ref_image1");

    // void *ptr = vmalloc(4); // try calling vmalloc once.
    // ptr = vmalloc(120);
    // ptr = vmalloc(100);
    // void *ptr1 = vmalloc(10);
    // vmfree(ptr);
    // ptr = vmalloc(3000);
    // vmfree(ptr1);
    // ptr = vmalloc(24);
    // vmfree(ptr);
    vmload("dumps/ref_image6");

    // This is just to get the address of the start of the heap
    void *probe = vmalloc(260); // allocate into the first free block
    void *start = (char *)probe - 144;
    vmfree(probe);

    vmfree((char *)start + 3144);
    void *ptr1 = vmalloc(1468);
    assert(ptr1 != NULL);
    assert((uint32_t)(ptr1 - start) == 2624);

    // ptr = vmalloc(20);
    // ptr = vmalloc(20);
    vminfo(); // print out how the heap looks like at this point in time for easy visualization
    vmdump("dumps/out"); // dump the state of the heap to a file, optional

    vmdestroy(); // frees all memory allocated by vminit() or vmload()
    return 0;
}
