# Context switch
#
#   void swtch(struct context **old, struct context *new);
# 
# Save the current registers on the stack, creating
# a struct context, and save its address in *old.
# Switch stacks to new and pop previously-saved registers.

.globl swtch
swtch:
  movl 4(%esp), %eax
  movl 8(%esp), %edx

  # Save old callee-saved registers
  pushl %ebp
  pushl %ebx
  pushl %esi
  pushl %edi

  # Switch stacks
  movl %esp, (%eax)
  movl %edx, %esp

  # Load new callee-saved registers
  popl %edi
  popl %esi
  popl %ebx
  popl %ebp
  ret

.globl getctx
getctx:
  movl 4(%esp), %eax

  # copy eip
  movl (%esp), %edx

  # Save old callee-saved registers
  pushl %ebp
  pushl %ebx
  pushl %esi
  pushl %edi

  # save reference to esp
  movl %esp, (%eax)

  # push eip back on top of stask
  push  %edx

  ret

.globl setesp
setesp:
  movl 4(%esp), %eax

  # swap stack pointer
  movl %eax, %esp

  ret

.globl setctx
setctx:
  movl 4(%esp), %eax

  # swap stack pointer
  movl %eax, %esp

  # Load new callee-saved registers
  popl %edi
  popl %esi
  popl %ebx
  popl %ebp

  ret