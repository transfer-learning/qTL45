//
// Created by codetector on 12/3/19.
//

#include <stdexcept>
#include "TL45EmulatorState.h"
#include "tl45_isa.h"
#ifdef _WIN32
#include <Windows.h>
#undef max
#else
#include <sys/mman.h>
#include <sys/stat.h>
#endif

uint16_t TL45EmulatorState::getRegisterCount() {
  return 16+4;
}

std::string TL45EmulatorState::getRegisterName(uint16_t regID) {
  if (regID < 16) {
    return "r" + std::to_string(regID);
  }
  switch (regID) {
    case 16:
      return "ZF";
    case 17:
      return "SF";
    case 18:
      return "OF";
    case 19:
      return "CF";
    default:
      break;
  }
  return "UNKNOWN";
}

uint16_t TL45EmulatorState::getRegisterBitSize(uint16_t regID) {
  if (regID >= 16) return 1;
  return 32;
}

bool TL45EmulatorState::hasRegisterAlias(uint16_t regID) {
  return false;
}

uint64_t TL45EmulatorState::getRegisterValue(uint64_t regID) {
  if (regID < 16)
    return state.read_reg(regID);
  switch (regID) {
    case 16:
      return state.flags.zf;
    case 17:
      return state.flags.sf;
    case 18:
      return state.flags.of;
    case 19:
      return state.flags.cf;
    default:
      break;
  }
  return 0;
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
#ifdef _WIN32
  VirtualFree(this->state.memory, 0, MEM_RELEASE);
  state.memory = (uint8_t *) VirtualAlloc(nullptr, std::numeric_limits<uint32_t>::max(),
                                        MEM_RESERVE, PAGE_READWRITE);
  printf("mem at %p\n", state.memory);
#else
  munmap(this->state.memory, std::numeric_limits<uint32_t>::max());
  state.memory = (uint8_t *) mmap(nullptr, std::numeric_limits<uint32_t>::max(),
                                        PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE | MAP_NORESERVE, 0, 0);
#endif
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
  this->clear();
  FILE *f = fopen(fileName.c_str(), "rb");
  if (!f) {
    return -1;
  }
  fseek(f, 0, SEEK_END);
  long fsize = ftell(f);
  fseek(f, 0, SEEK_SET);
#ifdef _WIN32
  // FUCK YOU WINDOWS!
  VirtualAlloc(state.memory, fsize, MEM_COMMIT, PAGE_READWRITE);
#endif
  size_t bytesLoaded = fread(state.memory, 1, fsize, f);
  fclose(f);
  printf("%zul bytes loaded.\n", bytesLoaded);
  return bytesLoaded > 0 ? 0 : -1;
}

