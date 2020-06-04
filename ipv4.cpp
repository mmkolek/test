#include "ipv4.hpp"

#include <string>

fnc::IPv4::IPv4()
{
	set(192, 168, 99, 100);
}

fnc::IPv4::IPv4(std::uint8_t octet1,
				std::uint8_t octet2,
				std::uint8_t octet3,
				std::uint8_t octet4)
{
	set(octet1,
		octet2,
		octet3,
		octet4);
}

fnc::IPv4::~IPv4()
{}

void fnc::IPv4::set(std::uint8_t octet1,
					std::uint8_t octet2,
					std::uint8_t octet3,
					std::uint8_t octet4)
{
	octets[0] = octet1;
	octets[1] = octet2;
	octets[2] = octet3;
	octets[3] = octet4;
}

bool fnc::IPv4::operator==(fnc::IPv4 v)
{
	for (int i = 0; i < 4; ++i)
		if ( octets[i] != (v.get())[i] )
			return false;
	return true;
}

fnc::IPv4::operator std::string () const
{
	std::string addr;
	for (int value : octets)
		addr += std::to_string(value) + '.';
	return addr.substr(0, addr.size()-1);
}

std::uint8_t * fnc::IPv4::get()
{
	std::uint8_t addr[4];
	for (std::size_t i = 0; i < 4; ++i)
		addr[i] = octets[i];
	return addr;
}
