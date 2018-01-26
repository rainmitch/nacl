
#ifndef NACL_H
#define NACL_H

#include <iostream>
#include <vector>

bool isLetter (char c);

class Word
{
public:
	Word (std::string val="", int line=0, int col=0);
	
	bool operator== (std::string val);
	bool operator== (Word val);
	
	std::string val;
	int line;
	int col;
};

class Lexer
{
public:
	void feed (std::string input);
	
	Word &operator[] (int place);
	
	std::vector<Word> parts;
};

class Error
{
public:
	std::string message;
	Word begining;
};

class AST
{
public:
	AST (std::string val="");
	AST (Word val);
	void add (Word val);
	void add (AST val);
	void add (std::vector<AST> values);
	int size ();
	AST &operator[] (int place);
	void print (int depth=0);
	
	Word val;
	std::vector<AST> children;
};

#endif
