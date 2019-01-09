#ifndef MYSTREAM_H
#define MYSTREAM_H
#include <iostream>


class MyStream {
	std::ostream& out1;
	std::ostream& out2;
public:
	template <typename T>
	MyStream& operator<<(T out) {
		out1 << out;
		out2 << out;
		return *this;
	}
	MyStream(std::ostream& fileStream, std::ostream& to_cout);
	operator bool();
	void flush();
};
#endif

