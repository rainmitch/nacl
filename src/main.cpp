
#include <iostream>
#include <fstream>
#include <algorithm>

#include <nacl/parser.h>

std::string read (std::string file)
{
	std::ifstream t(file);
	std::string str;
	
	t.seekg(0, std::ios::end);
	str.reserve(t.tellg());
	t.seekg(0, std::ios::beg);
	
	str.assign((std::istreambuf_iterator<char>(t)),
				std::istreambuf_iterator<char>());
	
	return str;
}

bool exists (std::string file)
{
	FILE *f = fopen (file.c_str (), "r");
	
	if (f)
	{
		fclose (f);
		return true;
	}
	
	return false;
}

void write (std::string path, std::string data)
{
	std::ofstream file (path);
	file << data;
	file.close ();
}

std::string getFileName (std::string text)
{
	std::vector<std::string> parts;
	std::string tmp;
	
	for (int i = 0; i < text.size (); i++)
	{
		if (text[i] == '/')
		{
			parts.push_back (tmp);
			tmp = "";
		}
		else
		{
			tmp += text[i];
		}
	}
	
	if (tmp.size () > 0)
	{
		parts.push_back (tmp);
		tmp = "";
	}
	
	return parts[parts.size () - 1];
}

std::string removeExtension (std::string text)
{
	std::string out;
	
	for (char c : text)
		if (c == '.')
			return out;
		else
			out += c;
	
	return out;
}

void help ()
{
	std::cout << "NACL Parser generator. Usage:" << std::endl;
	std::cout << "   nacl [input file] [options] -o [outputfile]" << std::endl;
}

int main (int argc, char **args)
{
	if (argc == 1)
	{
		std::cerr << "I need inputs! Try '--help'" << std::endl;
		return 1;
	}
	
	std::string file;
	
	for (int i = 1; i < argc; i++)
	{
		std::string op = args[i];
		
		if (exists (op))
		{
			file = op;
		}
		else if (op == "--help")
		{
			help ();
			return 0;
		}
		else
		{
			std::cerr << "Unknown Input '" << op << "'" << std::endl;
			return 1;
		}
	}
	
	std::string name = removeExtension (getFileName (file));
	setNamespace (name);
	
	std::string text = read (file);
	
	std::vector<Rule> rules = lex (text);
	
	//std::cout << "Rules:" << std::endl;
	//
	//for (Rule r : rules)
	//{
	//	std::cout << r.name << ":" << std::endl;
	//	for (std::string str : r.parts)
	//	{
	//		std::cout << "   " << str << std::endl;
	//	}
	//}
	
	std::string headers = constructHeaders (rules);
	
	std::string bodies = constructBodies (rules);
	
	std::string parser = constructParser (rules);
	
	std::string tmp = name;
	std::transform (tmp.begin (), tmp.end (), tmp.begin (), toupper);
	
	std::string headerTotal = "\n#ifndef " + tmp + "_H\n#define " + tmp + "_H\n\n#include \"nacl.h\"\n\n" + headers + "\n" + constructParserHeader () + "\n#endif\n";
	
	std::string bodyTotal = "\n#include \"" + name + ".h\"\n\n" + bodies + "\n\n" + parser;
	
	std::string total = headerTotal + "\n\n" + bodyTotal;
	
	write (name + ".h", headerTotal);
	write (name + ".cpp", bodyTotal);
}
