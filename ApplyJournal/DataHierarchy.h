//
// DataHierarchy.h
//
// Store sets of attribute=value pairs in a map, where the value can be either
// a string or a child map with its own set of attribute=value pairs.
//
// (c) 2014 Graham West

#if !defined(DATAHIERARCHY_H)
#define DATAHIERARCHY_H

// Library headers.
#include <QMap>
#include <QVariant>

class DataHierarchy;

class DataValue
{
public:
	enum Type {
		INVALID = 0,
		STRUCT,
		BASIC
	};
	
	DataValue();
	DataValue(uint basicValue);
	DataValue(DataHierarchy* structValue);

	// We do a shallow copy on the structure pointer.
	DataValue(const DataValue& src);
	DataValue& operator=(const DataValue& src);

	~DataValue();

	inline bool IsValid() const { return (IsBasic() || IsStruct()); }
	inline bool IsBasic() const { return (m_Type == BASIC); }
	inline bool IsStruct() const { return (m_Type == STRUCT); }

	uint BasicValue() const;
	const QString& BasicString() const;
	DataHierarchy* StructValue() const;

private:

	Type m_Type;
	uint m_BasicValue;
	DataHierarchy* m_StructValue;
};

class DataHierarchy
{
public:
	DataHierarchy();
	~DataHierarchy();

	inline int Children() const { return m_Children.size(); }
	bool Contains(const QString& attrib) const;
	bool Contains(uint attribHash) const;

	DataValue Value(const QString& attrib) const;
	DataValue Value(uint attribHash) const;

	int AllAttributes(QList<uint>& destAttribIds) const;

	bool Set(const QString& attrib, const QString& basicValue);
	bool Set(uint attribHash, const QString& basicValue);
	bool Set(const QString& attrib, DataHierarchy* structValue);
	bool Set(uint attribHash, DataHierarchy* structValue);
	
private:
	DataHierarchy(const DataHierarchy& src);
	DataHierarchy& operator=(const DataHierarchy& src);

	bool Set(uint attribHash, DataValue val);

	typedef QMap<uint,DataValue> ChildrenMap;
	
	ChildrenMap m_Children;
};

#endif // DATAHIERARCHY_H
