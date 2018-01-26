
#include <nacl/parser.h>

#include <algorithm>

#define FAILURE "<INTERNAL>FAILURE"

std::string Namespace = "PARSER";

std::string &Rule::operator[] (int place)
{
	return parts[place];
}

std::vector<std::string> split (std::string input, char c)
{
	std::vector<std::string> parts;
	std::string toks;
	
	for (int i = 0; i < input.size (); i++)
	{
		if (i == input.size () - 1)
		{
			parts.push_back (toks + std::string (1, input[i]));
			toks = "";
		}
		else if (input[i] == c)
		{
			if (toks.size () > 0)
				parts.push_back (toks);
			toks = "";
		}
		else
		{
			toks += input[i];
		}
	}
	
	return parts;
}

bool isLetter (char c)
{
	return
	(c >= 'a' and c <= 'z') or
	(c >= 'A' and c <= 'Z') or
	(c >= '0' and c <= '9');
}

std::vector<Rule> createRule (std::string input)
{
	std::vector<Rule> out;
	
	std::vector<std::string> parts;
	
	int i = 0;
	while (i < input.size ())
	{
		if (isLetter (input[i]))
		{
			std::string str;
			str += input[i];
			i++;
			
			while (i < input.size ())
			{
				if (!isLetter (input[i]))
				{
					break;
				}
				str += input[i++];
			}
			
			parts.push_back (str);
		}
		else if (input[i] == '\'')
		{
			std::string str = "\'";
			i++;
			while (i < input.size ())
			{
				if (input[i] == '\'')
				{
					str += "\'";
					i++;
					break;
				}
				else if (input[i] == '\\' and input[i+1] == '\'')
				{
					str += "\\\'";
					i += 2;
				}
				else
				{
					str += input[i++];
				}
			}
			
			parts.push_back (str);
		}
		else if (input[i] == '<')
		{
			std::string str = "<";
			i++;
			while (i < input.size ())
			{
				if (input[i] == '>')
				{
					str += ">";
					i++;
					break;
				}
				else
				{
					str += input[i++];
				}
			}
			
			parts.push_back (str);
		}
		else if (input[i] == '\"')
		{
			std::string str = "\"";
			i++;
			while (i < input.size ())
			{
				if (input[i] == '\"')
				{
					str += "\"";
					i++;
					break;
				}
				else if (input[i] == '\\' and input[i+1] == '\"')
				{
					str += "\\\"";
					i += 2;
				}
				else
				{
					str += input[i++];
				}
			}
			
			parts.push_back (str);
		}
		else if (input[i] == ' ' or input[i] == '\t' or input[i] == '\n')
		{
			i++;
		}
		else
		{
			parts.push_back (std::string (1, input[i]));
			i++;
		}
	}
	
	if (parts.size () < 2)
	{
		return out;
	}
	
	std::string name = parts[0];
	
	int offset = 2;
	if (parts[1] == "-")
	{
		offset += 1;
	}
	else if (parts[1] == ":" and parts[2] == ":")
	{
		offset += 2;
	}
	else if (parts[1] == ":")
	{
		offset += 1;
	}
	
	Rule r;
	
	r.name = name;
	
	for (int i = offset; i < parts.size (); i++)
	{
		if (parts[i][0] == '\'' or parts[i][0] == '\"')
		{
			r.parts.push_back (parts[i]);
		}
		else if (parts[i][0] == '<')
		{
			r.parts.push_back (parts[i]);
		}
		else if (parts[i] == "|")
		{
			out.push_back (r);
			r = Rule ();
			r.name = name;
		}
		else
		{
			std::cerr << "Unknown token '" << parts[i] << "'" << std::endl;
			std::cerr << "   " << input << std::endl;
			exit (1);
		}
	}
	
	if (r.parts.size () > 0)
	{
		out.push_back (r);
	}
	
	
	return out;
}

std::vector<Rule> lex (std::string text)
{
	std::vector<Rule> rules;
	
	std::vector<std::string> lines = split (text, ';');
	
	for (std::string line : lines)
	{
		std::vector<Rule> r = createRule (line);
		for (int i = 0; i < r.size (); i++)
		{
			if (r[i].name != FAILURE)
				rules.push_back (r[i]);
		}
	}
	
	return rules;
}

void setNamespace (std::string name)
{
	std::string tmp = name;
	std::transform (tmp.begin (), tmp.end (), tmp.begin (), toupper);
	
	Namespace = tmp;
}


std::string constructHeader (Rule rule)
{
	return "bool " + Namespace + "_" + rule.name + " (Lexer l, int place, int *out);\n"
		   "AST " + Namespace + "_" + rule.name + "_construct (Lexer l, int place, int *out);";
}

std::string constructHeaders (std::vector<Rule> rules)
{
	std::string out;
	std::vector<std::string> processed;
	
	for (Rule r : rules)
	{
		if (std::find (processed.begin (), processed.end (), r.name) == processed.end ())
		{
			out += constructHeader (r) + "\n";
			processed.push_back (r.name);
		}
	}
	
	return out;
}

std::string strip (std::string input)
{
	return input.substr (1, input.size () - 2);
}

std::string constructBody (std::vector<Rule> rules)
{
	std::string out = "bool " + Namespace + "_" + rules[0].name + " (Lexer l, int place, int *out)\n"
	"{\n"
	"\tint tmpPlace = 0;\n";
	
	for (Rule r : rules)
	{
		out += "\tif (";
		
		int num = 0;
		for (std::string part : r.parts)
		{
			if (part[0] == '\'')
			{
				out += "l[place+" + std::to_string (num) + "+tmpPlace] == \"" + strip (part) + "\"";
				num++;
			}
			else if (part[0] == '<')
			{
				out += Namespace + "_" + strip (part) + " (l, place+" + std::to_string (num) + ", &tmpPlace)";
			}
			
			out += " and ";
		}
		
		out = out.substr (0, out.size () - 5);
		
		out += ") {if (out != nullptr) *out = place+" + std::to_string (num) + "+tmpPlace; return true;}\n";
		out += "\ttmpPlace = 0;\n";
	}
	
	out += "\treturn false;\n";
	
	
	out += "}\n\n";
	
	
	std::string tmp = rules[0].name;
	std::transform (tmp.begin (), tmp.end (), tmp.begin (), toupper);
	
	out += "AST " + Namespace + "_" + rules[0].name + "_construct (Lexer l, int place, int *out)\n"
	"{\n"
	"\tAST outTree = AST (\"<" + tmp + ">\");\n"
	"\tint tmpPlace = 0;\n";
	
	for (Rule r : rules)
	{
		out += "\tif (";
		
		int num = 0;
		for (std::string part : r.parts)
		{
			if (part[0] == '\'')
			{
				out += "l[place+" + std::to_string (num) + "+tmpPlace] == \"" + strip (part) + "\"";
				num++;
			}
			else if (part[0] == '<')
			{
				out += Namespace + "_" + strip (part) + " (l, place+" + std::to_string (num) + "+tmpPlace, &tmpPlace)";
			}
			
			out += " and ";
		}
		
		out = out.substr (0, out.size () - 5);
		
		out += ")\n"
		"\t{\n";
		
		num = 0;
		out += "\t\ttmpPlace = 0;\n";
		for (std::string part : r.parts)
		{
			if (part[0] == '\'')
			{
				out += "\t\toutTree.add (l[place+" + std::to_string (num) + "+tmpPlace]);\n";
				num++;
			}
			else if (part[0] == '<')
			{
				out += "\t\toutTree.add (" + Namespace + "_" + strip (part) + "_construct (l, place+" + std::to_string (num) + "+tmpPlace, &tmpPlace));\n";
			}
		}
		
		
		out += "\t\tif (out != nullptr)\n\t\t\t*out = place+" + std::to_string (num) + "+tmpPlace;\n"
		"\t\treturn outTree;\n"
		"\t}\n"
		"\ttmpPlace = 0;\n";
	}
	
	out += "\treturn outTree;\n";
	
	
	out += "}\n";
	
	
	return out;
}

std::string constructBodies (std::vector<Rule> rules)
{
	std::string out;
	std::vector<std::string> processed;
	
	for (Rule r : rules)
	{
		if (std::find (processed.begin (), processed.end (), r.name) == processed.end ())
		{
			processed.push_back (r.name);
		}
	}
	
	for (std::string name : processed)
	{
		std::vector<Rule> tmp;
		for (Rule r : rules)
		{
			if (name == r.name)
			{
				tmp.push_back (r);
			}
		}
		
		out += constructBody (tmp) + "\n";
	}
	
	return out;
}

std::string constructParser (std::vector<Rule> rules)
{
	std::string out;
	
	out += "AST " + Namespace + "_parse (std::string input)\n"
	"{\n"
	"\tAST out = AST (\"ROOT\");\n"
	"\tLexer lex;\n"
	"\tlex.feed (input);\n"
	"\tint pos = 0;\n"
	"\tint tmp = 0;\n"
	"\tbool errorPass = false;\n"
	"\twhile (pos < lex.parts.size ())\n"
	"\t{\n";
	
	std::vector<std::string> processed;
	
	for (Rule r : rules)
	{
		if (std::find (processed.begin (), processed.end (), r.name) == processed.end ())
		{
			processed.push_back (r.name);
		}
	}
	
	for (std::string name : processed)
	{
		out += "\t\tif (" + Namespace + "_" + name + " (lex, pos, nullptr))\n"
		"\t\t{\n"
		"\t\t\tAST tree = " + Namespace + "_" + name + "_construct (lex, pos, &pos);\n"
		"\t\t\tout.add (tree);\n"
		"\t\t\terrorPass = false;\n"
		"\t\t\tcontinue;\n"
		"\t\t}\n";
	}
	
	out += "\t\tif (!errorPass)\n"
	"\t\t{\n"
	"\t\t\tError r;\n"
	"\t\t\tr.message = \"Parser:No matching rule found! Skipping to assure sanity!\";\n"
	"\t\t\tpos++;\n"
	"\t\t}\n";
	
	out += "\t}\n"
	"\treturn out;\n"
	"}\n";
	
	out +="std::vector<Error> " + Namespace + "_errors;\n"
	"bool " + Namespace + "_hasErrors ()\n"
	"{\n"
	"\treturn " + Namespace + "_errors.size () > 0;\n"
	"}\n";
	
	return out;
}

std::string constructParserHeader ()
{
	return "AST " + Namespace + "_parse (std::string input);\n";
}
