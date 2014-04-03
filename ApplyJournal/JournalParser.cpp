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

JournalParser::JournalParser(const QString& fileName, bool fixChecksums) :
	m_FileName(fileName), m_FixChecksums(fixChecksums), m_LinesRead(0)
{
}

JournalParser::~JournalParser()
{
}

bool JournalParser::Process()
{
	bool retval = false;

	QFile file(m_FileName);
	
	if (file.exists() && file.open(QIODevice::ReadOnly))
	{
		char lineBuffer[MAX_LINE_LEN];
		Error err = ERROR_OK;
		
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

	retval = ChecksumLine(line, checksum);

	if (retval == ERROR_OK)
	{
		QString termStr("");
		StringUtils::Term currTerm = StringUtils::NextTerm(line, termStr);
		bool done = false;
		QString attribName("");
		
		while (!done)
		{
			done = true;
		}
	}

	return retval;
}

JournalParser::Error JournalParser::ChecksumLine(QString& line, quint16 &value)
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
