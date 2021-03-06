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

// Abstract base classes for client tun interface objects.

#ifndef OPENVPN_TUN_CLIENT_TUNBASE_H
#define OPENVPN_TUN_CLIENT_TUNBASE_H

#include <string>

#include <boost/asio.hpp>

#include <openvpn/common/rc.hpp>
#include <openvpn/common/options.hpp>
#include <openvpn/buffer/buffer.hpp>
#include <openvpn/transport/client/transbase.hpp>

namespace openvpn {

  // Base class for objects that implement a client tun interface.
  struct TunClient : public RC<thread_unsafe_refcount>
  {
    typedef boost::intrusive_ptr<TunClient> Ptr;

    virtual void client_start(const OptionList&, TransportClient&) = 0;
    virtual void stop() = 0;
    virtual bool tun_send(BufferAllocated& buf) = 0; // return true if send succeeded
    virtual std::string tun_name() const = 0;
    virtual std::string vpn_ip4() const = 0;
    virtual std::string vpn_ip6() const = 0;
  };

  // Base class for parent of tun interface object, used to
  // communicate received data packets, exceptions, and progress
  // notifications.
  struct TunClientParent
  {
    virtual void tun_recv(BufferAllocated& buf) = 0;
    virtual void tun_error(const Error::Type fatal_err, const std::string& err_text) = 0;

    // progress notifications
    virtual void tun_pre_tun_config() = 0;
    virtual void tun_pre_route_config() = 0;
    virtual void tun_connected() = 0;
  };

  // Factory for tun interface objects.
  struct TunClientFactory : public RC<thread_unsafe_refcount>
  {
    typedef boost::intrusive_ptr<TunClientFactory> Ptr;

    virtual TunClient::Ptr new_client_obj(boost::asio::io_service& io_service,
					  TunClientParent& parent) = 0;
  };

} // namespace openvpn

#endif // OPENVPN_TUN_CLIENT_TUNBASE_H
