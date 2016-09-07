// Copyright (c) YugaByte, Inc.

#include "yb/docdb/docdb_test_util.h"

using std::vector;

namespace yb {
namespace docdb {

PrimitiveValue GenRandomPrimitiveValue(RandomNumberGenerator& rng) {
  switch (rng() % 5) {
    case 0:
      return PrimitiveValue(static_cast<int64_t>(rng()));
    case 1: {
      string s;
      for (int j = 0; j < rng() % 50; ++j) {
        s.push_back(rng() & 0xff);
      }
      return PrimitiveValue(s);
    }
    case 2: return PrimitiveValue(ValueType::kNull);
    case 3: return PrimitiveValue(ValueType::kTrue);
    case 4: return PrimitiveValue(ValueType::kFalse);
  }
  LOG(FATAL) << "Should never get here";
  return PrimitiveValue(); // to make the compiler happy
}


// Generate a vector of random primitive values.
vector<PrimitiveValue> GenRandomPrimitiveValues(RandomNumberGenerator& rng, int max_num) {
  vector<PrimitiveValue> result;
  for (int i = 0; i < rng() % (max_num + 1); ++i) {
    result.push_back(GenRandomPrimitiveValue(rng));
  }
  return result;
}

DocKey CreateMinimalDocKey(RandomNumberGenerator& rng, bool use_hash) {
  return use_hash ? DocKey(static_cast<DocKeyHash>(rng()), {}, {}) : DocKey();
}

DocKey GenRandomDocKey(RandomNumberGenerator& rng, bool use_hash) {
  if (use_hash) {
    return DocKey(
        static_cast<uint32_t>(rng()),  // this is just a random value, not a hash function result
        GenRandomPrimitiveValues(rng),
        GenRandomPrimitiveValues(rng));
  } else {
    return GenRandomPrimitiveValues(rng);
  }
}

}
}
