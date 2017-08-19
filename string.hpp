#ifndef R_STRING_HPP
#define R_STRING_HPP
#include <vector>
#include <string>

namespace rlib{
	using std::string;
	using std::vector;
	vector<string> splitString(const string &tod, const char divider = ' ')
	{
		size_t lastPos = 0;
		size_t thisPos = tod.find(divider);
		vector<string> sbuf;
		if (thisPos != ::std::string::npos)
		{
			sbuf.push_back(tod.substr(0, thisPos));
			goto gt_1;
		}
		else
	    {
	        sbuf.push_back(tod);
	        return sbuf;
	    }
		do {
			sbuf.push_back(tod.substr(lastPos + 1, thisPos - lastPos - 1));
		gt_1:
			lastPos = thisPos;
			thisPos = tod.find(divider, lastPos + 1);
		} while (thisPos != ::std::string::npos);
		sbuf.push_back(tod.substr(lastPos + 1));
		return ::std::move(sbuf);
	}
}

#endif