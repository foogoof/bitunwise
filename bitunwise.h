/*
 *  Copyright 2008, 2009 Seth Schroeder <goof@foognostic.net>
 *
 *  This file is part of bitunwise.
 *
 *  bitunwise is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  bitunwise is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with bitunwise.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef BITUNWISE_H
#define BITUNWISE_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef SWIG
%module bitunwise
%{
#include "bitunwise.h"
%}
#endif

#include <stddef.h>

  /*
   * These functions behave like the C stdlib implementations,
   * except slower. Much much slower.
   */
  extern void* bu_memcpy(void *dst, const void *src, size_t n);
  extern void* bu_memset(void *dst, int val, size_t n);
  extern void* bu_bzero(void *dst, size_t n);
  extern int bu_memcmp(const void *lhs, const void *rhs, size_t n);

  /* evaluates xor of each bit of left and right, puts results into dst. */
  extern void* bu_xor(void *dst, const void *left, const void *right, size_t n);

  /* bu_fill sets bits to one, the converse of bu_bzero.
   * The author of bzero never made bone, wonder why? :-)
   */
  extern void* bu_fill(void *dst, size_t n);

  /* writes a bitwise inverse of src into dst */
  extern void* bu_invert(void *dst, const void *src, size_t n);
  
  /* same as bu_memcmp, except returns 1 when equal */
  extern int bu_equal(const void *lhs, const void *rhs, size_t n);

  /* returns 1 when lhs and rhs are inverse bitwise. */
  extern int bu_opposite(const void *lhs, const void *rhs, size_t n);

  /* bu_isset returns 1 when n bytes of src match val (as an unsigned char). */
  extern int bu_isset(const void *src, int val, size_t n);

  /* emits verbose debugging when non-zero. */
  extern int bu_dbg_flag;

#ifdef __cplusplus
}
#endif

#endif /* BITUNWISE_H */
