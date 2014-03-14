//
// DataWriter.h
//
// Output a data hierarchy of attribute/value pairs as formatted text.
//
// (c) 2014 Graham West

#if !defined(DATAWRITER_H)
#define DATAWRITER_H

// Application headers.
#include "DataHierarchy.h"

class DataWriter
{
public:
	DataWriter();
	~DataWriter();
	
	bool Write(const DataHierarchy* const hierarchy, const QString& fileName);
	
private:
	DataWriter(const DataWriter& src);
	DataWriter& operator=(const DataWriter& src);
};

#endif // DATAWRITER_H
