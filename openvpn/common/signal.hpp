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

#ifndef OPENVPN_COMMON_SIGNAL_H
#define OPENVPN_COMMON_SIGNAL_H

#include <signal.h>

#include <openvpn/common/exception.hpp>

namespace openvpn {
  class Signal
  {
  public:
    OPENVPN_SIMPLE_EXCEPTION(signal_error);

    typedef void (*handler_t)(int signum);

    enum {
      F_SIGINT  = (1<<0),
      F_SIGTERM = (1<<1),
      F_SIGHUP  = (1<<2),
    };

    Signal(const handler_t handler, const unsigned int flags)
    {
#ifndef _WIN32_WINNT
      struct sigaction sa;
      sa.sa_handler = handler;
      sigemptyset(&sa.sa_mask);
      sa.sa_flags = SA_RESTART; // restart functions if interrupted by handler
      sigconf(sa, flags_ = flags);
#else
      signal(flags_ = flags, handler);
#endif
    }

    ~Signal()
    {
#ifndef _WIN32_WINNT
      struct sigaction sa;
      sa.sa_handler = SIG_DFL;
      sigemptyset(&sa.sa_mask);
      sa.sa_flags = 0;
      sigconf(sa, flags_);
#else
      signal(SIGINT, SIG_DFL);
#endif
    }

  private:
#ifndef _WIN32_WINNT
    static void sigconf(struct sigaction& sa, const unsigned int flags)
    {
      if (flags & F_SIGINT)
	sigact(sa, SIGINT);
      if (flags & F_SIGTERM)
	sigact(sa, SIGTERM);
      if (flags & F_SIGHUP)
	sigact(sa, SIGHUP);
    }


    static void sigact(struct sigaction& sa, const int sig)
    {
      if (sigaction(sig, &sa, NULL) == -1)
	throw signal_error();
    }
#endif
    unsigned int flags_;
  };
}
#endif
