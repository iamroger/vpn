//    OpenVPN -- An application to securely tunnel IP networks
//               over a single port, with support for SSL/TLS-based
//               session authentication and key exchange,
//               packet encryption, packet authentication, and
//               packet compression.
//
//    Copyright (C) 2013 OpenVPN Technologies, Inc.
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU Affero General Public License Version 3
//    as published by the Free Software Foundation.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU Affero General Public License for more details.
//
//    You should have received a copy of the GNU Affero General Public License
//    along with this program in the COPYING file.
//    If not, see <http://www.gnu.org/licenses/>.

#ifndef OPENVPN_COMMON_FFS_H
#define OPENVPN_COMMON_FFS_H

#include <strings.h> // for ffs (and fls on BSD)

#include <openvpn/common/platform.hpp>

namespace openvpn {
  // find the zero-based position of the first 1 bit in a word
  // (scanning from least significant bit to most significant)
  inline const int find_first_set(unsigned int v)
  {
    return ffs(v) - 1;
  }

  // find the one-based position of the last 1 bit in a word
  // (scanning from most significant bit to least significant)
  inline const int find_last_set(unsigned int v)
  {
#if defined(OPENVPN_PLATFORM_TYPE_APPLE)
    return fls(v); // apparently only BSD-based platforms have this
#else
    int r = 32;

    if (!v)
      return 0;
    if (!(v & 0xffff0000u)) {
      v <<= 16;
      r -= 16;
    }
    if (!(v & 0xff000000u)) {
      v <<= 8;
      r -= 8;
    }
    if (!(v & 0xf0000000u)) {
      v <<= 4;
      r -= 4;
    }
    if (!(v & 0xc0000000u)) {
      v <<= 2;
      r -= 2;
    }
    if (!(v & 0x80000000u)) {
      v <<= 1;
      r -= 1;
    }
    return r;
#endif
  }

} // namespace openvpn

#endif // OPENVPN_COMMON_FFS_H
