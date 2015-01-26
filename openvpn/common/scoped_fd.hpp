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

// A scoped file descriptor that is automatically closed by its destructor.

#ifndef OPENVPN_COMMON_SCOPED_FD_H
#define OPENVPN_COMMON_SCOPED_FD_H

#include <unistd.h> // for close()

#include <boost/noncopyable.hpp>

namespace openvpn {

  class ScopedFD : boost::noncopyable
  {
  public:
    ScopedFD() : fd(-1) {}

    explicit ScopedFD(const int fd_arg)
      : fd(fd_arg) {}

    int release()
    {
      const int ret = fd;
      fd = -1;
      //OPENVPN_LOG("**** SFD RELEASE=" << ret);
      return ret;
    }

    bool defined() const
    {
      return fd >= 0;
    }

    int operator()() const
    {
      return fd;
    }

    void reset(const int fd_arg)
    {
      close();
      fd = fd_arg;
      //OPENVPN_LOG("**** SFD RESET=" << fd);
    }

    // unusual semantics: replace fd without closing it first
    void replace(const int fd_arg)
    {
      //OPENVPN_LOG("**** SFD REPLACE " << fd << " -> " << fd_arg);
      fd = fd_arg;
    }

    int close()
    {
      if (defined())
	{
	  const int ret = ::close(fd);
	  fd = -1;
	  //OPENVPN_LOG("**** SFD CLOSE=" << ret);
	  return ret;
	}
      else
	return 0;
    }

    ~ScopedFD()
    {
      close();
    }

  private:
    int fd;
  };

} // namespace openvpn

#endif // OPENVPN_COMMON_SCOPED_FD_H
