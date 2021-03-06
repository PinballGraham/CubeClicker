//
// DataReader.h
//
// Read and parse a text file of hierarchical attribute=value pairs, where the
// value is either a string or a structure containing its own attribute=value
// pairs.
//
// (c) 2014 Graham West

#if !defined(DATAREADER_H)
#define DATAREADER_H

// Library headers.
#include <QStack>

// Application headers.
#include "DataHierarchy.h"

class DataReader
{
public:
	enum Error {
		ERROR_OK = 0,
		ERROR_MISSING_VALUE,
		ERROR_UNFINISHED_VALUE,
		ERROR_MALFORMED_ATTRIBUTE,
		ERROR_MISSING_ATTRIBUTE,
		ERROR_NO_EQUALS,
		ERROR_CONTEXT_UNDERFLOW,
		ERROR_UNKNOWN_TERM
	};

	enum State {
		STATE_CLOSE_OR_ATTRIB,
		STATE_EQUALS,
		STATE_VALUE_OR_OPEN
	};

	DataReader();
	~DataReader();

	DataHierarchy* Read(const QString& fileName);

private:
	DataReader(const DataReader& src);
	DataReader& operator=(const DataReader& src);

	Error ParseLine(const QString& line);

	QStack<DataHierarchy*> m_Contexts;
	QString m_CurrentLine;
};

#endif // DATAREADER_H
