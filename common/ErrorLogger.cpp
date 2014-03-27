//
// ErrorLogger.cpp
//
// Report errors, warnings and messages and append them to a log file in a
// structured format.
//
// (c) 2014 Graham West

// Class header, always comes first.
#include "ErrorLogger.h"

// Library headers.
#include <QDateTime>

ErrorLogger SystemLogger;

ErrorLogger::ErrorLogger() : m_Verbosity(MESSAGE), m_FileName("")
{
}

ErrorLogger::~ErrorLogger()
{
	if (m_LogFile.isOpen())
	{
		Stop("");
	}
}

bool ErrorLogger::Start(const QString& fileName, const QString& startMessage)
{
	bool retval = false;

	if (!fileName.isEmpty() && !m_LogFile.isOpen())
	{
		m_LogFile.setFileName(fileName);
		m_LogFile.open(QIODevice::Append | QIODevice::Text);

		if (m_LogFile.isOpen())
		{
			m_LogStream.setDevice(&m_LogFile);

			if (!startMessage.isEmpty())
			{
				QString timeStamp("");
				QDateTime now = QDateTime::currentDateTime();
				timeStamp = now.toString("yyyy-MM-dd hh:mm:ss");
				
				m_LogStream << "     ***** START *****\n";
				m_LogStream << timeStamp;
				m_LogStream << " ";
				m_LogStream << startMessage;
				m_LogStream << "\n     ***** START *****\n";
			}
			retval = true;
		}
	}

	return retval;
}

bool ErrorLogger::Stop(const QString& stopMessage)
{
	bool retval = false;

	if (m_LogFile.isOpen())
	{
		if (!stopMessage.isEmpty())
		{
			QString timeStamp("");
			QDateTime now = QDateTime::currentDateTime();
			timeStamp = now.toString("yyyy-MM-dd hh:mm:ss");

			m_LogStream << "     ***** STOP *****\n";
			m_LogStream << timeStamp;
			m_LogStream << " ";
			m_LogStream << stopMessage;
			m_LogStream << "\n     ***** STOP *****\n";
		}

		m_LogStream.flush();
		m_LogStream.setDevice(0);
		m_LogFile.close();

		retval = true;
	}

	return retval;
}

bool ErrorLogger::Log(ErrorLogger::Severity severity, const QString& logLine)
{
	bool retval = false;

	if (severity >= m_Verbosity && m_LogFile.isOpen())
	{
		QString prefix("");
		QDateTime now = QDateTime::currentDateTime();

		prefix = now.toString("yyyy-MM-dd hh:mm:ss");
		prefix += " ";

		switch (severity)
		{
			case FATAL_ERROR:
				prefix += "FATAL";
				break;

			case NONFATAL_ERROR:
				prefix += "ERROR";
				break;
				
			case WARNING:
				prefix += " WARN";
				break;
				
			case MESSAGE:
				prefix += " MESG";
				break;
				
			case DEBUG:
				prefix += "DEBUG";
				break;
				
			case VERBOSE_DEBUG:
				prefix += " VERB";
				break;
				
			default:
				prefix += QString("%1 ?").arg(severity, 3);
				break;
		}

		m_LogStream << prefix;
		m_LogStream << " - ";
		m_LogStream << logLine;
		m_LogStream << '\n';
	}

	return retval;
}

bool ErrorLogger::Log(ErrorLogger::Severity severity, const char* formatStr, ...)
{
	QString formatted("");
	va_list args;

	va_start(args, formatStr);
	formatted = formatted.vsprintf(formatStr, args);
	va_end(args);

	return Log(severity, formatted);
}

bool ErrorLogger::Fatal(const QString& logLine)
{
	return Log(FATAL_ERROR, logLine);
}

bool ErrorLogger::Nonfatal(const QString& logLine)
{
	return Log(NONFATAL_ERROR, logLine);
}

bool ErrorLogger::Warning(const QString& logLine)
{
	return Log(WARNING, logLine);
}

bool ErrorLogger::Message(const QString& logLine)
{
	return Log(MESSAGE, logLine);
}

bool ErrorLogger::Debug(const QString& logLine)
{
	return Log(DEBUG, logLine);
}

bool ErrorLogger::Verbose(const QString& logLine)
{
	return Log(VERBOSE_DEBUG, logLine);
}

bool ErrorLogger::Fatal(const char* formatStr, ...)
{
	QString formatted("");
	va_list args;
	
	va_start(args, formatStr);
	formatted = formatted.vsprintf(formatStr, args);
	va_end(args);
	
	return Log(FATAL_ERROR, formatted);
}

bool ErrorLogger::NonFatal(const char* formatStr, ...)
{
	QString formatted("");
	va_list args;
	
	va_start(args, formatStr);
	formatted = formatted.vsprintf(formatStr, args);
	va_end(args);
	
	return Log(NONFATAL_ERROR, formatted);
}

bool ErrorLogger::Warning(const char* formatStr, ...)
{
	QString formatted("");
	va_list args;
	
	va_start(args, formatStr);
	formatted = formatted.vsprintf(formatStr, args);
	va_end(args);
	
	return Log(WARNING, formatted);
}

bool ErrorLogger::Message(const char* formatStr, ...)
{
	QString formatted("");
	va_list args;
	
	va_start(args, formatStr);
	formatted = formatted.vsprintf(formatStr, args);
	va_end(args);
	
	return Log(MESSAGE, formatted);
}

bool ErrorLogger::Debug(const char* formatStr, ...)
{
	QString formatted("");
	va_list args;
	
	va_start(args, formatStr);
	formatted = formatted.vsprintf(formatStr, args);
	va_end(args);
	
	return Log(DEBUG, formatted);
}

bool ErrorLogger::Verbose(const char* formatStr, ...)
{
	QString formatted("");
	va_list args;
	
	va_start(args, formatStr);
	formatted = formatted.vsprintf(formatStr, args);
	va_end(args);
	
	return Log(VERBOSE_DEBUG, formatted);
}
