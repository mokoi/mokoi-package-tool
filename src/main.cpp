/***********************************************
 * Copyright © Luke Salisbury
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 * Changes:
 *	 2012/09/12 [luke]: new file.
 ***********************************************/
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <cstdint>
#include <cstdlib>

#include "mokoi_game.hpp"
#include "mokoi_patch.hpp"
#include "mokoi_resource.hpp"

#include "optionparser.hpp"

std::string file_open( char * filter, char * title )
{
/*
	std::string path;
	char szFileName[MAX_PATH] = "";

	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFilter = filter;
	ofn.lpstrTitle = title;
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

	if ( GetOpenFileName(&ofn) )
	{
		path = szFileName;
	}
	return path;
	*/
}

std::string file_get_name( std::string path )
{
	int lastslash = path.find_last_of( "\\", path.length() );
	if ( lastslash )
	{
		return path.substr( lastslash + 1 );
	}
	return path;
}


std::string text_input( char * caption )
{
	std::string path = "";
	std::cout << "Enter path:" << std::endl;
	std::cin >> path;

	return path;
}

struct Arg: public option::Arg
{
	static void printError(const char* msg1, const option::Option& opt, const char* msg2)
	{
		fprintf(stderr, "%s", msg1);
		fwrite(opt.name, opt.namelen, 1, stderr);
		fprintf(stderr, "%s", msg2);
	}

	static option::ArgStatus Required(const option::Option& option, bool msg)
	{
		if (option.arg != 0)
			return option::ARG_OK;

		if (msg) printError("Option '", option, "' requires an argument\n");
		return option::ARG_ILLEGAL;
	}
	static option::ArgStatus Numeric(const option::Option& option, bool msg)
	{
		char* endptr = 0;
		if (option.arg != 0 && strtol(option.arg, &endptr, 10)){};
		if (endptr != option.arg && *endptr == 0)
			return option::ARG_OK;

		if (msg) printError("Option '", option, "' requires a numeric argument\n");
			return option::ARG_ILLEGAL;
	}
};


enum  optionIndex { UNKNOWN, RESOURCE, GAME, PATCH, CONVERT, FILEOPT, LISTOPT, MISCOPT, RESRCOPT, PATCHOPT, GAMEOPT};
const option::Descriptor usage[] =
{
	{UNKNOWN,	0,	"",		"",			option::Arg::None,	"Mokoi Package Tool. Create and modify package formats used with Mokoi Gaming\n" "USAGE: mokoi-package-tool [options]\n\n" "Options:" },
	{RESOURCE,	0,	"r",	"resource",	Arg::Required,	"  --resource, -r\t Select Resource File." },
	{GAME,		0,	"g",	"game",		Arg::Required,	"  --game, -g\t Select Game File." },
	{PATCH,		0,	"p",	"patch",	Arg::Required,	"  --patch, -p\t Select Patch File." },
	{CONVERT,	0,	"z",	"zip",	Arg::Required,	"  --zip, -z\t Select Patch File." },
	{UNKNOWN,	0,	"",		"",			option::Arg::None, 0},
	{UNKNOWN,	0,	"",		"",			option::Arg::None, "\n"},
	{FILEOPT,	0,	"d",	"dir",		Arg::Required,	"  --dir -d\tSwitch Directory. Use to trim filenames." },
	{FILEOPT,	0,	"a",	"add",		Arg::Required,	"  --add, -a\t Add file to package." },
	{FILEOPT,	0,	"",		"remove",	Arg::Required,	"  --remove\t Remove file to package." },
	{LISTOPT,	0,	"l",	"",			option::Arg::None,	"  -l\t List package content." },
	{UNKNOWN,	0,	"",		"",			option::Arg::None, "\n"},
	{MISCOPT,	0,	"",		"name",		Arg::Required,	"  --name\t Set Package Name." },
	{MISCOPT,	0,	"",		"id",		Arg::Numeric,	"  --id\t Set Package ID number." },
	{RESRCOPT,	0,	"",		"author",	Arg::Required,	"  --author\t Set Resource Author." },
	{PATCHOPT,	0,	"",		"target-id",	Arg::Required,	"  --target-id\t Set Target Game id for the Patch." },
	{GAMEOPT,	0,	"",		"icon",		Arg::Required,	"  --icon\t Set Game Icon." },
	{0,0,0,0,0,0}
};


int main(int argc, char *argv[])
{
	std::string option_name;
	std::string filename = "test.output";
	mokoi_package * package = NULL;

	argc-=(argc>0); argv+=(argc>0); // skip program name argv[0] if present
	option::Stats  stats(usage, argc, argv);
	option::Option options[stats.options_max], buffer[stats.buffer_max];
	option::Parser parse(usage, argc, argv, options, buffer);

	if ( parse.error() )
		return 1;


	/* Get Misc options */
	std::string package_name = "";
	uint32_t package_id = 0;

	for (option::Option * opt = options[MISCOPT]; opt; opt = opt->next())
	{
		option_name = opt->name;
		if ( option_name == "--name" )
		{
			package_name.assign(opt->arg);
		}
		else if ( option_name == "--id" )
		{
			package_id = strtol(opt->arg, NULL, 10);
		}
	}

	if ( options[RESOURCE] )
	{
		std::string author_name = "Unknown";
		std::cout << "RESOURCE: " << options[RESOURCE].last()->arg << std::endl;

		if ( options[RESRCOPT] )
		{
			for (option::Option * opt = options[RESRCOPT]; opt; opt = opt->next())
			{
				option_name = opt->name;
				if ( option_name == "--author" )
				{
					author_name.assign( opt->arg );
				}
			}
		}

		filename.assign( options[RESOURCE].last()->arg );
		package = new mokoi_resource(filename, package_name, author_name);
	}
	else if ( options[GAME] )
	{
		std::string icon_file_path = "";
		std::cout << "GAME: " << options[GAME].last()->arg << std::endl;

		if ( options[GAMEOPT] )
		{
			for (option::Option * opt = options[GAMEOPT]; opt; opt = opt->next())
			{
				option_name = opt->name;
				if ( option_name == "--icon" )
				{
					icon_file_path.assign( opt->arg );
				}
			}
		}

		filename.assign( options[GAME].last()->arg );
		package = new mokoi_game(filename, icon_file_path);

	}
	else if ( options[PATCH] )
	{
		uint32_t game_id = 0;
		std::cout << "PATCH: " << options[PATCH].last()->arg << std::endl;

		if ( options[PATCHOPT] )
		{
			for (option::Option * opt = options[PATCHOPT]; opt; opt = opt->next())
			{
				option_name = opt->name;
				if ( option_name == "--target-id" )
				{
					game_id = strtol(opt->arg, NULL, 10);
				}
			}
		}

		filename.assign( options[PATCH].last()->arg );
		package = new mokoi_patch(filename, game_id);

	}
	else if ( options[CONVERT] )
	{


	}
	else
	{
		option::printUsage(std::cout, usage);
		return 0;
	}


	if ( options[LISTOPT] && package != NULL )
	{
		package->print();
		return 0;
	}

	if ( package != NULL )
	{
		std::string directory;
		/* */
		for (option::Option * opt = options[FILEOPT]; opt; opt = opt->next())
		{
			std::string name = opt->name;
			if ( name == "--dir" || name == "d" )
			{
				directory.assign(opt->arg);
				std::cout << "Switch Directory to " << directory << std::endl;
			}
			else if ( name == "--add" || name == "a" )
			{
				std::string src;
				std::string dest;

				src.assign(opt->arg);
				dest.assign(opt->arg);

				/* Trim values */
				if ( directory.length() )
				{
					/* Strip Directory from dest */
					size_t found = dest.find(directory);
					if ( found == 0 )
					{
						dest = dest.substr(directory.length());
					}
					else
					{
						src.insert(0, ELIX_DIR_SSEPARATOR);
						src.insert(0, directory);
					}
				}
				package->add(src, dest);

				std::cout << "Adding File " << src << " as " << dest << std::endl;
			}
			else if ( name == "--remove" || name == "r" )
			{
				std::string dest(opt->arg);

				package->remove(dest);
				std::cout << "Remove File " << dest << std::endl;
			}
		}

		package->save(filename);
	}
	return 0;
}
