/*******************************************************************************
 *
 * File: UdpValueTableParameter.h
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
#include <vector>

#include <gsi/UdpSocket.h>

namespace gsu
{

/*******************************************************************************
 *
 * This class uses templated methods to set, get, send, and receive values 
 * of different types and sizes.  It is intended to be used by UdpValueTable
 * but could potentially be used by others.
 * 
 ******************************************************************************/
class UdpValueTableParameter
{
	public:
		enum DataType
		{
			TYPE_NONE = 0,
			TYPE_BOOL,
			TYPE_INT8,
			TYPE_UINT8,
			TYPE_INT16,
			TYPE_UINT16,
			TYPE_INT32,
			TYPE_UINT32,
			TYPE_FLOAT32,
			TYPE_STRING,
			TYPE_BLOB
		};

		UdpValueTableParameter(void);

		// for BLOBs
		UdpValueTableParameter(void *bytes, uint16_t len);
		void init(void *bytes, uint16_t len);
		void set(void *bytes, uint16_t len);

		template <class T> void init(T val);
		
		template <class T> void set(T v);
		template <class T> T get(void);
		
		DataType getType()  { return type; }
		uint16_t getSize()  { return length; }
		
		template <class T> static DataType typeOf(T);
		template <class T> static uint16_t sizeOf(T);

		uint16_t toNetBytes(uint8_t *dest);
		uint16_t fromNetBytes(uint8_t *src, uint32_t length_arg);
		
	private:
		DataType type;
		uint16_t length;

		union
		{
			bool    	b;
			int8_t		i8;
			uint8_t 	u8;
			int16_t 	i16;
			uint16_t 	u16;
			int32_t 	i32;
			uint32_t 	u32;
			float 		f32;
			uint8_t		*blob;
		} value;
};

/*******************************************************************************
 *
 * Initialized an instance of this class with the provided value.
 *
 * @param	val	the value used to initialize this parameter, the type and
 *				size of the parameter are set based on the type of the
 *				value.
 *
 ******************************************************************************/
template <class T> inline void UdpValueTableParameter::init(T val)
{
	type = UdpValueTableParameter::typeOf<T>(val);
	length = UdpValueTableParameter::sizeOf<T>(val);
	set<T>(val);
}

/*******************************************************************************
 *
 *	Get the type enumeration for the provided value type
 *	
 *	This method uses Templated Method Specialization to get the enumeration
 *	value for the provided type.
 *	
 *  NOTE:	There is not a generic implementation of this method so if
 *			something attempts to get the enumeration for a type that is not
 *			supported (event void *), it should not compile.
 *
 *	@return the enumeration.
 *  
 ******************************************************************************/
template <> inline UdpValueTableParameter::DataType UdpValueTableParameter::typeOf(bool) 			{ return TYPE_BOOL; }
template <> inline UdpValueTableParameter::DataType UdpValueTableParameter::typeOf(int8_t) 			{ return TYPE_INT8; }
template <> inline UdpValueTableParameter::DataType UdpValueTableParameter::typeOf(uint8_t) 		{ return TYPE_UINT8; }
template <> inline UdpValueTableParameter::DataType UdpValueTableParameter::typeOf(int16_t) 		{ return TYPE_INT16; }
template <> inline UdpValueTableParameter::DataType UdpValueTableParameter::typeOf(uint16_t) 		{ return TYPE_UINT16; }
template <> inline UdpValueTableParameter::DataType UdpValueTableParameter::typeOf(int32_t) 		{ return TYPE_INT32; }
template <> inline UdpValueTableParameter::DataType UdpValueTableParameter::typeOf(uint32_t) 		{ return TYPE_UINT32; }
template <> inline UdpValueTableParameter::DataType UdpValueTableParameter::typeOf(float) 			{ return TYPE_FLOAT32; }
template <> inline UdpValueTableParameter::DataType UdpValueTableParameter::typeOf(std::string)		{ return TYPE_STRING; }
template <> inline UdpValueTableParameter::DataType UdpValueTableParameter::typeOf(void *)			{ return TYPE_BLOB; }

/******************************************************************************
 *
 *  Get the size of the provided value.
 *  
 *  In most cases, this method will use the default implementation and just
 *  return sizeof() on the provided value.  However, in a few cases the sizeof()
 *  function can not be used so Templated Method Specialization is used to
 *  get the "correct" size.  For example, the C++ standard for bool does not
 *  specify the size in which a bool will be stored, some implementations use
 *  1 byte, some use 4 bytes. In this class a bool will always be stored and
 *  transmitted as 1 byte.
 *  
 ******************************************************************************/
template <> 		inline uint16_t UdpValueTableParameter::sizeOf(bool v) 			{ return 1; }
template <>			inline uint16_t UdpValueTableParameter::sizeOf(std::string v)	{ return v.length() + 1; }
template <>			inline uint16_t UdpValueTableParameter::sizeOf(void *v)			{ return 0; }
template <class T> 	inline uint16_t UdpValueTableParameter::sizeOf(T v) 			{ return (uint16_t)sizeof(T); }

/*******************************************************************************
 *
 *  Put the specified value into the parameter structure.
 *  
 *  This method is implemented with Templated Method Specialization to put
 *  the correct 'type' of value into the parameters value union.
 *  
 *  NOTE:	There is not a generic implementation of this method so if
 *			something attempts to set the value with a type that is not
 *			supported (event void *), it should not compile.
 *
 *  @param	v	a type specific value to be stored 
 *  
 ******************************************************************************/
template<> inline void UdpValueTableParameter::set(bool v) 			{ value.b = v; 		}
template<> inline void UdpValueTableParameter::set(int8_t v) 		{ value.i8 = v; 	}
template<> inline void UdpValueTableParameter::set(uint8_t v) 		{ value.u8 = v; 	}
template<> inline void UdpValueTableParameter::set(int16_t v) 		{ value.i16 = v; 	}
template<> inline void UdpValueTableParameter::set(uint16_t v) 		{ value.u16 = v; 	}
template<> inline void UdpValueTableParameter::set(int32_t v) 		{ value.i32 = v; 	}
template<> inline void UdpValueTableParameter::set(uint32_t v) 		{ value.u32 = v; 	}
template<> inline void UdpValueTableParameter::set(float v) 		{ value.f32 = v; 	}

template<> inline void UdpValueTableParameter::set(std::string v) 	
{
	if (value.blob != NULL)
	{
		delete (std::string *)value.blob;
 	}

	value.blob = (uint8_t *) new std::string(v);
	length = v.size() + 1;
}

/*******************************************************************************
 *
 *  Get the value from the parameter structure.
 *  
 *  This method is implemented with Templated Method Specialization to get
 *  the correct 'type' of value from the parameters value union.
 *  
 *  @return		a type specific value
 *  
 ******************************************************************************/
template <> inline bool 		UdpValueTableParameter::get(void) 	{ return value.b; }
template <> inline int8_t 		UdpValueTableParameter::get(void) 	{ return value.i8; }
template <> inline uint8_t 		UdpValueTableParameter::get(void) 	{ return value.u8; }
template <> inline int16_t 		UdpValueTableParameter::get(void) 	{ return value.i16; }
template <> inline uint16_t		UdpValueTableParameter::get(void) 	{ return value.u16; }
template <> inline int32_t 		UdpValueTableParameter::get(void) 	{ return value.i32; }
template <> inline uint32_t		UdpValueTableParameter::get(void) 	{ return value.u32; }
template <> inline float 		UdpValueTableParameter::get(void) 	{ return value.f32; }
template <> inline std::string	UdpValueTableParameter::get(void) 	{ return *((std::string *)value.blob); }
template <> inline void *		UdpValueTableParameter::get(void) 	{ return value.blob; }

} // namespace gsu
