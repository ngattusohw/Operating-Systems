#ifndef PARSER
#define PARSER

#include <string>
class parser
{
	public:
		parser();
		~parser();
		void dirList(std::string file);
		void fileList(std::string file);
};

#endif

