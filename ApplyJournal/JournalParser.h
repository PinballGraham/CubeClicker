//
// JournalParser.h
//
// Read a journal file and apply updates to the data files, checking each
// transaction's integrity.
//
// (c) 2014 Graham West

#if !defined(JOURNALPARSER_H)
#define JOURNALPARSER_H

// Library headers.
#include <QString>

class JournalParser
{
public:
	enum Error {
		ERROR_OK = 0,
		ERROR_MISSING_CHECKSUM,
		ERROR_BAD_CHECKSUM,
		ERROR_MISSING_VALUE,
		ERROR_UNFINISHED_VALUE,
		ERROR_MALFORMED_ATTRIBUTE,
		ERROR_MISSING_ATTRIBUTE,
		ERROR_NO_EQUALS,
		ERROR_UNKNOWN_TERM
	};

	explicit JournalParser(const QString& fileName, bool fixChecksums = false);
	~JournalParser();

	bool Process();

private:
	JournalParser();
	JournalParser(const JournalParser& src);
	JournalParser& operator=(const JournalParser& src);

	Error ParseLine(const QString& fileLine);
	Error ChecksumLine(QString& line, quint16& value);

	QString m_FileName;
	bool m_FixChecksums;
	unsigned int m_LinesRead;
};

#endif // JOURNALPARSER_H