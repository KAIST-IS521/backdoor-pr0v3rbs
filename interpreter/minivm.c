//-----------------------------------------------------------------------------
// minivm.c -- The basic definitions for the vm.
// Copyright (C) Philip Conrad 4/30/2013 @ 3:56 PM -- MIT License
//
//-----------------------------------------------------------------------------

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "minivm.h"

//---------------------------------------------------------
// FUNCTION IMPLEMENTATIONS:
void haltFunction(struct VMContext* ctx, __attribute__((unused)) const uint32_t instr)
{
    ctx->is_running = false;
}

void loadFunction(struct VMContext* ctx, const uint32_t instr)
{
    const uint8_t r0 = EXTRACT_B1(instr);
    const uint8_t r1 = EXTRACT_B2(instr);
    ctx->r[r0].value = 0;
    ctx->r[r0].value = *(uint8_t*)(ctx->heap + ctx->r[r1].value);
}

void storeFunction(struct VMContext* ctx, const uint32_t instr)
{
    const uint8_t r0 = EXTRACT_B1(instr);
    const uint8_t r1 = EXTRACT_B2(instr);
    *(uint8_t*)(ctx->heap + ctx->r[r0].value) = EXTRACT_B0(ctx->r[r1].value);
}


void moveFunction(struct VMContext* ctx, const uint32_t instr)
{
    const uint8_t r0 = EXTRACT_B1(instr);
    const uint8_t r1 = EXTRACT_B2(instr);
    ctx->r[r0].value = ctx->r[r1].value;
}

void putiFunction(struct VMContext* ctx, const uint32_t instr)
{
    const uint8_t r0 = EXTRACT_B1(instr);
    const uint8_t imm = EXTRACT_B2(instr);
    ctx->r[r0].value = (uint32_t)imm;
}

void addFunction(struct VMContext* ctx, const uint32_t instr)
{
    const uint8_t r0 = EXTRACT_B1(instr);
    const uint8_t r1 = EXTRACT_B2(instr);
    const uint8_t r2 = EXTRACT_B3(instr);
    ctx->r[r0].value = ctx->r[r1].value + ctx->r[r2].value;
}

void subFunction(struct VMContext* ctx, const uint32_t instr)
{
    const uint8_t r0 = EXTRACT_B1(instr);
    const uint8_t r1 = EXTRACT_B2(instr);
    const uint8_t r2 = EXTRACT_B3(instr);
    ctx->r[r0].value = ctx->r[r1].value - ctx->r[r2].value;
}

void gtFunction(struct VMContext* ctx, const uint32_t instr)
{
    const uint8_t r0 = EXTRACT_B1(instr);
    const uint8_t r1 = EXTRACT_B2(instr);
    const uint8_t r2 = EXTRACT_B3(instr);
    if (ctx->r[r1].value > ctx->r[r2].value)
        ctx->r[r0].value = 1;
    else
        ctx->r[r0].value = 0;
}

void geFunction(struct VMContext* ctx, const uint32_t instr)
{
    const uint8_t r0 = EXTRACT_B1(instr);
    const uint8_t r1 = EXTRACT_B2(instr);
    const uint8_t r2 = EXTRACT_B3(instr);
    if (ctx->r[r1].value >= ctx->r[r2].value)
        ctx->r[r0].value = 1;
    else
        ctx->r[r0].value = 0;
}

void eqFunction(struct VMContext* ctx, const uint32_t instr)
{
    const uint8_t r0 = EXTRACT_B1(instr);
    const uint8_t r1 = EXTRACT_B2(instr);
    const uint8_t r2 = EXTRACT_B3(instr);
    if (ctx->r[r1].value == ctx->r[r2].value)
        ctx->r[r0].value = 1;
    else
        ctx->r[r0].value = 0;
}

void iteFunction(struct VMContext* ctx, const uint32_t instr)
{
    const uint8_t r0 = EXTRACT_B1(instr);

    if (ctx->r[r0].value > 0)
    {
        ctx->jmp_flag = true;
        ctx->jmp_value = EXTRACT_B2(instr);
    }
    else if (ctx->r[r0].value == 0)
    {
        ctx->jmp_flag = true;
        ctx->jmp_value = EXTRACT_B3(instr);
    }
}

void jumpFunction(struct VMContext* ctx, const uint32_t instr)
{
    ctx->jmp_flag = true;
    ctx->jmp_value = EXTRACT_B1(instr);
}

void putsFunction(struct VMContext* ctx, const uint32_t instr)
{
    const uint8_t r0 = EXTRACT_B1(instr);
    printf("%s", (char*)(ctx->heap + ctx->r[r0].value));
}

void getsFunction(struct VMContext* ctx, const uint32_t instr)
{
    const uint8_t r0 = EXTRACT_B1(instr);
    char* str = (char*)(ctx->heap + ctx->r[r0].value);
    while (true)
    {
        *str = getc(stdin);
        if (*str == '\n')
        {
            *(str) = '\0';
            break;
        }
        str++;
    }
}


// Defers decoding of register args to the called function.
// dispatch :: VMContext -> uint32_t -> Effect()
bool dispatch(struct VMContext* ctx, const uint32_t instr) {
    const uint8_t i = EXTRACT_B0(instr);
    if ((*ctx->funtable[i]) == NULL)
    {
        puts("\n*****error*****");
	printf("unknown opcode number(0x%x) ", i);
        return false;
    }
    (*ctx->funtable[i])(ctx, instr);
    return true;
}


// Initializes a VMContext in-place.
// initVMContext :: VMContext -> uint32_t -> uint32_t -> [Reg] -> [FunPtr] -> Effect()
void initVMContext(struct VMContext* ctx, const uint32_t numRegs, const uint32_t numFuns, Reg* registers, FunPtr* funtable) {
    ctx->numRegs    = numRegs;
    ctx->numFuns    = numFuns;
    ctx->r          = registers;
    ctx->funtable   = funtable;
    ctx->instr_num  = 0;
    ctx->code       = NULL;
    ctx->is_running = true;
    ctx->jmp_flag   = false;
    ctx->jmp_value  = 0;
    ctx->heap       = malloc(8192);
}


// Reads an instruction, executes it, then steps to the next instruction.
// stepVMContext :: VMContext -> uint32_t** -> Effect()
void stepVMContext(struct VMContext* ctx, uint32_t** pc) {
    // Read a 32-bit bytecode instruction.
    uint32_t instr = **pc;

    // Dispatch to an opcode-handler.
    if (dispatch(ctx, instr) == false)
    {
        printf("on %ld instruction\n", *pc - ctx->code);
        exit(1);
    }

    // Increment to next instruction.
    (*pc)++;
}

