.intel_syntax noprefix

.text
.global my_memcpy

my_memcpy:
  mov ecx, edx
  shr ecx, 3
  rep movsq
  
  mov ecx, edx
  shl ecx, 61
  shr ecx, 61
  rep movsb

  ret
