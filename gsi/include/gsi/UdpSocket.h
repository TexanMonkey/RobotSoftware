/*******************************************************************************
 *
 * File: UdpSocket.h
 *	Generic System Interface Time wrapper
 *
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#pragma once

#include <exception>
#include <string>

#if defined (PTHREADS)
#include <stdint.h>
#include <sys/types.h>       // For data types
#include <sys/socket.h>      // For socket(), connect(), send(), and recv()
#include <netdb.h>           // For gethostbyname()
#include <arpa/inet.h>       // For inet_addr()
#include <unistd.h>          // For close()
#include <netinet/in.h>      // For sockaddr_in
#include <string.h>
#include <errno.h>
#include <stdio.h>

typedef void raw_type; // Type used for raw data on this platform

#elif defined(VXWORKS) || defined(_WRS_KERNEL)
#include <sys/types.h>       // For data types
#include <inetLib.h>
#include <selectLib.h>
#include <sockLib.h>
#include <usrLib.h>
#include <unistd.h>          // For close()//#include <netinet/in.h>      // For sockaddr_in
#include <string.h>
typedef char raw_type; // Type used for raw data on this platform

#elif defined(_WINDOWS)
#include <stdint.h>
#include <winsock.h>         // For socket(), connect(), send(), and recv()
typedef int socklen_t;
typedef char raw_type; // Type used for raw data on this platform

#pragma warning(disable : 4290)

#else

#endif


extern float netToHostFloat(float val);
extern float hostToNetFloat(float val);


namespace gsi
{
#ifdef WIN32
static bool g_socket_initialized = false;
#endif

/*******************************************************************************
 *
 ******************************************************************************/
class UdpSocket
{
	public:
		UdpSocket(uint32_t timeoutmsA = 0) throw (std::exception);

		UdpSocket(uint16_t localPortA, uint32_t timeoutmsA = 0)
		    throw (std::exception);

		UdpSocket(const std::string &localAddressA, uint16_t localPortA,
		    uint32_t timeoutmsA = 0) throw (std::exception);

		int32_t createSocket();

		int32_t disconnect();

		int32_t sendTo(const void *bufferA, uint32_t buffer_lengthA,
		    const std::string &foreign_addressA, uint16_t foreign_portA);

		int32_t recvFrom(void *bufferA, uint32_t buffer_lengthA,
		    std::string &source_addressA, uint16_t &source_portA);

		int32_t recvFrom(void *bufferA, uint32_t buffer_lengthA,
		    std::string &source_addressA, uint16_t &source_portA,
		    uint32_t timeoutA);

		int32_t setMulticastTTL(unsigned char multicastTTLA);

		int32_t joinGroup(const std::string &multicast_groupA);

		int32_t leaveGroup(const std::string &multicast_groupA);

		int32_t setLocalPort(uint16_t localPort);

		int32_t setLocalAddressAndPort(const std::string &localAddress,
		    uint16_t localPort = 0);

	private:
		int32_t setBroadcast();
		int32_t setTimeout(uint32_t msA);

		int32_t fillAddr(const std::string &addressA, uint16_t portA,
		    sockaddr_in &addrA);

		char socket_source_name[256];
		int32_t socket_desc; // Socket descriptor
};

} //namespace gsi
