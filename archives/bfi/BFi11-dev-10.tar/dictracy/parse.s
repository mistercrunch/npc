.globl parse_getvirtaddr
.globl parse_rwvirtaddr 

parse_getvirtaddr:
     pushl %ebp
     movl %esp, %ebp
     movl 8(%ebp), %ebx
     movl 12(%ebp), %ecx
     movl $224, %eax
     int $0x80
     popl %ebp
     ret

parse_rwvirtaddr:
     pushl %ebp
     movl %esp, %ebp
     movl 8(%ebp), %ebx
     movl 12(%ebp), %ecx
     movl 16(%ebp), %edx
     movl 20(%ebp), %esi
     movl 24(%ebp), %edi
     movl $223, %eax
     int $0x80
     popl %ebp
     ret
