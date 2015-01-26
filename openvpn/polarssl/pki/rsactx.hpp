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

// Wrap a PolarSSL rsa_context object.

#ifndef OPENVPN_POLARSSL_PKI_RSACTX_H
#define OPENVPN_POLARSSL_PKI_RSACTX_H

#include <string>
#include <sstream>
#include <cstring>

#include <polarssl/x509.h>

#include <openvpn/common/types.hpp>
#include <openvpn/common/exception.hpp>
#include <openvpn/common/rc.hpp>
#include <openvpn/polarssl/util/error.hpp>

namespace openvpn {
  namespace PolarSSLPKI {

    class RSAContext : public RC<thread_unsafe_refcount>
    {
    public:
      typedef boost::intrusive_ptr<RSAContext> Ptr;

      RSAContext() : ctx(NULL) {}

      RSAContext(const std::string& key_txt, const std::string& title, const std::string& priv_key_pwd)
	: ctx(NULL)
      {
	try {
	  parse(key_txt, title, priv_key_pwd);
	}
	catch (...)
	  {
	    dealloc();
	    throw;
	  }
      }

      void parse(const std::string& key_txt, const std::string& title, const std::string& priv_key_pwd)
      {
	alloc();
	const int status = x509parse_key(ctx,
					 (const unsigned char *)key_txt.c_str(),
					 key_txt.length(),
					 (const unsigned char *)priv_key_pwd.c_str(),
					 priv_key_pwd.length());
	if (status < 0)
	  throw PolarSSLException("error parsing " + title + " private key", status);
      }

      rsa_context* get() const
      {
	return ctx;
      }

      ~RSAContext()
      {
	dealloc();
      }

    private:
      void alloc()
      {
	if (!ctx)
	  {
	    ctx = new rsa_context;
	    std::memset(ctx, 0, sizeof(rsa_context));
	  }
      }

      void dealloc()
      {
	if (ctx)
	  {
	    rsa_free(ctx);
	    delete ctx;
	    ctx = NULL;
	  }
      }

      rsa_context *ctx;
    };

  }
}
#endif
