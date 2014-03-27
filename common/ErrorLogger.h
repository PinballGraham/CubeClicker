//
// ErrorLogger.h
//
// Report errors, warnings and messages and append them to a log file in a
// structured format.
//
// (c) 2014 Graham West

#if !defined(ERRORLOGGER_H)
#define ERRORLOGGER_H

// Library headers.
#include <QFile>
#include <QString>
#include <QTextStream>

class ErrorLogger;

extern ErrorLogger SystemLogger;

class ErrorLogger
{
public:
	typedef enum
	{
		FATAL_ERROR = 6,
		NONFATAL_ERROR = 5,
		WARNING = 4,
		MESSAGE = 3,
		DEBUG = 2,
		VERBOSE_DEBUG = 1
	} Severity;

	ErrorLogger();
	~ErrorLogger();

	inline void Verbosity(Severity newVerbosity) { m_Verbosity = newVerbosity; }
	inline Severity Verbosity(void) const { return m_Verbosity; }

	bool Start(const QString& fileName, const QString& startMessage);
	bool Stop(const QString& stopMessage);

	bool Log(Severity severity, const QString& logLine);
	bool Log(Severity severity, const char* formatStr, ...);

	bool Fatal(const QString& logLine);
	bool Nonfatal(const QString& logLine);
	bool Warning(const QString& logLine);
	bool Message(const QString& logLine);
	bool Debug(const QString& logLine);
	bool Verbose(const QString& logLine);

	bool Fatal(const char* formatStr, ...);
	bool NonFatal(const char* formatStr, ...);
	bool Warning(const char* formatStr, ...);
	bool Message(const char* formatStr, ...);
	bool Debug(const char* formatStr, ...);
	bool Verbose(const char* formatStr, ...);

private:
	ErrorLogger(const ErrorLogger& src);
	ErrorLogger& operator=(const ErrorLogger& src);

	Severity m_Verbosity;

	QString m_FileName;
	QFile m_LogFile;
	QTextStream m_LogStream;
};

#endif // ERRORLOGGER_H
