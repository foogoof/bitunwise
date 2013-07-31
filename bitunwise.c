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
 *
 */

#include "bitunwise.h"
#include <stdio.h>

/******************************************************************************/

#define L0R0 (1 << 0)
#define L1R0 (1 << 1)
#define L0R1 (1 << 2)
#define L1R1 (1 << 3)

/******************************************************************************/

#define op_never        (    0 |     0 |     0 |     0 )
#define op_always       ( L0R0 |  L1R0 |  L0R1 |  L1R1 )

#define op_left         (    0 |  L1R0 |     0 |  L1R1 )
#define op_not_left     ( L0R0 |     0 |  L0R1 |     0 )

#define op_same         ( L0R0 |     0 |     0 |  L1R1 )
#define op_different    (    0 |  L1R0 |  L0R1 |     0 )

#define EMIT  0
#define MATCH 1

int bu_dbg_flag = 0;

/******************************************************************************/

typedef unsigned char u08;
typedef u08(*bytegetter)(const void *src, size_t byte);

u08 something(const void *src, size_t byte) {
        u08 random_byte_on_the_stack;
        return random_byte_on_the_stack;
}

u08 onebyte(const void *src, size_t byte) {
        return *(const u08*)src;
}

u08 nthbyte(const void *src, size_t byte) {
        return ((const u08*)src)[byte];
}

/******************************************************************************/

typedef struct stream {
        bytegetter reader;
        const void *src;
} stream;

/******************************************************************************/

#define trace(val, pat)                                                 \
        do {                                                            \
                if (!bu_dbg_flag) { (val); }                            \
                else { printf("%s:%d: " pat "\n",                       \
                              __FILE__, __LINE__, (val)); }             \
        } while (0)

/******************************************************************************/

void process(const stream *left, const stream *right, size_t n,
             int op, int type, void *context) {

        size_t i, j;
        void *dst;
        int *result;

        if (type == EMIT)
                dst = context;
        else if (type == MATCH)
                result = context;

        for (i = 0; i < n; i++) {
                unsigned char lc, rc, dc;
                
                trace(lc = left->reader(left->src, i), "%02X");
                trace(rc = right->reader(right->src, i), "%02X");

                for (j = 0; j < 8; j++) {
                        const int flag = 1 << j;
                        const int lbit = lc & flag;
                        const int rbit = rc & flag;
                        int test = 0;

                        if (!lbit && !rbit)
                                trace(test = op & L0R0, "%d");
                        else if (lbit && !rbit)
                                trace(test = op & L1R0, "%d");
                        else if (!lbit && rbit)
                                trace(test = op & L0R1, "%d");
                        else
                                trace(test = op & L1R1, "%d");
                        
                        if (type == EMIT) {
                                if (test)
                                        trace(dc |= flag, "%02x");
                                else
                                        trace(dc &= ~flag, "%02x");
                        }
                        else if (type == MATCH) {
                                if (!test) { 
                                        trace(*result = 0, "%d");
                                        return;
                                }
                        }
                }
                
                if (type == EMIT)
                        trace(((u08*)dst)[i] = dc, "%02x");
        }

        if (type == MATCH)
                trace(*result = 1, "%d");
}

/******************************************************************************/

int match(const void *left, bytegetter lreader,
          const void *right, bytegetter rreader,
          size_t n, int op) {
        stream lstream, rstream;
        int result;

        lstream.reader = lreader;
        lstream.src = left;
        rstream.reader = rreader;
        rstream.src = right;
        
        process(&lstream, &rstream, n, op, MATCH, &result);

        return result;
}

/******************************************************************************/

int simple_match(const void *left, const void *right, size_t n, int op) {
        return match(left, nthbyte, right, nthbyte, n, op);
}

/******************************************************************************/

void* nullary_emit(void *dst, size_t n, int op) {
        stream lstream, rstream;

        lstream.reader = rstream.reader = something;

        process(&lstream, &rstream, n, op, EMIT, dst);
        return dst;
}

/******************************************************************************/

void* unary_emit(void *dst, const void *src, bytegetter reader, size_t n, int op) {
        stream lstream, rstream;

        lstream.reader = reader;
        lstream.src = src;
        
        rstream.reader = something;

        process(&lstream, &rstream, n, op, EMIT, dst);
        return dst;
}

/******************************************************************************/

void* binary_emit(void *dst, const void *left, bytegetter lreader,
                  const void *right, bytegetter rreader, size_t n, int op) {
        stream lstream, rstream;

        lstream.reader = lreader;
        lstream.src = left;

        rstream.reader = rreader;
        rstream.src = right;

        process(&lstream, &rstream, n, op, EMIT, dst);
        return dst;
}

/******************************************************************************/
/* Begin implementation of external functions */

void* bu_bzero(void *dst, size_t n) {
        return nullary_emit(dst, n, op_never);
}

/******************************************************************************/

void* bu_fill(void *dst, size_t n) {
        return nullary_emit(dst, n, op_always);
}

/******************************************************************************/

void* bu_memcpy(void *dst, const void *src, size_t n) {
        return unary_emit(dst, src, nthbyte, n, op_left);
}

/******************************************************************************/

void *bu_memset(void *dst, int val, size_t n) {
        u08 src = val;
        return unary_emit(dst, &src, onebyte, n, op_left);
}

/******************************************************************************/

void* bu_invert(void *dst, const void *src, size_t n) {
        return unary_emit(dst, src, nthbyte, n, op_not_left);
}

/******************************************************************************/

void* bu_xor(void *dst, const void *left, const void *right, size_t n) {
        return binary_emit(dst, left, nthbyte, right, nthbyte, n, op_different);
}

/******************************************************************************/

int bu_memcmp(const void *left, const void *right, size_t n) {
        return !simple_match(left, right, n, op_same);
}

/******************************************************************************/

int bu_equal(const void *left, const void *right, size_t n) {
        return simple_match(left, right, n, op_same);
}

/******************************************************************************/

int bu_opposite(const void *left, const void *right, size_t n) {
        return simple_match(left, right, n, op_different);
}

/******************************************************************************/

int bu_isset(const void *src, int val, size_t n) {
        u08 pat = val;
        return match(src, nthbyte, &pat, onebyte, n, op_same);
}
