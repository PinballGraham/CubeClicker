//
// StringUtils.h
//
// String token parsing and manipulation.
//
// (c) 2014 Graham West

#if !defined(STRINGUTILS_H)
#define STRINGUTILS_H

// Library headers.
#include <QString>

class StringUtils
{
public:
	enum Term {
		OPEN_STRUCT = 0,
		CLOSE_STRUCT,
		EQUALS,
		COMMENT,
		END_OF_LINE,
		ATTRIB_OR_VALUE,
		VALUE_ONLY
	};
	
	enum ValueError {
		VALUE_OK,
		VALUE_MISSING_QUOTE,
		VALUE_UNFINISHED_ESCAPE,
		VALUE_UNQUOTED_ESCAPE
	};
	
	static Term NextTerm(QString& line, QString& termDest);
	static ValueError UnquoteTerm(const QString& term, QString& termDest);

	// By default this will only add quotes, backslashes etc if the term
	// needs them, to avoid things like "layer=1000" winding up with quotes.
	static QString QuoteTerm(const QString& term, bool always = false);
	
private:
	static bool MustQuote(const QString& term);
};

#endif // STRINGUTILS_H
