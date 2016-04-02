/*******************************************************************************
 *
 * File: UdpBufferedReceiver.h
 * 
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#pragma once

#include <stdio.h>
#include <stdlib.h>

#include <string>

#include "gsi/UdpSocket.h"
#include "gsi/Thread.h"
#include "gsi/Mutex.h"

#include "gsu/UdpBufferedDefs.h"

namespace gsi
{

/**********************************************************************
 *
 **********************************************************************/
class UdpBufferedReceiver : public Thread
{
	public:
		UdpBufferedReceiver(std::string name, std::string src_host,
			uint16_t src_port, uint16_t max_length, uint16_t max_count,
			double interval, int32_t priority);
		~UdpBufferedReceiver();
		
		void run(void);
		
		bool getPacket(uint16_t *type, uint16_t *flags, uint16_t *data_length, char *data);

	protected:
		void doPeriodic();

	private:
		void init();
		void receivePacket(void);

		UdpSocket *src_socket;
		
		std::string src_host;
		int32_t src_port;
		
		uint16_t max_packet_size;
		uint16_t max_packet_count;
		uint16_t head_idx;
		uint16_t tail_idx;
		
		double pkt_interval;
		
		uint8_t *buffer;
		
		UdpBufferedPacket *receive_packet;
		
		Mutex buffer_lock;
};

} // namespace gsi
