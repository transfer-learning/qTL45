//
// Created by codetector on 11/23/19.
//

#ifndef TL45_EMU_TL45_EMU_CORE_H
#define TL45_EMU_TL45_EMU_CORE_H

typedef struct {
    uint8_t zf:1;
    uint8_t cf:1;
    uint8_t sf:1;
    uint8_t of:1;
} tl45_flags;

typedef struct {
    uint32_t registers[15];
    uint32_t pc;
    tl45_flags flags;
    uint8_t *memory;
} tl45_state;

void tl45_tick(tl45_state *state);
#endif //TL45_EMU_TL45_EMU_CORE_H
