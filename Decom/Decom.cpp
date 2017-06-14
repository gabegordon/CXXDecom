#include "stdafx.h"
#include <iostream>
#include <bitset>
#include <iterator>
#include <stdlib.h>  
#include "Decom.h"
#include "ByteManipulation.h"
#include "HeaderDecode.h"

#define LAST(k,n) ((k) & ((1<<(n))-1))
#define BITS(k,m,n) LAST((k)>>(m),((n)-(m)))

typedef uint8_t BYTE;
using namespace ByteManipulation;
using std::cout;
using std::endl;

void Decom::init(const std::string& infile)
{
	m_infile.open("C:/JPSS/tlm.pkt", std::ios::binary | std::ios::in);
	if (!m_infile || !m_infile.is_open())
	{
		std::cerr << "Failed to open .pkt file" << std::endl;
	}
	HeaderDecode::decodeHeaders(m_infile);
}

void Decom::debugPrinter() const
{

}


