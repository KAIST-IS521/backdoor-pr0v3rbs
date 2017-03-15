//-----------------------------------------------------------------------------
// minivm.c -- The basic definitions for the vm.
// Copyright (C) Philip Conrad 4/30/2013 @ 3:56 PM -- MIT License
//
//-----------------------------------------------------------------------------

#include <stdbool.h>
#include <stdlib.h>
#include "minivm.h"

bool g_ite_flag = false;
uint8_t g_ite_value = 0;

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
    const uint8_t imm0 = EXTRACT_B2(instr);
    const uint8_t imm1 = EXTRACT_B3(instr);

    if (ctx->r[r0].value > 0)
    {
        g_ite_flag = true;
        g_ite_value = imm0;
    }
    else if (ctx->r[r0].value == 0)
    {
        g_ite_flag = true;
        g_ite_value = imm1;
    }
}

// Defers decoding of register args to the called function.
// dispatch :: VMContext -> uint32_t -> Effect()
void dispatch(struct VMContext* ctx, const uint32_t instr) {
    const uint8_t i = EXTRACT_B0(instr);
    (*ctx->funtable[i])(ctx, instr);
}


// Initializes a VMContext in-place.
// initVMContext :: VMContext -> uint32_t -> uint32_t -> [Reg] -> [FunPtr] -> Effect()
void initVMContext(struct VMContext* ctx, const uint32_t numRegs, const uint32_t numFuns, Reg* registers, FunPtr* funtable) {
    ctx->numRegs    = numRegs;
    ctx->numFuns    = numFuns;
    ctx->r          = registers;
    ctx->funtable   = funtable;
    ctx->is_running = true;
    ctx->heap       = NULL;
}


// Reads an instruction, executes it, then steps to the next instruction.
// stepVMContext :: VMContext -> uint32_t** -> Effect()
void stepVMContext(struct VMContext* ctx, uint32_t** pc) {
    // Read a 32-bit bytecode instruction.
    uint32_t instr = **pc;

    // Dispatch to an opcode-handler.
    if (g_ite_flag)
    {
        *(uint8_t*)(&instr) = g_ite_value;
        g_ite_flag = false;
    }
    dispatch(ctx, instr);

    // Increment to next instruction.
    (*pc)++;
}

