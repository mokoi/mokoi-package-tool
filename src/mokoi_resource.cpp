#include "mokoi_resource.hpp"
#include <cstring>

/* Resource File Header
uint8_t[8] magic = {138, 'M', 'o', 'k', 'o', 'i', '1', 10}; // - magic[6] stores the file version
uint8_t[128] name;
uint8_t[128] author;
uint8_t category;
uint32_t crc;
*/

static uint8_t mokoi_resource_magic[6] = {138, 'M', 'o', 'k', 'o', 'i'};


mokoi_resource::mokoi_resource( std::string filename, std::string file_title, std::string file_author )
{
	this->version = '1';
	this->title = (file_title.length() ? file_title : "Resource File");
	this->author = file_author;



	this->files_selection.push_back( file_wildcard("./package/", "") );
	this->files_selection.push_back( file_wildcard("./map.options", "") );
	this->files_selection.push_back( file_wildcard("./preload/", ".txt") );

	this->files_selection.push_back( file_wildcard("./scripts/temp/", "", true) );
	this->files_selection.push_back( file_wildcard("./scripts/", ".mps") );
	this->files_selection.push_back( file_wildcard("./scripts/", ".js") );
	this->files_selection.push_back( file_wildcard("./scripts/", ".lua") );
	this->files_selection.push_back( file_wildcard("./scripts/", ".sq") );
	this->files_selection.push_back( file_wildcard("./scripts/", ".inc") );
	this->files_selection.push_back( file_wildcard("./scripts/", ".managed") );
	this->files_selection.push_back( file_wildcard("./scripts/", ".options") );

	this->files_selection.push_back( file_wildcard("./scripts/", ".amx") );
	this->files_selection.push_back( file_wildcard("./scripts/", ".amx64") );


	this->files_selection.push_back( file_wildcard("./lang/", ".txt") );
	this->files_selection.push_back( file_wildcard("./dialog/", ".txt") );
	this->files_selection.push_back( file_wildcard("./dialog/", ".ogg") );
	this->files_selection.push_back( file_wildcard("./dialog/", ".opus") );

	this->files_selection.push_back( file_wildcard("./sprites/old/", "", true) );
	this->files_selection.push_back( file_wildcard("./sprites/", ".png.txt") );
	this->files_selection.push_back( file_wildcard("./sprites/", ".png.xml") );
	this->files_selection.push_back( file_wildcard("./sprites/", ".png") );
	this->files_selection.push_back( file_wildcard("./masks/", ".xpm") );

	this->files_selection.push_back( file_wildcard("./sections/", ".txt") );
	this->files_selection.push_back( file_wildcard("./maps/", ".xml") );
	this->files_selection.push_back( file_wildcard("./maps/", ".entities") );
	this->files_selection.push_back( file_wildcard("./maps/", ".png") );

	this->files_selection.push_back( file_wildcard("./soundfx/", "") );
	this->files_selection.push_back( file_wildcard("./music/", "") );

	this->load(filename);
}

void mokoi_resource::sort_files()
{

}


bool mokoi_resource::scan_header( elix::File * file )
{
	uint8_t buf[8];
	file->Read( &buf, 1, 8 );
	/* magic */
	if ( memcmp(buf, mokoi_resource_magic, 6) == 0 && buf[6] == this->version && buf[7] == 10 )
	{
		/* title */
		this->title.resize ( 128, 0 );
		file->Read( this->title, 128 );

		/* author */
		this->author.resize ( 128, 0 );
		file->Read( this->author, 128 );

		/* category */
		this->category = file->Read_uint8();

		/* Checksum */
		this->checksum = file->Read_uint32( true );
		valid = true;

		std::cout << "Title:" << this->title << std::endl;
		std::cout << "Author:" << this->author << std::endl;
	}
	else
	{
		std::cout << "Invalid Game File" << std::endl;
		valid = false;
	}
	return valid;
}

bool mokoi_resource::dump_header( elix::File * file )
{
	/* magic */
	file->Write( (data_pointer)mokoi_resource_magic, 1, 6 );
	file->Write( this->version );
	file->Write( (uint8_t)'\n' );

	/* title + author */
	this->title.resize(128, 0);
	file->WriteString( this->title );

	this->author.resize(128, 0);
	file->WriteString( this->author );

	/* Category */
	file->Write( this->category );

	/* Checksum */
	file->Write( this->checksum );

	return true;
}



