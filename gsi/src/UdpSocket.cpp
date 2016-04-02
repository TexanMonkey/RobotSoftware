/*******************************************************************************
 *
 * File: UdpSocket.cpp
 *	Generic System Interface UDP Sockets wrapper
 *
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#include <gsi/UdpSocket.h>

// @TODO: create a network class to hold byte swap info

bool do_byte_swap = (1 != ntohs(1));

float hostToNetFloat(float val)
{
	return netToHostFloat(val);
}

float netToHostFloat(float val)
{
	if (do_byte_swap)
	{
		uint32_t in_val;
		float *fp = (float *)(&in_val);
		*fp = val;
		in_val = htonl(in_val);
		return (*fp);
	}
	else
	{
		return val;
	}
}


namespace gsi
{




/*******************************************************************************
 *
 *  Construct a UDP socket
 *  @exception std::exception thrown if unable to create UDP socket
 *
 *******************************************************************************/
UdpSocket::UdpSocket(uint32_t timeoutmsA) throw (std::exception)
{
	createSocket();
	setBroadcast();
	if (timeoutmsA > 0)
	{
		setTimeout(timeoutmsA);
	}
}

/*******************************************************************************
 *
 *  Construct a UDP socket with the given local port
 *  @param localPort local port
 *  @exception std::exception thrown if unable to create UDP socket
 *
 *******************************************************************************/
UdpSocket::UdpSocket(uint16_t localPort, uint32_t timeoutmsA)
    throw (std::exception)
{
	createSocket();
	setLocalPort(localPort);
	setBroadcast();
	if (timeoutmsA > 0)
	{
		setTimeout(timeoutmsA);
	}
}

/*******************************************************************************
 *
 *  Construct a UDP socket with the given local port and address
 *  @param localAddress local address
 *  @param localPort local port
 *  @exception std::exception thrown if unable to create UDP socket
 *
 *******************************************************************************/
UdpSocket::UdpSocket(const std::string &local_addressA, uint16_t local_portA,
    uint32_t timeoutmsA) throw (std::exception)
{
	createSocket();
	setLocalAddressAndPort(local_addressA, local_portA);
	setBroadcast();
	if (timeoutmsA > 0)
	{
		setTimeout(timeoutmsA);
	}
}

/*******************************************************************************
 *
 * Used by all of the constructors to create the socket
 *  @return 0 on success, -1 on error
 *
 *******************************************************************************/
int32_t UdpSocket::createSocket()
{
	int32_t err = 0;
#ifdef WIN32
	if (!g_socket_initialized)
	{
		WORD wVersionRequested;
		WSADATA wsaData;

		wVersionRequested = MAKEWORD(2, 0); // Request WinSock v2.0
		if (WSAStartup(wVersionRequested, &wsaData) != 0)
		{ // Load WinSock DLL
			throw std::exception("Unable to load WinSock DLL");
		}
		g_socket_initialized = true;
	}
#endif

	// Make a new socket
	if ((socket_desc = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
	{
		err = -1;
	}
	return (err);
}

/*******************************************************************************
 *
 ******************************************************************************/
int32_t UdpSocket::setBroadcast()
{
	int32_t err = 0;
	// If this fails, we'll hear about it when we try to send.  This will allow 
	// system that cannot broadcast to continue if they don't plan to broadcast
	int broadcast_permission = 1;

	setsockopt(socket_desc, SOL_SOCKET, SO_BROADCAST,
	    (raw_type *) &broadcast_permission, sizeof(broadcast_permission));

	return (err);
}

/*******************************************************************************
 *
 ******************************************************************************/
int32_t UdpSocket::setTimeout(uint32_t msA)
{
	int32_t err = 0;
#ifdef WIN32
	DWORD timeout = msA;
	setsockopt(socket_desc, SOL_SOCKET, SO_RCVTIMEO,(char *)&timeout, sizeof(timeout));
#else
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = msA * 1000;
	err = setsockopt(socket_desc, SOL_SOCKET, SO_RCVTIMEO, (raw_type *) &tv,
	    sizeof(tv));
#endif
	return (err);
}

/*******************************************************************************
 *
 *  Unset foreign address and port
 *  @return true if disassociation is successful
 *  @return 0 on success
 *
 *******************************************************************************/
int32_t UdpSocket::disconnect()
{
	int32_t err = 0;
	sockaddr_in nullAddr;
	memset(&nullAddr, 0, sizeof(nullAddr));
	nullAddr.sin_family = AF_UNSPEC;

	// Try to disconnect
	if (::connect(socket_desc, (sockaddr *) &nullAddr, sizeof(nullAddr)) < 0)
	{
#ifdef WIN32
		if (errno != WSAEAFNOSUPPORT)
		{
#else
		if (errno != EAFNOSUPPORT)
		{
#endif
			err = -1;
		}
	}
	return (err);
}

/*******************************************************************************
 *
 *  Send the given buffer as a UDP datagram to the
 *  specified address/port
 *  @param buffer buffer to be written
 *  @param bufferLen number of bytes to write
 *  @param foreignAddress address (IP address or name) to send to
 *  @param foreignPort port number to send to
 *  @return 0 on success
 *
 *******************************************************************************/
int32_t UdpSocket::sendTo(const void *buffer, uint32_t bufferLen,
    const std::string &foreignAddress, uint16_t foreignPort)
{
	int32_t err = 0;
	sockaddr_in destAddr;
	fillAddr(foreignAddress, foreignPort, destAddr);

	// Write out the whole buffer as a single message.
	if (sendto(socket_desc, (raw_type *) buffer, bufferLen, 0,
	    (sockaddr *) &destAddr, sizeof(destAddr)) != (int) bufferLen)
	{
		err = -1;
	}
	return (err);
}

/*******************************************************************************
 *
 *  Read read up to bufferLen bytes data from this socket.  The given buffer
 *  is where the data will be placed
 *  @param buffer buffer to receive data
 *  @param bufferLen maximum number of bytes to receive
 *  @param sourceAddress address of datagram source
 *  @param sourcePort port of data source
 *  @param timeout amount of time to wait for data before returning
 *  @return number of bytes received and -1 for error
 *
 ******************************************************************************/
int32_t UdpSocket::recvFrom(void *bufferA, uint32_t buffer_lengthA,
    std::string &source_addressA, uint16_t &source_portA, uint32_t timeoutA)
{
	sockaddr_in clntAddr;
	int addrLen = sizeof(clntAddr);
	int err = 0;
	int rtn;
	fd_set fds;
	int n;
	struct timeval tv;

	// Set up the file descriptor set.
	FD_ZERO(&fds);
	FD_SET(socket_desc, &fds);

	// Set up the struct timeval for the timeout.
	tv.tv_sec = timeoutA;
	tv.tv_usec = 0;

	// Wait until timeout or data received.
	n = select(socket_desc, &fds, NULL, NULL, &tv);
	if (n == 0)
	{
		err = -1;
	}
	else if (n == -1)
	{
		err = -2;
	}

	if (!err)
	{
#if defined (LINUX)
		rtn = recvfrom(socket_desc, (raw_type*) bufferA, buffer_lengthA, 0,
		    (sockaddr*) &clntAddr, (socklen_t*) &addrLen);
#else
		rtn = recvfrom(socket_desc, (raw_type*) bufferA, buffer_lengthA, 0,
		    (sockaddr*) &clntAddr, (unsigned int*) &addrLen);
#endif
		if (rtn < 0)
		{
			err = errno;
			switch (err)
			{
				case 0:
				case EAGAIN:
				case ETIMEDOUT:
					err = -1;
					break;

				default:

					fprintf(stderr, "Socket Error : %s\n", strerror(err));
					break;
			}
		}
		if (!err)
		{
			// vvv RJP: Get source by domain name instead of by IP address
			//			getnameinfo((struct sockaddr *)&clntAddr, sizeof (struct sockaddr), socket_source_name, 256, NULL, 0, 0);
			//			source_addressA = socket_source_name;
			source_addressA = inet_ntoa(clntAddr.sin_addr);
			// ^^^ RJP:
			source_portA = ntohs(clntAddr.sin_port);
		}
	}
	if (err)
		return (err);
	else
		return (rtn);
}

/*******************************************************************************
 *
 *  Read read up to bufferLen bytes data from this socket.  The given buffer
 *  is where the data will be placed
 *  @param buffer buffer to receive data
 *  @param bufferLen maximum number of bytes to receive
 *  @param sourceAddress address of datagram source
 *  @param sourcePort port of data source
 *  @param timeout amount of time to wait for data before returning
 *  @return number of bytes received and -1 for error
 *
 ******************************************************************************/
int32_t UdpSocket::recvFrom(void *bufferA, uint32_t buffer_lengthA,
    std::string &source_addressA, uint16_t &source_portA)
{
	sockaddr_in clntAddr;
	int addrLen = sizeof(clntAddr);
	int err = 0;
	int rtn;

#if defined(LINUX)
	rtn = recvfrom(socket_desc, (raw_type*) bufferA, buffer_lengthA, 0,
	    (sockaddr*) &clntAddr, (socklen_t*) &addrLen);
#else
	rtn = recvfrom(socket_desc, (raw_type*) bufferA, buffer_lengthA, 0,
	    (sockaddr*) &clntAddr, (unsigned int*) &addrLen);
#endif

	if (rtn < 0)
	{
		err = errno;
		switch (err)
		{
			case 0:
			case EAGAIN:
			case ETIMEDOUT:
				return (-1);
			default:

				fprintf(stderr, "Socket Error : %s\n", strerror(err));
				break;
		}
	}
	// vvv RJP: Get source by domain name instead of by IP address
	//		getnameinfo((struct sockaddr *)&clntAddr, sizeof (struct sockaddr), socket_source_name, 256, NULL, 0, 0);
	//		source_addressA = socket_source_name;
	source_addressA = inet_ntoa(clntAddr.sin_addr);
	// ^^^ RJP:
	source_portA = ntohs(clntAddr.sin_port);

	return rtn;
}

/*******************************************************************************
 *
 *   Set the multicast TTL
 *   @param multicastTTL multicast TTL
 *   @return 0 on success
 *
 *******************************************************************************/
int32_t UdpSocket::setMulticastTTL(unsigned char multicastTTL)
{
	int32_t err = 0;
	if (setsockopt(socket_desc, IPPROTO_IP, IP_MULTICAST_TTL,
	    (raw_type *) &multicastTTL, sizeof(multicastTTL)) < 0)
	{
		err = -1;
	}
	return (err);
}

/*******************************************************************************
 *
 *  Join the specified multicast group
 *   @param multicastGroup multicast group address to join
 *   @return 0 on success
 *
 *******************************************************************************/
int32_t UdpSocket::joinGroup(const std::string &multicastGroup)
{

	int32_t err = 0;
	struct ip_mreq multicastRequest;

	multicastRequest.imr_multiaddr.s_addr = inet_addr(
	    (char *) multicastGroup.c_str());
	multicastRequest.imr_interface.s_addr = htonl(INADDR_ANY);
	if (setsockopt(socket_desc, IPPROTO_IP, IP_ADD_MEMBERSHIP,
	    (raw_type *) &multicastRequest, sizeof(multicastRequest)) < 0)
	{
		err = -1;
	}
	return (err);
}

/*******************************************************************************
 *
 *   Leave the specified multicast group
 *   @param multicastGroup multicast group address to leave
 *   @return 0 on success
 *
 *******************************************************************************/
int32_t UdpSocket::leaveGroup(const std::string &multicastGroup)
{
	int32_t err = 0;
	struct ip_mreq multicastRequest;

	multicastRequest.imr_multiaddr.s_addr = inet_addr(
	    (char *) multicastGroup.c_str());
	multicastRequest.imr_interface.s_addr = htonl(INADDR_ANY);
	if (setsockopt(socket_desc, IPPROTO_IP, IP_DROP_MEMBERSHIP,
	    (raw_type *) &multicastRequest, sizeof(multicastRequest)) < 0)
	{
		err = -1;
	}
	return (err);
}

/*******************************************************************************
 *
 * Function to fill in address structure given an address and port
 *
 *******************************************************************************/
int32_t UdpSocket::fillAddr(const std::string &addressA, uint16_t portA,
    sockaddr_in &addrA)
{
	int32_t err = 0;
	memset(&addrA, 0, sizeof(addrA)); // Zero out address structure
	addrA.sin_family = AF_INET; // Internet address
	addrA.sin_addr.s_addr = inet_addr((char *) addressA.c_str());
	addrA.sin_port = htons(portA); // Assign port in network byte order
	return (err);
}

/*******************************************************************************
 *
 *  Set the local port to the specified port and the local address
 *  to any interface
 *  @param localPort local port
 *  @return foreign port or -1 for error
 *
 *******************************************************************************/
int32_t UdpSocket::setLocalPort(uint16_t portA)
{
	int32_t err = 0;
	// Bind the socket to its port
	sockaddr_in localAddr;
	memset(&localAddr, 0, sizeof(localAddr));
	localAddr.sin_family = AF_INET;
	localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	localAddr.sin_port = htons(portA);

	if (bind(socket_desc, (sockaddr *) &localAddr, sizeof(sockaddr_in)) < 0)
	{
		err = -1;
	}
	return (err);
}

/*******************************************************************************
 *
 *   Set the local port to the specified port and the local address
 *   to the specified address.  If you omit the port, a random port 
 *   will be selected.
 *   @param localAddress local address
 *   @param localPort local port
 *   @return foreign port or -1 for error
 *   
 ******************************************************************************/
int32_t UdpSocket::setLocalAddressAndPort(const std::string &addressA,
    uint16_t portA)
{
	int32_t err = 0;
	// Get the address of the requested host
	sockaddr_in localAddr;
	fillAddr(addressA, portA, localAddr);

	if (bind(socket_desc, (sockaddr *) &localAddr, sizeof(sockaddr_in)) < 0)
	{
		err = -1;
	}
	return (err);
}
} // namespace gsi
