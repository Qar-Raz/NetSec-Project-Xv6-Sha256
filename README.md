# xv6-riscv-Project-
SHA256 Implementation in Xv6 by Qamar Raza and Mudasir Javad

1)User Space Implementation
Run  $sha256 sample.txt  
This will hash the text in the sample.txt file and give the output as well as the ticks count.

2)Kernel Space Implementation
kernel space implementation runs on every boot. A hard coded value is hash and the output as well as the ticks count is printed.

3)System call Implementation
call $sha_syscall <text_to_hash> 
this program calls the sha256 system and hashes the arguments passed. The output and ticks count is printed.
 
