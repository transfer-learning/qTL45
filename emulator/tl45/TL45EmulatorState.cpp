//
// Created by codetector on 12/3/19.
//

#include <stdexcept>
#include "TL45EmulatorState.h"
#include "tl45_isa.h"
#include <sys/mman.h>
#include <gmpxx.h>
#include <sys/stat.h>

uint16_t TL45EmulatorState::getRegisterCount() {
  return 16;
}

std::string TL45EmulatorState::getRegisterName(uint16_t regID) {
  if (regID < 16) {
    return "r" + std::to_string(regID);
  }
  return "UNKNOWN";
}

uint16_t TL45EmulatorState::getRegisterBitSize(uint16_t regID) {
  return 32;
}

bool TL45EmulatorState::hasRegisterAlias(uint16_t regID) {
  return false;
}

uint64_t TL45EmulatorState::getRegisterValue(uint64_t regID) {
  return state.read_reg(regID);
}

uint64_t TL45EmulatorState::getProgramCounterValue() {
  return state.pc;
}

unsigned int TL45EmulatorState::getMemoryAddrWidth() {
  return 32;
}

unsigned int TL45EmulatorState::getMemoryDataWidth() {
  return 8;
}

MemoryMapping TL45EmulatorState::getMemoryMapping(uint64_t addr) {
  return MemoryMapping::STANDARD_MEMORY;
}

uint64_t TL45EmulatorState::getMemoryValue(uint64_t addr) {
  return state.memory[addr];
}

void TL45EmulatorState::setMemoryValue(uint64_t addr, uint64_t data) {
  throw std::runtime_error("not implemented");
}

std::string TL45EmulatorState::getMemoryDisassembly(uint64_t &addr) {
  if (addr % 4 == 0) {
    return TL45::disassemble(state.read_word(addr));
  }
  return "";
}

void TL45EmulatorState::step() {
  TL45::tick(&state);
}

void TL45EmulatorState::clear() {
  munmap(this->state.memory, std::numeric_limits<uint32_t>::max());
  state.memory = (uint8_t *) mmap(nullptr, std::numeric_limits<uint32_t>::max(),
                                        PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE | MAP_NORESERVE, 0, 0);
  for (int i = 0; i < 15; ++i) {
    state.registers[i] = 0;
  }
  state.pc = 0;
  state.flags.cf = 0;
  state.flags.zf = 0;
  state.flags.of = 0;
  state.flags.sf = 0;
}

int TL45EmulatorState::load(std::string fileName) {
  munmap(this->state.memory, std::numeric_limits<uint32_t>::max());
  state.memory = (uint8_t *) mmap(nullptr, std::numeric_limits<uint32_t>::max(),
                                  PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE | MAP_NORESERVE, 0, 0);
  FILE *f = fopen(fileName.c_str(), "r");
  if (!f) {
    return -1;
  }
  size_t bytesLoaded = fread(state.memory, 1, std::numeric_limits<uint32_t>::max(), f);
  printf("%zul bytes loaded.\n", bytesLoaded);
  return bytesLoaded > 0 ? 0 : -1;
}

