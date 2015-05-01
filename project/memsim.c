#include "memsim.h"

/* Cache Parameters */
/* L1 */
int L1_block_size = 32;
int L1_cache_size = 8192;
int L1_assoc = 1;
int L1_hit_time = 1;
int L1_miss_time = 1;
unsigned long long int L1_array_size = 0;
ulli L1_index_bits = 0;
ulli L1_offset_bits = 0;
unsigned long long int L1_index_mask = 0;
unsigned long long int L1_tag_mask = 0;

/* L2 */
int L2_block_size = 64;
int L2_cache_size = 32768;
int L2_assoc = 1;
int L2_hit_time = 5;
int L2_miss_time= 7;
int L2_transfer_time = 5;
int L2_bus_width = 16;
unsigned long long int L2_array_size = 0;
ulli L2_index_bits = 0;
ulli L2_offset_bits = 0;
unsigned long long int L2_index_mask = 0;
unsigned long long int L2_tag_mask = 0;

/* Main Memory Parameters */
int mem_sendaddr = 10;
int mem_ready = 30;
int mem_chunktime = 15;
int mem_chunksize = 8;

/* General Parameters */
int flush_cache = 380000;

/* Report Parameters */
unsigned long long int execute_time = 0;
/* Reference Counts */
unsigned long long int total_refs = 0;
unsigned long long int flush_time = 0;
unsigned long long int inst_refs = 0;
unsigned long long int data_refs = 0;
unsigned long long int read_data_refs = 0;
unsigned long long int write_data_refs = 0;
/* Cycles */
//unsigned long long int total_cycles = 0;
unsigned long long int read_data_cycles = 0;
unsigned long long int write_data_cycles = 0;
unsigned long long int inst_cycles = 0;
double average_read_cycles = 0;
double average_write_cycles = 0;
double average_inst_cycles = 0;
unsigned long long int ideal_exec_time = 0;
double ideal_cpi = 0;
unsigned long long int ideal_misaligned_exec_time = 0;
double ideal_misaligned_cpi = 0;
/* L1 Instruction Cache */
unsigned long long int L1i_hit_count = 0;
unsigned long long int L1i_miss_count = 0;
unsigned long long int L1i_total_requests = 0;
double L1i_hit_rate = 0;
double L1i_miss_rate = 0;
unsigned long long int L1i_kickouts = 0;
unsigned long long int L1i_dirty_kickouts = 0;
unsigned long long int L1i_transfers = 0;
unsigned long long int L1i_flush_kickouts = 0;
/* L1 Data Cache */
unsigned long long int L1d_hit_count = 0;
unsigned long long int L1d_miss_count = 0;
unsigned long long int L1d_total_requests = 0;
double L1d_hit_rate = 0;
double L1d_miss_rate = 0;
unsigned long long int L1d_kickouts = 0;
unsigned long long int L1d_dirty_kickouts = 0;
unsigned long long int L1d_transfers = 0;
unsigned long long int L1d_flush_kickouts = 0;
/* L2 Cache */
unsigned long long int L2_hit_count = 0;
unsigned long long int L2_miss_count = 0;
unsigned long long int L2_total_requests = 0;
double L2_hit_rate = 0;
double L2_miss_rate = 0;
unsigned long long int L2_kickouts = 0;
unsigned long long int L2_dirty_kickouts = 0;
unsigned long long int L2_transfers = 0;
unsigned long long int L2_flush_kickouts = 0;
/* Costs */
int total_cost = 0;
int L1_cache_cost = 0;
int L1i_cache_cost = 0;
int L1d_cache_cost = 0;
int L2_cache_cost = 0;
int main_memory_cost = 0;
int total_memory_cost = 0;
/* Flushes */
unsigned long long int flushes = 0;
unsigned long long int invalidates = 0;

int * parameter_values[] = {&L1_block_size, &L1_cache_size, &L1_assoc, &L1_hit_time,
		&L1_miss_time, &L2_block_size, &L2_cache_size, &L2_assoc, &L2_hit_time, &L2_miss_time,
		&L2_transfer_time, &L2_bus_width, &mem_sendaddr, &mem_ready, &mem_chunktime, &mem_chunksize};

char parameter_names[16][30] = {"L1_block_size", "L1_cache_size", "L1_assoc", "L1_hit_time",
		"L1_miss_time", "L2_block_size", "L2_cache_size", "L2_assoc", "L2_hit_time", "L2_miss_time",
		"L2_transfer_time", "L2_bus_width", "mem_sendaddr", "mem_ready", "mem_chunktime", "mem_chunksize"};

/* Cache type declarations */
Block **L1_Dcache;
Block **L1_Icache;
Block **L2_cache;

/* Functions */

// Reads Parameters.
bool read_parameters(FILE* config){
	char line[50];
	int i;
	for(i = 0; i < 16; ++i){
		fgets(line, 50, config);
		char temp[40] = "";
		strcat(temp, parameter_names[i]);
		strcat(temp, ": %d\n");
		sscanf(line, temp, parameter_values[i]);
	}
	return true;
}

void list_parameters(){
	int i = 16;
	for(i = 0; i < 16; ++i){
		printf("%s = %d\n", parameter_names[i], *parameter_values[i]);
	}
}

void set_up_caches(){
	// Set up L1_caches
	int L1_number_of_blocks = L1_cache_size/L1_block_size;
	L1_array_size = L1_number_of_blocks/L1_assoc;
	L1_Dcache = (Block**) malloc(L1_array_size*sizeof(Block*));
	L1_Icache = (Block**) malloc(L1_array_size*sizeof(Block*));
	// Set up ways and sets for L1 Caches
	for(unsigned int i = 0; i < L1_array_size; i++){
		// L1 Data Cache
		L1_Dcache[i] = (Block *) malloc(sizeof(Block));
		L1_Dcache[i]->dirty = 0;
		L1_Dcache[i]->valid = 0;
		L1_Dcache[i]->tag = 0;
		L1_Dcache[i]->address = 0;
		Block * current_block = L1_Dcache[i];
		for(int j = 0; j < L1_assoc; j++){
			if(j+1 == L1_assoc){
				current_block->next_block = NULL;
			}
			else{
				Block * next_block = (Block *) malloc(sizeof(Block));
				next_block->dirty = 0;
				next_block->valid = 0;
				next_block->tag = 0;
				next_block->address = 0;
				current_block->next_block = next_block;
				current_block = next_block;
			}
		}
		//L1 Instruction Cache
		L1_Icache[i] = (Block *) malloc(sizeof(Block));
		L1_Icache[i]->dirty = 0;
		L1_Icache[i]->valid = 0;
		L1_Icache[i]->tag = 0;
		L1_Icache[i]->address = 0;
		current_block = L1_Icache[i];
		for(int j = 0; j < L1_assoc; j++){
			if(j+1 == L1_assoc){
				current_block->next_block = NULL;
			}
			else{
				Block * next_block = (Block *) malloc(sizeof(Block));
				next_block->dirty = 0;
				next_block->valid = 0;
				next_block->tag = 0;
				next_block->address = 0;
				current_block->next_block = next_block;
				current_block = next_block;
			}
		}
	}
	//Set up L2 Cache
	int L2_number_of_blocks = L2_cache_size/L2_block_size;
	L2_array_size = L2_number_of_blocks/L2_assoc;
	L2_cache = (Block**) malloc(L2_array_size*sizeof(Block*));
	for(unsigned int i = 0; i < L2_array_size; i++){
		// L2 Cache
		L2_cache[i] = (Block *) malloc(sizeof(Block));
		L2_cache[i]->dirty = 0;
		L2_cache[i]->valid = 0;
		L2_cache[i]->tag = 0;
		L2_cache[i]->address = 0;
		Block * current_block = L2_cache[i];
		for(int j = 0; j < L2_assoc; j++){
			if(j+1 == L2_assoc){
				current_block->next_block = NULL;
			}
			else{
				Block * next_block = (Block *) malloc(sizeof(Block));
				next_block->dirty = 0;
				next_block->valid = 0;
				next_block->tag = 0;
				next_block->address = 0;
				current_block->next_block = next_block;
				current_block = next_block;
			}
		}
	}
}

void simulator(){
	set_up_caches();
	set_up_tags_indices();
	char op;
	unsigned long long int address;
	unsigned int bytesize;
	unsigned long long int L1_tag;
	unsigned long long int L1_index;
	unsigned long long int L2_tag;
	unsigned long long int L2_index;
	unsigned long long int address_end;
	ulli flush_counter = 0;
	Status L1_status;
	Status L2_status;
	while(scanf("%c %llx %d\n", &op, &address, &bytesize) == 3){
		total_refs += 1;
		address_end = address + (bytesize - 1); // Determine the final address of the reference
		address = address - (address % 4); // Byte align the address to 4 bytes
		switch(op){
			case 'I':
				inst_refs += 1;
				ideal_exec_time += 2; 
				ideal_misaligned_exec_time += 1;
				flush_counter += 1;
				break;
			case 'R':
				read_data_refs += 1;
				data_refs += 1;
				ideal_exec_time += 1;
				break;
			case 'W':
				write_data_refs += 1;
				data_refs += 1;
				ideal_exec_time += 1;
				break;
		}
		while(address <= address_end){
			L1_tag = (address >> (L1_offset_bits + L1_index_bits)) & L1_tag_mask;
			L1_index = (address >> L1_offset_bits) & L1_index_mask;
			L2_tag = (address >> (L2_offset_bits + L2_index_bits)) & L2_tag_mask;
			L2_index = (address >> L2_offset_bits) & L2_index_mask;
			//printf("Address: |%llx| L1 index: |%llx| L1 tag: |%llx|\n", address, L1_index, L1_tag);
			//printf("Address: |%llx| L2 index: |%llx| L2 tag: |%llx|\n", address, L2_index, L2_tag);
			switch(op){
			case 'I':
				ideal_misaligned_exec_time += 1;
				L1i_total_requests += 1;
				// Check if in L1_Icache
				L1_status = cache_hit(L1_Icache, L1_index, L1_tag, op);
				if(L1_status.hit){
					L1i_hit_count += 1;
					execute_time += L1_hit_time;
					inst_cycles += L1_hit_time;
				}else{ // If there is a L1 miss check L2
					if(L1_status.full){ // If the L1 cache is full kickout a block before proceeding to L2
						L1i_kickout(L1_Icache, L1_index);
					}
					L2_total_requests += 1;
					L1i_miss_count += 1;
					execute_time += L1_miss_time;
					inst_cycles += L1_miss_time;
					L1i_transfers += 1;
					// Check if in L2_cache
					L2_status = cache_hit(L2_cache, L2_index, L2_tag, op);
					if(L2_status.hit){
						L2_hit_count += 1;
						execute_time += L2_hit_time + L2_transfer_time*(L1_block_size/L2_bus_width);
						inst_cycles += L2_hit_time + L2_transfer_time*(L1_block_size/L2_bus_width);
						execute_time += L1_hit_time;
						inst_cycles += L1_hit_time;
						cache_insert(L1_Icache, L1_index, L1_tag, address, 0);
					}else{ // If there is a L2 miss go to main memory
						L2_miss_count += 1;
						execute_time += L2_miss_time + L2_transfer_time*(L1_block_size/L2_bus_width);
						inst_cycles += L2_miss_time + L2_transfer_time*(L1_block_size/L2_bus_width);
						execute_time += mem_sendaddr + mem_ready + mem_chunktime*(L2_block_size/mem_chunksize);
						inst_cycles += mem_sendaddr + mem_ready + mem_chunktime*(L2_block_size/mem_chunksize);
						execute_time += L2_hit_time;
						inst_cycles += L2_hit_time;
						execute_time += L1_hit_time;
						inst_cycles += L1_hit_time;
						L2_transfers += 1;
						if(L2_status.full){
							L2_kickout(L2_index, op); // Kickout the block and insert the new one
						}
						cache_insert(L2_cache, L2_index, L2_tag, address, 0); // Just insert the block 
						cache_insert(L1_Icache, L1_index, L1_tag, address, 0); // Go back to L1 and insert the block
					}
				}
				break;
			case 'R':
				ideal_misaligned_exec_time += 1;
				L1d_total_requests += 1;
				// Check if in L1_Dcache
				L1_status = cache_hit(L1_Dcache, L1_index, L1_tag, op);
				if(L1_status.hit){
					L1d_hit_count += 1;
					execute_time += L1_hit_time;
					read_data_cycles += L1_hit_time;
				}else{ // If there is a L1 miss check the L2 cache
					if(L1_status.full){ // If the L1 caches is full kickout a block before proceeding to L2
						L1d_kickout(L1_Dcache, L1_index, op);
					}
					L2_total_requests += 1;
					L1d_miss_count += 1;
					execute_time += L1_miss_time;
					read_data_cycles += L1_miss_time;
					L1d_transfers += 1;
					// Check if in L2_cache
					L2_status = cache_hit(L2_cache, L2_index, L2_tag, op);
					if(L2_status.hit){ // Go back to L1 and insert the block
						L2_hit_count += 1;
						execute_time += L2_hit_time + L2_transfer_time*(L1_block_size/L2_bus_width);
						read_data_cycles += L2_hit_time + L2_transfer_time*(L1_block_size/L2_bus_width);
						execute_time += L1_hit_time;
						read_data_cycles += L1_hit_time;
						cache_insert(L1_Dcache, L1_index, L1_tag, address, 0);
					}else{ // If there is a L2 miss go to main memory
						L2_miss_count += 1;
						execute_time += L2_miss_time + L2_transfer_time*(L1_block_size/L2_bus_width);
						read_data_cycles += L2_miss_time + L2_transfer_time*(L1_block_size/L2_bus_width);
						execute_time += mem_sendaddr + mem_ready + mem_chunktime*(L2_block_size/mem_chunksize);
						read_data_cycles += mem_sendaddr + mem_ready + mem_chunktime*(L2_block_size/mem_chunksize);
						execute_time += L2_hit_time;
						read_data_cycles += L2_hit_time;
						execute_time += L1_hit_time;
						read_data_cycles += L1_hit_time;
						L2_transfers += 1;
						if(L2_status.full){
							L2_kickout(L2_index, op); // Kickout the block and insert the new one
						}
						cache_insert(L2_cache, L2_index, L2_tag, address, 0); // Just insert the block in L2
						cache_insert(L1_Dcache, L1_index, L1_tag, address, 0);// Go back to L1 and insert it
					}
				}
				break;
			case 'W':
				ideal_misaligned_exec_time += 1;
				L1d_total_requests += 1;
				L1_status = cache_hit(L1_Dcache, L1_index, L1_tag, op);
				if(L1_status.hit){
					L1d_hit_count += 1;
					execute_time += L1_hit_time;
					write_data_cycles += L1_hit_time;
				}else{ // If there is a L1 miss check L2
					if(L1_status.full){ // If the L1 cache is full kickout a block before proceeding to L2
						L1d_kickout(L1_Dcache, L1_index, op);
					}
					L2_total_requests += 1;
					L1d_miss_count += 1;
					execute_time += L1_miss_time;
					write_data_cycles += L1_miss_time;
					L1d_transfers += 1;
					L2_status = cache_hit(L2_cache, L2_index, L2_tag, 'R');
					if(L2_status.hit){	// Go back to L1 and insert the block
						L2_hit_count += 1;
						execute_time += L2_hit_time + L2_transfer_time*(L1_block_size/L2_bus_width);
						write_data_cycles += L2_hit_time + L2_transfer_time*(L1_block_size/L2_bus_width);
						execute_time += L1_hit_time;
						write_data_cycles += L1_hit_time;
						cache_insert(L1_Dcache, L1_index, L1_tag, address, 1);
					}else{ // If there is a L2 miss go to main memory
						L2_miss_count += 1;
						execute_time += L2_miss_time + L2_transfer_time*(L1_block_size/L2_bus_width);
						write_data_cycles += L2_miss_time + L2_transfer_time*(L1_block_size/L2_bus_width);
						execute_time += mem_sendaddr + mem_ready + mem_chunktime*(L2_block_size/mem_chunksize);
						write_data_cycles += mem_sendaddr + mem_ready + mem_chunktime*(L2_block_size/mem_chunksize);
						execute_time += L2_hit_time;
						write_data_cycles += L2_hit_time;
						execute_time += L1_hit_time;
						write_data_cycles += L1_hit_time;
						L2_transfers += 1;
						if(L2_status.full){
							L2_kickout(L2_index, op); // Kickout the block and insert the new one
						}
						cache_insert(L2_cache, L2_index, L2_tag, address, 0); // Just insert the block in L2
						cache_insert(L1_Dcache, L1_index, L1_tag, address, 1); // Go back to L1 and insert the block
						
					}
				}
			}
			address += 4LL;
		}
		if(flush_counter == 380000){
			flush_counter = 0;
			flushes += 1;
			invalidates += 1;
			flush();
		}
	}
	//PrintCache(L1_Icache, L1_array_size, "L1_Icache");
	//PrintCache(L1_Dcache, L1_array_size, "L1_Dcache");
	//PrintCache(L2_cache, L2_array_size, "L2_cache");
}

void L1i_kickout(Block ** cache, ulli index){
	Block * current = cache[index];
	while(current){
		if(current->next_block == NULL){
			L1i_kickouts += 1;
			current->valid = 0;
			return;
		}
		current = current->next_block;
	}
}

void L1d_kickout(Block ** cache, ulli index, char op){
	Block * current = cache[index];
	while(current){
		if(current->next_block == NULL){
			if(current->dirty){
				L1d_dirty_kickouts += 1;
				L2_total_requests += 1;
				if(op == 'R'){
					read_data_cycles += L2_transfer_time*(L1_block_size/L2_bus_width);
				}else if(op == 'W'){
					write_data_cycles += L2_transfer_time*(L1_block_size/L2_bus_width);
				}
				execute_time += L2_transfer_time*(L1_block_size/L2_bus_width);
				ulli kickout_index = (current->address >> L2_offset_bits) & L2_index_mask;
				ulli kickout_tag = (current->address >> (L2_offset_bits + L2_index_bits)) & L2_tag_mask;
				Status L2_status = cache_hit(L2_cache, kickout_index, kickout_tag, 'W'); 
				if(L2_status.hit){
					L2_hit_count += 1;
					execute_time += L2_hit_time;
					if(op == 'R'){
						read_data_cycles += L2_hit_time;
					}else if(op == 'W'){
						write_data_cycles += L2_hit_time;
					}
				}else{
					if(op == 'R'){
						read_data_cycles += L2_miss_time;
						read_data_cycles += mem_sendaddr + mem_ready + mem_chunktime*(L2_block_size/mem_chunksize);
					}else if(op == 'W'){
						write_data_cycles += L2_miss_time;
						write_data_cycles += mem_sendaddr + mem_ready + mem_chunktime*(L2_block_size/mem_chunksize);
					}
					execute_time += L2_miss_time;
					execute_time += mem_sendaddr + mem_ready + mem_chunktime*(L2_block_size/mem_chunksize);
					L2_miss_count += 1;
					L2_transfers += 1;
					if(L2_status.full){
						L2_kickout(kickout_index, op);
						if(op == 'R'){
							read_data_cycles += L2_hit_time;
						}else if(op == 'W'){
							write_data_cycles += L2_hit_time;
						}
						execute_time += L2_hit_time;
					}
					cache_insert(L2_cache, kickout_index, kickout_tag, current->address, 1);
				}
			}
			L1d_kickouts += 1;
			current->valid = 0;
			return;
		}
		current = current->next_block;
	}
}

// Detects cache hits
Status cache_hit(Block ** cache, ulli index, ulli tag, char op){
	Block * current_block = cache[index];
	Block * previous_block = NULL;
	Status status;
	status.hit = false;
	status.full = true;
	while(current_block){ // Go through cache set to see if it is full
		if(current_block->valid == 0){
			status.full = false;
		}
		if(current_block->tag == tag && current_block->valid == 1){ // If there is a cache hit
			if(op == 'W'){
				current_block->dirty = 1;
			}
			if(previous_block){
				previous_block->next_block = current_block->next_block;
				current_block->next_block = cache[index];
				cache[index] = current_block;
			}
			status.hit = true;
		}
		previous_block = current_block;
		current_block = current_block->next_block;
	}
	return status;
}

// Inserts block if cache NOT full
void cache_insert(Block ** cache, ulli index, ulli tag, ulli address, int dirty){
	Block * current_block = cache[index];
	Block * previous_block = NULL;
	while(current_block){
		if(current_block->valid == 0){ // Find a block that is empty and insert the new address
			if(previous_block){
				previous_block->next_block = current_block->next_block;
				current_block->next_block = cache[index];
				cache[index] = current_block;
			}
			current_block->valid = 1;
			current_block->tag = tag;
			current_block->address = address;
			current_block->dirty = dirty;
			return;
		}else{ // Proceed to next block in cache
			previous_block = current_block;
			current_block = current_block->next_block;
		}
	}
}

// Handles L2 kickouts
void L2_kickout(ulli L2_index, char op){
	Block * current_block = L2_cache[L2_index];
	while(current_block){
		if(current_block->next_block == NULL){ 
			if(current_block->dirty){ 	// L2 dirty kickout to main memory
				switch(op){
					case 'R':
						read_data_cycles += mem_sendaddr + mem_ready + mem_chunktime*(L2_block_size/mem_chunksize);
						break;
					case 'I':
						inst_cycles += mem_sendaddr + mem_ready + mem_chunktime*(L2_block_size/mem_chunksize);
						break;
					case 'W':
						write_data_cycles += mem_sendaddr + mem_ready + mem_chunktime*(L2_block_size/mem_chunksize);
						break;
					case 'F':
						inst_cycles += mem_sendaddr + mem_ready + mem_chunktime*(L2_block_size/mem_chunksize);
						flush_time += mem_sendaddr + mem_ready + mem_chunktime*(L2_block_size/mem_chunksize);
						break;
				}
				execute_time += mem_sendaddr + mem_ready + mem_chunktime*(L2_block_size/mem_chunksize);
				L2_dirty_kickouts += 1;
			}
			L2_kickouts += 1;
			current_block->valid = 0;
			return;
		}
		current_block = current_block->next_block;
	}
}

void L1_flush_kickout(ulli L2_index, ulli L2_tag, ulli address){
	Status L2_status = cache_hit(L2_cache, L2_index, L2_tag, 'W');
	if(L2_status.hit){
		flush_time += L2_hit_time;
		execute_time += L2_hit_time;
		inst_cycles += L2_hit_time;
		L2_hit_count += 1;
	}
	else{
		L2_miss_count += 1;
		L2_transfers += 1;
		flush_time += L2_miss_time;
		execute_time += L2_miss_time;
		inst_cycles += L2_miss_time;
		flush_time += mem_sendaddr + mem_ready + mem_chunktime*(L2_block_size/mem_chunksize);
		execute_time += mem_sendaddr + mem_ready + mem_chunktime*(L2_block_size/mem_chunksize);
		inst_cycles += mem_sendaddr + mem_ready + mem_chunktime*(L2_block_size/mem_chunksize);
		if(L2_status.full){
			L2_kickout(L2_index, 'F');
			inst_cycles += L2_hit_time;
			execute_time += L2_hit_time;
			flush_time += L2_hit_time;
		}
		cache_insert(L2_cache, L2_index, L2_tag, address, 1);
	}
}

// flush
void flush(){
	Block * current;
	for(ulli i = 0; i < L1_array_size; i++){
		current = L1_Icache[i];
		while(current){
			current->valid = 0;
			current = current->next_block;
		}
	}

	for(ulli i = 0; i < L1_array_size; i++){
		current = L1_Dcache[i];
		while(current){
			if(current->dirty == 1){
				L1d_flush_kickouts += 1;
				L1d_transfers += 1;
				L2_total_requests += 1;
				execute_time += L2_transfer_time*(L1_block_size/L2_bus_width);
				flush_time += L2_transfer_time*(L1_block_size/L2_bus_width);
				inst_cycles += L2_transfer_time*(L1_block_size/L2_bus_width);
				L1_flush_kickout((current->address >> L2_offset_bits) & L2_index_mask, (current->address >> (L2_offset_bits + L2_index_bits)) & L2_tag_mask, current->address);
			}
			current->valid = 0;
			current = current->next_block;
		}
	}

	for(ulli i = 0; i < L2_array_size; i++){
		current = L2_cache[i];
		while(current){
			if(current->dirty == 1){
				L2_flush_kickouts += 1;
				L2_transfers += 1;
				flush_time += mem_sendaddr + mem_ready + mem_chunktime*(L2_block_size/mem_chunksize);
				execute_time += mem_sendaddr + mem_ready + mem_chunktime*(L2_block_size/mem_chunksize);
				inst_cycles += mem_sendaddr + mem_ready + mem_chunktime*(L2_block_size/mem_chunksize);
			}
			current->valid = 0;
			current = current->next_block;
		}
	}
}

void set_up_tags_indices(){
	L1_offset_bits = CalcOffsetBits(L1_block_size);
	L1_index_bits = CalcIndexBits(L1_array_size);
	L1_index_mask = CalcIndexMask(L1_index_bits);
	L1_tag_mask = CalcTagMask(L1_index_bits, L1_offset_bits);
	L2_offset_bits = CalcOffsetBits(L2_block_size);
	L2_index_bits = CalcIndexBits(L2_array_size);
	L2_index_mask = CalcIndexMask(L2_index_bits);
	L2_tag_mask = CalcTagMask(L2_index_bits, L2_offset_bits);
}

void generate_report(char title[]){
	char report[10000] = "";
	char border[] = "------------------------------------------------------------------------------";
	sprintf(report, "%s\n", border);
	sprintf(report + strlen(report), "\t%s\tSimulation Results\n", title);
	sprintf(report + strlen(report), "%s\n", border);
	sprintf(report + strlen(report), "\nMemory System:\n");

	sprintf(report + strlen(report), "\tDcache size = %d : ways = %d : block size = %d\n", L1_cache_size, L1_assoc, L1_block_size);
	sprintf(report + strlen(report), "\tIcache size = %d : ways = %d : block size = %d\n", L1_cache_size, L1_assoc, L1_block_size);
	sprintf(report + strlen(report), "\tL2-caches size = %d : ways = %d : block size = %d\n", L2_cache_size, L2_assoc, L2_block_size);
	sprintf(report + strlen(report), "\tMemory ready time = %d : chunksize = %d : chunktime = %d\n", mem_ready, mem_chunksize, mem_chunktime);

	sprintf(report + strlen(report), "\nExecute time = %llu; Total Refs = %llu\n", execute_time, total_refs);
	sprintf(report + strlen(report), "Flush time = %llu\n", flush_time);
	sprintf(report + strlen(report), "Inst refs = %llu; Data refs = %llu\n", inst_refs, data_refs);

	sprintf(report + strlen(report), "\nNumber of reference types: [Percentage]\n");
	sprintf(report + strlen(report), "\tReads = %llu [%.1f%%]\n", read_data_refs, ((double)read_data_refs/total_refs)*100);
	sprintf(report + strlen(report), "\tWrites = %llu [%.1f%%]\n", write_data_refs, ((double)write_data_refs/total_refs)*100);
	sprintf(report + strlen(report), "\tInst = %llu [%.1f%%]\n", inst_refs, ((double)inst_refs/total_refs)*100);
	sprintf(report + strlen(report), "\tTotal = %llu\n", total_refs);

	sprintf(report + strlen(report), "\nTotal cycles for activities:  [Percentage]\n");
	sprintf(report + strlen(report), "\tReads = %llu [%.1f%%]\n", read_data_cycles, ((double)read_data_cycles/execute_time)*100);
	sprintf(report + strlen(report), "\tWrites = %llu [%.1f%%]\n", write_data_cycles, ((double)write_data_cycles/execute_time)*100);
	sprintf(report + strlen(report), "\tInst. = %llu [%.1f%%]\n", inst_cycles, ((double)inst_cycles/execute_time)*100);
	sprintf(report + strlen(report), "\tTotal = %llu\n", execute_time);

	sprintf(report + strlen(report), "\nAverage cycles per activity:\n");
	average_read_cycles = (double)read_data_cycles/read_data_refs;
	average_write_cycles = (double)write_data_cycles/write_data_refs;
	average_inst_cycles = (double)execute_time/inst_refs;
	sprintf(report + strlen(report), "\tRead = %.1f; Write = %.1f; Inst. = %.1f\n",average_read_cycles, average_write_cycles, average_inst_cycles);
	ideal_cpi = (double)ideal_exec_time/inst_refs;
	sprintf(report + strlen(report), "Ideal: Exec. Time = %llu; CPI =  %.1f\n", ideal_exec_time, ideal_cpi);
	ideal_misaligned_cpi = (double)ideal_misaligned_exec_time/inst_refs;
	sprintf(report + strlen(report), "Ideal mis-aligned: Exec. Time = %llu; CPI =  %.1f\n", ideal_misaligned_exec_time, ideal_misaligned_cpi);

	sprintf(report + strlen(report), "\nMemory Level: L1i\n");
	sprintf(report + strlen(report), "\tHit Count = %llu Miss Count = %llu\n", L1i_hit_count, L1i_miss_count);
	sprintf(report + strlen(report), "\tTotal Requests = %llu\n", L1i_total_requests);
	L1i_hit_rate = ((double)L1i_hit_count/L1i_total_requests)*100;
	L1i_miss_rate = ((double)L1i_miss_count/L1i_total_requests)*100;
	sprintf(report + strlen(report), "\tHit Rate = %.1f%% Miss Rate = %.1f%%\n", L1i_hit_rate, L1i_miss_rate);
	sprintf(report + strlen(report), "\tKickouts = %llu; Dirty Kickouts = %llu; Transfers = %llu\n", L1i_kickouts, L1i_dirty_kickouts, L1i_transfers);
	sprintf(report + strlen(report), "\tFlush Kickouts = %llu\n", L1i_flush_kickouts);

	sprintf(report + strlen(report), "\nMemory Level: L1d\n");
	sprintf(report + strlen(report), "\tHit Count = %llu Miss Count = %llu\n", L1d_hit_count, L1d_miss_count);
	sprintf(report + strlen(report), "\tTotal Requests = %llu\n", L1d_total_requests);
	L1d_hit_rate = ((double)L1d_hit_count/L1d_total_requests)*100;
	L1d_miss_rate = ((double)L1d_miss_count/L1d_total_requests)*100;
	sprintf(report + strlen(report), "\tHit Rate = %.1f%% Miss Rate = %.1f%%\n", L1d_hit_rate, L1d_miss_rate);
	sprintf(report + strlen(report), "\tKickouts = %llu; Dirty Kickouts = %llu; Transfers = %llu\n", L1d_kickouts, L1d_dirty_kickouts, L1d_transfers);
	sprintf(report + strlen(report), "\tFlush Kickouts = %llu\n", L1d_flush_kickouts);

	sprintf(report + strlen(report), "\nMemory Level: L2\n");
	sprintf(report + strlen(report), "\tHit Count = %llu Miss Count = %llu\n", L2_hit_count, L2_miss_count);
	sprintf(report + strlen(report), "\tTotal Requests = %llu\n", L2_total_requests);
	L2_hit_rate = ((double)L2_hit_count/L2_total_requests)*100;
	L2_miss_rate = ((double)L2_miss_count/L2_total_requests)*100;
	sprintf(report + strlen(report), "\tHit Rate = %.1f%% Miss Rate = %.1f%%\n", L2_hit_rate, L2_miss_rate);
	sprintf(report + strlen(report), "\tKickouts = %llu; Dirty Kickouts = %llu; Transfers = %llu\n", L2_kickouts, L2_dirty_kickouts, L2_transfers);
	sprintf(report + strlen(report), "\tFlush Kickouts = %llu\n", L2_flush_kickouts);

	L1i_cache_cost = (int)((100 + 100*log2(L1_assoc)) * ceil(L1_cache_size/4096.0));
	L1d_cache_cost = L1i_cache_cost;
	L1_cache_cost = L1i_cache_cost + L1d_cache_cost;
	L2_cache_cost = (50*log2(L2_assoc) + 50*L2_cache_size/32768);
	main_memory_cost = 200*(15/mem_ready) + 100*(mem_chunksize/16) + 75;
	total_memory_cost = L1_cache_cost + L2_cache_cost + main_memory_cost;

	sprintf(report + strlen(report), "\nL1 cache cost (Icache $%d) + (Dcache $%d) = $%d\n", L1i_cache_cost, L1d_cache_cost, L1_cache_cost);
	sprintf(report + strlen(report), "L2 cache cost = $%d;  Memory cost = $%d  Total cost = $%d\n", L2_cache_cost, main_memory_cost, total_memory_cost);

	sprintf(report + strlen(report), "Flushes = %llu : Invalidates = %llu\n", flushes, invalidates);

	printf("%s", report);

	char filename[100] = "";
	strcat(filename, title);
	strcat(filename, ".txt");

	FILE *f = fopen(filename, "w");
	fprintf(f, "%s", report);
	fclose(f);
}

ulli CalcOffsetBits(int block_size){
	ulli offset_bits = 0LL;
	while(block_size >>= 1LL){
		offset_bits++;
	}
	return offset_bits;
}

ulli CalcIndexBits(int array_size){
	ulli index_bits = 0LL;
	while(array_size >>= 1LL){
		index_bits++;
	}
	return index_bits;
}

unsigned long long int CalcIndexMask(int index_bits){
	unsigned long long int index_mask = 0xFFFFFFFFFFFF >> (48 - index_bits);
	return index_mask;
}

unsigned long long int CalcTagMask(int index_bits, int offset_bits){
	unsigned long long int tag_mask = 0xFFFFFFFFFFFF >> (index_bits + offset_bits);
	return tag_mask;
}

void PrintCache(Block **cache, unsigned long long int array_size, char cache_name[20]){
	Block *block;
	unsigned long long int i;
	printf("\nMemory Level: %s\n", cache_name);
	for(i = 0;i < array_size;i++){
		block = cache[i];
		printf("Index |%llx|\n", i);
		while(block != NULL){
			if(block->valid){
				printf("V: |%d| D: |%d|; Tag: |%llx| Address: |%llx|\n", block->valid, block->dirty, block->tag, block->address);
			}
			block = block->next_block;
		}
	}
}
