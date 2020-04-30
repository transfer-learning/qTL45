//
// Created by codetector on 12/3/19.
//

#include <cstring>
#include <limits>
#include <stdexcept>
#include <mutex>
#include "TL45EmulatorState.h"
#include "tl45_isa.h"
#ifdef _WIN32
#include <Windows.h>
#undef max
#else
#include <sys/mman.h>
#include <sys/stat.h>
#endif

static std::mutex g_mutex; // we lock on anything that may affect memory.
static TL45EmulatorState* g_pState;

#ifdef _WIN32
// Lazy paging
LONG WINAPI PageFaultExceptionFilter(EXCEPTION_POINTERS *ExceptionInfo) {
  if (ExceptionInfo->ExceptionRecord->ExceptionCode != EXCEPTION_ACCESS_VIOLATION) {
    return EXCEPTION_CONTINUE_SEARCH;
  }

  // the faulting code should have g_mutex locked!
  if (!g_pState || !g_pState->getRawMemoryPtr()) {
    return EXCEPTION_CONTINUE_SEARCH;
  }

  LPVOID FaultAddress = (LPVOID) ExceptionInfo->ExceptionRecord->ExceptionInformation[1];
  // printf("fault at %p\n", FaultAddress);
  LPVOID memory_start = g_pState->getRawMemoryPtr();
  LPVOID memory_end = (LPVOID)((uintptr_t)memory_start + std::numeric_limits<uint32_t>::max());
  if (FaultAddress >= memory_start && FaultAddress <= memory_end) {
    if (g_mutex.try_lock()) {
      printf("faulting instruction: %p\n", (void*) ExceptionInfo->ContextRecord->Rip);
      throw std::runtime_error("faulting code did not have the global state mutex locked");
    }
    // printf("committing %p\n", FaultAddress);
    VirtualAlloc(FaultAddress, 1, MEM_COMMIT, PAGE_READWRITE);
    return EXCEPTION_CONTINUE_EXECUTION;
  }

  return EXCEPTION_CONTINUE_SEARCH;
}
#endif

TL45EmulatorState::TL45EmulatorState() : state() {
  const std::unique_lock<std::mutex> lock(g_mutex);
  if (g_pState) {
    printf("TL45EmulatorState singleton already initialized");
    abort();
  }
  g_pState = this;
#ifdef _WIN32
  AddVectoredExceptionHandler(FALSE, PageFaultExceptionFilter);
#endif
}


uint16_t TL45EmulatorState::getRegisterCount() {
  return 16+5;
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
    case 20:
      return "PC";
    default:
      break;
  }
  return "UNKNOWN";
}

uint16_t TL45EmulatorState::getRegisterBitSize(uint16_t regID) {
  if (regID == 16 || regID == 17 || regID == 18 || regID == 19) return 1;
  if (regID == 20) return 32;
  return 32;
}

bool TL45EmulatorState::hasRegisterAlias(uint16_t regID) {
  return false;
}

uint64_t TL45EmulatorState::getRegisterValue(uint64_t regID) {
  if (regID < 16)
    return state.read_reg(regID).value;
  switch (regID) {
    case 16:
      return state.flags.zf;
    case 17:
      return state.flags.sf;
    case 18:
      return state.flags.of;
    case 19:
      return state.flags.cf;
    case 20:
      return state.pc;
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

void TL45EmulatorState::readMemory(uint64_t addr, size_t n, void* data) {
  const std::unique_lock<std::mutex> lock(g_mutex);
  memcpy(data, &state.memory[addr], n);
}

void TL45EmulatorState::writeMemory(uint64_t addr, size_t n, void* data) {
  const std::unique_lock<std::mutex> lock(g_mutex);
  memcpy(&state.memory[addr], data, n);
}

std::string TL45EmulatorState::getMemoryDisassembly(uint64_t &addr) {
  const std::unique_lock<std::mutex> lock(g_mutex);
  if (addr % 4 == 0) {
    return TL45::disassemble(state.read_word((uint32_t) addr).value);
  }
  return "";
}

void TL45EmulatorState::step() {
  const std::unique_lock<std::mutex> lock(g_mutex);
  TL45::tick(&state);
}

void TL45EmulatorState::run() {
  const std::unique_lock<std::mutex> lock(g_mutex);
  do {
    TL45::tick(&state);
  } while (state.fetch_instruction(state.pc) != 0xffffffff);
}

void TL45EmulatorState::clear() {
  const std::unique_lock<std::mutex> lock(g_mutex);
#ifdef _WIN32
  if (this->state.memory)
    VirtualFree(this->state.memory, 0, MEM_RELEASE);
  state.memory = (uint8_t *) VirtualAlloc(nullptr, std::numeric_limits<uint32_t>::max(),
                                        MEM_RESERVE, PAGE_READWRITE);
  printf("mem at %p\n", state.memory);
#else
  if (this->state.memory)
    munmap(this->state.memory, std::numeric_limits<uint32_t>::max());
  state.memory = (uint8_t *) mmap(nullptr, std::numeric_limits<uint32_t>::max(),
                                        PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE | MAP_NORESERVE, 0, 0);
#endif
  if (!state.memory) {
    printf("Failed to allocate memory!\n");
    exit(1);
  }
  for (int i = 0; i < 15; ++i) {
    state.registers[i] = 0;
  }
  state.pc = 0;
  state.flags.cf = 0;
  state.flags.zf = 0;
  state.flags.of = 0;
  state.flags.sf = 0;

  // clear taint analysis
  state.taint.memory.clear();
  for (int i = 0; i < 15; ++i) {
    state.taint.registers[i].clear();
  }
  state.taint.flags.clear();

  // clear profiling
  state.profile.branch_count.clear();
  state.profile.branch_taint.clear();
}

int TL45EmulatorState::load(uint64_t addr, std::string fileName) {
  const std::unique_lock<std::mutex> lock(g_mutex);
  FILE *f = fopen(fileName.c_str(), "rb");
  if (!f) {
    return -1;
  }
  fseek(f, 0, SEEK_END);
  long fsize = ftell(f);
  fseek(f, 0, SEEK_SET);
#ifdef _WIN32
  // SCREW YOU WINDOWS!
  VirtualAlloc(state.memory + addr, fsize, MEM_COMMIT, PAGE_READWRITE);
#endif
  size_t bytesLoaded = fread(&state.memory[addr], 1, fsize, f);
  fclose(f);
  printf("%zul bytes loaded.\n", bytesLoaded);
  return bytesLoaded > 0 ? 0 : -1;
}

void* TL45EmulatorState::getRawMemoryPtr() const {
  // THIS IS NOT LOCK SAFE!
  return state.memory;
}

