//
// StringDeduplicator.h
//
// Keep a single copy of all strings passed in and return a reference to that one copy.
//
// (c) 2014 Graham West

#include <QString>

class StringDeduplicator
{
public:
	StringDeduplicator();
	~StringDeduplicator();

	const QString& Deduplicate(const QString& str);
	
private:
	StringDeduplicator(const StringDeduplicator& src);
	StringDeduplicator& operator=(const StringDeduplicator& src);
};
