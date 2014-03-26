//
// StringUtils.cpp
//
// String token parsing and manipulation.
//
// (c) 2014 Graham West

// Class header, always comes first.
#include "StringUtils.h"

// Library headers.
#include <QRegExp>

StringUtils::Term StringUtils::NextTerm(QString& line, QString& termDest)
{
	Term retval = END_OF_LINE;
	int used = 0;
	
	termDest.clear();
	line = line.trimmed();
	
	QChar ch = line[0];
	
	if (!line.isEmpty())
	{
		if (ch == '{')
		{
			used++;
			retval = OPEN_STRUCT;
		}
		else if (ch == '}')
		{
			used++;
			retval = CLOSE_STRUCT;
		}
		else if (ch == '=')
		{
			used++;
			retval = EQUALS;
		}
		else if (ch == '#')
		{
			// A comment uses everything left; there's no close-comment marker.
			used = line.length();
			retval = COMMENT;
		}
		else
		{
			bool done = false;
			bool inQuotes = false;
			bool valueOnly = false;
			
			while (!done && used <= line.length())
			{
				if (inQuotes)
				{
					// We need to handle escape characters so we don't
					// falsely use an escaped quote as a close quote.
					if (ch == '"')
					{
						inQuotes = false;
					}
					else if (ch == '\\' && (used + 1) < line.length())
					{
						// We don't care what's quoted, just skip over it.
						used++;
					}
					
					used++;
				}
				else
				{
					if (ch == '#' || ch == '{' || ch == '}' || ch == '=' ||
						ch.isSpace())
					{
						done = true;
					}
					else
					{
						// Attributes cannot have quotes or escapes, to
						// prevent confusion. They also cannot have dots
						// because those are used in the journal entries to
						// find the attribute in the file and tree.
						if (ch == '\\' || ch == '.')
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
				
				if (used < line.length())
				{
					ch = line[used];
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
	
	termDest = line.left(used);
	line = line.remove(0, used);

	return retval;
}

StringUtils::ValueError StringUtils::UnquoteTerm(const QString& term,
	QString& termDest)
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

QString StringUtils::QuoteTerm(const QString& term, bool always)
{
	QString retval("");

	if (always || MustQuote(term))
	{
		retval = '"';

		for (int count = 0; count < term.length(); count++)
		{
			if (term[count] == '"')
			{
				retval.push_back("\\\"");
			}
			else if (term[count] == '\\')
			{
				retval.push_back("\\\\");
			}
			else
			{
				retval.push_back(term[count]);
			}
		}

		retval += '"';
	}
	else
	{
		// No need to transform something which has no special characters.
		retval = term;
	}

	return retval;
}

bool StringUtils::MustQuote(const QString& term)
{
	bool retval = false;

	// Needs two-deep escaping so the expression is '[\s"\.\\]', which will
	// match a double quote, a period, a backslash or whitespace.
	QRegExp quoteables("[\\s\"\\.\\\\]");

	if (term.contains(quoteables))
	{
		retval = true;
	}

	return retval;
}
