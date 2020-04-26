#include "tl45emu.h"

#include <QApplication>
#include <emulator/tl45/TL45EmulatorState.h>
#include <emulator/scomp/SCOMPEmulator.h>

#ifdef _WIN32
#include <Windows.h>
#undef max
#else
#include <sys/mman.h>
#include <sys/stat.h>
#endif

#pragma clang diagnostic push
#pragma ide diagnostic ignored "hicpp-signed-bitwise"

TL45EmulatorState state{};

#ifdef _WIN32
// Lazy paging
LONG WINAPI PageFaultExceptionFilter(EXCEPTION_POINTERS *ExceptionInfo) {
  if (ExceptionInfo->ExceptionRecord->ExceptionCode != EXCEPTION_ACCESS_VIOLATION) {
    return EXCEPTION_CONTINUE_SEARCH;
  }

  LPVOID FaultAddress = (LPVOID) ExceptionInfo->ExceptionRecord->ExceptionInformation[1];
  // printf("fault at %p\n", FaultAddress);
  if (FaultAddress >= state.state.memory
    && FaultAddress <= (LPVOID) ((uintptr_t) state.state.memory + std::numeric_limits<uint32_t>::max())) {
    // printf("committing %p\n", FaultAddress);
    VirtualAlloc(FaultAddress, 1, MEM_COMMIT, PAGE_READWRITE);
    return EXCEPTION_CONTINUE_EXECUTION;
  }

  return EXCEPTION_CONTINUE_SEARCH;
}
#endif

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);

#ifdef _WIN32
  AddVectoredExceptionHandler(FALSE, PageFaultExceptionFilter);
  state.state.memory = (uint8_t *) VirtualAlloc(nullptr, std::numeric_limits<uint32_t>::max(),
                                        MEM_RESERVE, PAGE_READWRITE);
  printf("mem at %p\n", state.state.memory);
#else
  state.state.memory = (uint8_t *) mmap(nullptr, std::numeric_limits<uint32_t>::max(),
                                        PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE | MAP_NORESERVE, 0, 0);
#endif
  if (!state.state.memory) {
    printf("Failed to allocate memory!\n");
    return 1;
  }
  TLEmulator w(&state);
  w.show();
  return a.exec();
}

#pragma clang diagnostic pop
