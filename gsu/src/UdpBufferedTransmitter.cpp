/*******************************************************************************
 *
 * File: UdpBufferedTransmitter.cpp
 *
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#include "gsu/UdpBufferedTransmitter.h"

void hexDumpp(char *buf, uint32_t len)
{
    for(uint32_t i = 0; i < len; i++)
    {
        if ((i > 0) && (i%16==0)) printf("\n");
        printf("%02X ", (unsigned char)(buf[i]));
    }
    printf("\n");
}

namespace gsi
{

/*******************************************************************************
 *
 ******************************************************************************/
UdpBufferedTransmitter::UdpBufferedTransmitter(std::string name, std::string host,
	uint16_t port, uint16_t max_length, uint16_t max_count,
	double period, int32_t priority) :
	Thread(name)
{
	dest_socket = NULL;
	
	dest_host = host;
	dest_port = port;
	
	max_packet_size = max_length + UDP_BUFFERED_HEADER_SIZE;
	max_packet_count = max_count + 2;  // must have at least 2 for rolling
	
	send_packet = NULL;
	buffer = NULL;

	head_idx = 0;
	tail_idx = 0;

    init();
}

/*******************************************************************************
 *
 ******************************************************************************/
UdpBufferedTransmitter::~UdpBufferedTransmitter()
{
	printf("DashboardComm::~DashboardComm\n");
	
	if (dest_socket != NULL)
	{
		delete dest_socket;
		dest_socket = NULL;
	}

	// free buffer
	// free send packet
}

/*******************************************************************************
 *
 ******************************************************************************/
void UdpBufferedTransmitter::init()
{
	if ((dest_port <= 0) || (dest_host.length() < 1))
	{
		printf("ERROR: UdpTransmitter did not attempt to create socket, invalid config\n");
		return;
	}
	
	dest_socket = new UdpSocket();
	if (dest_socket == NULL)
	{
		printf("ERROR: UdpTransmitter could not create socket (ret=%d, err = %d)\n", (int)dest_socket, errno);
		return;
	}
	else
	{
		printf("UdpTransmitter socket created for sending to %s:%d\n", dest_host.c_str(), (int)dest_port);
	}
	
	send_packet = new UdpBufferedPacket;
	
	buffer = (uint8_t *) new uint8_t[max_packet_size * max_packet_count];
	head_idx = 0;
	tail_idx = 0;
}

/*******************************************************************************
 *
 ******************************************************************************/
void UdpBufferedTransmitter::run(void)
{
    printf("UdpBufferedTransmitter::running\n");
    while (!isStopRequested())
	{
		try
		{
			doPeriodic();
			
//			//
//			// Wait until the next update cycle should begin
//			//
//			double wait_time = (control_phase_time + (control_phase_count
//			    * control_period)) - GetTime();
//	
//			if (wait_time > (3* control_period))
//			{
//				printf("PeriodicControl::run -- bad wait time of %f, waiting for period\n",
//					wait_time);
//				
				sleep(0.01);
//			}
//			else if (wait_time >= MIN_SLEEP_TIME)
//			{
//				Wait(wait_time);
//			}
//			else
//			{
//				Wait(MIN_SLEEP_TIME);
//			}
		}
		catch (...)
		{
			printf("EXCEPTION: caught in PeriodicControl::run -- \n");	
		}
	}	
}

/*******************************************************************************
 *
 ******************************************************************************/
void UdpBufferedTransmitter::doPeriodic()
{
	UdpBufferedPacket *pkt = NULL;
    uint32_t send_length;

	bool ready = ((dest_socket >= 0) && (buffer != NULL) && (send_packet != NULL));
	
	while (ready)
	{
		buffer_lock.lock();
		try
		{
			if (head_idx == tail_idx)
			{
				ready = false;
			}
			else
			{
				pkt = (UdpBufferedPacket *)&(buffer[tail_idx * max_packet_size]);
                send_length = pkt->length + UDP_BUFFERED_HEADER_SIZE;

                send_packet->sync = htons(pkt->sync);
                send_packet->flags = htons(pkt->flags);
                send_packet->type = htons(pkt->type);
                send_packet->length = htons(pkt->length);
                memcpy(send_packet->data, pkt->data, pkt->length);
			
				tail_idx = (tail_idx + 1) % max_packet_count;
			}
		}
		catch (...) {}
		buffer_lock.unlock();
		
		if (ready)
		{
			hexDumpp((char *)send_packet, send_length);
            dest_socket->sendTo(send_packet, send_length,
				dest_host, dest_port);

            printf("UdpBufferedTransmitter::doPeriodic sent  %d\n", (int)send_length);
        }
	}
}

/*******************************************************************************
 *
 ******************************************************************************/
void UdpBufferedTransmitter::putPacket(uint16_t data_type, uint16_t data_flags,
	uint16_t data_length, const char *data)
{
	if (data_length + UDP_BUFFERED_HEADER_SIZE > max_packet_size)
	{
		return;
	}

	buffer_lock.lock();
	try
	{			
		UdpBufferedPacket *pkt = (UdpBufferedPacket *)&(buffer[head_idx * max_packet_size]);
		
		pkt->sync = UDP_BUFFERED_SYNC_0;
		pkt->flags = data_flags;
		pkt->type = data_type;
		pkt->length = data_length;
		memcpy(pkt->data, data, data_length);
		
		head_idx = (head_idx + 1) % max_packet_count;
		
		// if the buffer is full, the oldest is lost
		if (head_idx == tail_idx)
		{
			tail_idx = (tail_idx + 1) % max_packet_count;
		}
	}
	catch (...) {}
	buffer_lock.unlock();
}

} // namespace gsi
