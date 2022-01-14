#include "kernel/process.h"
#include "kernel/mem.h"
#include "kernel/exceptionHandlers.h"
#include "common/stdlib.h"

uint32_t next_proc_num = 1;

extern uint8_t __end;

IMPLEMENT_LIST(pcb);

pcb_list_t run_queue;
pcb_list_t all_proc_list;


process_control_block_t * current_process;

extern void switch_to_thread(process_control_block_t *old, process_control_block_t *new);
extern int try_lock(int * lock_var);

void schedule(void){

    DISABLE_INTERRUPTS();
    if(size_pcb_list(&run_queue) == 0){
        timer_set(10000);
        ENABLE_INTERRUPTS();
        return;
    }
    
    process_control_block_t *new_thread, *old_thread;



    new_thread = pop_pcb_list(&run_queue);
    old_thread = current_process;
    current_process = new_thread;



    append_pcb_list(&run_queue, old_thread);

    switch_to_thread(old_thread, new_thread);
  
    ENABLE_INTERRUPTS();

}

void process_init(void){
    process_control_block_t * main_pcb;

    INITIALIZE_LIST(run_queue);
    INITIALIZE_LIST(all_proc_list);

    char * name = "Init";
    int name_len = 4;
    main_pcb = kmalloc(sizeof(process_control_block_t));
    main_pcb -> stack_page = (void *)&__end;
    main_pcb ->pid = getNewPid();
    //memcpy(main_pcb->proc_name, name, MIN(name_len,19));
    main_pcb -> proc_name[MIN(name_len,19)] = 0;

    append_pcb_list(&all_proc_list, main_pcb);

    current_process = main_pcb;

    timer_set(10000);
}

static void reap(void) {

    DISABLE_INTERRUPTS();
    process_control_block_t * new_thread, * old_thread;

    // If nothing on the run queue, there is nothing to do now. just loop
    while (size_pcb_list(&run_queue) == 0);

    // Get the next thread to run.  For now we are using round-robin
    new_thread = pop_pcb_list(&run_queue);
    old_thread = current_process;
    current_process = new_thread;

    // Free the resources used by the old process.  Technically, we are using dangling pointers here, but since interrupts are disabled and we only have one core, it
    // should still be fine
    free_page(old_thread->stack_page);
    kfree(old_thread);

    // remove from all threads list
    remove_pcb(&all_proc_list, old_thread);

    // Context Switch
    switch_to_thread(old_thread, new_thread);
}

int getNewPid(void){
    return next_proc_num++;
}

void create_kernel_thread(main_func_pointer main_func, char * name, int name_len){
    
    process_control_block_t * pcb;
    proc_saved_state_t * new_proc_state;

    pcb = kmalloc(sizeof(process_control_block_t));
    
    pcb -> stack_page = alloc_page();
    pcb -> pid = getNewPid();
    //memcpy(pcb->proc_name, name, MIN(name_len,19));
    pcb -> proc_name[MIN(name_len,19)] = 0;

    new_proc_state = pcb -> stack_page + PAGE_SIZE - sizeof(proc_saved_state_t);
    pcb -> saved_state = new_proc_state;

    bzero(new_proc_state, sizeof(proc_saved_state_t));
    new_proc_state->lr = (uint32_t)main_func;     // lr is used as return address in switch_to_thread
    new_proc_state->sp = (uint32_t)reap;            // When the thread function returns, this reaper routine will clean it up
    new_proc_state->cpsr = 0x13 | (8 << 1);         // Sets the thread up to run in supervisor mode with irqs only

    // add the thread to the lists
    append_pcb_list(&all_proc_list, pcb);
    append_pcb_list(&run_queue, pcb);

}
