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

#ifndef OPENVPN_OPENSSL_UTIL_INIT_H
#define OPENVPN_OPENSSL_UTIL_INIT_H

#include <boost/asio/ssl/detail/openssl_init.hpp>

namespace openvpn {

  // Instantiate this object to ensure openssl is initialised.
  typedef boost::asio::ssl::detail::openssl_init<> openssl_init;

} // namespace openvpn

#endif // OPENVPN_OPENSSL_UTIL_INIT_H
