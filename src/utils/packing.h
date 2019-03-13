#ifndef PACKING_H
#define PACKING_H
#pragma once

#ifdef __GNUC__

#define PACKED(type_to_pack) type_to_pack __attribute__((__packed__))

#define ALIGNED(type_to_align, n) type_to_pack __attribute__((aligned(n)))

#else

#define PACKED(type_to_pack)                                                   \
  __pragma(pack(push, 1)) type_to_pack __pragma(pack(pop))

#define ALIGNED(type_to_align, n) __declspec(align(n)) type_to_align

#endif

#endif // PACKING_H
