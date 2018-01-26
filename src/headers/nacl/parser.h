
#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <vector>

class Rule
{
public:
	std::string &operator[] (int place);
	
	std::string name;
	std::vector<std::string> parts;
};

std::vector<Rule> lex (std::string text);
void setNamespace (std::string name);

std::string constructHeader (Rule rule);
std::string constructHeaders (std::vector<Rule> rules);

std::string constructBody (std::vector<Rule> rule);
std::string constructBodies (std::vector<Rule> rules);


std::string constructParser (std::vector<Rule> rules);
std::string constructParserHeader ();

#endif
