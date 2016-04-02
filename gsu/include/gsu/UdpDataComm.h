/*******************************************************************************
 *
 * File: UdpDataComm.h
 * 
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#pragma once

#include "gsi/UdpSocket.h"

namespace gsu
{
	template<class Datagram>
	class UdpDataComm 
	{
        UdpSocket *data_socket_ptr;
		uint16_t data_port;
        uint32_t data_response_timeout; //milliseconds

	public:
		UdpDataComm(std::string recipientA,
				   uint16_t portA,
				   uint32_t response_timeoutA = 0);
		~UdpDataComm();

		std::string data_foreign_address;

		int32_t writeData(Datagram &dataA);
		int32_t readData(Datagram &data_packetA);
	};

template<class Datagram>
	UdpDataComm<Datagram>::UdpDataComm(std::string foreign_addressA,
									   uint16_t portA,
									   uint32_t response_timeoutA) :
		data_foreign_address(foreign_addressA),
		data_port(portA),
		data_response_timeout(response_timeoutA)

	{
		try
		{
            data_socket_ptr = new UdpSocket(data_foreign_address,data_port,data_response_timeout);
		}
		catch(...)
		{
			data_socket_ptr = NULL;
		}
	}

	template<class Datagram>
	UdpDataComm<Datagram>::~UdpDataComm()
	{
		if(data_socket_ptr)
		{
			delete data_socket_ptr;
			data_socket_ptr = NULL;
		}
	}


	template<class Datagram>
	int32_t UdpDataComm<Datagram>::writeData(Datagram &dataA)
	{
		int32_t ret = 0;

		ret  = data_socket_ptr->sendTo(&dataA,sizeof(Datagram),data_foreign_address,data_port);

		return(ret);
	}

	template<class Datagram>
	int32_t UdpDataComm<Datagram>::readData(Datagram &data_packetA)
    {
		int32_t err = -1;
        err = data_socket_ptr->recvFrom(&(data_packetA),sizeof(Datagram),data_foreign_address,data_port);
        return(err);
	}


}	// namespace gsu
