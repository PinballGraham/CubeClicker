//
// DataHierarchy.h
//
// Store sets of attribute=value pairs in a map, where the value can be either
// a string or a child map with its own set of attribute=value pairs.
//
// (c) 2014 Graham West

// Class header, always comes first.
#include "DataHierarchy.h"

#include "StringDeduplicator.h"

static QString emptyStr("");

DataValue::DataValue() : m_Type(INVALID), m_BasicValue(0), m_StructValue(0)
{
}

DataValue::DataValue(uint basicValue) : m_Type(BASIC),
	m_BasicValue(basicValue), m_StructValue(0)
{
}

DataValue::DataValue(DataHierarchy* structValue) : m_Type(STRUCT),
	m_BasicValue(0), m_StructValue(structValue)
{
}

DataValue::DataValue(const DataValue& src) :
	m_Type(src.m_Type), m_BasicValue(src.m_BasicValue),
	m_StructValue(src.m_StructValue)
{
}

DataValue::~DataValue()
{
}

DataValue& DataValue::operator=(const DataValue& src)
{
	m_Type = src.m_Type;
	m_BasicValue = src.m_BasicValue;
	m_StructValue = src.m_StructValue;
	
	return *this;
}

uint DataValue::BasicValue() const
{
	uint retval = 0;

	if (IsBasic())
	{
		retval = m_BasicValue;
	}

	return retval;
}

const QString& DataValue::BasicString() const
{
	if (IsBasic())
	{
		return StringDeduplicator::Retrieve(m_BasicValue);
	}
	else
	{
		return emptyStr;
	}
}

DataHierarchy* DataValue::StructValue() const
{
	DataHierarchy* retval = 0;

	if (IsStruct())
	{
		retval = m_StructValue;
	}
	
	return retval;
}

DataHierarchy::DataHierarchy()
{
}

DataHierarchy::~DataHierarchy()
{
	// The DataValue instances don't delete a struct value, so we have
	// to do that ourselves.
	if (!m_Children.isEmpty())
	{
		ChildrenMap::iterator iter = m_Children.begin();

		while (iter != m_Children.end())
		{
			if (iter->IsStruct() && iter->StructValue())
			{
				delete iter->StructValue();
			}

			iter++;
		}
	}
}

bool DataHierarchy::Contains(const QString& attrib) const
{
	return Contains(qHash(attrib));
}

bool DataHierarchy::Contains(uint attribHash) const
{
	bool retval = false;

	ChildrenMap::const_iterator iter = m_Children.find(attribHash);

	if (iter != m_Children.end())
	{
		retval = true;
	}

	return retval;
}

DataValue DataHierarchy::Value(const QString& attrib) const
{
	return Value(qHash(attrib));
}

DataValue DataHierarchy::Value(uint attribHash) const
{
	DataValue retval;
	ChildrenMap::const_iterator iter = m_Children.find(attribHash);

	if (iter != m_Children.end())
	{
		retval = iter.value();
	}

	return retval;
}

int DataHierarchy::AllAttributes(QList<uint>& destAttribIds) const
{
	int retval = 0;

	destAttribIds.clear();
	ChildrenMap::const_iterator iter = m_Children.begin();
	
	while (iter != m_Children.end())
	{
		destAttribIds.push_back(iter.key());
		iter++;
		retval++;
	}

	return retval;
}

bool DataHierarchy::Set(const QString& attrib, const QString& basicValue)
{
	uint attribHash = StringDeduplicator::Store(attrib);
	return Set(attribHash, basicValue);
}

bool DataHierarchy::Set(uint attribHash, const QString& basicValue)
{
	uint valueId = StringDeduplicator::Store(basicValue);
	DataValue newChild(valueId);
	
	return Set(attribHash, newChild);
}

bool DataHierarchy::Set(const QString& attrib, DataHierarchy* structValue)
{
	bool retval = false;
	
	if (structValue)
	{
		uint attribHash = StringDeduplicator::Store(attrib);
		retval = Set(attribHash, structValue);
	}

	return retval;
}

bool DataHierarchy::Set(uint attribHash, DataHierarchy* structValue)
{
	DataValue newChild(structValue);
	
	return Set(attribHash, newChild);
}

bool DataHierarchy::Set(uint attribHash, DataValue val)
{
	bool retval = false;
	
	ChildrenMap::iterator iter = m_Children.find(attribHash);
	
	// Distinguish between newly-added attributes, and modifications of
	// existing attributes.
	if (iter == m_Children.end())
	{
		retval = true;
	}
	
	m_Children.insert(attribHash, val);
	
	return retval;
}
