OpenVPN 3
-----------

OpenVPN 3 is a C++ class library that implements the functionality of an
OpenVPN client, and is protocol-compatible with the OpenVPN 2.x branch.

Building OpenVPN 3 on Mac OS X
------------------------------

OpenVPN 3 should be built in a non-root Mac OS X account.
Make sure that Xcode is installed with optional command-line tools.
(These instructions have been tested with Xcode 4.6).

Create a directory ~/src and ~/src/mac.  This can be done with the
command:

  mkdir -p ~/src/mac

Expand the OpenVPN 3 tarball:

 cd ~/src
 tar xf openvpn3.tar.gz

Export the shell variable O3 to point to the OpenVPN 3 top level directory:

 export O3=~/src/openvpn3

Download source tarballs (.tar.gz or .tgz) for these dependency libraries
into ~/Downloads

1. Boost -- http://www.boost.org/
2. PolarSSL (1.2.8 or higher) -- https://polarssl.org/
3. Snappy -- https://code.google.com/p/snappy/

See the file ~/src/openvpn3/lib-versions for the expected
version numbers of each dependency.  If you want to use a different
version of the library than listed here, you can edit this file.

Note that while OpenSSL is listed in lib-versions, it is
not required for Mac builds.

Build the dependencies:

  cd ~/src/mac
  OSX_ONLY=1 $O3/scripts/mac/build-all

Now build the OpenVPN 3 client executable:

  cd $O3
  . vars-osx
  . setpath
  cd test/ovpncli
  PSSL=1 SNAP=1 build cli

This will build the OpenVPN 3 client library with a small client wrapper (cli).
It will also statically link in all external dependencies (Boost, PolarSSL, and Snappy),
so "cli" may be distributed to other Macs and will run as a standalone executable.

To view the client wrapper options:

  ./cli -h

To connect:

  ./cli client.ovpn
