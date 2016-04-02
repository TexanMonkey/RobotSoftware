/*******************************************************************************
 *
 * File: UdpBufferedDefs.h
 * 
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#pragma once

namespace gsi
{
// I want the buffer header to be 8 bytes (or a
// multiple of 8 bytes) to avoid any issues with
// byte alignement and structure padding
static const uint16_t UDP_BUFFERED_HEADER_SIZE = 8;

// This is kind of a sync pattern and a version
// number combined, if later versions are needed
// just change to a new sync pattern
static const uint16_t UDP_BUFFERED_SYNC_0 =	0x7AC0;

struct UdpBufferedPacket
{
	uint16_t sync;
	uint16_t type;    // user provided type
	uint16_t flags;   // user provided flags
	uint16_t length;
	char data[4];  // data will be of size 'length'
};

}
