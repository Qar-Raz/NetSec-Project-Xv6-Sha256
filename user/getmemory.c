#include "kernel/types.h"
#include "user/user.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "kernel/param.h"
#include "kernel/riscv.h"

int main(){
int x = getmem();
printf("The memory in megabytes is = %d\n ", x);
}
