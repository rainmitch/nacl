
#include "nacl.h"

bool isLetter (char c)
{
	return
	(c >= 'a' and c <= 'z') or
	(c >= 'A' and c <= 'Z') or
	(c >= '0' and c <= '9');
}


Word::Word (std::string val, int line, int col)
{
	this->val = val;
	this->line = line;
	this->col = col;
}

bool Word::operator== (std::string val)
{
	return val == this->val;
}

bool Word::operator== (Word val)
{
	return (val.val == this->val) and (val.line == line) and (val.col == col);
}


void Lexer::feed (std::string input)
{
	int i = 0;
	int col = 0;
	int line = 1;
	
	while (i < input.size ())
	{
		if (isLetter (input[i]))
		{
			std::string str;
			str += input[i];
			i++;
			col++;
			
			while (i < input.size ())
			{
				if (!isLetter (input[i]))
				{
					break;
				}
				str += input[i++];
				col++;
			}
			
			parts.push_back (Word (str, line, col));
		}
		else if (input[i] == '\'')
		{
			std::string str = "\'";
			i++;
			col++;
			
			while (i < input.size ())
			{
				if (input[i] == '\'')
				{
					str += "\'";
					i++;
					col++;
					break;
				}
				else if (input[i] == '\\' and input[i+1] == '\'')
				{
					str += "\\\'";
					i += 2;
					col += 2;
				}
				else
				{
					str += input[i++];
					col++;
				}
			}
			
			parts.push_back (Word (str, line, col));
		}
		else if (input[i] == '<')
		{
			std::string str = "<";
			i++;
			col++;
			
			while (i < input.size ())
			{
				if (input[i] == '>')
				{
					str += ">";
					i++;
					col++;
					break;
				}
				else
				{
					str += input[i++];
					col++;
				}
			}
			
			parts.push_back (Word (str, line, col));
		}
		else if (input[i] == '\"')
		{
			std::string str = "\"";
			i++;
			col++;
			
			while (i < input.size ())
			{
				if (input[i] == '\"')
				{
					str += "\"";
					i++;
					col++;
					break;
				}
				else if (input[i] == '\\' and input[i+1] == '\"')
				{
					str += "\\\"";
					i += 2;
					col += 2;
				}
				else
				{
					str += input[i++];
					col++;
				}
			}
			
			parts.push_back (Word (str, line, col));
		}
		else if (input[i] == ' ' or input[i] == '\t' or input[i] == '\n')
		{
			i++;
			if (input[i] == '\n')
			{
				line++;
				col = 0;
			}
			else
			{
				col++;
			}
		}
		else
		{
			parts.push_back (std::string (1, input[i]));
			i++;
			col++;
		}
	}
}


Word &Lexer::operator[] (int place)
{
	return parts[place];
}


AST::AST (std::string val)
{
	this->val = Word (val);
}

AST::AST (Word val)
{
	this->val = val;
}

void AST::add (Word val)
{
	children.push_back (AST (val));
}

void AST::add (AST val)
{
	children.push_back (val);
}

void AST::add (std::vector<AST> values)
{
	for (AST ast : values)
		add (ast);
}

int AST::size ()
{
	return children.size ();
}

AST &AST::operator[] (int place)
{
	return children[place];
}

void AST::print (int depth)
{
	for (int i = 0; i < depth; i++)
	{
		std::cout << "   ";
	}
	std::cout << val.val;
	
	if (children.size () > 0)
	{
		std::cout << ":";
	}
	std::cout << std::endl;
	
	for (AST t : children)
	{
		t.print (depth+1);
	}
}
