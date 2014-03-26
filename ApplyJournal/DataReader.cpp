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

// Library headers.
#include <QFile>

// Common headers.
#include "StringUtils.h"

static const qint64 MAX_LINE_LEN = 50000;

DataReader::DataReader() : m_CurrentLine("")
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
				QString line(lineBuffer);
				err = ParseLine(line);
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

	m_CurrentLine = fileLine.trimmed();
	State currState = STATE_CLOSE_OR_ATTRIB;

	QString termStr("");
	StringUtils::Term currTerm = StringUtils::NextTerm(m_CurrentLine, termStr);
	bool done = false;
	QString attribName("");

	while (!done)
	{
		switch (currState)
		{
			case STATE_CLOSE_OR_ATTRIB:
				if (currTerm == StringUtils::CLOSE_STRUCT)
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
				else if (currTerm == StringUtils::ATTRIB_OR_VALUE)
				{
					attribName = termStr;
					currState = STATE_EQUALS;
				}
				else if (currTerm == StringUtils::COMMENT ||
					currTerm == StringUtils::END_OF_LINE)
				{
					// We're not partway through an attrib/value pair.
					done = true;
				}
				else if (currTerm == StringUtils::OPEN_STRUCT ||
					currTerm == StringUtils::EQUALS)
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
				if (currTerm == StringUtils::EQUALS)
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
				if (currTerm == StringUtils::ATTRIB_OR_VALUE ||
					currTerm == StringUtils::VALUE_ONLY)
				{
					QString unquotedTerm("");
					StringUtils::ValueError verr = StringUtils::UnquoteTerm(termStr, unquotedTerm);

					if (verr == StringUtils::VALUE_OK)
					{
						current->Set(attribName, unquotedTerm);
						currState = STATE_CLOSE_OR_ATTRIB;
					}
					else
					{
						retval = ERROR_UNFINISHED_VALUE;
						done = true;
					}
				}
				else if (currTerm == StringUtils::OPEN_STRUCT)
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
			currTerm = StringUtils::NextTerm(m_CurrentLine, termStr);
		}
	}

	return retval;
}

