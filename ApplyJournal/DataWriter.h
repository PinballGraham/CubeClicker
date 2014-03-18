//
// DataWriter.h
//
// Output a data hierarchy of attribute/value pairs as formatted text.
//
// (c) 2014 Graham West

#if !defined(DATAWRITER_H)
#define DATAWRITER_H

// Library includes.
#include <QDateTime>
#include <QTextStream>

// Application includes.
#include "DataHierarchy.h"

class DataWriter
{
public:
	DataWriter();
	~DataWriter();
	
	bool Write(const DataHierarchy* const hierarchy, const QString& fileName,
		const QDateTime& loaded) const;
	
private:
	DataWriter(const DataWriter& src);
	DataWriter& operator=(const DataWriter& src);

	bool WriteHierarchy(QTextStream& stream, const DataHierarchy* const hierarchy, uint depth) const;
	bool LeadingComment(QTextStream& stream, const QDateTime& loaded) const;
	bool TrailingComment(QTextStream& stream) const;
	void Indent(QTextStream& stream, uint amount) const;

	uint m_Indent;
};

#endif // DATAWRITER_H
