/* GPLv2 (c) Airbus */
#include <debug.h>
#include <intr.h>

void bp_handler() {
	asm volatile("pusha");
	uint32_t val;
   	asm volatile ("mov 4(%%ebp), %0":"=r"(val));
	debug("val : %x\n", val);
	debug("Handling a BreakPoint :)\n");
	asm volatile("popa");
	asm volatile("leave; iret");
}

void bp_trigger() {
	asm volatile("int3");
}

void tp() {
	// TODO print idtr
	idt_reg_t idtr;
	get_idtr(idtr);
	debug("IDTR : addr=0x%lx \t limit=0x%x\n", idtr.addr, idtr.limit);

	int_desc_t* bp_entry_idt = (int_desc_t*)(idtr.addr + 3*sizeof(int_desc_t)); // Address of the BP descriptor
	int_desc_t bp_desc = *bp_entry_idt;

	bp_desc.offset_1 = (uint16_t)(uint32_t)&bp_handler & 0x0000ffff;
	bp_desc.offset_2 = (uint16_t)((uint32_t)&bp_handler >> 16) & 0x0000ffff;

	*bp_entry_idt = bp_desc; 

	bp_trigger();
}
