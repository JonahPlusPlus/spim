#include "spimcore.h"


/* ALU */
/* 10 Points */
void ALU(unsigned A,unsigned B,char ALUControl,unsigned *ALUresult,char *Zero)
{
    switch (ALUControl) {
        case 0b000: *ALUresult = A + B; break;
        case 0b001: *ALUresult = A - B; break;
        case 0b010: *ALUresult = ((signed)A < (signed)B) ? 1 : 0; break;
        case 0b011: *ALUresult = (A < B) ? 1 : 0; break;
        case 0b100: *ALUresult = A & B; break;
        case 0b101: *ALUresult = A | B; break;
        case 0b110: *ALUresult = B << 16; break;
        case 0b111: *ALUresult = !A; break;
        default: break;
    }

    if (*ALUresult == 0) {
        *Zero = 1;
    } else {
        *Zero = 0;
    }
}

/* instruction fetch */
/* 10 Points */
int instruction_fetch(unsigned PC,unsigned *Mem,unsigned *instruction)
{
    *instruction = MEM(PC);

    if (*instruction == 0) {
        return 1;
    }

    return 0;
}


/* instruction partition */
/* 10 Points */
void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1,unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec)
{
    *op = instruction >> 26;
    *r1 = (instruction >> 21) & 0x1F;
    *r2 = (instruction >> 16) & 0x1F;
    *r3 = (instruction >> 11) & 0x1F;
    *funct = instruction & 0x3F;
    *offset = instruction & 0xFFFF;
    *jsec = instruction & 0x3FFFFFF;
}



/* instruction decode */
/* 15 Points */
int instruction_decode(unsigned op,struct_controls *controls)
{
    memset(controls, 0, sizeof(struct_controls));

    switch (op) {
        case 0b000000: // R-format
            controls->RegDst = 1;
            controls->ALUOp = 7;
            break;
        case 0b000001: // bltz/gez

            break;
        case 0b000010: // jump
            controls->Jump = 1;
            break;
        case 0b000100: // branch eq
            controls->Branch = 1;
            break;
        case 0b001000: // addi

            break;
        case 0b001001: // addiu

            break;
        case 0b001010: // set less than immediate

            break;
        case 0b001011: // sltiu

            break;
        case 0b001100: // andi
            controls->ALUOp = 0b100;
            break;
        case 0b001101: // ori

            break;
        case 0b001110: // xori

            break;
        case 0b001111: // load upper immediately

            break;
        case 0b010000: // TLB

            break;
        case 0b010001: // FlPt

            break;
        case 0b100000: // load byte

            break;
        case 0b100001: // load half

            break;
        case 0b100010: // lwl

            break;
        case 0b100011: // load word

            break;
        case 0b100100: // lbu

            break;
        case 0b100101: // lhu

            break;
        case 0b100110: // lwr

            break;
        case 0b101000: // store byte

            break;
        case 0b101001: // store half

            break;
        case 0b101010: // swl

            break;
        case 0b101011: // store word

            break;
        case 0b101110: // swr

            break;
        case 0b110000: // lwc0

            break;
        case 0b110001: // lwc1

            break;
        case 0b111000: // swc0

            break;
        case 0b111001: // swc1

            break;
        default:
            return 1;
    }

    return 0;
}

/* Read Register */
/* 5 Points */
void read_register(unsigned r1,unsigned r2,unsigned *Reg,unsigned *data1,unsigned *data2)
{

}


/* Sign Extend */
/* 10 Points */
void sign_extend(unsigned offset,unsigned *extended_value)
{

}

/* ALU operations */
/* 10 Points */
int ALU_operations(unsigned data1,unsigned data2,unsigned extended_value,unsigned funct,char ALUOp,char ALUSrc,unsigned *ALUresult,char *Zero)
{

}

/* Read / Write Memory */
/* 10 Points */
int rw_memory(unsigned ALUresult,unsigned data2,char MemWrite,char MemRead,unsigned *memdata,unsigned *Mem)
{

}


/* Write Register */
/* 10 Points */
void write_register(unsigned r2,unsigned r3,unsigned memdata,unsigned ALUresult,char RegWrite,char RegDst,char MemtoReg,unsigned *Reg)
{

}

/* PC update */
/* 10 Points */
void PC_update(unsigned jsec,unsigned extended_value,char Branch,char Jump,char Zero,unsigned *PC)
{

}

