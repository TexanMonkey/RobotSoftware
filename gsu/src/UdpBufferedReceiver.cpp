/*******************************************************************************
 *
 * File: UdpBufferedReceiver.cpp
 *
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#include "gsu/UdpBufferedReceiver.h"


namespace gsi
{

/*******************************************************************************
 *
 ******************************************************************************/
UdpBufferedReceiver::UdpBufferedReceiver(std::string name, std::string host, 
	uint16_t port, uint16_t max_length, uint16_t max_count, double interval, 
	int32_t priority) :
	Thread(name)
{
	head_idx = 0;
	tail_idx = 0;
	src_socket = NULL;

	buffer = NULL;
	receive_packet = NULL;
	
	src_host = host;
	src_port = port;
	pkt_interval = interval;
	
	max_packet_size = max_length + UDP_BUFFERED_HEADER_SIZE;
	max_packet_count = max_count;  
	
	// must have at least 2 packets for rolling
	if (max_packet_count < 2)
	{
		max_packet_count = 2;
	}
}

/*******************************************************************************
 *
 ******************************************************************************/
UdpBufferedReceiver::~UdpBufferedReceiver()
{
	printf("UdpReceiver::~UdpReceiver\n");
	
	// close socket
	if (src_socket != NULL)
	{
		delete src_socket;
		src_socket = NULL;
	}

	if (buffer != NULL)
	{
		delete buffer;
		buffer = NULL;
	}

	if (receive_packet != NULL)
	{
		delete receive_packet;
		receive_packet = NULL;
	}
}

/*******************************************************************************
 *
 ******************************************************************************/
void UdpBufferedReceiver::init()
{
	if ((src_port <= 0) || (src_host.length() < 1))
	{
		printf("ERROR: UdpReceiver did not attempt to create socket, invalid config\n");
		return;
	}
	
	src_socket = new UdpSocket(src_host, src_port);
	if (src_socket == NULL)
	{
		printf("ERROR: UdpReceiver could not create socket (ret=%d, err = %d)\n", (int)src_socket, errno);
		return;
	}

	receive_packet = new UdpBufferedPacket();
	if (receive_packet == NULL) 
	{
		printf("ERROR: UdpReceiver could not allocate receive packet\n");
		delete src_socket;
		src_socket = NULL;
		return;
	}

	buffer = new uint8_t[max_packet_size * max_packet_count];
	if (buffer == NULL)
	{
		printf("ERROR: UdpReceiver could not allocate buffer space\n");
		delete src_socket;
		src_socket = NULL;
		delete receive_packet;
		receive_packet = NULL;
		return;
	}
	
    printf("UdpReceiver socket created for receiving %s:%d\n", src_host.c_str(), (int)src_port);
}

/*******************************************************************************
 *
 ******************************************************************************/
void UdpBufferedReceiver::run(void)
{
	std::string from;
	uint16_t fromlen;

	init();
	
	if ((src_socket == NULL) || (buffer == NULL) || (receive_packet == NULL))
	{
		printf("UdpReceiver::run: cannot run, initialization failed\n");
		return;
	}

    printf("UdpReceiver::running\n");
    while (!isStopRequested())
	{
		try
		{
            uint32_t ret = src_socket->recvFrom((void *)receive_packet,
                 max_packet_size, from, fromlen);

            if (ret >= 0)
			{
                buffer_lock.lock();
				try
				{	
					if (ntohs(receive_packet->sync) == UDP_BUFFERED_SYNC_0)
					{
						UdpBufferedPacket *pkt = (UdpBufferedPacket *)&(buffer[head_idx * max_packet_size]);
						pkt->sync 		= ntohs(receive_packet->sync);
						pkt->flags 	    = ntohs(receive_packet->flags);
						pkt->type 		= ntohs(receive_packet->type);
						pkt->length 	= ntohs(receive_packet->length);
	
                        memcpy(pkt->data, receive_packet->data, pkt->length);
                        pkt->data[pkt->length] = 0;

						head_idx = (head_idx + 1) % max_packet_count;
		
						// if the buffer is full, the oldest is lost
						if (head_idx == tail_idx)
						{
							tail_idx = (tail_idx + 1) % max_packet_count;
						}
					}
				}
				catch (...) {}
				buffer_lock.unlock();
			}
			else
			{
				sleep(pkt_interval);
			}
		}
		catch (...)
		{
			printf("EXCEPTION: caught in UdpReceiver::run -- \n");	
		}
	}	
}

/*******************************************************************************
 *
 ******************************************************************************/
bool UdpBufferedReceiver::getPacket(uint16_t *data_type, uint16_t *data_flags, 
	uint16_t *data_length, char *data)
{
	bool ret_val = false;
	
	buffer_lock.lock();
	try
	{
		if (tail_idx != head_idx)
		{
			UdpBufferedPacket *pkt = (UdpBufferedPacket *)&(buffer[tail_idx * max_packet_size]);
	
            *data_type   = pkt->type;
            *data_length = pkt->length;
            *data_flags  = pkt->flags;
            memcpy(data, pkt->data, *data_length);

			tail_idx = (tail_idx + 1) % max_packet_count;
	
            ret_val = true;
		}
	}
    catch (...)
    {
        printf("UdpBufferedReceiver::getPacket error\n");
    }
	buffer_lock.unlock();
	
	return ret_val;
}

} // namespace gsi
