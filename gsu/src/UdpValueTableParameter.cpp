/*******************************************************************************
 *
 * File: UdpValueTableParameter.cpp
 * 
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#include <string>
#include <vector>

#include <gsi/UdpSocket.h>

#include "gsu/UdpValueTableParameter.h"

namespace gsu
{

/*******************************************************************************
 *
 * The default constructor initializes this parameter with a type of NONE,
 * a length of 0, and a value of NULL
 *
 ******************************************************************************/
UdpValueTableParameter::UdpValueTableParameter()
{
	type = TYPE_NONE;
	length = 0;
	value.blob = NULL;
}

/*******************************************************************************
 *
 * This constructor is for creating a parameter of type Binary Large Object
 * (BLOB) with the provided bytes and length.
 *
 * NOTE: If the bytes are sent across a network, there is not handling of
 *		 byte order, that is left to the user
 *
 * @param	bytes	a pointer to the data
 * @param	len		the number of bytes in this BLOB
 *
 ******************************************************************************/
UdpValueTableParameter::UdpValueTableParameter(void *bytes, uint16_t len)
{
	type = TYPE_BLOB;
	length = 0;
	value.blob = NULL;
	set(bytes, len);
}

/*******************************************************************************
 *
 * This method is for initializing a parameter of type Binary Large Object
 * (BLOB) with the provided bytes and length.
 *
 * NOTE: If the bytes are sent across a network, there is not handling of
 *		 byte order, that is left to the user
 *
 * @param	bytes	a pointer to the data
 * @param	len		the number of bytes in this BLOB
 *
 ******************************************************************************/
void UdpValueTableParameter::init(void *bytes, uint16_t len)
{
	type = TYPE_BLOB;
	set(bytes, len);
}

/*******************************************************************************
 *
 * This method is for setting a parameter of type Binary Large Object
 * (BLOB) with the provided bytes and length. The value is copied into
 * internally managed memory.
 *
 * NOTE: If the bytes are sent across a network, there is not handling of
 *		 byte order, that is left to the user
 *
 * @param	bytes	a pointer to the data
 * @param	len		the number of bytes in this BLOB
 *
 ******************************************************************************/
void UdpValueTableParameter::set(void *bytes, uint16_t len)
{
	if (value.blob != NULL)
	{
		delete value.blob;
		value.blob = NULL;
	}

	length = len;

	if (len > 0)
	{
		value.blob = new uint8_t[len];
		if (value.blob == NULL)
		{
			length = 0;
		}
	}
	else
	{
		value.blob = NULL;
	}

	if (value.blob != NULL)
	{
		memcpy(value.blob, bytes, length);
	}
}

/*******************************************************************************
 *
 * This method converts the current value into bytes that are safe to send
 * across the network, parameters of numeric types are changed to network
 * byte order, BLOBs are sent as is with no byte swapping
 *
 * @param	dest	a pointer to a buffer in which the bytes in network 
 *					byte order should be put
 *
 * @return	the number of bytes put into the destination buffer.
 *
 ******************************************************************************/
uint16_t UdpValueTableParameter::toNetBytes(uint8_t  *dest)
{
	switch (type)
	{
		case TYPE_BOOL:
		{
			// NOTE: transmitted as uint8_t because receiver may
			// declare bools as different sizes
			*((uint8_t *)(dest)) = get<bool>();
		} break;
		
		case TYPE_INT8:
		{
			*((int8_t *)(dest)) = get<int8_t>();			
		} break;
		
		case TYPE_UINT8:
		{
			*((uint8_t *)(dest)) = get<uint8_t>();	
		} break;
		
		case TYPE_INT16:
		{
			*((int16_t *)(dest)) = htons(get<int16_t>());
		} break;
		
		case TYPE_UINT16:
		{
			*((uint16_t *)(dest)) = htons(get<uint16_t>());
		} break;
		
		case TYPE_INT32:
		{
			*((int32_t *)(dest)) = htonl(get<int32_t>());
		} break;
		
		case TYPE_UINT32:
		{
			*((uint32_t *)(dest)) = htonl(get<uint32_t>());
		} break;
		
		case TYPE_FLOAT32:
		{
			float f = get<float>();
			*((uint32_t *)(dest)) = htonl(*((uint32_t *)(&f)));
		} break;

	 	case TYPE_STRING:
		{
    		memcpy(dest, ((std::string *)(value.blob))->c_str(), length);
			dest[length] = 0;
		} break;

		case TYPE_BLOB:
		{
			memcpy(dest, value.blob, length);
		} break;
	
		default:
		{
			// bad type here
			return 0;
		} break;
	}

	return length;
}

/*******************************************************************************
 *
 * Read bytes from the buffer and convert then to host byte order before storing
 * them in this parameter.  No byte swapping is done on BLOBs.
 *
 * @param	src			a pointer to the buffer from which the bytes should be read
 * @param	length_arg	the number of bytes that are available for reading
 *
 ******************************************************************************/
uint16_t UdpValueTableParameter::fromNetBytes(uint8_t *src, uint32_t length_arg)
{
	switch(type)
	{
		case TYPE_BOOL:
		{
			set<bool>( *((bool *)(src)));
		} break;
		
		case TYPE_INT8:
		{
			set<int8_t>(*((int8_t *)(src)));
		} break;
		
		case TYPE_UINT8:
		{
			set<uint8_t>( *((uint8_t *)(src)));
		} break;
		
		case TYPE_INT16:
		{
			set<int16_t>((int16_t)ntohs(*((int16_t *)(src))));
		} break;
		
		case TYPE_UINT16:
		{
			set<uint16_t>((uint16_t)ntohs(*((uint16_t *)(src))));
		} break;
		
		case TYPE_INT32:
		{
			set<int32_t>((int32_t)ntohl(*((int32_t *)(src))));
		} break;
		
		case TYPE_UINT32:
		{
			set<uint32_t>((uint32_t)ntohl(*((uint32_t *)(src))));
		} break;
		
		case TYPE_FLOAT32:
		{
			float f;
			*((uint32_t *)(&f)) = ntohl(*((uint32_t *)(src)));
			set<float>(f);
		} break;

		case TYPE_STRING:
		{
			set<std::string>(std::string((char *)src));		
		} break;
		
		case TYPE_BLOB:
		{
			set(src, length_arg);
		} break;

		default:
		{
			// bad type here
			return 0;
		} break;
	}
	return length;
}

} // namespace gsu
