//-----------------------------------------------------------------------------
// minivm.c -- The basic definitions for the vm.
// Copyright (C) Philip Conrad 4/30/2013 @ 3:56 PM -- MIT License
//
//-----------------------------------------------------------------------------

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
    dispatch(ctx, instr);

    // Increment to next instruction.
    (*pc)++;
}

