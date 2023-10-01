#include "lowlevel/OPCodes.h"

#include <lowlevel/bytecode_data.h>
#include <lowlevel/utils.h>

namespace HumongousFileEditor
{
    namespace chunk_reader
    {
        size_t extended_b_op(unsigned char* data, size_t data_size, unsigned char*& return_data)
        {
            return sizeof(uint8_t);
        }

        size_t extended_w_op(unsigned char* data, size_t data_size, unsigned char*& return_data)
        {
            return sizeof(uint16_t);
        }

        size_t jump_cmd(unsigned char* data, size_t data_size, unsigned char*& return_data)
        {
            uint16_t rel = *reinterpret_cast<uint16_t*>(data);
            return sizeof(uint16_t);
        }

        size_t msg_cmd(unsigned char* data, size_t data_size, unsigned char*& return_data)
        {
            size_t size = 0;
            uint8_t cmd = *reinterpret_cast<uint8_t*>(data);
            size += sizeof(uint8_t);
            if (cmd == 75 || cmd == 194)
            {
                std::string message = std::string(reinterpret_cast<char*>(utils::add(data, size)));
                return_data = utils::add(data, size);
                size += message.size() + 1; // Add one for the null terminated character.
            }
            return size;
        }

        size_t msg_op(unsigned char* data, size_t data_size, unsigned char*& return_data)
        {
            std::string message = std::string(reinterpret_cast<char*>(data));
            return_data = data;
            return message.size() + 1; // Add one for the null terminated character.
        }

        size_t actor_ops_v6(unsigned char* data, size_t data_size, unsigned char*& return_data)
        {
            size_t size = 0;
            uint8_t cmd = *reinterpret_cast<uint8_t*>(data);
            size += sizeof(uint8_t);
            if (cmd == 0x58)
            {
                std::string message = std::string(reinterpret_cast<char*>(utils::add(data, size)));
                return_data = utils::add(data, size);
                size += message.size() + 1; // Add one for the null terminated character.
            }
            return size;
        }

        size_t verb_ops_v6(unsigned char* data, size_t data_size, unsigned char*& return_data)
        {
            size_t size = 0;
            uint8_t cmd = *reinterpret_cast<uint8_t*>(data);
            size += sizeof(uint8_t);
            if (cmd == 0x7D)
            {
                std::string message = std::string(reinterpret_cast<char*>(utils::add(data, size)));
                return_data = utils::add(data, size);
                size += message.size() + 1; // Add one for the null terminated character.
            }
            return size;
        }

        size_t array_ops_v6(unsigned char* data, size_t data_size, unsigned char*& return_data)
        {
            size_t size = 0;
            uint8_t cmd = *reinterpret_cast<uint8_t*>(data);
            size++;
            uint16_t arr = *reinterpret_cast<uint16_t*>(utils::add(data, size));
            size += sizeof(uint16_t);
            if (cmd == 205)
            {
                std::string message = std::string(reinterpret_cast<char*>(utils::add(data, size)));
                return_data = utils::add(data, size);
                size += message.size() + 1; // Add one for the null terminated character.
            }
            return size;
        }

        size_t wait_ops(unsigned char* data, size_t data_size, unsigned char*& return_data)
        {
            size_t size = 0;
            uint8_t cmd = *reinterpret_cast<uint8_t*>(data);
            size += sizeof(uint8_t);
            if (cmd == 168 || cmd == 226 || cmd == 232)
            {
                uint16_t rel = *reinterpret_cast<uint16_t*>(utils::add(data, size));
                size += sizeof(uint16_t);
            }
            return size;
        }

        size_t extended_bw_op(unsigned char* data, size_t data_size, unsigned char*& return_data)
        {
            return sizeof(uint8_t) + sizeof(uint16_t);
        }

        size_t room_ops_he60(unsigned char* data, size_t data_size, unsigned char*& return_data)
        {
            size_t size = 0;
            uint8_t cmd = *reinterpret_cast<uint8_t*>(data);
            size += sizeof(uint8_t);
            if (cmd == 221)
            {
                std::string message = std::string(reinterpret_cast<char*>(utils::add(data, size)));
                return_data = utils::add(data, size);
                size += message.size() + 1; // Add one for the null terminated character.
            }
            return size;
        }

        size_t actor_ops_he60(unsigned char* data, size_t data_size, unsigned char*& return_data)
        {
            size_t size = 0;
            uint8_t cmd = *reinterpret_cast<uint8_t*>(data);
            size += sizeof(uint8_t);
            if (cmd == 225)
            {
                std::string message = std::string(reinterpret_cast<char*>(utils::add(data, size)));
                return_data = utils::add(data, size);
                size += message.size() + 1; // Add one for the null terminated character.
            }
            return size;
        }

        size_t dmsg_op(unsigned char* data, size_t data_size, unsigned char*& return_data)
        {
            size_t size = 0;
            size += std::string(reinterpret_cast<char*>(utils::add(data, size))).size() + 1; // Add one for the null terminated character.
            size += std::string(reinterpret_cast<char*>(utils::add(data, size))).size() + 1; // Add one for the null terminated character.
            return size;
        }

        size_t sys_msg(unsigned char* data, size_t data_size, unsigned char*& return_data)
        {
            size_t size = sizeof(uint8_t);
            size += std::string(reinterpret_cast<char*>(utils::add(data, size))).size() + 1; // Add one for the null terminated character.
            return size;
        }

        // TODO: Figure out what this does.
        size_t ini_op_v71(unsigned char* data, size_t data_size, unsigned char*& return_data)
        {
            return 0;
        }

        size_t array_ops(unsigned char* data, size_t data_size, unsigned char*& return_data)
        {
            size_t size = 0;
            uint8_t cmd = *reinterpret_cast<uint8_t*>(data);
            size++;
            uint16_t arr = *reinterpret_cast<uint16_t*>(utils::add(data, size));
            size += sizeof(uint16_t);
            if (cmd == 127)
                size += sizeof(uint16_t);
            else if (cmd == 138)
                size += sizeof(uint16_t) + sizeof(uint16_t);
            return size;
        }

        size_t extended_dw_op(unsigned char* data, size_t data_size, unsigned char*& return_data)
        {
            return sizeof(uint32_t);
        }

        size_t file_op(unsigned char* data, size_t data_size, unsigned char*& return_data)
        {
            size_t size = 0;
            uint8_t cmd = *reinterpret_cast<uint8_t*>(data);
            size += sizeof(uint8_t);
            if (cmd == 8)
                size += sizeof(uint8_t);
            return size;
        }

        size_t extended_ww_op(unsigned char* data, size_t data_size, unsigned char*& return_data)
        {
            return sizeof(uint16_t) + sizeof(uint16_t);
        }

        size_t default_func(unsigned char* data, size_t data_size, unsigned char*& return_data)
        {
            return 0;
        }
    }
}