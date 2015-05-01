#ifndef MEMSIM_H
#define MEMSIM_H

#include <stdbool.h>
#include <stdio.h>	
#include <string.h>
#include <stdlib.h>
#include <math.h>

typedef unsigned long long int ulli;

/* Cache Parameters */
/* L1 */
extern int L1_block_size;	// default: 32
extern int L1_cache_size;	// default: 8192
extern int L1_assoc;		// default: 1
extern int L1_hit_time;		// default: 1
extern int L1_miss_time;	// default: 1
extern unsigned long long int L1_index_mask;
extern ulli L1_index_bits;
extern ulli L1_offset_bits;
extern unsigned long long int L1_tag_mask;

/* L2 */
extern int L2_block_size;	// default: 64
extern int L2_cache_size;	// default: 32768
extern int L2_assoc;		// default: 1
extern int L2_hit_time;		// default: 5
extern int L2_miss_time;	// default: 7
extern int L2_transfer_time;	// default: 5
extern int L2_bus_width;	// default: 16
extern unsigned long long int L2_index_mask;
extern ulli L2_index_bits;
extern ulli L2_offset_bits;
extern unsigned long long int L2_tag_mask;

/* Main Memory Parameters */
extern int mem_sendaddr;	// default: 10
extern int mem_ready;		// default: 30
extern int mem_chunktime;	// default: 15
extern int mem_chunksize;	// default: 8

/* General Parameters */
extern int flush_cache;		// default 380000

/* Report Parameters */
extern unsigned long long int execute_time;
/* Reference Counts */
extern unsigned long long int total_refs;
extern unsigned long long int flush_refs;
extern unsigned long long int inst_refs;
extern unsigned long long int data_refs;
extern unsigned long long int read_data_refs;
extern unsigned long long int write_data_refs;
/* Cycles */
extern unsigned long long int total_cycles;
extern unsigned long long int read_data_cycles;
extern unsigned long long int write_data_cycles;
extern unsigned long long int inst_cycles;
extern double average_read_cycles;
extern double average_write_cycles;
extern double average_inst_cycles;
extern unsigned long long int ideal_exec_time;
extern double ideal_cpi;
extern unsigned long long int ideal_misaligned_exec_time;
extern double ideal_misaligned_cpi;
/* L1 Instruction Cache */
extern unsigned long long int L1i_hit_count;
extern unsigned long long int L1i_miss_count;
extern unsigned long long int L1i_total_requests;
extern double L1i_hit_rate;
extern double L1i_miss_rate;
extern unsigned long long int L1i_kickouts;
extern unsigned long long int L1i_dirty_kickouts;
extern unsigned long long int L1i_transfers;
extern unsigned long long int L1i_flush_kickouts;
/* L1 Data Cache */
extern unsigned long long int L1d_hit_count;
extern unsigned long long int L1d_miss_count;
extern unsigned long long int L1d_total_requests;
extern double L1d_hit_rate;
extern double L1d_miss_rate;
extern unsigned long long int L1d_kickouts;
extern unsigned long long int L1d_dirty_kickouts;
extern unsigned long long int L1d_transfers;
extern unsigned long long int L1d_flush_kickouts;
/* L2 Cache */
extern unsigned long long int L2_hit_count;
extern unsigned long long int L2_miss_count;
extern unsigned long long int L2_total_requests;
extern double L2_hit_rate;
extern double L2_miss_rate;
extern unsigned long long int L2_kickouts;
extern unsigned long long int L2_dirty_kickouts;
extern unsigned long long int L2_transfers;
extern unsigned long long int L2_flush_kickouts;
/* Costs */
extern int total_cost;
extern int L1_cache_cost;
extern int L1i_cache_cost;
extern int L1d_cache_cost;
extern int L2_cache_cost;
extern int memory_cost;
/* Flushes */
extern unsigned long long int flushes;
extern unsigned long long int invalidates;

typedef struct Block{
	int valid;
	int dirty;
	unsigned long long int tag;
	unsigned long long int address;
	struct Block *next_block;
} Block;

typedef struct Status{
	bool hit;
	bool full;
} Status;

/* Cache type declarations */
extern Block **L1_Dcache;
extern Block **L1_Icache;
extern Block **L2_cache;

extern unsigned long long int L1_array_size;
extern unsigned long long int L2_array_size;


/* function prototypes */
bool read_parameters(FILE* config);
void list_parameters();
void simulator();
void generate_report();
void set_up_caches();

void PrintCache(Block **cache, unsigned long long int array_size, char cache_name[10]);
ulli CalcOffsetBits(int block_size);
ulli CalcIndexBits(int array_size);
unsigned long long int CalcIndexMask(int array_size);
unsigned long long int CalcTagMask(int index_bits, int offset_bits);
void set_up_tags_indices();
Status cache_hit(Block ** cache, ulli index, ulli tag, char op);
void kickout(Block ** cache, ulli address, ulli L1_tag, ulli L1_index, ulli L2_tag, ulli L2_index, char op, Status L1_status, Status L2_status);
void L2_kickout(ulli L2_index, char op);
void cache_insert(Block ** cache, ulli index, ulli tag, ulli address, int dirty);
void flush();
void L1_flush_kickout(ulli L2_index, ulli L2_tag, ulli address);
void L1i_kickout(Block ** cache, ulli index);
void L1d_kickout(Block ** cache, ulli index, char op);
#endif /* MEMSIM_H */