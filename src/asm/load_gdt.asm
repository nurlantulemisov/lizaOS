section .text
  global load_gdt

load_gdt:
  mov eax, [esp + 4]  ; get gdt pointer from fun args
  lgdt [eax]          ; load gdt
  
  mov ax, 0x10    ; kernel data segment. its const
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax
  mov ss, ax

  jmp 0x08:far_jump   ; jump to far with code data segment
far_jump:
  ret

