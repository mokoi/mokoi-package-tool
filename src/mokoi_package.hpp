#ifndef MOKOI_PACKAGE_HPP
#define MOKOI_PACKAGE_HPP
#if defined (__GNUWIN32__) || defined (__WINDOWS__)
	#define ELIX_DIR_SEPARATOR '\\'
	#define ELIX_DIR_SSEPARATOR "\\"
#else
	#define ELIX_DIR_SEPARATOR '/'
	#define ELIX_DIR_SSEPARATOR "/"
#endif

#include <string>
#include <map>
#include <vector>
#include "elix_file.hpp"

typedef struct {
	std::string name;
	std::string path;
	uint8_t path_type;
	uint32_t len, compress;
	int32_t offset;
	uint8_t * compress_data;
} StoredFileInfo;

struct classcomp {
  bool operator() (std::string a, std::string b) const
  {
	return a.find_last_of('/') < b.find_last_of('/');
  }
};

class file_wildcard
{
	public:
		file_wildcard(std::string _prefix, std::string _suffix, bool _ignore = false);
		uint8_t match(std::string name);
		std::string print()
		{
			return (prefix.length() ? prefix : "") + "*" + (suffix.length() ? suffix : "");

		}

	private:
		std::string prefix;
		std::string suffix;
		bool ignore;
};



class mokoi_package
{
public:
	mokoi_package();

	virtual std::string type() = 0;

	bool save( std::string filename );
	bool load( std::string filename );

	virtual bool scan_header( elix::File * file ) = 0;
	virtual bool dump_header( elix::File * file ) = 0;
	virtual void sort_files() = 0;

	bool scan( elix::File * file);
	bool dump( elix::File * file);

	void print();

	bool add( std::string sourcefile, std::string destname);
	bool remove( std::string destname );


	std::string getError()
	{
		return error_msg;
	}

	void setError( std::string message)
	{
		this->error_msg.assign(message);
	}

protected:
	bool valid;
	std::map< std::string, StoredFileInfo *> files;
	std::vector< file_wildcard > files_selection;

private:
	std::string error_msg;
	std::string local_filename;

	void add_detail_with_check( std::string sourcefile, std::string destname);
	void add_detail( std::string sourcefile, std::string destname);
};

#endif // MOKOI_PACKAGE_HPP
