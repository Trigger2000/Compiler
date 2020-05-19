#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include "commands.h"
#include <sys/types.h>
#include <sys/stat.h>

int pc = 0;     // индекс команды (не смещение)
bool halted = false;
int mem[1000];  // размер не важен: всё равно пока не пользуемся

typedef int (*op)(int src1, int src2, int dest, int imm); // все возможные входные значения

const char* fdata = NULL; // весь прочитанный п-код

extern op ops[]; // объявлен ниже

int main(int argc, char** argv) 
{

    if(argc!=2) 
    {
        printf("Missing pcode file name.\n");
        exit(1);
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd<0) 
    {
        printf("Cannot open pcode file.\n");
        exit(1);
    }
    struct stat finfo;
    fstat(fd, &finfo);
    fdata = (const char*)mmap(0, finfo.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (!fdata) 
    {
        printf("Cannot read pcode file.\n");
        exit(1);
    }

    const command* pcode = (const command*) fdata;

    int r[256] = {0}; // registers

    while (!halted) 
    {
        command next = pcode[pc++];
        r[next.dest] = ops[next.opcode](r[next.src1], r[next.src2], r[next.dest], next.imm);
    }

    munmap((void*)fdata, finfo.st_size);
    close(fd);
    return 0;
}

int hlt(int src1, int src2, int dest, int imm) { halted = true; return dest; }
int store(int src1, int src2, int dest, int imm) { mem[imm] = dest; return dest; }
int jz(int src1, int src2, int dest, int imm) { if (!dest) pc+=imm; return dest; }
int echo(int src1, int src2, int dest, int imm) { if (imm) printf("%s", fdata+imm); else printf("%d", dest); return dest; }
int mov(int src1, int src2, int dest, int imm) { return imm; }
int load(int src1, int src2, int dest, int imm) { return mem[imm]; }
int input(int src1, int src2, int dest, int imm) { int d; scanf(" %d", &d); return d; }
int add(int src1, int src2, int dest, int imm) { return src1+src2; }
int sub(int src1, int src2, int dest, int imm) { return src1-src2; }
int mul(int src1, int src2, int dest, int imm) { return src1*src2; }
int div(int src1, int src2, int dest, int imm) { return src1/src2; }
int eq(int src1, int src2, int dest, int imm) { return src1==src2; }
int ne(int src1, int src2, int dest, int imm) { return src1!=src2; }
int ge(int src1, int src2, int dest, int imm) { return src1>=src2; }
int le(int src1, int src2, int dest, int imm) { return src1<=src2; }
int gt(int src1, int src2, int dest, int imm) { return src1>src2; }
int lt(int src1, int src2, int dest, int imm) { return src1<src2; }

op ops[] = {hlt, store, jz, echo, mov, load, input, add, sub, mul, div, eq, ne, ge, le, gt, lt};