#include "stdint.h"
#include <cstdio>
#include "tl45_emu_core.h"

// Created by codetector on 11/22/19.
//
#define DEBUG

uint32_t fetch_instruction(const uint8_t *memory, uint32_t location) {
    uint32_t value = 0;
    for (int i = 0; i < 4; ++i) {
        value = (value << 8U) | memory[location + i];
    }
    return value;
}

struct u32_carry {
    uint32_t res;
    unsigned carry;
};

struct u32_carry add_carryout(uint32_t x, uint32_t y) {
    uint32_t retval = x + y;
    unsigned carry = (retval < x);
    return (struct u32_carry) {retval, carry};
}

void tl45_tick(tl45_state *state) {
    uint32_t instruction = fetch_instruction(state->memory, state->pc);
    uint8_t opcode = (instruction >> (32U - 5U)) & 0x1FU; // Extract Opcode
    uint8_t RI = instruction >> (32U - 6U) & 1U;
    uint8_t LH = instruction >> (32U - 7U) & 1U;
    uint8_t ZS = instruction >> (32U - 8U) & 1U;
    uint8_t DR = instruction >> (32U - 12U) & 0xFU;
    uint8_t SR1 = instruction >> (32U - 16U) & 0xFU;
    uint8_t SR2 = instruction >> (32U - 20U) & 0xFU;
    uint16_t IMM = instruction & 0xFFFFU;
#ifdef DEBUG
    printf("0x%08X => OP: %02X, DR: %X, SR1: %X, SR2: %X, IMM16: %04X\n", state->pc, opcode, DR, SR1, SR2, IMM);
#endif
    switch (opcode) {
        case 0x0:
            break; // NOP
        case 0x1: {
            uint32_t val2 = state->registers[SR2 - 1];
            if (RI) { // IMM Mode
                val2 = (uint32_t) (int32_t) IMM;
            }
//            u32_carry add_result = add_carryout();
        }
            break; // ADD
        case 0x2:
            break; // SUB
        case 0x3:
            break; // MUL
        case 0x6:
            break; // OR
        case 0x7:
            break; // XOR
        case 0x8:
            break; // AND
        case 0x9:
            break; // NOT
        case 0xa:
            break; // SHL
        case 0xb:
            break; // SHR
        case 0x5:
            break; // SHRA
        case 0xC:
            break; // JUMPs
        case 0xD:
            break; // CALL
        case 0xE:
            break; // RET
        case 0x10:
            break; // LHW
        case 0x11:
            break; // LHWSE
        case 0x16:
            break; // SHW
        case 0x0F:
            break; // LBSE
        case 0x12:
            break; // LB
        case 0x13:
            break; // SB
        case 0x14:
            break; // LW
        case 0x15:
            break; // SW
        case 0x17:
            break; // SDIV
        case 0x18:
            break; // UDIV
        default:
            break; // DECODE ERROR
    }
}