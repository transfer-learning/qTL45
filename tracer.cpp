#include <vector>
#include <cstdio>
#include <algorithm>

#include <emulator/tl45/TL45EmulatorState.h>

#pragma clang diagnostic push
#pragma ide diagnostic ignored "hicpp-signed-bitwise"

int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf("usage: %s <program.bin> <input file>", argv[0]);
    return 1;
  }
  TL45EmulatorState state;
	state.clear(); // allocate virtual memory
  state.load(0, argv[1]);
  state.load(INPUT_TAINT_START, argv[2]);
  state.run();

  // display profiling info
  std::vector<TL45::branch> branches;
  auto& profile = state.state.profile;
  for (auto it = profile.branch_count.begin(); it != profile.branch_count.end(); ++it) {
    branches.push_back(it->first);
  }
  std::sort(branches.begin(), branches.end(), [&profile](const auto& a, const auto& b) -> bool { 
    return profile.branch_count[a] < profile.branch_count[b]; // we want sort in descending order
  });
  printf("top branch report:\n");
  for (int i = 0; i < 200 && i < branches.size(); i++) {
    auto& taint_set = profile.branch_taint[branches[i]];
    if (taint_set.size() == 0)
      continue;
    printf("%08x:%c | %08x hits, tainted by: ", branches[i].first, branches[i].second ? 't': 'f', profile.branch_count[branches[i]]);
    for (uint32_t x : taint_set) {
      printf("%u ", x);
    }
    printf("\n");
  }
  return 0;
}

#pragma clang diagnostic pop
