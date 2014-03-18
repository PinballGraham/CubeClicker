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

DataReader::DataReader() : m_currentLine("")
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
		Error err = ERROR_OK;

		// Process the file line by line.
		while (!file.atEnd() && err == ERROR_OK)
		{
			qint64 lineRead = file.readLine(lineBuffer, MAX_LINE_LEN);

			if (lineRead > 0)
			{
				err = ParseLine(QString(lineBuffer));
			}
		}
		
		file.close();
		
		if (err != ERROR_OK || root->Children() == 0)
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

DataReader::Error DataReader::ParseLine(const QString& fileLine)
{
	Error retval = ERROR_OK;
	DataHierarchy* current = m_Contexts.top();

	m_currentLine = fileLine.trimmed();
	State currState = STATE_CLOSE_OR_ATTRIB;

	QString termStr("");
	Term currTerm = NextTerm(termStr);
	bool done = false;
	QString attribName("");

	while (!done)
	{
		switch (currState)
		{
			case STATE_CLOSE_OR_ATTRIB:
				if (currTerm == CLOSE_STRUCT)
				{
					// The top of the stack is the context we are currently
					// in, so it should never be empty.
					if (m_Contexts.size() > 1)
					{
						m_Contexts.pop();
						current = m_Contexts.top();
					}
					else
					{
						retval = ERROR_CONTEXT_UNDERFLOW;
						done = true;
					}
				}
				else if (currTerm == ATTRIB_OR_VALUE)
				{
					attribName = termStr;
					currState = STATE_EQUALS;
				}
				else if (currTerm == COMMENT || currTerm == END_OF_LINE)
				{
					// We're not partway through an attrib/value pair.
					done = true;
				}
				else if (currTerm == OPEN_STRUCT || currTerm == EQUALS)
				{
					retval = ERROR_MISSING_ATTRIBUTE;
					done = true;
				}
				else
				{
					retval = ERROR_MALFORMED_ATTRIBUTE;
					done = true;
				}

				break;

			case STATE_EQUALS:
				//
				if (currTerm == EQUALS)
				{
					currState = STATE_VALUE_OR_OPEN;
				}
				else
				{
					// Anything else leaves us with a dangling attribute.
					retval = ERROR_NO_EQUALS;
					done = true;
				}

				break;

			case STATE_VALUE_OR_OPEN:
				if (currTerm == ATTRIB_OR_VALUE || currTerm == VALUE_ONLY)
				{
					QString unquotedTerm("");
					ValueError verr = UnquoteTerm(termStr, unquotedTerm);

					if (verr == VALUE_OK)
					{
						current->Set(attribName, termStr);
						currState = STATE_CLOSE_OR_ATTRIB;
					}
					else
					{
						retval = ERROR_UNFINISHED_VALUE;
						done = true;
					}
				}
				else if (currTerm == OPEN_STRUCT)
				{
					// This attribute is a structure rather than a simple
					// value, so it needs to be added to the context stack
					// as well as set as a property in its parent.
					DataHierarchy* newStruct = new DataHierarchy;
					current->Set(attribName, newStruct);
					m_Contexts.push(newStruct);
					current = m_Contexts.top();
					currState = STATE_CLOSE_OR_ATTRIB;
				}
				else
				{
					retval = ERROR_MISSING_VALUE;
					done = true;
				}

				break;

			default:
				retval = ERROR_UNKNOWN_TERM;
				done = true;
				break;
		}

		if (!done)
		{
			currTerm = NextTerm(termStr);
		}
	}

	return retval;
}

DataReader::Term DataReader::NextTerm(QString& termDest)
{
	Term retval = END_OF_LINE;
	int used = 0;

	termDest.clear();
	m_currentLine = m_currentLine.trimmed();

	QChar ch = m_currentLine[0];
	used++;

	if (!m_currentLine.isEmpty())
	{
		if (ch == '{')
		{
			retval = OPEN_STRUCT;
		}
		else if (ch == '}')
		{
			retval = CLOSE_STRUCT;
		}
		else if (ch == '=')
		{
			retval = EQUALS;
		}
		else if (ch == '#')
		{
			retval = COMMENT;
		}
		else
		{
			bool done = false;
			bool inQuotes = false;
			bool valueOnly = false;

			while (!done && used < m_currentLine.length())
			{
				if (inQuotes)
				{
					// We need to handle escape characters so we don't
					// falsely use an escaped quote as a close quote.
					if (ch == '"')
					{
						inQuotes = false;
					}
					else if (ch == '\\' && (used + 1) < m_currentLine.length())
					{
						// We don't care what's quoted, just skip over it.
						used++;
					}

					used++;
				}
				else
				{
					// 
					if (ch == '#' || ch == '{' || ch == '}' || ch == '=' ||
						ch.isSpace())
					{
						done = true;
					}
					else
					{
						// Attributes cannot have quotes or escapes, to
						// prevent confusion.
						if (ch == '\\')
						{
							valueOnly = true;
						}
						else if (ch == '"')
						{
							inQuotes = true;
						}
						used++;
					}
				}

				if (used < m_currentLine.length())
				{
					ch = m_currentLine[used];
				}
			}

			if (valueOnly)
			{
				retval = VALUE_ONLY;
			}
			else
			{
				retval = ATTRIB_OR_VALUE;
			}
		}
	}

	termDest = m_currentLine.left(used);
	m_currentLine = m_currentLine.remove(0, used);
	return retval;
}

DataReader::ValueError DataReader::UnquoteTerm(const QString& term,
	QString& termDest) const
{
	ValueError retval = VALUE_OK;
	bool done = false;
	bool inQuotes = false;
	bool ok = true;
	int pos = 0;

	termDest.clear();

	// We have to build the results character by character because we want
	// to skip quotes and escape characters in the middle of the term.
	while (ok && !done)
	{
		if (inQuotes)
		{
			if (pos == term.length())
			{
				// Don't allow unmatched quotes.
				done = true;
				ok = false;
				retval = VALUE_MISSING_QUOTE;
			}
			if (term[pos] == '"')
			{
				inQuotes = false;
			}
			else if (term[pos] == '\\')
			{
				// Escape must be followed by a character.
				if ((pos + 1) < term.length())
				{
					pos++;
					termDest.push_back(term[pos]);
				}
				else
				{
					done = true;
					ok = false;
					retval = VALUE_UNFINISHED_ESCAPE;
				}
			}
			else
			{
				termDest.push_back(term[pos]);
			}
		}
		else
		{
			// We're not inside quotes, so whitespace or EOL marks our end.
			if (pos == term.length() || term[pos].isSpace())
			{
				done = true;
			}
			else if (term[pos] == '"')
			{
				inQuotes = true;
			}
			else if (term[pos] == '\\')
			{
				done = true;
				retval = VALUE_UNQUOTED_ESCAPE;
			}
			else
			{
				termDest.push_back(term[pos]);
			}
		}

		// We stopped at this character, no need to move on.
		if (!done)
		{
			pos++;
		}
	}

	if (!ok || pos <= 0)
	{
		// The line might have ended at an unmatched quote.
		termDest.clear();
	}

	return retval;
}
