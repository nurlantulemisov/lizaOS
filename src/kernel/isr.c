#include "isr.h"
#include "screen.h"

ISR g_interrupt_handlers[NO_INTERRUPT_HANDLERS];

// for more details, see Intel manual -> Interrupt & Exception Handling
char *exception_messages[32] = {"Division By Zero",
                                "Debug",
                                "Non Maskable Interrupt",
                                "Breakpoint",
                                "Overflow",
                                "BOUND Range Exceeded",
                                "Invalid Opcode",
                                "Device Not Available (No Math Coprocessor)",
                                "Double Fault",
                                "Coprocessor Segment Overrun",
                                "Invalid TSS",
                                "Segment Not Present",
                                "Stack-Segment Fault",
                                "General Protection",
                                "Page Fault",
                                "Unknown Interrupt (intel reserved)",
                                "x87 FPU Floating-Point Error (Math Fault)",
                                "Alignment Check",
                                "Machine Check",
                                "SIMD Floating-Point Exception",
                                "Virtualization Exception",
                                "Reserved",
                                "Reserved",
                                "Reserved",
                                "Reserved",
                                "Reserved",
                                "Reserved",
                                "Reserved",
                                "Reserved",
                                "Reserved",
                                "Reserved",
                                "Reserved"};

/**
 * invoke isr routine and send eoi to pic,
 * being called in irq.asm
 */
void isr_irq_handler(REGISTERS *reg) {
  // if (g_interrupt_handlers[reg->int_no] != NULL) {
  // ISR handler = g_interrupt_handlers[reg->int_no];
  // handler(reg);
  // }
  screen_put_text("received interrupt: ");
}

void isr_exception_handler(REGISTERS reg) {
  screen_put_text("received exception");
}
