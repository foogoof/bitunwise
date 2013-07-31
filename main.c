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

#include <assert.h>
#include <stdio.h>
#include "bitunwise.h"

int main(int argc, char **argv) {

        unsigned char dst, src;
        int dsti, srci, src2i;

        dst = 1;
        bu_bzero(&dst, 1);
        assert(dst == 0);

        dst = 0xFF;
        bu_bzero(&dst, 1);
        assert(dst == 0x00);

        dst = 0x00;
        bu_fill(&dst, 1);
        assert(dst == 0xFF);

        src = 23;
        bu_memcpy(&dst, &src, 1);
        assert(dst == 23);

        src = 0x5A;
        bu_invert(&dst, &src, 1);
        assert(dst = 0xA5);

        assert(0 == bu_memcmp(&src, &src, 1));
        assert(bu_equal(&src, &src, 1));
        assert(!bu_opposite(&src, &src, 1));

        src = 123;
        dst = 98;
        assert(0 != bu_memcmp(&src, &dst, 1));
        assert(1 != bu_equal(&src, &dst, 1));
        assert(1 != bu_opposite(&src, &dst, 1));

        src = 123; dst = ~src;
        assert(0 != bu_memcmp(&src, &dst, 1));
        assert(1 != bu_equal(&src, &dst, 1));
        assert(1 == bu_opposite(&src, &dst, 1));

        srci = 0x35;
        dsti = 0;
        bu_memset(&dsti, srci, 4);
        assert(0x35353535 == dsti);
        
        assert(bu_isset(&dsti, srci, 4));
        dsti = 0x35000035;
        assert(!bu_isset(&dsti, srci, 4));

        srci  = 0x01020304;
        src2i = 0x1F2E3D4C;
        bu_xor(&dsti, &srci, &src2i, 4);
        assert(dsti == 0x1E2C3E48);

        argc = argc;
        argv = argv;

        printf("Done.\n");

        return 0;
}
