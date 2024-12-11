#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "lab4.h"


Page_Entry* page_table;
TLB_Table tlb;
Main_Memory main_mem;
FILE* backing_store_fp;

// Implement get_page_from() and get_offset_from() functions below this line
unsigned char get_page_from(int logical_address)
{
    unsigned char page_num = (logical_address >> PAGE_ADDR_BITS) & 0x00FF;
    return page_num;
}

unsigned char get_offset_from(int logical_address)
{
    unsigned char offset = logical_address & 0x00FF;
    return offset;
}


// Implement the page table functions below this line
void init_page_table()
{
    //page_table->frame = NULL;
   // page_table->is_valid = NULL;
   //allocate 256 entries
   page_table = (Page_Entry*)malloc(sizeof(Page_Entry)* (1 << PAGE_ADDR_BITS));
   //page_table[1 << PAGE_ADDR_BITS];
}

void close_page_table()
{
    free(page_table);
}

short page_table_lookup(unsigned char page)
{
    if(page_table[page].is_valid == 1)
    {
        return page_table[page].frame;
    }
    else
    {
        return -1;
    }
}

void update_page_table(unsigned char page, unsigned char frame)
{
    page_table[page].frame = frame;
    page_table[page].is_valid = 1;
}


// Implement the TLB functions below this line
void init_tlb()
{
    //allocate 16 entries
    tlb.entry = (TLB_Entry*)malloc(sizeof(TLB_Entry)*TLB_LEN);
    //use fifo to replace entries when the table becomes full
    tlb.head = 0;
    tlb.tail = 0;
    tlb.length = 0;
    tlb.is_full = 0;

}

short tlb_lookup(unsigned char page)
{
    if(tlb.entry[page].is_valid == 0)
    {
        return tlb.entry[page].frame;
    }
    else
    {
        return -1;
    }
}

void update_tlb(unsigned char page, unsigned char frame)
{
    if(tlb.is_full == 0)
    {
        tlb.entry[page].frame = frame;
        tlb.tail = page;
        tlb.length++;
    }
    else if(tlb.length == TLB_LEN)
    {
        tlb.entry[page].frame = frame;
        tlb.tail = page;
        tlb.is_full = 1;
    }
    else
    {
        tlb.tail = tlb.entry[page].frame;
        tlb.head = tlb.entry[page].frame;
    }
    
    
}
void close_tlb()
{
    free(tlb.entry);
}


// Implement the Physical Memory functions below this line
void init_physical_memory()
{
    //allocate main_mem
    main_mem.mem = (char*)malloc(sizeof(char)*((1 << PAGE_ADDR_BITS)*(1 << FRAME_ADDR_BITS)));
    //main_mem.mem[(1 << PAGE_ADDR_BITS)*(1 << FRAME_ADDR_BITS)];
    main_mem.next_available_frame = 0;
}

void free_physical_memory()
{
   free(main_mem.mem);
}

// Returns the signed value stored at the logical_address
signed char get_value(int logical_address) //make sure tlb_lookup is working
{
    //get page and offset
    //tlb lookup
    //page_table lookup
    //handle any page fault
    //update tlb
    //get value

    signed char val;
    unsigned char page = get_page_from(logical_address);
    unsigned char offset = get_offset_from(logical_address);
    short frame = tlb_lookup(page);

    if(frame != -1)
    {
        frame = page_table_lookup(page);
        if(frame == -1)
        {
            frame = main_mem.next_available_frame++;
        }
    }

    update_tlb(page, frame);
    val = main_mem.mem[frame * (1 << OFFSET_ADDR_BITS) + offset];

    return val;
}


// Implement the Backing Store functions below this line
void init_backing_store(char* filename)
{
    backing_store_fp = fopen(filename, "rb");
}

int roll_in(unsigned char page, unsigned char frame)
{
    fseek(backing_store_fp, page, SEEK_SET);
    fread(&main_mem.mem[frame * (1 << OFFSET_ADDR_BITS)], sizeof(char), 1 << OFFSET_ADDR_BITS, backing_store_fp);
    
    if((fseek(backing_store_fp, page, SEEK_SET) || fread(&main_mem.mem[frame * (1 << OFFSET_ADDR_BITS)], sizeof(char), 1 >> OFFSET_ADDR_BITS, backing_store_fp) == -1))
    {
        return -1;
    }
    else
    {
        return 0;
    }
}

void close_backing_store()
{
    fclose(backing_store_fp);
}
