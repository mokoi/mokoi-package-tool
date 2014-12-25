#include "mokoi_package.hpp"
#include <fstream>
#include <sys/stat.h>
#include <dirent.h>
#include <algorithm>

#define MINIZ_NO_ZLIB_COMPATIBLE_NAMES 1
#define MINIZ_NO_STDIO 1
#define MINIZ_NO_ARCHIVE_APIS 1
#define MINIZ_NO_ARCHIVE_WRITING_APIS 1
#include "miniz.c"


/*
 * Data
uint16_t filename_length;
int8_t * filename;
uint32_t file_size_length;
uint32_t compressed_size_length;
uint8_t * data;
 *
 *
 *
 */
bool sort_files (std::string a, std::string b)
{
	return a.find_last_of('/') < b.find_last_of('/');
}


int32_t file_exist(std::string src)
{
	std::ifstream f;
	f.open(src.c_str(), std::ios::binary | std::ios::in);
	if (!f.good() || f.eof() || !f.is_open()) { return 0; }
	return 1;
}

int32_t is_directory(std::string path)
{
	if ( path.at( path.length()-1 ) == ELIX_DIR_SEPARATOR )
	{
		path.erase(path.length()-1);
	}
	struct stat directory;
	if ( !stat( path.c_str(), &directory ) )
	{
		return S_ISDIR(directory.st_mode);
	}
	return false;
}

void string_replace( std::string & str, std::string find, std::string replace )
{
	std::string::size_type look = 0;
	std::string::size_type found;
	while((found = str.find(find, look)) != std::string::npos)
	{
		str.replace(found, find.size(), replace);
		look = found + replace.size();
	}
}

bool file_list( std::string path,  std::string wildcard, std::map<std::string, std::string> & list, std::string orig )
{
	if ( path.at( path.length()-1 ) == ELIX_DIR_SEPARATOR )
	{
		path.erase(path.length()-1);
	}

	if ( !orig.length() )
	{
		orig = path;
	}

	DIR * dir = opendir( path.c_str() );
	if ( !dir )
	{
		return false;
	}

	dirent * entry = NULL;
	while ( (entry = readdir(dir)) != NULL )
	{
		std::string full_path = path + ELIX_DIR_SSEPARATOR;
		full_path.append(entry->d_name);

		if ( entry->d_name[0] != '.' )
		{
			if ( is_directory( full_path ) )
			{
				file_list( full_path, wildcard, list, orig );
			}
			else
			{
				std::string local = "./";
				size_t found = full_path.find(orig);
				if ( found == 0 )
				{
					local.append( full_path.substr(orig.length()+1) );
				}
				string_replace(local, "\\", "/");

				list[full_path] = local;

			}
		}
	}
	closedir(dir);
	return true;
}

file_wildcard::file_wildcard( std::string _prefix, std::string _suffix, bool _ignore)
{
	prefix = _prefix;
	suffix = _suffix;
	ignore = _ignore;
}

uint8_t file_wildcard::match(std::string name)
{
	bool pre = true, suf = true;
	size_t lastslash = name.find_last_of('/');

	if ( !name.length() )
	{
		return false;
	}
	if ( this->prefix.length() )
	{
		if ( name.find(this->prefix) != 0)
		{
			pre = false;
		}
	}
	if ( this->suffix.length() )
	{
		size_t pos = name.length() - this->suffix.length();
		if ( name.find(this->suffix) != pos )
		{
			suf = false;
		}
	}
	return (suf && pre) + (this->ignore*(suf && pre));
}


mokoi_package::mokoi_package()
{
}

bool mokoi_package::save( std::string filename )
{
	std::cout << "----------------------------" << std::endl;
	std::cout << "Saving " << filename <<  " as " << this->type() << std::endl;

	elix::File * file = new elix::File(filename, true);
	if ( this->dump_header(file) )
	{
		this->dump(file);
	}
	delete file;

	std::cout << "----------------------------" << std::endl;
}

bool mokoi_package::load( std::string filename )
{
	std::cout << "----------------------------" << std::endl;
	std::cout << "Loading " << filename <<  " as " << this->type() << std::endl;

	elix::File * file = new elix::File(filename, false);
	if ( file->Exist() )
	{
		if ( this->scan_header(file) )
		{
			this->scan(file);
		}
	}
	delete file;
	std::cout << "----------------------------" << std::endl;
}

bool mokoi_package::scan( elix::File * file )
{
	std::cout << "Scanning Content." << std::endl;

	uint16_t name_length = 0;

	while ( !file->EndOfFile() )
	{
		name_length = file->Read_uint16( true );
		if ( name_length < 3 ) /* File Name should be longer in 3 char */
		{
			if ( name_length )
				std::cerr << "Invalid File Name in package." << std::endl;
			break;
		}

		StoredFileInfo * info = new StoredFileInfo;

		file->Read( info->name, name_length );

		info->len = file->Read_uint32( true );
		info->compress = file->Read_uint32( true );
		info->offset = file->Tell();
		info->path = "";
		info->path_type = 0;

		info->compress_data = new uint8_t[info->compress];

		file->error_note =  "Scan data " + info->name;
		file->Read( info->compress_data, 1, info->compress );

		this->files[info->name] = info;
		if ( file->EndOfFile() )
			break;

	}
	std::cout << "Files Found: " << this->files.size() << std::endl;
}

bool mokoi_package::dump( elix::File * file )
{
	std::cout << "Dumping Content." << std::endl;

	for( std::map<std::string, StoredFileInfo *>::iterator iter = this->files.begin(); iter != this->files.end(); ++iter )
	{
		StoredFileInfo * s = (*iter).second;
		if (s->compress_data != NULL)
		{
			//std::cout << "\t" << s->name << " Size:" << s->compress << std::endl;

			/* Filename */
			uint16_t filename_length = s->name.length();

			file->Write( filename_length );
			file->Write( (data_pointer)s->name.c_str(), 1, filename_length );

			/* Content */
			file->Write( s->len );
			file->Write( s->compress );
			file->Write( s->compress_data, 1, s->compress );
		}
		else
		{
			std::cout << "Skipped:" << s->name << " (compress_data)" << std::endl;
		}
	}
	std::cout << "Files Saved: " << this->files.size() << std::endl;
}


void mokoi_package::print()
{
	std::cout << "----------------------------" << std::endl;
	std::cout << "Files:" << this->files.size() << std::endl;
	for( std::map<std::string, StoredFileInfo *>::iterator iter = this->files.begin(); iter != this->files.end(); ++iter )
	{
		StoredFileInfo * s = (*iter).second;
		std::cout <<  "\t" << s->name << std::endl;
	}
	std::cout << "----------------------------" << std::endl;
}

void mokoi_package::add_detail( std::string sourcefile, std::string destname )
{
	StoredFileInfo * info = new StoredFileInfo;

	info->name = destname;
	info->len = 0;
	info->compress = 0;
	info->offset = 0;
	info->path = sourcefile;
	info->path_type = 0;
	info->compress_data = NULL;

	/* Read file */
	unsigned long length = 0;
	uint8_t * buffer = NULL;

	elix::File * content_file = new elix::File(sourcefile);
	info->len = content_file->ReadAll((data_pointer*)&buffer);
	delete content_file;

	length = (info->len + (info->len / 10) + 12);

	info->compress_data = new uint8_t[length];

	int error = mz_compress2(info->compress_data, &length, buffer, info->len, MZ_BEST_COMPRESSION);
	if ( error )
	{
		std::cout << "Compesssion Error: " << mz_error(error) << std::endl;
	}
	else
	{
		info->compress = length;
	}

	/* Add File */
	this->files[info->name] = info;
}

void mokoi_package::add_detail_with_check( std::string sourcefile, std::string destname)
{
	if ( this->files_selection.empty() )
	{
		std::cout << "Adding:" << destname << std::endl;
		this->add_detail(sourcefile, destname);
	}
	else
	{
		std::vector< file_wildcard >::iterator it;

		for ( it = files_selection.begin() ; it < files_selection.end(); it++ )
		{
			uint8_t m = (*it).match(destname);
			if ( m == 2 )
			{
				std::cout << "Ignoring:" << destname << std::endl;
				return;
			}
			else if ( m == 1)
			{
				std::cout << "Adding:" << destname << std::endl;
				this->add_detail(sourcefile, destname);
				return;
			}

		}
		std::cout << "Skipped:" << destname << std::endl;
	}
}

bool mokoi_package::add( std::string sourcefile, std::string destname)
{
	// Add All files
	if ( destname.compare("*") == 0)
	{
		std::cout << "Scanning Directory:" << sourcefile.substr(0, sourcefile.length()-2) << std::endl;
		std::map<std::string, std::string> list;
		file_list( sourcefile.substr(0, sourcefile.length()-2), "", list, "" );
		for( std::map<std::string, std::string>::iterator iter = list.begin(); iter != list.end(); ++iter )
		{
			add_detail_with_check((*iter).first, (*iter).second);
		}
	}
	else
	{
		std::cout << "Adding:" << destname << std::endl;
		add_detail(sourcefile, destname);
	}
}

bool mokoi_package::remove( std::string destname )
{
	this->files.erase(destname);
}
