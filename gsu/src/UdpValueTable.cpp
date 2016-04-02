/*******************************************************************************
 *
 * File: UdpValueTable.cpp
 *
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#include "gsu/UdpValueTable.h"

namespace gsu
{

const std::string UdpValueTable::DEFAULT_DEST_HOST = "10.1.18.5";
const double 	  UdpValueTable::DEFAULT_PERIOD	   = 0.05;

/*******************************************************************************
 *
 ******************************************************************************/
UdpValueTable::UdpValueTable(std::string name, tinyxml2::XMLElement *xml) 
	: PeriodicThread(name)
{
    std::string local_host;
    int32_t 	local_port = -1;

    std::string remote_host;
    int32_t 	remote_port = -1;

    double 		period = 0.05;
	int32_t 	priority = 0;
	
	txControl = NULL;
	rxControl = NULL;
	
	if (xml != NULL)
	{
        local_host = xml->Attribute("local_host");
        local_port = xml->IntAttribute("local_port");

        remote_host = xml->Attribute("remote_host");
        remote_port = xml->IntAttribute("remote_port");

        period    = xml->FloatAttribute("period");
		priority  = xml->IntAttribute("priority");
	}

    if (local_host.length() < 1)
    {
//        dest_host = DEFAULT_DEST_HOST;
        local_host = "127.0.0.1";
    }

    if (remote_host.length() < 1)
    {
//        dest_host = DEFAULT_DEST_HOST;
        remote_host = "127.0.0.1";
    }

    if (local_port <= 0)
    {
//		dest_port = DEFAULT_DEST_PORT;
        if (name.compare("master") == 0)
        {
            local_port = 1130;
        }
        else
        {
            local_port = 1140;
        }
    }

    if (remote_port <= 0)
    {
//		dest_port = DEFAULT_DEST_PORT;
        if (name.compare("master") == 0)
        {
            remote_port = 1140;
        }
        else
        {
            remote_port = 1130;
        }
    }

    if (period <= 0.0)
	{
		period = DEFAULT_PERIOD;
	}

	echo_received_data = false;
	
//	priority = DEFAULT_PRIORITY + priority;	// @TODO: fix priority
	
    rxControl = new gsi::UdpBufferedReceiver(name, local_host, local_port,
		NAME_LENGTH + 4 + MAX_STR_LENGTH, 100, period, priority);
	
    txControl = new gsi::UdpBufferedTransmitter(name, remote_host, remote_port,
		NAME_LENGTH + 4 + MAX_STR_LENGTH, 100, period, priority);
		 
    rxControl->start();
    txControl->start();
    start();
}

/*******************************************************************************
 *
 ******************************************************************************/
UdpValueTable::~UdpValueTable()
{
	requestStop();	
}

/*******************************************************************************

 *
 ******************************************************************************/
void UdpValueTable::init(std::string name, tinyxml2::XMLElement *xml)
{
}

/*******************************************************************************
 *
 ******************************************************************************/
void UdpValueTable::finalize(void)
{
    rxControl->requestStop();
    txControl->requestStop();
    requestStop();
}

/*******************************************************************************
 *
 ******************************************************************************/
void UdpValueTable::doPeriodic(void)
{
	uint16_t type; 
	uint16_t flags; 
	uint16_t data_length;
	char buffer[NAME_LENGTH + MAX_STR_LENGTH];
	
	while (rxControl->getPacket(&type, &flags, &data_length, buffer))
	{
        put(&buffer[0], (UdpValueTableParameter::DataType)type, (uint8_t *)&buffer[NAME_LENGTH], 
			data_length - NAME_LENGTH, false);
	}
}

/*******************************************************************************
 *
 ******************************************************************************/
void UdpValueTable::put(std::string name, UdpValueTableParameter::DataType type,
    uint8_t *bytes, uint16_t length, bool do_send)
{
    UdpValueTableParameter *p = getParameter(name);
    if (p == NULL)
    {
        p = new UdpValueTableParameter();

        switch(type)
        {
            case UdpValueTableParameter::TYPE_BOOL:      p->init<bool>(false);   p->fromNetBytes(bytes, length); break;
            case UdpValueTableParameter::TYPE_INT8:      p->init<int8_t>(0);     p->fromNetBytes(bytes, length); break;
            case UdpValueTableParameter::TYPE_UINT8:     p->init<uint8_t>(0);    p->fromNetBytes(bytes, length); break;
            case UdpValueTableParameter::TYPE_INT16:     p->init<int16_t>(0);    p->fromNetBytes(bytes, length); break;
            case UdpValueTableParameter::TYPE_UINT16:    p->init<uint16_t>(0);   p->fromNetBytes(bytes, length); break;
            case UdpValueTableParameter::TYPE_INT32:     p->init<int32_t>(0);    p->fromNetBytes(bytes, length); break;
            case UdpValueTableParameter::TYPE_UINT32:    p->init<uint32_t>(0);   p->fromNetBytes(bytes, length); break;
            case UdpValueTableParameter::TYPE_FLOAT32:   p->init<float>(0.0);    p->fromNetBytes(bytes, length); break;

			case UdpValueTableParameter::TYPE_STRING:	
				p->init<std::string>(std::string("")); 
				p->fromNetBytes(bytes, length); 
				break;

            case UdpValueTableParameter::TYPE_BLOB: 
				p->init(bytes, length); 
				break;

            default: break;
        }
        parameters.insert(std::pair<std::string, UdpValueTableParameter *>(name, p));
    }
    else
    {
        if (p->getType() != type)
        {
            printf("UdpValueTable::put: type mismatch, for parameter %s\n", name.c_str());
            return;
        }

//        if (p->get<T>() != val)
//        {
            p->fromNetBytes(bytes, length);
			// tell observers data changed
//        }
    }

    if (do_send)
    {
        send(name, p);
    }
}

/*******************************************************************************
 *
 ******************************************************************************/
UdpValueTableParameter *UdpValueTable::getParameter(std::string name)
{
	std::map<std::string, UdpValueTableParameter *>::iterator ittr = parameters.find(name);
    if (ittr == parameters.end())
    {
        return NULL;
    }
    else
    {
        return ittr->second;
    }
}

/*******************************************************************************
 *
 ******************************************************************************/
void UdpValueTable::send(std::string name, UdpValueTableParameter *p)
{
    char *buffer = new char[NAME_LENGTH + p->getSize()];
	strncpy(buffer, name.c_str(), NAME_LENGTH);
    p->toNetBytes((uint8_t *)&buffer[NAME_LENGTH]);

	txControl->putPacket(p->getType(), DEFAULT_FLAGS, NAME_LENGTH + p->getSize(), buffer);
}

/*******************************************************************************
 *
 ******************************************************************************/
void UdpValueTable::printTable(void)
{
	printf("---------------- --------\n");
	for (std::map<std::string, UdpValueTableParameter *>::iterator it=parameters.begin(); it!=parameters.end(); ++it)
	{
		switch(it->second->getType())
		{
			case UdpValueTableParameter::TYPE_BOOL:
				printf("%-16s %3d %8s\n", it->first.c_str(), it->second->getSize(), it->second->get<bool>()?"true":"false");
				break;
		
			case UdpValueTableParameter::TYPE_FLOAT32:
				printf("%-16s %3d %8.2f\n", it->first.c_str(), it->second->getSize(), it->second->get<float>());
				break;

			case UdpValueTableParameter::TYPE_STRING:
				printf("%-16s %3d %s\n", it->first.c_str(), it->second->getSize(), it->second->get<std::string>().c_str());
				break;

			case UdpValueTableParameter::TYPE_BLOB:
			{
				printf("%-16s %3d", it->first.c_str(), it->second->getSize());
				for (int i = 0; i<it->second->getSize(); i++)
				{
					printf(" %02X", ((unsigned char *)(it->second->get<void *>()))[i]);
				}
				printf("\n");
			} break;

			default:
				printf("%-16s %3d %08X\n", it->first.c_str(), it->second->getSize(), (unsigned int)it->second->get<uint32_t>());
				break;
		}
	}
	printf("\n");
}

} // namespace gsu
