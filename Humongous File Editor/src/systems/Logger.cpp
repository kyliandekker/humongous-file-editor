#include "systems/Logger.h"

#include <cstdarg>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <format>
#include <windows.h>
#include <direct.h>
#include "HumongousEditorForm.h"

HumongousFileEditor::logger::Logger HumongousFileEditor::logger::logger;

namespace HumongousFileEditor
{
	namespace logger
	{
		char arr[5][10] =
		{
			"INFO",
			"WARNING",
			"ERROR",
			"ASSERT"
		};

		template<typename ... Args>
		std::string string_format(const std::string& format, Args ... args)
		{
			int size_s = std::snprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'
			if (size_s <= 0) { throw std::runtime_error("Error during formatting."); }
			auto size = static_cast<size_t>(size_s);
			std::unique_ptr<char[]> buf(new char[size]);
			std::snprintf(buf.get(), size, format.c_str(), args ...);
			return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
		}

		Logger::Logger()
		{
			m_Running = true;
			m_Thread = std::thread(&Logger::MessageQueue, this);

			const char* directory = "../logs/";
			_mkdir(directory);

			struct tm newtime;
			time_t now = time(0);
			localtime_s(&newtime, &now);
			std::string name = string_format("%02i-%02i-%04i %02i-%02i-%02i",
				newtime.tm_mon,
				newtime.tm_mday,
				newtime.tm_year,
				newtime.tm_hour,
				newtime.tm_min,
				newtime.tm_sec);

			std::string path = directory + name;
			path += ".txt";
			fopen_s(&m_File, path.c_str(), "w");
			if (!m_File)
				LOGF(logger::LOGSEVERITY_INFO, "Cannot open file %s.", path.c_str());
		}

		Logger::~Logger()
		{
			m_MessagesMutex.lock();
			while (!m_Messages.empty()) m_Messages.pop();
			m_MessagesMutex.unlock();
			m_Running = false;
			m_Thread.join();
			fclose(m_File);
		}

		void Logger::Log(LogSeverity a_Severity, const char* a_Message, const char* a_File, int a_Line)
		{
			PrintMessage(a_Severity, a_Message, a_File, a_Line);
		}

		void Logger::LogF(LogSeverity a_Severity, const char* a_Message, const char* a_File, int a_Line, ...)
		{
			va_list va_format_list;
			va_start(va_format_list, a_Line);

			size_t buffersize = vsnprintf(NULL, 0, a_Message, va_format_list) + 1;
			char* formatted_message = reinterpret_cast<char*>(malloc(buffersize));
			vsnprintf(formatted_message, buffersize, a_Message, va_format_list);

			PrintMessage(a_Severity, formatted_message, a_File, a_Line);

			free(formatted_message);
		}

		void Logger::PrintMessage(LogSeverity a_Severity, const char* a_Message, const char* a_File, int a_Line)
		{
			struct tm newtime;
			time_t now = time(0);
			localtime_s(&newtime, &now);

			std::string message = string_format("[%s] [%02i:%02i:%02i] \"%s\" on line %i: \"%s\".\n",
				arr[a_Severity],
				newtime.tm_hour,
				newtime.tm_min,
				newtime.tm_sec,
				a_File,
				a_Line,
				a_Message
			);

			m_MessagesMutex.lock();
			m_Messages.push({ message, a_Severity });
			m_MessagesMutex.unlock();
		}

		void Logger::MessageQueue()
		{
			while (m_Running)
			{
				m_MessagesMutex.lock();
				if (m_Messages.size() > 0)
				{
					Message lm = m_Messages.front();
					m_Messages.pop();

					DWORD written = 0;
					WriteConsoleA(
						GetStdHandle(STD_OUTPUT_HANDLE),
						lm.message.c_str(),
						static_cast<DWORD>(lm.message.size()),
						&written,
						nullptr
					);

					if (lm.severity == LOGSEVERITY_ASSERT || lm.severity == LOGSEVERITY_ERROR)
						System::Windows::Forms::MessageBox::Show(gcnew System::String(lm.message.c_str()), gcnew System::String("Error"), System::Windows::Forms::MessageBoxButtons::OK, System::Windows::Forms::MessageBoxIcon::Information);
					
					if (lm.severity == logger::LOGSEVERITY_ASSERT)
						assert(0 && lm.message.c_str());
				}
				m_MessagesMutex.unlock();
			}
		}
	}
}