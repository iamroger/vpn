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

// General-purpose HMAC classes that are independent of the underlying CRYPTO_API

#ifndef OPENVPN_CRYPTO_HMAC_H
#define OPENVPN_CRYPTO_HMAC_H

#include <string>

#include <openvpn/common/types.hpp>
#include <openvpn/common/exception.hpp>
#include <openvpn/common/memcmp.hpp>
#include <openvpn/crypto/static_key.hpp>

namespace openvpn {
  template <typename CRYPTO_API>
  class HMACContext
  {
  public:
    OPENVPN_SIMPLE_EXCEPTION(hmac_context_digest_size);
    OPENVPN_SIMPLE_EXCEPTION(hmac_context_bad_sizing);

  public:
    HMACContext() {}

    HMACContext(const typename CRYPTO_API::Digest& digest, const StaticKey& key)
    {
      init(digest, key);
    }

    bool defined() const { return ctx.is_initialized(); }

    // size of out buffer to pass to hmac
    size_t output_size() const
    {
      return ctx.size();
    }

    void init(const typename CRYPTO_API::Digest& digest, const StaticKey& key)
    {
      // check that key is large enough
      if (key.size() < digest.size())
	throw hmac_context_digest_size();

      // initialize HMAC context with digest type and key
      ctx.init(digest, key.data(), digest.size());
    }

    void hmac(unsigned char *out, const size_t out_size,
	      const unsigned char *in, const size_t in_size)
    {
      ctx.reset();
      ctx.update(in, in_size);
      ctx.final(out);
    }

    // Special HMAC for OpenVPN control packets

    void hmac3_gen(unsigned char *data, const size_t data_size,
		   const size_t l1, const size_t l2, const size_t l3)
    {
      if (hmac3_pre(data, data_size, l1, l2, l3))
	ctx.final(data + l1);
      else
	throw hmac_context_bad_sizing();
    }

    // verify the HMAC generated by hmac3_gen, return true if verified
    bool hmac3_cmp(const unsigned char *data, const size_t data_size,
		   const size_t l1, const size_t l2, const size_t l3)
    {
      unsigned char local_hmac[CRYPTO_API::HMACContext::MAX_HMAC_SIZE];
      if (hmac3_pre(data, data_size, l1, l2, l3))
	{
	  ctx.final(local_hmac);
	  return !memcmp_secure(data + l1, local_hmac, l2);
	}
      else
	return false;
    }

  private:
    // Convoluting OpenVPN control channel packets for HMAC:
    // <-- L1  -->   <-L2>   <L3>
    // [OP]  [PSID]  [HMAC]  [PID] [...]  -> canonical order
    //
    // [HMAC] [PID] [OP] [PSID] [...]     -> HMAC order

    bool hmac3_pre(const unsigned char *data, const size_t data_size,
		   const size_t l1, const size_t l2, const size_t l3)
    {
      const size_t lsum = l1 + l2 + l3;
      if (lsum > data_size || l2 != ctx.size())
	return false;
      ctx.reset();
      ctx.update(data + l1 + l2, l3);
      ctx.update(data, l1);
      ctx.update(data + lsum, data_size - lsum);
      return true;
    }

    typename CRYPTO_API::HMACContext ctx;
  };
}

#endif
