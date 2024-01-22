#include "cmd/OPCodes.h"

#include "cmd/ArgsAllocator.h"
#include "low_level/utils.h"
#include "cmd/ScriptInstruction.h"
#include "system/Logger.h"

namespace resource_editor
{
	namespace chunk_reader
	{
		// According to rzil, the new position is relative from the position after the offset is read. Meaning it is scrp_pos + instruction_pos + arg_pos (of offset).
		size_t jump(ScriptInstruction& instruction, unsigned char* data, bool absolute)
		{
			// Normal wait instructions.
			if (instruction.m_Code == 0x5C // jump if
				|| instruction.m_Code == 0x5D // jump if not
				|| instruction.m_Code == 0x73 // jump
				)
			{
				const size_t abs_offset =
					instruction.m_SCRPOffset + // scrp offset in (a).
					instruction.m_OffsetInSCRPChunk + // offset of instruction in scrp chunk.
					instruction.m_Args[0].m_Offset + // offset of arg (this will be 0 for index 0).
					1; // basically, we need to start reading from the actual arg offset. Adding 1 because without this, we'd get the offset of the instruction.

				const size_t arg_size = instruction.m_Args[0].m_Size; // position after offset is read.
				const int16_t offset = *reinterpret_cast<int16_t*>(low_level::utils::add(data, abs_offset));

				return (absolute ? abs_offset : 0) + static_cast<size_t>(offset) + arg_size;
			}
			// Wait and jump.
			else if (instruction.m_Code == 0xA9) // wait and then jump.
			{
				const size_t abs_offset =
					instruction.m_SCRPOffset + // scrp offset in (a).
					instruction.m_OffsetInSCRPChunk + // offset of instruction in scrp chunk.
					instruction.m_Args[1].m_Offset + // offset of arg (this will be 0 for index 0). We want the second arg, cause the first arg says if we even have a second arg.
					1; // basically, we need to start reading from the actual arg offset. Adding 1 because without this, we'd get the offset of the instruction.

				const size_t arg_size = instruction.m_Args[0].m_Size; // position after offset is read.
				const int16_t offset = *reinterpret_cast<int16_t*>(low_level::utils::add(data, abs_offset));

				return (absolute ? abs_offset : 0) + static_cast<size_t>(offset) + arg_size;
			}

			LOG(logger::LOGSEVERITY_ASSERT, "Should never come here");
			return instruction.m_OffsetInSCRPChunk;
		}

		void extended_b_op(unsigned char* data, ArgsAllocator& return_data)
		{
			const uint8_t arg = *reinterpret_cast<uint8_t*>(data);
			return_data.AddArg({ 0, ArgType::ArgType_Byte, data, std::string(reinterpret_cast<const char*>(&arg)) });
		}

		void extended_w_op(unsigned char* data, ArgsAllocator& return_data)
		{
			const uint8_t arg = *reinterpret_cast<uint8_t*>(data);
			return_data.AddArg({ 0, ArgType::ArgType_Short, data, std::string(reinterpret_cast<const char*>(&arg)) });
		}

		void jump_cmd(unsigned char* data, ArgsAllocator& return_data)
		{
			const int16_t arg = *reinterpret_cast<int16_t*>(data);
			return_data.AddArg({ 0, ArgType::ArgType_Ref, data, std::string(reinterpret_cast<const char*>(&arg)) });
		}

		void msg_cmd(unsigned char* data, ArgsAllocator& return_data)
		{
			size_t size = 0;
			const uint8_t cmd = *reinterpret_cast<uint8_t*>(data);
			return_data.AddArg({ 0, ArgType::ArgType_Byte, data, std::string(reinterpret_cast<const char*>(&cmd)) });
			size += sizeof(uint8_t);
			if (cmd == 75 || cmd == 194)
			{
				const std::string message = std::string(reinterpret_cast<char*>(low_level::utils::add(data, size)));
				return_data.AddArg({ size, ArgType::ArgType_String, reinterpret_cast<unsigned char*>(low_level::utils::add(data, size)), message, message.size() + 1 });
			}
		}

		void msg_op(unsigned char* data, ArgsAllocator& return_data)
		{
			const std::string message = std::string(reinterpret_cast<char*>(data));
			return_data.AddArg({ 0, ArgType::ArgType_String, data, message, message.size() + 1 });
		}

		void actor_ops_v6(unsigned char* data, ArgsAllocator& return_data)
		{
			size_t size = 0;
			const uint8_t cmd = *reinterpret_cast<uint8_t*>(data);
			return_data.AddArg({ 0, ArgType::ArgType_Byte, data, std::string(reinterpret_cast<const char*>(&cmd)) });
			size += sizeof(uint8_t);
			if (cmd == 0x58)
			{
				const std::string message = std::string(reinterpret_cast<char*>(low_level::utils::add(data, size)));
				return_data.AddArg({ size, ArgType::ArgType_String, reinterpret_cast<unsigned char*>(low_level::utils::add(data, size)), message, message.size() + 1 });
			}
		}

		void verb_ops_v6(unsigned char* data, ArgsAllocator& return_data)
		{
			size_t size = 0;
			const uint8_t cmd = *reinterpret_cast<uint8_t*>(data);
			return_data.AddArg({ 0, ArgType::ArgType_Byte, data, std::string(reinterpret_cast<const char*>(&cmd)) });
			size += sizeof(uint8_t);
			if (cmd == 0x7D)
			{
				const std::string message = std::string(reinterpret_cast<char*>(low_level::utils::add(data, size)));
				return_data.AddArg({ size, ArgType::ArgType_String, reinterpret_cast<unsigned char*>(low_level::utils::add(data, size)), message, message.size() + 1 });
			}
		}

		void array_ops_v6(unsigned char* data, ArgsAllocator& return_data)
		{
			size_t size = 0;
			const uint8_t cmd = *reinterpret_cast<uint8_t*>(data);
			return_data.AddArg({ 0, ArgType::ArgType_Byte, data, std::string(reinterpret_cast<const char*>(&cmd)) });
			size = sizeof(uint8_t);
			const uint16_t arr = *reinterpret_cast<uint16_t*>(low_level::utils::add(data, size));
			return_data.AddArg({ size, ArgType::ArgType_Short, reinterpret_cast<unsigned char*>(low_level::utils::add(data, size)), std::string(reinterpret_cast<const char*>(&arr)) });
			size += sizeof(uint16_t);
			if (cmd == 205)
			{
				const std::string message = std::string(reinterpret_cast<char*>(low_level::utils::add(data, size)));
				return_data.AddArg({ size, ArgType::ArgType_String, reinterpret_cast<unsigned char*>(low_level::utils::add(data, size)), message, message.size() + 1 });
			}
		}

		void wait_ops(unsigned char* data, ArgsAllocator& return_data)
		{
			size_t size = 0;
			const uint8_t cmd = *reinterpret_cast<uint8_t*>(data);
			return_data.AddArg({ 0, ArgType::ArgType_Byte, data, std::string(reinterpret_cast<const char*>(&cmd)) });
			size += sizeof(uint8_t);
			if (cmd == 168 || cmd == 226 || cmd == 232)
			{
				const int16_t arg = *reinterpret_cast<int16_t*>(data);
				//uint16_t rel = *reinterpret_cast<uint16_t*>(utils::add(data, size));
				return_data.AddArg({ size, ArgType::ArgType_Ref, reinterpret_cast<unsigned char*>(low_level::utils::add(data, size)), std::string(reinterpret_cast<const char*>(&arg)) });
			}
		}

		void extended_bw_op(unsigned char* data, ArgsAllocator& return_data)
		{
			size_t size = 0;
			const uint8_t cmd = *reinterpret_cast<uint8_t*>(data);
			return_data.AddArg({ 0, ArgType::ArgType_Byte, data, std::string(reinterpret_cast<const char*>(&cmd)) });
			size += sizeof(uint8_t);
			const uint8_t arg = *reinterpret_cast<uint8_t*>(data);
			return_data.AddArg({ size, ArgType::ArgType_Short, reinterpret_cast<unsigned char*>(low_level::utils::add(data, size)), std::string(reinterpret_cast<const char*>(&arg)) });
		}

		void room_ops_he60(unsigned char* data, ArgsAllocator& return_data)
		{
			size_t size = 0;
			const uint8_t cmd = *reinterpret_cast<uint8_t*>(data);
			return_data.AddArg({ 0, ArgType::ArgType_Byte, data, std::string(reinterpret_cast<const char*>(&cmd)) });
			size += sizeof(uint8_t);
			if (cmd == 221)
			{
				const std::string message = std::string(reinterpret_cast<char*>(low_level::utils::add(data, size)));
				return_data.AddArg({ size, ArgType::ArgType_String, reinterpret_cast<unsigned char*>(low_level::utils::add(data, size)), message, message.size() + 1 });
			}
		}

		void actor_ops_he60(unsigned char* data, ArgsAllocator& return_data)
		{
			size_t size = 0;
			const uint8_t cmd = *reinterpret_cast<uint8_t*>(data);
			return_data.AddArg({ 0, ArgType::ArgType_Byte, data, std::string(reinterpret_cast<const char*>(&cmd)) });
			size += sizeof(uint8_t);
			if (cmd == 225)
			{
				const std::string message = std::string(reinterpret_cast<char*>(low_level::utils::add(data, size)));
				return_data.AddArg({ size, ArgType::ArgType_String, reinterpret_cast<unsigned char*>(low_level::utils::add(data, size)), message, message.size() + 1 });
			}
		}

		void dmsg_op(unsigned char* data, ArgsAllocator& return_data)
		{
			const std::string message1 = std::string(reinterpret_cast<char*>(data)); // Add one for the null terminated character.
			return_data.AddArg({ 0, ArgType::ArgType_String, data, message1, message1.size() + 1 });
			size_t size = message1.size() + 1;
			const std::string message2 = std::string(reinterpret_cast<char*>(low_level::utils::add(data, size))); // Add one for the null terminated character.
			return_data.AddArg({ size, ArgType::ArgType_String, reinterpret_cast<unsigned char*>(low_level::utils::add(data, size)), message2, message2.size() + 1 });
		}

		void sys_msg(unsigned char* data, ArgsAllocator& return_data)
		{
			const uint8_t cmd = *reinterpret_cast<uint8_t*>(data);
			return_data.AddArg({ 0, ArgType::ArgType_Byte, data, std::string(reinterpret_cast<const char*>(&cmd)) });
			const size_t size = sizeof(uint8_t);
			const std::string message = std::string(reinterpret_cast<char*>(low_level::utils::add(data, size))); // Add one for the null terminated character.
			return_data.AddArg({ size, ArgType::ArgType_String, reinterpret_cast<unsigned char*>(low_level::utils::add(data, size)), message, message.size() + 1 });
		}

		// TODO: Figure out what this does.
		void ini_op_v71(unsigned char* data, ArgsAllocator& return_data)
		{
			default_func(data, return_data);
		}

		void array_ops(unsigned char* data, ArgsAllocator& return_data)
		{
			const uint8_t cmd = *reinterpret_cast<uint8_t*>(data);
			return_data.AddArg({ 0, ArgType::ArgType_Byte, data, std::string(reinterpret_cast<const char*>(&cmd)) });
			size_t size = sizeof(uint8_t);
			const uint16_t arr = *reinterpret_cast<uint16_t*>(data);
			return_data.AddArg({ size, ArgType::ArgType_Short, reinterpret_cast<unsigned char*>(low_level::utils::add(data, size)), std::string(reinterpret_cast<const char*>(&arr)) });
			size += sizeof(uint16_t);
			if (cmd == 127)
			{
				const uint16_t arg = *reinterpret_cast<uint16_t*>(data);
				return_data.AddArg({ size, ArgType::ArgType_Short, reinterpret_cast<unsigned char*>(low_level::utils::add(data, size)), std::string(reinterpret_cast<const char*>(&arg)) });
			}
			else if (cmd == 138)
			{
				const uint16_t arg = *reinterpret_cast<uint16_t*>(data);
				return_data.AddArg({ size, ArgType::ArgType_Short, reinterpret_cast<unsigned char*>(low_level::utils::add(data, size)), std::string(reinterpret_cast<const char*>(&arg)) });
				size += sizeof(uint16_t);
				const uint16_t arg2 = *reinterpret_cast<uint16_t*>(data);
				return_data.AddArg({ size, ArgType::ArgType_Short, reinterpret_cast<unsigned char*>(low_level::utils::add(data, size)), std::string(reinterpret_cast<const char*>(&arg2)) });
			}
		}

		void extended_dw_op(unsigned char* data, ArgsAllocator& return_data)
		{
			const uint8_t cmd = *reinterpret_cast<uint32_t*>(data);
			return_data.AddArg({ 0, ArgType::ArgType_Long, data, std::string(reinterpret_cast<const char*>(&cmd)) });
		}

		void file_op(unsigned char* data, ArgsAllocator& return_data)
		{
			size_t size = 0;
			const uint8_t cmd = *reinterpret_cast<uint8_t*>(data);
			return_data.AddArg({ 0, ArgType::ArgType_Byte, data, std::string(reinterpret_cast<const char*>(&cmd)) });
			size += sizeof(uint8_t);
			if (cmd == 8)
			{
				const uint8_t arg = *reinterpret_cast<uint8_t*>(data);
				return_data.AddArg({ size, ArgType::ArgType_Byte, reinterpret_cast<unsigned char*>(low_level::utils::add(data, size)), std::string(reinterpret_cast<const char*>(&arg)) });
			}
		}

		void extended_ww_op(unsigned char* data, ArgsAllocator& return_data)
		{
			const uint8_t cmd = *reinterpret_cast<uint8_t*>(data);
			return_data.AddArg({ 0, ArgType::ArgType_Byte, data, std::string(reinterpret_cast<const char*>(&cmd)) });
			size_t size = sizeof(uint8_t);
			const uint8_t arg = *reinterpret_cast<uint8_t*>(data);
			return_data.AddArg({ size, ArgType::ArgType_Byte, reinterpret_cast<unsigned char*>(low_level::utils::add(data, size)), std::string(reinterpret_cast<const char*>(&arg)) });
		}

		void default_func(unsigned char*, ArgsAllocator&)
		{
		}
	}
}