#include "cmd/OPCodes.h"

#include "cmd/ArgsAllocator.h"
#include "lowlevel/bytecode_data.h"
#include "lowlevel/utils.h"

namespace HumongousFileEditor
{
    namespace chunk_reader
    {
        void extended_b_op(unsigned char* data, size_t data_size, ArgsAllocator& return_data)
        {
            return_data.AddArg({ 0, ArgType::ArgType_Byte });
        }

        void extended_w_op(unsigned char* data, size_t data_size, ArgsAllocator& return_data)
        {
            return_data.AddArg({ 0, ArgType::ArgType_Short });
        }

        void jump_cmd(unsigned char* data, size_t data_size, ArgsAllocator& return_data)
        {
            return_data.AddArg({ 0, ArgType::ArgType_Ref });
        }

        void msg_cmd(unsigned char* data, size_t data_size, ArgsAllocator& return_data)
        {
            size_t size = 0;
            uint8_t cmd = *reinterpret_cast<uint8_t*>(data);
            return_data.AddArg({ 0, ArgType::ArgType_Byte });
            size += sizeof(uint8_t);
            if (cmd == 75 || cmd == 194)
            {
                std::string message = std::string(reinterpret_cast<char*>(utils::add(data, size)));
                return_data.AddArg({ size, ArgType::ArgType_String, message.size() + 1 });
            }
        }

        void msg_op(unsigned char* data, size_t data_size, ArgsAllocator& return_data)
        {
            std::string message = std::string(reinterpret_cast<char*>(data));
            return_data.AddArg({ 0, ArgType::ArgType_String, message.size() + 1 });
        }

        void actor_ops_v6(unsigned char* data, size_t data_size, ArgsAllocator& return_data)
        {
            size_t size = 0;
            uint8_t cmd = *reinterpret_cast<uint8_t*>(data);
            return_data.AddArg({ 0, ArgType::ArgType_Byte });
            size += sizeof(uint8_t);
            if (cmd == 0x58)
            {
                std::string message = std::string(reinterpret_cast<char*>(utils::add(data, size)));
                return_data.AddArg({ size, ArgType::ArgType_String, message.size() + 1 });
            }
        }

        void verb_ops_v6(unsigned char* data, size_t data_size, ArgsAllocator& return_data)
        {
            size_t size = 0;
            uint8_t cmd = *reinterpret_cast<uint8_t*>(data);
            return_data.AddArg({ 0, ArgType::ArgType_Byte });
            size += sizeof(uint8_t);
            if (cmd == 0x7D)
            {
                std::string message = std::string(reinterpret_cast<char*>(utils::add(data, size)));
                return_data.AddArg({ size, ArgType::ArgType_String, message.size() + 1 });
            }
        }

        void array_ops_v6(unsigned char* data, size_t data_size, ArgsAllocator& return_data)
        {
            size_t size = 0;
            uint8_t cmd = *reinterpret_cast<uint8_t*>(data);
            return_data.AddArg({ 0, ArgType::ArgType_Byte });
            size = sizeof(uint8_t);
            uint16_t arr = *reinterpret_cast<uint16_t*>(utils::add(data, size));
            return_data.AddArg({ size, ArgType::ArgType_Short });
            size += sizeof(uint16_t);
            if (cmd == 205)
            {
                std::string message = std::string(reinterpret_cast<char*>(utils::add(data, size)));
                return_data.AddArg({ size, ArgType::ArgType_String, message.size() + 1 });
            }
        }

        void wait_ops(unsigned char* data, size_t data_size, ArgsAllocator& return_data)
        {
            size_t size = 0;
            uint8_t cmd = *reinterpret_cast<uint8_t*>(data);
            return_data.AddArg({ 0, ArgType::ArgType_Byte });
            size += sizeof(uint8_t);
            if (cmd == 168 || cmd == 226 || cmd == 232)
            {
                uint16_t rel = *reinterpret_cast<uint16_t*>(utils::add(data, size));
                return_data.AddArg({ size, ArgType::ArgType_Ref });
            }
        }

        void extended_bw_op(unsigned char* data, size_t data_size, ArgsAllocator& return_data)
        {
            size_t size = 0;
            uint8_t cmd = *reinterpret_cast<uint8_t*>(data);
            return_data.AddArg({ 0, ArgType::ArgType_Byte });
            size += sizeof(uint8_t);
            return_data.AddArg({ size, ArgType::ArgType_Short });
        }

        void room_ops_he60(unsigned char* data, size_t data_size, ArgsAllocator& return_data)
        {
            size_t size = 0;
            uint8_t cmd = *reinterpret_cast<uint8_t*>(data);
            return_data.AddArg({ 0, ArgType::ArgType_Byte });
            size += sizeof(uint8_t);
            if (cmd == 221)
            {
                std::string message = std::string(reinterpret_cast<char*>(utils::add(data, size)));
                return_data.AddArg({ size, ArgType::ArgType_String, message.size() + 1 });
            }
        }

        void actor_ops_he60(unsigned char* data, size_t data_size, ArgsAllocator& return_data)
        {
            size_t size = 0;
            uint8_t cmd = *reinterpret_cast<uint8_t*>(data);
            return_data.AddArg({ 0, ArgType::ArgType_Byte });
            size += sizeof(uint8_t);
            if (cmd == 225)
            {
                std::string message = std::string(reinterpret_cast<char*>(utils::add(data, size)));
                return_data.AddArg({ size, ArgType::ArgType_String, message.size() + 1 });
            }
        }

        void dmsg_op(unsigned char* data, size_t data_size, ArgsAllocator& return_data)
        {
            std::string message1 = std::string(reinterpret_cast<char*>(data)); // Add one for the null terminated character.
            return_data.AddArg({ 0, ArgType::ArgType_String, message1.size() + 1 });
            size_t size = message1.size() + 1;
            std::string message2 = std::string(reinterpret_cast<char*>(utils::add(data, size))); // Add one for the null terminated character.
            return_data.AddArg({ size, ArgType::ArgType_String, message2.size() + 1});
        }

        void sys_msg(unsigned char* data, size_t data_size, ArgsAllocator& return_data)
        {
            return_data.AddArg({ 0, ArgType::ArgType_Byte });
            size_t size = sizeof(uint8_t);
            std::string message = std::string(reinterpret_cast<char*>(utils::add(data, size))); // Add one for the null terminated character.
            return_data.AddArg({ size, ArgType::ArgType_String, message.size() + 1});
        }

        // TODO: Figure out what this does.
        void ini_op_v71(unsigned char* data, size_t data_size, ArgsAllocator& return_data)
        {
            default_func(data, data_size, return_data);
        }

        void array_ops(unsigned char* data, size_t data_size, ArgsAllocator& return_data)
        {
            uint8_t cmd = *reinterpret_cast<uint8_t*>(data);
            return_data.AddArg({ 0, ArgType::ArgType_Byte });
            size_t size = sizeof(uint8_t);
            return_data.AddArg({ size, ArgType::ArgType_Short });
            size += sizeof(uint16_t);
            if (cmd == 127)
            {
                return_data.AddArg({ size, ArgType::ArgType_Short });
            }
            else if (cmd == 138)
            {
                return_data.AddArg({ size, ArgType::ArgType_Short });
                size += sizeof(uint16_t);
                return_data.AddArg({ size, ArgType::ArgType_Short });
            }
        }

        void extended_dw_op(unsigned char* data, size_t data_size, ArgsAllocator& return_data)
        {
            return_data.AddArg({ 0, ArgType::ArgType_Long });
        }

        void file_op(unsigned char* data, size_t data_size, ArgsAllocator& return_data)
        {
            size_t size = 0;
            uint8_t cmd = *reinterpret_cast<uint8_t*>(data);
            return_data.AddArg({ 0, ArgType::ArgType_Byte });
            size += sizeof(uint8_t);
            if (cmd == 8)
                return_data.AddArg({ size, ArgType::ArgType_Byte});
        }

        void extended_ww_op(unsigned char* data, size_t data_size, ArgsAllocator& return_data)
        {
            return_data.AddArg({ 0, ArgType::ArgType_Byte });
            size_t size = sizeof(uint16_t);
            return_data.AddArg({ size, ArgType::ArgType_Byte });
        }

        void default_func(unsigned char* data, size_t data_size, ArgsAllocator& return_data)
        {
        }
    }
}