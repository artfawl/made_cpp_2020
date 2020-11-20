#ifndef MEANWHILE_IN_THE_SEVENTH_GALAXY_SECOND_VARIANT_FLOOR_H
#define MEANWHILE_IN_THE_SEVENTH_GALAXY_SECOND_VARIANT_FLOOR_H

#include "binary.h"
#include "value_list.h"

const int Del = 1000000007;  // просто вынес константу
//**
//* работа с масками
//**

template <size_t msk, size_t len>
struct mask {  // создаём битовую маску для числа msk
  using type = typename PushBack<GetBit<len - 1, msk>::value,
                                 typename mask<msk, len - 1>::type>::type;
};

template <size_t msk>
struct mask<msk, 1> {
  using type = typename PushFront<GetBit<0, msk>::value, NullType>::type;
};

//**
//* проверка двух масок
//**

template <class mask1, class mask2, size_t len>
struct check {  // проверяем можно ли получить из маски mask1 mask2
 private:
  static constexpr bool mask1_bit = Get<len - 1, typename mask1::type>::value;
  static constexpr bool mask2_bit = Get<len - 1, typename mask2::type>::value;

 public:
  static constexpr bool value =  // длинное условие, не придумал как сделать
                                 // короче, поэтому объясню
      (mask1_bit ^
       mask2_bit)  // если биты различаются, то всё хорошо, смотрим следущие
          ? check<mask1, mask2, len - 1>::value
          : (mask1_bit == 0 && mask2_bit == 0  // если два бита равны 0 смотрим,
                                               // равны ли нулю следущие два
                 ? !Get<len - 2, typename mask1::type>::value &&
                       !Get<len - 2, typename mask2::type>::value &&
                       check<mask1, mask2, len - 2>::
                           value  // если да пропускаем 2 бита и смотрим дальше
                 : false);  // иначе получить не можем, значит фолс
};

template <class mask1, class mask2>
struct check<mask1, mask2, 1> {
 private:
  static constexpr bool mask1_bit = Get<0, typename mask1::type>::value;
  static constexpr bool mask2_bit = Get<0, typename mask2::type>::value;

 public:
  static constexpr bool value = mask1_bit ^ mask2_bit;
};

template <class mask1, class mask2>
struct check<mask1, mask2, 0> {
  static constexpr bool value = true;
};

//**
//* вычисления
//**

template <size_t i, size_t k, size_t j, size_t height, size_t, size_t width>
struct sum;

template <size_t i, size_t j, size_t height, size_t max_num, size_t width>
struct d {  // отвечает за таблицу динамики d[i][j] - кол-во способов замесить
            // площадку  (i - 1) х width с битовой маской j на i столбце
  static constexpr size_t value =
      sum<i - 1, max_num, j, height, max_num, width>::value % Del;
};

template <size_t j, size_t height, size_t max_num, size_t width>
struct d<1, j, height, max_num, width> {  // предельный случай
  static constexpr size_t value = 0;
};

template <size_t height, size_t max_num, size_t width>
struct d<1, 1, height, max_num, width> {  // предельный случай
  static constexpr size_t value = 1;
};

template <size_t i, size_t k, size_t j, size_t height, size_t max_num,
          size_t width>
struct sum {  // вычисляет сумму по i столбцу в d (проверяя возможность перехода
              // битовой маски k - 1 в j - 1)
  static constexpr size_t value =
      (sum<i, k - 1, j, height, max_num, width>::value +
       d<i, k, height, max_num, width>::value *
           check<mask<k - 1, width>, mask<j - 1, width>, width>::value) %
      Del;
};

template <size_t i, size_t j, size_t height, size_t max_num, size_t width>
struct sum<i, 1, j, height, max_num, width> {  // предельный случай
  static constexpr size_t value =
      (d<i, 1, height, max_num, width>::value *
       check<mask<0, width>, mask<j - 1, width>, width>::value) %
      Del;
};

template <size_t cur, size_t height, size_t max_num, size_t width>
struct d_prep {  // подготовка таблицы. вычисление рекурсии с конца переполняет
                 // стек рекурсии
  // поэтому вычислим d[1][1]->d[2][1]->d[3][1] и тд
  static constexpr size_t prep =
      (d<height - cur + 1, 1, height, max_num, width>::value,
       d_prep<cur - 1, height, max_num,
              width>::prep);  // оператор "," (:-D) чтобы по итогу лежало
                              // d[height + 1][1] но вычисляли постепенно
  //наверное костыль, но ничего лучше я не придумал
};

template <size_t height, size_t max_num, size_t width>
struct d_prep<0, height, max_num, width> {
  static constexpr size_t prep =
      d<height + 1, 1, height, max_num, width>::value;
};

//**
//* ответыч
//**

template <size_t width, size_t height>
struct Floor {
  static constexpr size_t value =
      d_prep<height, height, (1 << width), width>::prep;
};

#endif  /// MEANWHILE_IN_THE_SEVENTH_GALAXY_SECOND_VARIANT_FLOOR_H.
