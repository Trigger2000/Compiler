#pragma once

typedef unsigned char regnum;

struct command 
{
    enum opcodes 
    {
        hlt,
        store, // dst>
        jz,    // dst>
        echo,  // dst>
        mov,   // >dst
        load,  // >dst
        input, // >dst
        add,   // src>dst
        sub,   // src>dst
        mul,   // src>dst
        div,   // src>dst
        eq,    // src>dst
        ne,    // src>dst
        ge,    // src>dst
        le,    // src>dst
        gt,    // src>dst
        lt     // src>dst
    };

    opcodes opcode;
    regnum dest;
    union 
    {
        struct 
        {
            regnum src1, src2;
        };
        short imm;
    };

    command(opcodes opcode, regnum dest, regnum src1, regnum src2) :
                opcode(opcode), dest(dest), src1(src1), src2(src2) {}

    command(opcodes opcode, regnum dest, short imm) :
                opcode(opcode), dest(dest), imm(imm) {}
};