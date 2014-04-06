//
// JournalParser.cpp
//
// Read a journal file and apply updates to the data files, checking each
// transaction's integrity.
//
// (c) 2014 Graham West

// Class header, always comes first.
#include "JournalParser.h"

// Library headers.
#include <QByteArray>
#include <QFile>

// Common headers.
#include "StringDeduplicator.h"
#include "StringUtils.h"

static const qint64 MAX_LINE_LEN = 50000;

JournalParser::JournalParser(const QString& fileName, DataFileTracker* tracker,
	bool fixChecksums) :
		m_FileName(fileName), m_FileTracker(tracker), m_FixChecksums(fixChecksums),
		m_LinesRead(0)
{
}

JournalParser::~JournalParser()
{
}

bool JournalParser::Process()
{
	bool retval = false;
	QFile file(m_FileName);
	
	if (m_FileTracker && file.exists() && file.open(QIODevice::ReadOnly))
	{
		char lineBuffer[MAX_LINE_LEN];
		Error err = ERROR_OK;

		// We might be reprocessing the file.
		m_LinesRead = 0;

		// Process the file line by line.
		while (!file.atEnd() && err == ERROR_OK)
		{
			qint64 lineRead = file.readLine(lineBuffer, MAX_LINE_LEN);
			m_LinesRead++;

			if (lineRead > 0)
			{
				QString line(lineBuffer);
				line = line.trimmed();

				if (!line.isEmpty())
				{
					err = ParseLine(line);

					if (err != ERROR_OK)
					{
						// Log non-fatal error.
					}
				}
			}
		}
		
		file.close();
		
		if (err == ERROR_OK)
		{
			retval = true;
		}
	}

	return retval;
}

JournalParser::Error JournalParser::ParseLine(const QString& fileLine)
{
	Error retval = ERROR_OK;
	QString line = fileLine.trimmed();
	quint16 checksum = 0;

	ClearUpdates();
	retval = ChecksumLine(line, checksum);

	if (retval == ERROR_OK)
	{
		StringUtils::Term currTerm;
		QString termStr("");
		QString attribPath("");
		bool done = false;
		QString attribName("");
		bool duplicates = false;

		while (!done)
		{
			currTerm = StringUtils::NextTerm(line, termStr);

			if (currTerm == StringUtils::ATTRIB_OR_VALUE)
			{
				attribPath = termStr;
				currTerm = StringUtils::NextTerm(line, termStr);

				if (currTerm == StringUtils::EQUALS)
				{
					currTerm = StringUtils::NextTerm(line, termStr);

					if (currTerm == StringUtils::ATTRIB_OR_VALUE ||
						currTerm == StringUtils::VALUE_ONLY)
					{
						if (!CacheUpdate(attribPath, termStr))
						{
							duplicates = true;
						}
					}
					else
					{
						retval = ERROR_MISSING_VALUE;
					}
				}
				else
				{
					retval = ERROR_NO_EQUALS;
				}
			}
			else if (currTerm == StringUtils::END_OF_LINE)
			{
				done = true;
			}
			else
			{
				retval = ERROR_MISSING_ATTRIBUTE;
			}
		}

		if (duplicates)
		{
			// Log a warning about the line containing duplicate attribs.
		}

		if (retval == ERROR_OK)
		{
			if (CheckUpdates() == ERROR_OK)
			{
				ApplyUpdates();
			}
			else
			{
				// Log a warning about invalid attribs/values.
			}
		}
	}

	return retval;
}

JournalParser::Error JournalParser::ChecksumLine(QString& line, quint16 &value) const
{
	Error retval = ERROR_OK;
	value = 0;

	// There must be something to checksum. Empty lines should not have had
	// a checksum written for them.
	if (!line.isEmpty() && line.length() > 5)
	{
		// A space is required between the line itself and its checksum.
		if (line.at(line.length() - 5).isSpace())
		{
			QString checksumStr = line.right(4);
			line = line.left(line.length() - 5);

			QByteArray utf8 = line.toUtf8();
			quint16 calcChecksum = qChecksum(utf8.data(), utf8.length());

			// Special case for testing.
			if (m_FixChecksums && checksumStr.compare("****") == 0)
			{
				// Log a message with the correct checksum.
			}
			else
			{
				bool isNum = false;
				quint16 storedChecksum = 0;
				storedChecksum = checksumStr.toShort(&isNum, 16);

				if (!isNum || storedChecksum != calcChecksum)
				{
					retval = ERROR_BAD_CHECKSUM;
				}
			}

			if (retval == ERROR_OK)
			{
				value = calcChecksum;
			}
		}
		else
		{
			retval = ERROR_BAD_CHECKSUM;
		}
	}
	else
	{
		retval = ERROR_MISSING_CHECKSUM;
	}

	return retval;
}

void JournalParser::ClearUpdates()
{
	m_PendingUpdates.clear();
}

bool JournalParser::CacheUpdate(const QString& attribPath, const QString& value)
{
	bool retval = true;
	QString key = attribPath.toLower().trimmed();

	if (m_PendingUpdates.contains(key))
	{
		// Log a message with the specific duplicate attrib and value.
		retval = false;
	}

	m_PendingUpdates.insert(key, value);
	return retval;
}

JournalParser::Error JournalParser::CheckUpdates() const
{
	Error retval = ERROR_OK;
	PendingUpdatesMap::const_iterator iter = m_PendingUpdates.begin();

	while (retval == ERROR_OK && iter != m_PendingUpdates.end())
	{
		QStringList pathParts = iter.key().split('.');

		if (pathParts.count() >= 2)
		{
			DataHierarchy* fileHierarchy = 0;

			if (m_FileTracker)
			{
				fileHierarchy = m_FileTracker->Hierarchy(pathParts[0]);
			}

			if (fileHierarchy)
			{
				if (ExistingStructAttribute(pathParts))
				{
					// Log non-fatal error for replacing a struct with a basic value.
					retval = ERROR_STRUCT_REDEFINITION;
				}
			}
			else
			{
				// Log non-fatal error for unidentified file.
				retval = ERROR_FILE_ID_NOT_FOUND;
			}
		}
		else
		{
			// Log non-fatal error for incomplete attribute path.
			retval = ERROR_MALFORMED_ATTRIBUTE;
		}

		iter++;
	}

	return retval;
}

bool JournalParser::ApplyUpdates()
{
	bool retval = true;

	return retval;
}

bool JournalParser::ExistingStructAttribute(const QStringList& attribPath) const
{
	bool retval = false;

	if (attribPath.count() > 1)
	{
		// Work down the tree until we find the final attrib, or a
		// missing attrib - which is fine, we'll create the structs
		// as we go.
	}

	return retval;
}
