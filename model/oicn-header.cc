/* -*- Mode:C++; c-file-style; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2016, CTO Networks Lab, Tata Consultancy Services (TCS)
 *
 * Author: Anirudh Morabagal <anirudhmk.2010@gmail.com>
 */
#include "oicn-header.h"
#include "ns3/type-name.h"
#include "ns3/type-id.h"
#include "ns3/log.h"
#include <stdint.h>
#include <string>
#include <cstring>

NS_LOG_COMPONENT_DEFINE ("OICNHeader");

namespace ns3{

NS_OBJECT_ENSURE_REGISTERED (OICNHeader);

TypeId OICNHeader::GetTypeId (void)
{
	static TypeId tid = TypeId ("ns3::OICNHeader")
    		.SetParent<Header> ()
			.AddConstructor<OICNHeader> ();

	return tid;
}

OICNHeader::OICNHeader ()
: m_name(),
  header_length(0),
  first4bytes(0)
{
}

OICNHeader::~OICNHeader ()
{
}

TypeId
OICNHeader::GetInstanceTypeId (void) const
{
	return GetTypeId ();
}

void OICNHeader::SetName(std::string name)
{
	m_name = name;
	header_length = name.size() + 4; //4 bytes
}

std::string OICNHeader::GetName()
{
	return(m_name);
}

void OICNHeader::SetFirst4Bytes(uint32_t HeaderLength, bool reply, bool cachable)
{
	if (reply)
	{
		if (cachable) first4bytes = cachable_start + HeaderLength;
		else first4bytes = non_cachable_start + HeaderLength;
	}
	else first4bytes = request_start + HeaderLength;
}

uint32_t OICNHeader::GetFirst4Bytes()
{
	return(first4bytes);
}

void OICNHeader::SetRequest()
{
	SetFirst4Bytes(header_length, false, false);
}

void OICNHeader::SetCachable()
{
	SetFirst4Bytes(header_length, true, true);
}

void OICNHeader::SetNonCachable()
{
	SetFirst4Bytes(header_length, true, false);
}

void OICNHeader::Serialize (Buffer::Iterator start) const
{
	Buffer::Iterator i = start;
	i.WriteHtonU32(first4bytes);
	char name[m_name.size()];
	memset(name,0,m_name.size());
	strcpy(name,m_name.c_str());
	i.Write ((uint8_t*)name, m_name.size());
}

uint32_t OICNHeader::Deserialize (Buffer::Iterator start)
{
	Buffer::Iterator i = start;
	first4bytes = i.ReadNtohU32();
	if (first4bytes > cachable_start) header_length = first4bytes - cachable_start;
	else if(first4bytes > non_cachable_start) header_length = first4bytes - non_cachable_start;
	else if(first4bytes > request_start) header_length = first4bytes - request_start;
	char name[header_length-3];
	memset(name,0,header_length-3);
	i.Read((uint8_t*)name, (header_length-4));
	m_name = name;
	return GetSerializedSize ();
}

uint32_t OICNHeader::GetSerializedSize (void) const
{
	return(header_length);
}

void OICNHeader::Print (std::ostream &os) const
{
}
}