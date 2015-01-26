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

// An artificial TunBuilder object, used to log the tun builder settings,
// but doesn't actually configure anything.

#ifndef OPENVPN_TUN_BUILDER_CAPTURE_H
#define OPENVPN_TUN_BUILDER_CAPTURE_H

#include <string>
#include <sstream>
#include <vector>

#include <openvpn/common/rc.hpp>
#include <openvpn/tun/builder/base.hpp>
#include <openvpn/client/rgopt.hpp>

namespace openvpn {
  class TunBuilderCapture : public TunBuilderBase, public RC<thread_unsafe_refcount>
  {
  public:
    typedef boost::intrusive_ptr<TunBuilderCapture> Ptr;

    // builder data classes

    struct RemoteAddress {
      RemoteAddress() : ipv6(false) {}
      std::string address;
      bool ipv6;

      std::string to_string() const
      {
	std::string ret = address;
	if (ipv6)
	  ret += " [IPv6]";
	return ret;
      }
    };

    struct RerouteGW {
      RerouteGW() : ipv4(false), ipv6(false), flags(0) {}
      bool ipv4;
      bool ipv6;
      unsigned int flags;

      std::string to_string() const
      {
	std::ostringstream os;
	const RedirectGatewayFlags rgf(flags);
	os << "IPv4=" << ipv4 << "  IPv6=" << ipv6 << " flags=" << rgf.to_string();
	return os.str();
      }
    };

    struct Route {
      Route() : prefix_length(0), ipv6(false) {}
      std::string address;
      int prefix_length;
      bool ipv6;

      std::string to_string() const
      {
	std::ostringstream os;
	os << address << '/' << prefix_length;
	if (ipv6)
	  os << " [IPv6]";
	return os.str();
      }
    };

    struct DNSServer {
      DNSServer() : ipv6(false) {}
      std::string address;
      bool ipv6;

      std::string to_string() const
      {
	std::string ret = address;
	if (ipv6)
	  ret += " [IPv6]";
	return ret;
      }
    };

    struct SearchDomain {
      std::string domain;

      std::string to_string() const
      {
	return domain;
      }
    };

    struct ProxyBypass {
      std::string bypass_host;

      std::string to_string() const
      {
	return bypass_host;
      }
    };

    struct ProxyAutoConfigURL {
      std::string url;

      bool defined() const {
	return !url.empty();
      }

      std::string to_string() const
      {
	return url;
      }
    };

    struct ProxyHostPort {
      std::string host;
      int port;

      ProxyHostPort() : port(0) {}

      bool defined() const {
	return !host.empty();
      }

      std::string to_string() const
      {
	std::ostringstream os;
	os << host << ' ' << port;
	return os.str();
      }
    };

    TunBuilderCapture() : mtu(0) {}

    virtual bool tun_builder_set_remote_address(const std::string& address, bool ipv6)
    {
      remote_address.address = address;
      remote_address.ipv6 = ipv6;
      return true;
    }

    virtual bool tun_builder_add_address(const std::string& address, int prefix_length, bool ipv6)
    {
      Route r;
      r.address = address;
      r.prefix_length = prefix_length;
      r.ipv6 = ipv6;
      tunnel_addresses.push_back(r);
      return true;
    }

    virtual bool tun_builder_reroute_gw(const std::string& server_address, bool server_address_ipv6, bool ipv4, bool ipv6, unsigned int flags)
    {
      reroute_gw.ipv4 = ipv4;
      reroute_gw.ipv6 = ipv6;
      reroute_gw.flags = flags;
      return true;
    }

    virtual bool tun_builder_add_route(const std::string& address, int prefix_length, bool ipv6)
    {
      Route r;
      r.address = address;
      r.prefix_length = prefix_length;
      r.ipv6 = ipv6;
      add_routes.push_back(r);
      return true;
    }

    virtual bool tun_builder_exclude_route(const std::string& address, int prefix_length, bool ipv6)
    {
      Route r;
      r.address = address;
      r.prefix_length = prefix_length;
      r.ipv6 = ipv6;
      exclude_routes.push_back(r);
      return true;
    }

    virtual bool tun_builder_add_dns_server(const std::string& address, bool ipv6)
    {
      DNSServer dns;
      dns.address = address;
      dns.ipv6 = ipv6;
      dns_servers.push_back(dns);
      return true;
    }

    virtual bool tun_builder_add_search_domain(const std::string& domain)
    {
      SearchDomain dom;
      dom.domain = domain;
      search_domains.push_back(dom);
      return true;
    }

    virtual bool tun_builder_set_mtu(int mtu)
    {
      this->mtu =  mtu;
      return true;
    }

    virtual bool tun_builder_set_session_name(const std::string& name)
    {
      session_name = name;
      return true;
    }

    virtual bool tun_builder_add_proxy_bypass(const std::string& bypass_host)
    {
      ProxyBypass b;
      b.bypass_host = bypass_host;
      proxy_bypass.push_back(b);
      return true;
    }

    virtual bool tun_builder_set_proxy_auto_config_url(const std::string& url)
    {
      proxy_auto_config_url.url = url;
      return true;
    }

    virtual bool tun_builder_set_proxy_http(const std::string& host, int port)
    {
      http_proxy.host = host;
      http_proxy.port = port;      
      return true;
    }

    virtual bool tun_builder_set_proxy_https(const std::string& host, int port)
    {
      https_proxy.host = host;
      https_proxy.port = port;      
      return true;
    }

    std::string to_string() const
    {
      std::ostringstream os;
      os << "Session Name: " << session_name << std::endl;
      if (mtu)
	os << "MTU: " << mtu << std::endl;
      os << "Remote Address: " << remote_address.to_string() << std::endl;
      render_route_list(os, "Tunnel Addresses", tunnel_addresses);
      os << "Reroute Gateway: " << reroute_gw.to_string() << std::endl;
      render_route_list(os, "Add Routes", add_routes);
      render_route_list(os, "Exclude Routes", exclude_routes);
      {
	os << "DNS Servers:" << std::endl;
	for (std::vector<DNSServer>::const_iterator i = dns_servers.begin(); i != dns_servers.end(); ++i)
	  os << "  " << i->to_string() << std::endl;
      }
      {
	os << "Search Domains:" << std::endl;
	for (std::vector<SearchDomain>::const_iterator i = search_domains.begin(); i != search_domains.end(); ++i)
	  os << "  " << i->to_string() << std::endl;
      }
      if (!proxy_bypass.empty()) {
	os << "Proxy Bypass:" << std::endl;
	for (std::vector<ProxyBypass>::const_iterator i = proxy_bypass.begin(); i != proxy_bypass.end(); ++i)
	  os << "  " << i->to_string() << std::endl;
      }
      if (proxy_auto_config_url.defined())
	os << "Proxy Auto Config URL: " << proxy_auto_config_url.to_string() << std::endl;
      if (http_proxy.defined())
	os << "HTTP Proxy: " << http_proxy.to_string() << std::endl;
      if (https_proxy.defined())
	os << "HTTPS Proxy: " << https_proxy.to_string() << std::endl;
      return os.str();
    }

    // builder data
    std::string session_name;
    int mtu;
    RemoteAddress remote_address;          // real address of server
    std::vector<Route> tunnel_addresses;   // local tunnel addresses
    RerouteGW reroute_gw;                  // redirect-gateway info for ipv4
    std::vector<Route> add_routes;         // routes that should be added to tunnel
    std::vector<Route> exclude_routes;     // routes that should be excluded from tunnel
    std::vector<DNSServer> dns_servers;    // VPN DNS servers
    std::vector<SearchDomain> search_domains;  // domain suffixes whose DNS requests should be tunnel-routed

    std::vector<ProxyBypass> proxy_bypass; // hosts that should bypass proxy
    ProxyAutoConfigURL proxy_auto_config_url;
    ProxyHostPort http_proxy;
    ProxyHostPort https_proxy;

  private:
    void render_route_list(std::ostream& os, const char *title, const std::vector<Route>& list) const
    {
      os << title << ':' << std::endl;
      for (std::vector<Route>::const_iterator i = list.begin(); i != list.end(); ++i)
	os << "  " << i->to_string() << std::endl;
    }
  };
}

#endif
