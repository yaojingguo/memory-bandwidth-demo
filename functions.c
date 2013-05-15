// Copyright 2013 Alex Reece.

#include "./functions.h"

#include <assert.h>
#include <string.h>

#ifdef __SSE4_1__
#include <smmintrin.h>
#endif

#ifdef __AVX__
#include <immintrin.h>
#endif

void write_memory_memset(void* array, size_t size) {
  memset(array, 0xff, size);
}

void write_memory_loop(void* array, size_t size) {
  size_t* carray = (size_t*) array;
  size_t i;
  for (i = 0; i < size / sizeof(size_t); i++) {
    carray[i] = 1;
  }
}

void read_memory_loop(void* array, size_t size) {
  size_t* carray = (size_t*) array;
  size_t val = 0;
  size_t i;
  for (i = 0; i < size / sizeof(size_t); i++) {
    val += carray[i];
  }

  // This is unlikely, and we want to make sure the reads are not optimized
  // away.
  assert(val != 0xdeadbeef);
}

#ifdef __SSE4_1__
void write_memory_nontemporal_sse(void* array, size_t size) {
  __m128i* varray = (__m128i*) array;

  __m128i vals = _mm_set1_epi32(1);
  size_t i;
  for (i = 0; i < size / sizeof(__m128i); i++) {
    _mm_stream_si128(&varray[i], vals);
    vals = _mm_add_epi16(vals, vals);
  }
}

void write_memory_sse(void* array, size_t size) {
  __m128i* varray = (__m128i*) array;

  __m128i vals = _mm_set1_epi32(1);
  size_t i;
  for (i = 0; i < size / sizeof(__m128i); i++) {
    _mm_store_si128(&varray[i], vals);
    vals = _mm_add_epi16(vals, vals);
  }
}

void read_memory_sse(void* array, size_t size) {
  __m128i* varray = (__m128i*) array;
  __m128i accum = _mm_set1_epi32(0xDEADBEEF);
  size_t i;
  for (i = 0; i < size / sizeof(__m128i); i++) {
    accum = _mm_add_epi16(varray[i], accum);
  }

  // This is unlikely, and we want to make sure the reads are not optimized
  // away.
  assert(!_mm_testz_si128(accum, accum));
}
#endif

#ifdef __AVX__
void write_memory_nontemporal_avx(void* array, size_t size) {
  __m256* varray = (__m256*) array;

  __m256 vals = _mm256_set1_ps((float) 0xDEADBEEF);
  size_t i;
  for (i = 0; i < size / sizeof(__m256); i++) {
    _mm256_stream_si256((__m256i*) &varray[i], vals);
  }
}

void write_memory_avx(void* array, size_t size) {
  __m256* varray = (__m256*) array;

  __m256 vals = _mm256_set1_ps((float) 0xDEADBEEF);
  size_t i;
  for (i = 0; i < size / sizeof(__m256i); i++) {
    _mm256_store_si256((__m256i*) &varray[i], vals);
  }
}

void read_memory_avx(void* array, size_t size) {
  __m256* varray = (__m256*) array;
  __m256 accum = _mm256_set1_ps((float) 0xDEADBEEF);
  size_t i;
  for (i = 0; i < size / sizeof(__m256i); i++) {
    accum = _mm256_add_ps(varray[i], accum);
  }

  // This is unlikely, and we want to make sure the reads are not optimized
  // away.
  assert(!_mm256_testz_si256(accum, accum));
}
#endif  // __AVX__
