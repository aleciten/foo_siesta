//
// MulticastSocket.cpp
//
// $Id: //poco/1.4/Net/src/MulticastSocket.cpp#1 $
//
// Library: Net
// Package: Sockets
// Module:  MulticastSocket
//
// Copyright (c) 2005-2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
// 
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//


#include "Poco/Net/MulticastSocket.h"
#include "Poco/Net/NetException.h"
#include <cstring>


#if defined(hpux) && defined(_XOPEN_SOURCE_EXTENDED) && defined(POCO_HPUX_IP_MREQ_HACK)
// netinet/in.h does not define struct ip_mreq if
// _XOPEN_SOURCE_EXTENDED is #define'd in HP-UX 11.x 
// versions prior to 11.30. Compile with -DPOCO_HPUX_IP_MREQ_HACK
// if you experience problems.
struct ip_mreq 
{
	struct in_addr imr_multiaddr;
	struct in_addr imr_interface;
};
#endif


// some Unix variants don't have IPV6_ADD_MEMBERSHIP/IPV6_DROP_MEMBERSHIP
#if defined(IPV6_JOIN_GROUP) && !defined(IPV6_ADD_MEMBERSHIP)
#define IPV6_ADD_MEMBERSHIP  IPV6_JOIN_GROUP
#define IPV6_DROP_MEMBERSHIP IPV6_LEAVE_GROUP
#endif


namespace Poco {
namespace Net {


MulticastSocket::MulticastSocket()
{
}


MulticastSocket::MulticastSocket(IPAddress::Family family): DatagramSocket(family)
{
}


MulticastSocket::MulticastSocket(const SocketAddress& address, bool reuseAddress): DatagramSocket(address, reuseAddress)
{
}


MulticastSocket::MulticastSocket(const Socket& socket): DatagramSocket(socket)
{
}


MulticastSocket::~MulticastSocket()
{
}


MulticastSocket& MulticastSocket::operator = (const Socket& socket)
{
	DatagramSocket::operator = (socket);
	return *this;
}


void MulticastSocket::setInterface(const NetworkInterface& interfc)
{
	if (!interfc.supportsIPv6())
	{
		impl()->setOption(IPPROTO_IP, IP_MULTICAST_IF, interfc.address());
	}
	else
	{
#if defined(POCO_HAVE_IPv6)
		impl()->setOption(IPPROTO_IPV6, IPV6_MULTICAST_IF, interfc.index());
#endif
	}
}

	
NetworkInterface MulticastSocket::getInterface() const
{
	try
	{
		IPAddress addr;
		impl()->getOption(IPPROTO_IP, IP_MULTICAST_IF, addr);
		return NetworkInterface::forAddress(addr);
	}
	catch (Poco::Exception&)
	{
#if defined(POCO_HAVE_IPv6)
		int ix;
		impl()->getOption(IPPROTO_IPV6, IPV6_MULTICAST_IF, ix);
		return NetworkInterface::forIndex(ix);
#else
		throw;
#endif
	}
}

	
void MulticastSocket::setLoopback(bool flag)
{
	if (address().af() == AF_INET)
	{
		unsigned char uflag = flag ? 1 : 0;
		impl()->setOption(IPPROTO_IP, IP_MULTICAST_LOOP, uflag);
	}
	else
	{
#if defined(POCO_HAVE_IPv6)
		unsigned uflag = flag ? 1 : 0;
		impl()->setOption(IPPROTO_IPV6, IPV6_MULTICAST_LOOP, uflag);
#endif
	}
}

	
bool MulticastSocket::getLoopback() const
{
	bool flag = false;
	if (address().af() == AF_INET)
	{
		unsigned char uflag;
		impl()->getOption(IPPROTO_IP, IP_MULTICAST_LOOP, uflag);
		flag = uflag != 0;
	}
	else
	{
#if defined(POCO_HAVE_IPv6)
		unsigned uflag;
		impl()->getOption(IPPROTO_IPV6, IPV6_MULTICAST_LOOP, uflag);
		flag = uflag != 0;
#endif
	}
	return flag;
}

	
void MulticastSocket::setTimeToLive(unsigned value)
{
	if (address().af() == AF_INET)
	{
		unsigned char ttl = (unsigned char) value;
		impl()->setOption(IPPROTO_IP, IP_MULTICAST_TTL, ttl);
	}
	else
	{
#if defined(POCO_HAVE_IPv6)
		impl()->setOption(IPPROTO_IPV6, IPV6_MULTICAST_HOPS, value);
#endif
	}
}

	
unsigned MulticastSocket::getTimeToLive() const
{
	unsigned ttl(0);
	if (address().af() == AF_INET)
	{
		unsigned char cttl;
		impl()->getOption(IPPROTO_IP, IP_MULTICAST_TTL, cttl);
		ttl = cttl;
	}
	else
	{
#if defined(POCO_HAVE_IPv6)
		impl()->getOption(IPPROTO_IPV6, IPV6_MULTICAST_HOPS, ttl);
#endif
	}
	return ttl;
}

	
void MulticastSocket::joinGroup(const IPAddress& groupAddress)
{
	NetworkInterface intf;
	joinGroup(groupAddress, intf);
}

	
void MulticastSocket::joinGroup(const IPAddress& groupAddress, const NetworkInterface& interfc)
{
	if (groupAddress.af() == AF_INET)
	{
		struct ip_mreq mr;
		std::memcpy(&mr.imr_multiaddr, groupAddress.addr(), groupAddress.length());
		std::memcpy(&mr.imr_interface, interfc.address().addr(), interfc.address().length());
		impl()->setRawOption(IPPROTO_IP, IP_ADD_MEMBERSHIP, &mr, sizeof(mr));
	}
	else
	{
#if defined(POCO_HAVE_IPv6)
		struct ipv6_mreq mr;
		std::memcpy(&mr.ipv6mr_multiaddr, groupAddress.addr(), groupAddress.length());
		mr.ipv6mr_interface = interfc.index();
		impl()->setRawOption(IPPROTO_IPV6, IPV6_ADD_MEMBERSHIP, &mr, sizeof(mr));
#endif
	}
}

	
void MulticastSocket::leaveGroup(const IPAddress& groupAddress)
{
	NetworkInterface intf;
	leaveGroup(groupAddress, intf);
}

	
void MulticastSocket::leaveGroup(const IPAddress& groupAddress, const NetworkInterface& interfc)
{
	if (groupAddress.af() == AF_INET)
	{
		struct ip_mreq mr;
		std::memcpy(&mr.imr_multiaddr, groupAddress.addr(), groupAddress.length());
		std::memcpy(&mr.imr_interface, interfc.address().addr(), interfc.address().length());
		impl()->setRawOption(IPPROTO_IP, IP_DROP_MEMBERSHIP, &mr, sizeof(mr));
	}
	else
	{
#if defined(POCO_HAVE_IPv6)
		struct ipv6_mreq mr;
		std::memcpy(&mr.ipv6mr_multiaddr, groupAddress.addr(), groupAddress.length());
		mr.ipv6mr_interface = interfc.index();
		impl()->setRawOption(IPPROTO_IPV6, IPV6_DROP_MEMBERSHIP, &mr, sizeof(mr));
#endif
	}
}


} } // namespace Poco::Net
