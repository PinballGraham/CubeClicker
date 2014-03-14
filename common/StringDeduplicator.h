//
// StringDeduplicator.h
//
// Keep a single copy of all strings passed in and return a reference to that
// one copy.
//
// (c) 2014 Graham West

#include <QMap>
#include <QString>

class StringDeduplicator
{
public:
	StringDeduplicator();
	~StringDeduplicator();

	static StringDeduplicator* Instance();

	static uint Find(const QString& str);
	static uint Store(const QString& str);
	static uint StoreNoCase(const QString& str);
	static const QString& Retrieve(uint hash);
	
	static int Total();
	
private:
	StringDeduplicator(const StringDeduplicator& src);
	StringDeduplicator& operator=(const StringDeduplicator& src);

	static StringDeduplicator* m_Instance;
	
	static uint Add(uint hash, const QString& str);

	typedef QMap<uint, QString> DeduplicatorMap;
	
	DeduplicatorMap m_Strings;
	
};
