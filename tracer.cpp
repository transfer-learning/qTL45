#include <vector>
#include <unordered_set>
#include <cstdio>
#include <algorithm>

#include <emulator/tl45/TL45EmulatorState.h>

#pragma clang diagnostic push
#pragma ide diagnostic ignored "hicpp-signed-bitwise"

int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf("usage: %s <program.bin> <input file>\n", argv[0]);
    return 1;
  }
  TL45EmulatorState state;
	state.clear(); // allocate virtual memory
  state.load(0, argv[1]);
  int bytes_loaded = state.load(INPUT_TAINT_START, argv[2]);
  if (bytes_loaded < 0) {
    printf("failed to load input!\n");
    return 1;
  }
  uint32_t input_size = bswap_32((uint32_t) bytes_loaded);
  state.writeMemory(INPUT_LEN_ADDR, 4, &input_size);
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

  std::unordered_set<uint32_t> seen;
  std::vector<uint32_t> interesting;
  fprintf(stderr, "top branch report:\n");
  for (int i = 0; i < 200 && i < branches.size(); i++) {
    auto& taint_set = profile.branch_taint[branches[i]];
    if (taint_set.set.size() == 0)
      continue;
    fprintf(stderr, "%08x:%c | %08x hits, tainted by: ", branches[i].first, branches[i].second ? 't': 'f', profile.branch_count[branches[i]]);
    for (uint32_t x : taint_set.set) {
      if (seen.find(x) == seen.end()) {
        seen.insert(x);
        interesting.push_back(x);
      }
      fprintf(stderr, "%u ", x);
    }
    fprintf(stderr, "\n");
  }
  //printf("interest: ");
  for (uint32_t i : interesting) {
    printf("%d\n", i);
  }
  return 0;
}

#pragma clang diagnostic pop
