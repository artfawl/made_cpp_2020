#ifndef MEANWHILE_IN_THE_SEVENTH_GALAXY_FIRST_VARIANT_GAME_H
#define MEANWHILE_IN_THE_SEVENTH_GALAXY_FIRST_VARIANT_GAME_H

#include <iostream>

#include "binary.h"
#include "xor_sum.h"

template <size_t x_sum, size_t len>
struct Bit {
  static constexpr size_t value =
      GetBit<len - 1, x_sum>::value ? len : Bit<x_sum, len - 1>::value;
};

template <size_t x_sum>
struct Bit<x_sum, 0> {
  static constexpr size_t value = 0;
};

template <size_t heap_num, size_t bit_num, size_t x_sum, size_t... Args>
struct Count {};

template <size_t heap_num, size_t bit_num, size_t x_sum, size_t Head,
          size_t... Tail>
struct Count<heap_num, bit_num, x_sum, Head, Tail...> {
 private:
  static constexpr bool in_head = GetBit<bit_num - 1, Head>::value;

 public:
  static constexpr ssize_t heap_ind =
      in_head ? heap_num
              : Count<heap_num + 1, bit_num, x_sum, Tail...>::heap_ind;
  static constexpr ssize_t rocks_count =
      in_head ? (Head - XorSum<Head, x_sum>::value)
              : Count<heap_num + 1, bit_num, x_sum, Tail...>::rocks_count;
};

template <size_t heap_num, size_t bit_num, size_t x_sum, size_t Head>
struct Count<heap_num, bit_num, x_sum, Head> {
 private:
  static constexpr bool in_head = GetBit<bit_num - 1, Head>::value;

 public:
  static constexpr ssize_t heap_ind = in_head ? heap_num : -1;
  static constexpr ssize_t rocks_count =
      in_head ? (Head - XorSum<Head, x_sum>::value) : -1;
};

template <size_t heap_num, size_t bit_num, size_t x_sum>
struct Count<heap_num, bit_num, x_sum> {
  static constexpr ssize_t heap_ind = -1;
  static constexpr ssize_t rocks_count = -1;
};

template <size_t... Heaps>
struct Game {
 private:
  static constexpr size_t x_sum = XorSum<Heaps...>::value;
  static constexpr size_t sum_len = Log2<x_sum>::value;
  static constexpr size_t unzero = Bit<x_sum, sum_len>::value;

 public:
  static constexpr bool first_player_wins = x_sum;
  static constexpr ssize_t first_move_heap_index =
      x_sum ? Count<0, unzero, x_sum, Heaps...>::heap_ind : -1;
  static constexpr ssize_t first_move_rocks_count =
      x_sum ? Count<0, unzero, x_sum, Heaps...>::rocks_count : -1;
};

#endif  /// MEANWHILE_IN_THE_SEVENTH_GALAXY_FIRST_VARIANT_GAME_H.
