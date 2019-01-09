#include "mystream.h"

MyStream::MyStream(std::ostream& fileStream, std::ostream& to_cout) : out1(fileStream), out2(to_cout) {}

MyStream::operator bool()
{
	return  out1 && out2;
}

void MyStream::flush()
{
	out1.flush();
	out2.flush();
}

