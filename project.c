// project.c
// Team:
// * Jonah Henriksson
// * Asier Arambarri
// * Jessica Solanki

#include "spimcore.h"

#define MEMSIZE (65536 >> 2)
#define REGSIZE 32
#define BUFSIZE 256

/* ALU */
/* 10 Points */
void ALU(unsigned A,unsigned B,char ALUControl,unsigned *ALUresult,char *Zero)
{
    switch (ALUControl) {
        case 0b000: *ALUresult = A + B; break;
        case 0b001: *ALUresult = A - B; break;
        case 0b010: *ALUresult = (A > B) ? 1 : 0; break; // Negative numbers are "greater" than positive
        case 0b011: *ALUresult = (A < B) ? 1 : 0; break;
        case 0b100: *ALUresult = A & B; break;
        case 0b101: *ALUresult = A | B; break;
        case 0b110: *ALUresult = B << 16; break;
        case 0b111: *ALUresult = ~A; break;
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
    if (PC >> 2 > MEMSIZE - 1) { // check if PC is in range
        return 1;
    }

    *instruction = MEM(PC);

    if (*instruction == 0x0 || *instruction > 0xC0000000) { // check if instruction is in range
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
    memset(controls, 0, sizeof(struct_controls)); // Prevents data corruption from previous cycles

    switch (op) {
        case 0b000000: // R-format *
            controls->RegDst = 1;
            controls->ALUOp = 0b111;
            controls->RegWrite = 1;
            break;
        case 0b000010: // jump *
            controls->Jump = 1;
            break;
        case 0b000100: // branch eq *
            controls->Branch = 1;
            controls->ALUOp = 0b001;
            controls->ALUSrc = 2;
            break;
        case 0b001000: // addi *
            controls->ALUOp = 0b000;
            controls->RegWrite = 1;
            controls->ALUSrc = 1;
            break;
        case 0b001010: // set less than immediate *
            controls->ALUOp = 0b010;
            controls->RegWrite = 1;
            controls->ALUSrc = 1;
            break;
        case 0b001011: // sltiu *
            controls->ALUOp = 0b011;
            controls->RegWrite = 1;
            controls->ALUSrc = 1;
            break;
        case 0b001111: // load upper immediately *
            controls->ALUOp = 0b110;
            controls->RegWrite = 1;
            controls->ALUSrc = 1;
            break;
        case 0b100011: // load word *
            controls->ALUOp = 0b000; // we have to add the offset to the original location
            controls->MemRead = 1;
            controls->MemtoReg = 1;
            controls->ALUSrc = 1;
            break;
        case 0b101011: // store word *
            controls->ALUOp = 0b000;
            controls->MemWrite = 1;
            controls->ALUSrc = 1;
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
    *data1 = Reg[r1];
    *data2 = Reg[r2];
}


/* Sign Extend */
/* 10 Points */
void sign_extend(unsigned offset,unsigned *extended_value)
{
    *extended_value = offset;

    if ((offset & 0x8000) == 0x8000) {
        *extended_value |= 0xFFFF0000;
    }
}

/* ALU operations */
/* 10 Points */
int ALU_operations(unsigned data1,unsigned data2,unsigned extended_value,unsigned funct,char ALUOp,char ALUSrc,unsigned *ALUresult,char *Zero)
{
    if (ALUSrc == 1) {
        ALU(data1, extended_value, ALUOp, ALUresult, Zero);
    } else if (ALUSrc == 2) {
        ALU(data1, data2, ALUOp, ALUresult, Zero);
    } else if (ALUOp == 0b111) {
        switch (funct) {
            case 0b100000: // add
                ALU(data1, data2, 0b000, ALUresult, Zero);
                break;
            case 0b100010: // subtract
                ALU(data1, data2, 0b001, ALUresult, Zero);
                break;
            case 0b101010: // less than
                ALU(data1, data2, 0b010, ALUresult, Zero);
                break;
            case 0b101011: // less than unsigned
                ALU(data1, data2, 0b011, ALUresult, Zero);
                break;
            case 0b100100: // and
                ALU(data1, data2, 0b100, ALUresult, Zero);
                break;
            case 0b100101: // or
                ALU(data1, data2, 0b101, ALUresult, Zero);
                break;
            default:
                return 1;
        }
    } else if (ALUOp != 0b000) {
        return 1;
    }

    return 0;
}

/* Read / Write Memory */
/* 10 Points */
int rw_memory(unsigned ALUresult,unsigned data2,char MemWrite,char MemRead,unsigned *memdata,unsigned *Mem)
{
    // if reading/writing, make sure addr is in range and is word-aligned
    if ((MemRead == 1 || MemWrite == 1) && (((ALUresult >> 2) > (MEMSIZE - 1)) || (ALUresult % 4 != 0))) {
        return 1;
    }

    if (MemRead == 1) {
        *memdata = MEM(ALUresult);
    }

    if (MemWrite == 1) {
        MEM(ALUresult) = data2;
    }

    return 0;
}


/* Write Register */
/* 10 Points */
void write_register(unsigned r2,unsigned r3,unsigned memdata,unsigned ALUresult,char RegWrite,char RegDst,char MemtoReg,unsigned *Reg)
{
    if (RegWrite == 1) {
        if (RegDst == 1) {
            Reg[r3] = ALUresult;
        } else {
            Reg[r2] = ALUresult;
        }
    }

    if (MemtoReg == 1) {
        if (RegDst == 1) {
            Reg[r3] = memdata;
        } else {
            Reg[r2] = memdata;
        }
    }
}

/* PC update */
/* 10 Points */
void PC_update(unsigned jsec,unsigned extended_value,char Branch,char Jump,char Zero,unsigned *PC)
{
    if (Jump == 1) {
        unsigned addr = (jsec << 2);
        addr &= 0x0FFFFFFF;
        addr |= (*PC & 0xF0000000);
        *PC = addr;
    } else if (Branch == 1) {
        if (Zero == 1) {
            *PC = *PC + 4 + (extended_value << 2);
        }
    } else {
        *PC += 4;
    }
}
