//
// DataWriter.h
//
// Output a data hierarchy of attribute/value pairs as formatted text.
//
// (c) 2014 Graham West

// Class header, always comes first.
#include "DataWriter.h"

// Library includes.
#include <QFile>

// Common includes.
#include "StringDeduplicator.h"

// Number of spaces prepended to each line, per child depth.
static const uint DEFAULT_INDENT = 4;

DataWriter::DataWriter() : m_Indent(DEFAULT_INDENT)
{
}

DataWriter::~DataWriter()
{
}

bool DataWriter::Write(const DataHierarchy* const hierarchy,
	const QString& fileName, const QDateTime& loaded) const
{
	bool retval = false;

	if (hierarchy && !fileName.isEmpty())
	{
		QFile file(fileName);

		if (file.open(QIODevice::WriteOnly))
		{
			QTextStream stream(&file);

			if (LeadingComment(stream, loaded))
			{
				retval = WriteHierarchy(stream, hierarchy, 0);
			}

			if (retval)
			{
				retval = TrailingComment(stream);
			}
		}

		file.close();
	}

	return retval;
}

bool DataWriter::WriteHierarchy(QTextStream& stream,
	const DataHierarchy* const hierarchy, uint depth) const
{
	bool retval = true;

	if (hierarchy)
	{
		QList<uint> attribIds;
		hierarchy->AllAttributes(attribIds);
		int count = 0;

		while (retval && count < attribIds.size())
		{
			const QString& attribName = StringDeduplicator::Retrieve(attribIds[count]);
			DataValue dval = hierarchy->Value(attribIds[count]);

			if (dval.IsBasic())
			{
				const QString& valueStr = dval.BasicString();

				Indent(stream, depth * m_Indent);
				stream << attribName;
				stream << " = ";
				stream << valueStr;
				stream << "\n";
			}
			else if (dval.IsStruct())
			{
				Indent(stream, depth * m_Indent);
				stream << attribName;
				stream << " = ";
				stream << "{\n";
				retval = WriteHierarchy(stream, dval.StructValue(), depth + 1);
				Indent(stream, depth * m_Indent);
				stream << "}\n";
			}

			count++;
		}
	}
	
	return retval;
}

bool DataWriter::LeadingComment(QTextStream& stream, const QDateTime& loaded) const
{
	stream << "# Processed by ApplyJournal:\n";
	stream << "# Previous version read at ";
	stream << loaded.toString("yyyy-MM-dd hh:mm:ss");
	stream << "\n\n";
	return true;
}

bool DataWriter::TrailingComment(QTextStream& stream) const
{
	stream << "\n";
	stream << "# New version written at ";
	stream << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
	return true;
}

void DataWriter::Indent(QTextStream& stream, uint amount) const
{
	for (uint count = 0; count < amount; count++)
	{
		stream << " ";
	}
}
