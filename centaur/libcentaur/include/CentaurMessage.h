#pragma once

#include "CentaurTypes.h"

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <boost/serialization/binary_object.hpp>

#define CENTAUR_MESSAGE_VERSION 		1

enum CentaurMessageType {
	CENTAUR_MESSAGE_INVALID	= 0
};

class CentaurMessage {

	friend class boost::serialization::access;

	template<class Archive>
	void serialize(Archive &ar, const unsigned int version = CENTAUR_MESSAGE_VERSION)
	{
		ar << m_type;
	}

protected:

	CentaurMessageType m_type;

public:

	CentaurMessage(CentaurMessageType type)
	:	m_type(type)
	{}

	virtual ~CentaurMessage(){}

	CentaurMessageType getType() { return m_type; }

	bool serialize(CentaurBlob &serialization, bool compress = false)
	{

	}
};
