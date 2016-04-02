/*******************************************************************************
 *
 * File: UdpBufferedTransmitter.h
 * 
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#pragma once

#include <string>

#include "gsi/Thread.h"
#include "gsi/Mutex.h"
#include "gsi/UdpSocket.h"

#include "gsu/UdpBufferedDefs.h"

namespace gsi
{

/**********************************************************************
 *
 **********************************************************************/
class UdpBufferedTransmitter : public Thread
{
	public:
		UdpBufferedTransmitter(std::string name, std::string dest_host,
			uint16_t dest_port, uint16_t max_length, uint16_t max_count,
			double period, int32_t priority) ;
		~UdpBufferedTransmitter();
		
		void run(void);
		void init();
		void putPacket(uint16_t data_type, uint16_t data_flags, 
			uint16_t data_length, const char *data);

	protected:
		void doPeriodic();

	private:
		void sendPacket(void);
		
		std::string dest_host;
		int32_t dest_port;
		UdpSocket *dest_socket;
		
		uint16_t max_packet_size;
		uint16_t max_packet_count;
		uint16_t head_idx;
		uint16_t tail_idx;
		
		uint8_t *buffer;
		
		UdpBufferedPacket *send_packet;
		
		Mutex buffer_lock;
};

} // namespace gsi
