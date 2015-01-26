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

// Wrapper for Apple SCNetworkReachability methods.

#ifndef OPENVPN_APPLECRYPTO_UTIL_REACHABLE_H
#define OPENVPN_APPLECRYPTO_UTIL_REACHABLE_H

#import "TargetConditionals.h"

#include <netinet/in.h>
#include <SystemConfiguration/SCNetworkReachability.h>

#include <string>
#include <sstream>

namespace openvpn {
  // Helper class for determining network reachability
  class Reachability
  {
  public:
    Reachability()
      : didRetrieveFlags(FALSE),
	flags(0)
    {
      struct sockaddr_in zeroAddress;
      bzero(&zeroAddress, sizeof(zeroAddress));
      zeroAddress.sin_len = sizeof(zeroAddress);
      zeroAddress.sin_family = AF_INET;
      SCNetworkReachabilityRef defaultRouteReachability = SCNetworkReachabilityCreateWithAddress(kCFAllocatorDefault, (struct sockaddr*)&zeroAddress);
      didRetrieveFlags = SCNetworkReachabilityGetFlags(defaultRouteReachability, &flags);
      CFRelease(defaultRouteReachability);
    }

    bool defined() const { return bool(didRetrieveFlags); }
    bool reachable() const { return bool(flags & kSCNetworkReachabilityFlagsReachable); }
    bool connectionRequired() const { return bool(flags & kSCNetworkReachabilityFlagsConnectionRequired); }

#if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR // iOS device or simulator
    bool isWWAN() const { return bool(flags & kSCNetworkReachabilityFlagsIsWWAN); } // cellular
#else
    bool isWWAN() const { return false; }
#endif

    bool reachableVia(const std::string& net_type) const
    {
      if (net_type == "cellular")
	return reachable() && isWWAN();
      else if (net_type == "wifi")
	return reachable() && !isWWAN();
      else
	return reachable();
    }

    std::string to_string() const {
      std::ostringstream out;
      out << "Reachability<";
      if (defined())
	{
	  out << "reachable=" << reachable();
	  out << " connectionRequired=" << connectionRequired();
	  out << " isWWAN=" << isWWAN();
	}
      else
	out << "UNDEF";
      out << '>';
      return out.str();
    }

  private:
    Boolean didRetrieveFlags;
    SCNetworkReachabilityFlags flags;
  };

}

#endif
