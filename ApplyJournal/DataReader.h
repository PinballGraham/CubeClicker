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
		NONE = 0,
		MALFORMED_ATTRIBUTE,
		CONTEXT_UNDERFLOW,
		UNFINISHED_VALUE,
		MORE_AFTER_VALUE
	};

	DataReader();
	~DataReader();

	DataHierarchy* Read(const QString& fileName);

private:
	DataReader(const DataReader& src);
	DataReader& operator=(const DataReader& src);

	Error ParseLine(QString line);

	QStack<DataHierarchy*> m_Contexts;
};

#endif // DATAREADER_H
