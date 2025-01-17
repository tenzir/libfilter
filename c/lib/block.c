#include "filter/block.h"

#include <string.h>           // for memset
#include "filter/memory.h"    // for libfilter_region
#include "memory-internal.h"  // for libfilter_region_alloc_result, libfilte...
#include "util-internal.h"    // for libfilter_block_bytes_needed_detail

double libfilter_block_fpp(double ndv, double bytes) {
  return libfilter_block_fpp_detail(ndv, bytes, 32, 8, 32);
}

uint64_t libfilter_block_capacity(uint64_t bytes, double fpp) {
  return libfilter_block_capacity_detail(bytes, fpp, 32, 8, 32);
}

uint64_t libfilter_block_bytes_needed(double ndv, double fpp) {
  return libfilter_block_bytes_needed_detail(ndv, fpp, 32, 8, 32);
}

__attribute__((visibility("hidden"))) int libfilter_block_calloc(uint64_t heap_space,
                                                                 uint64_t bucket_bytes,
                                                                 libfilter_block* here) {
  heap_space = (heap_space > bucket_bytes) ? heap_space : bucket_bytes;
  const libfilter_region_alloc_result allocated =
      libfilter_alloc_at_most(heap_space, bucket_bytes);
  if (0 == allocated.block_bytes) return -1;
  if (!allocated.zero_filled) memset(allocated.region.block, 0, allocated.block_bytes);
  here->num_buckets_ = allocated.block_bytes / bucket_bytes;
  here->block_ = allocated.region;
  return 0;
}

// TODO: intersection, union, serialize, deserialize

__attribute__((visibility("hidden"))) int libfilter_block_free(uint64_t bucket_bytes,
                                                               libfilter_block* here) {
  return libfilter_do_free(here->block_, here->num_buckets_ * bucket_bytes, bucket_bytes);
}

int libfilter_block_init(uint64_t heap_space, libfilter_block *here) {
  return libfilter_block_calloc(heap_space, (8 * 32 / CHAR_BIT), here);
}

int libfilter_block_destruct(libfilter_block *here) {
  return libfilter_block_free((8 * 32 / CHAR_BIT), here);
}

void libfilter_block_zero_out(libfilter_block* const here) {
  here->num_buckets_ = 0;
  libfilter_clear_region(&here->block_);
}

libfilter_block libfilter_block_clone(const libfilter_block * here) {
  uint64_t new_request = libfilter_new_alloc_request(here->num_buckets_ * 32, 32);
  libfilter_region_alloc_result r = libfilter_alloc_at_most(new_request, 32);
  // TODO: check for failure here!
  memcpy(r.region.block, here->block_.block, here->num_buckets_ * 32);
  libfilter_block result = {.num_buckets_ = here->num_buckets_, .block_ = r.region};
  return result;
}

bool libfilter_block_equals(const libfilter_block* here, const libfilter_block* there) {
  if (here->num_buckets_ != there->num_buckets_) return false;
  return 0 == memcmp(here->block_.block, there->block_.block, here->num_buckets_ * 32);
}
