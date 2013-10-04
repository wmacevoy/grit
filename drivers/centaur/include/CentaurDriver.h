#pragma once

#include <boost/crc.hpp>

#pragma pack(1)

enum CentaurDriverType {

	CENTAURDRIVERTYPE_
};


struct CentaurDriverRegistryEntry {

	CentaurDriverType m_type;
	CentaurDriverType m_type;

	boost::uint_t<32> m_crc32;



}

#pragma pack()
