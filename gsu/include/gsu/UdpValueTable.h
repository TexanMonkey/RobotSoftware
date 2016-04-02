/*******************************************************************************
 *
 * File: UdpValueTable.h
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
#include <map>
#include <vector>

#include "gsi/PeriodicThread.h"

#include "gsu/UdpBufferedTransmitter.h"
#include "gsu/UdpBufferedReceiver.h"
#include "gsu/tinyxml2.h"

#include "UdpValueTableParameter.h"

namespace gsu
{

/**********************************************************************
 *
 **********************************************************************/
class UdpValueTable : public gsi::PeriodicThread
{
// TODO: Add an initial values file / parser
// TODO: Add a subscribe/notify interface

	public:
		static const uint16_t DEFAULT_FLAGS = 0;
		
		static const uint8_t NAME_LENGTH = 16;
		static const uint8_t MAX_STR_LENGTH = 100;
		static const uint8_t MAX_BLOB_LENGTH = 96;
		
		static const std::string DEFAULT_DEST_HOST;
		static const uint32_t    DEFAULT_DEST_PORT = 1140;
        static const double 	 DEFAULT_PERIOD;
//		static const int32_t	 DEFAULT_PRIORITY  = Task::kDefaultPriority;
	
		UdpValueTable(std::string name, tinyxml2::XMLElement *xml);
		~UdpValueTable();
		void init(std::string name, tinyxml2::XMLElement *xml) ;

        void put(std::string name, UdpValueTableParameter::DataType type, uint8_t *bytes, uint16_t length, bool do_send=true);

		template <class T> void put(std::string name, T val, bool do_send=true);
		template <class T> T get(std::string name, T default_val = NULL);

		void printTable(void);

	protected:
		void doPeriodic(void);
		
	private:
		std::map<std::string, UdpValueTableParameter *> parameters;
		gsi::UdpBufferedTransmitter *txControl;
		gsi::UdpBufferedReceiver *rxControl;
		
		UdpValueTableParameter *getParameter(std::string name);
		void send(std::string name, UdpValueTableParameter *p);
		
		bool echo_received_data;

        void finalize(void);
};

/*******************************************************************************
 *
 ******************************************************************************/
template <class T>
void UdpValueTable::put(std::string name, T val, bool do_send)
{
	UdpValueTableParameter *p = getParameter(name);
	if (p == NULL)
	{
        printf("inserting new for %s\n", name.c_str());
        p = new UdpValueTableParameter();
        p->init<T>(val);
        parameters.insert(std::pair<std::string, UdpValueTableParameter *>(name, p));
    }
    else
    {
        if (p->getType() != UdpValueTableParameter::typeOf<T>(val))
        {
            printf("UdpValueTable::put: type mismatch, for parameter %s\n", name.c_str());
            return;
        }

        if (p->get<T>() != val)
        {
            // tell observers data changed
            p->set(val);
        }
    }

	if (do_send)
	{
        printf("sending %s\n", name.c_str());
		send(name, p);
	}
}

/*******************************************************************************
 *
 ******************************************************************************/
template <class T>
T UdpValueTable::get(std::string name, T default_val)
{
    UdpValueTableParameter *p = getParameter(name);
    if (p == NULL)
    {
        return default_val;
    }

    if (p->getType() != UdpValueTableParameter::typeOf(default_val))
    {
        return default_val;
    }

    return p->get<T>();
}

} // namespace gsu
