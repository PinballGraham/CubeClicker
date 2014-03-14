//
// DataReader.h
//
// Read and parse a text file of hierarchical attribute=value pairs, where the
// value is either a string or a structure containing its own attribute=value
// pairs.
//
// (c) 2014 Graham West

// Class header, always comes first.
#include "DataReader.h"

// Library includes.
#include <QFile>

static const qint64 MAX_LINE_LEN = 50000;

DataReader::DataReader()
{
}

DataReader::~DataReader()
{
}

DataHierarchy* DataReader::Read(const QString& fileName)
{
	DataHierarchy* retval = 0;
	QFile file(fileName);
	
	if (file.exists() && file.open(QIODevice::ReadOnly))
	{
		// We're starting a new hierarchy.
		m_Contexts.clear();

		DataHierarchy* root = new DataHierarchy;
		m_Contexts.push(root);

		char lineBuffer[MAX_LINE_LEN];
		bool ok = true;

		// Process the file line by line.
		while (!file.atEnd() && ok)
		{
			qint64 lineRead = file.readLine(lineBuffer, MAX_LINE_LEN);

			if (lineRead > 0)
			{
				ok = ParseLine(QString(lineBuffer));
			}
		}
		
		file.close();
		
		if (!ok || root->Children() == 0)
		{
			delete root;
		}
		else
		{
			if (m_Contexts.size() == 1)
			{
				retval = m_Contexts.pop();
			}
			else
			{
				// Mismatched { and }.
				// DataHierarchy's destructor deletes all children.
				delete root;
			}
		}
	}

	return retval;
}

DataReader::Error DataReader::ParseLine(QString line)
{
	Error retval = NONE;
	QString trimmedLine = line.trimmed();
	bool ok = true;
	DataHierarchy* current = m_Contexts.top();

	while (ok && !trimmedLine.isEmpty() && trimmedLine[0] != '#')
	{
		// Find the leftmost equals
		// Take the left substring up to that and trim it
		// Check it contains no spaces
		// That's the attribute name
		// Take the right substring after the equals and trim it
		// If it's a {
		//   Start a new DataHierarchy and push it onto the context stack
		// Else if it's a }
		//   Pop the last DataHierarchy from the context stack
		//   If the stack's empty, error out
		// Else (it's a basic value)
		//   Find the end of the value accounting for quotes etc
		//   Check it doesn't end partway through a quoted string
		//   That's the value
		//   Add the attrib and value to the current hierarchy
		//   If the line's not empty, error out
	}

	return retval;
}
