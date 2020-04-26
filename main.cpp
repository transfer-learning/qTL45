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

	if (!state.getRawMemoryPtr()) {
		return EXCEPTION_CONTINUE_SEARCH;
	}

  LPVOID FaultAddress = (LPVOID) ExceptionInfo->ExceptionRecord->ExceptionInformation[1];
  // printf("fault at %p\n", FaultAddress);
	LPVOID memory_start = state.getRawMemoryPtr();
	LPVOID memory_end = (LPVOID)((uintptr_t)memory_start + std::numeric_limits<uint32_t>::max());
  if (FaultAddress >= memory_start && FaultAddress <= memory_end) {
    // printf("committing %p\n", FaultAddress);
    VirtualAlloc(FaultAddress, 1, MEM_COMMIT, PAGE_READWRITE);
    return EXCEPTION_CONTINUE_EXECUTION;
  }

  return EXCEPTION_CONTINUE_SEARCH;
}
#endif

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);

	state.clear(); // allocate virtual memory
#ifdef _WIN32
  AddVectoredExceptionHandler(FALSE, PageFaultExceptionFilter);
#endif
  TLEmulator w(&state);
  w.show();
  return a.exec();
}

#pragma clang diagnostic pop
