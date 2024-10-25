#include <algorithm>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <new>
#include <vector>

// some constants
#define MAX_BINS 7
#define MIN_BINS 3
#define GROWTH 8

class CachingAllocator {
public:
  CachingAllocator(bool debug = false) : debug_(debug) {
    initialiseBins();
    precalculateBinSizes();
  };

  std::byte* allocate(std::size_t size) {
    if (debug_)
      printf("Asking for %lld Bytes\n", static_cast<long long>(size));

    auto it = std::find_if(bin_sizes.begin(), bin_sizes.end(), [size](auto s){ return size < s;});
    if (it != bin_sizes.end()) [[likely]] {
      const auto bin_index = std::distance(bin_sizes.begin(), it);
      if (cached_blocks[bin_index].empty()) {
        // allocate a new block
        used_blocks[bin_index].emplace_back(*it);
        if (debug_)
          printf("Allocated a new block of %lld Bytes at %p\n", static_cast<long long>(*it), used_blocks[bin_index].back().ptr);
        return used_blocks[bin_index].back().ptr;
      } else {
        // take that block and use it
        used_blocks[bin_index].push_back(cached_blocks[bin_index].back());
        cached_blocks[bin_index].pop_back();
        if (debug_)
          printf("Reused a previously cached block of %lld Bytes at %p\n", static_cast<long long>(*it), used_blocks[bin_index].back().ptr);
        return used_blocks[bin_index].back().ptr;
      }
    } else {
      std::cerr << "ERROR: Size " << size << " is too large to be allocated.\n";
      throw std::bad_alloc{};
    }
  }

  void deallocate(void *ptr) {
    for ( int i = 0; i < used_blocks.size(); ++i) {
      auto it = std::find_if(used_blocks[i].begin(), used_blocks[i].end(), [ptr](auto mb){ return mb.ptr == ptr; });
      if (it != used_blocks[i].end()) {
        if (debug_)
          printf("Freed a block of %lld Bytes at %p\n", static_cast<long long>(it->size), it->ptr);
        cached_blocks[i].push_back(*it);
        used_blocks[i].erase(it);
        return;
      }
    }
    std::cerr << "ERROR: Ptr not found in the allocator.\n";
  }

  // Function to release all allocated memory
  void free() {
    for ( int i = 0; i < used_blocks.size(); ++i) {
      if (not used_blocks[i].empty()) {
        std::cerr << "ERROR: There are blocks still in use.\n";
        return;
      }
    }
    for ( int i = 0; i < cached_blocks.size(); ++i) {
      for (const auto& block : cached_blocks[i]) {
        if (debug_)
          printf("Released a block of %lld Bytes at %p\n", static_cast<long long>(block.size), block.ptr);
        std::free(block.ptr);
      }
    }
  }

private:
  struct MemoryBlock { // this class is used to represent blocks we are going to
                       // allocate
    MemoryBlock() = delete; // I remove the default constructor because I want
                            // to create a block only when I know the size
    MemoryBlock(std::size_t allocSize) {
      ptr = (std::byte *)std::aligned_alloc(std::size_t(64), allocSize);
      size = allocSize;
    }

    bool operator== (const MemoryBlock& other) {
      if (ptr == other.ptr and size == other.size)
        return true;
      return false;
    }

    std::byte *ptr;
    std::size_t size;
  };

  bool debug_;

  std::vector<std::vector<MemoryBlock>> used_blocks;   // currently in used
  std::vector<std::vector<MemoryBlock>> cached_blocks; // currently cached
  std::vector<std::size_t> bin_sizes;

  void initialiseBins() {
    used_blocks.resize(MAX_BINS);
    cached_blocks.resize(MAX_BINS);
  }

  void precalculateBinSizes() {
    bin_sizes.reserve(MAX_BINS);
    for (std::size_t i = 0; i <= MAX_BINS; ++i) {
      bin_sizes.push_back(static_cast<std::size_t>(std::pow(GROWTH, i)));
    }
  }
};
