#include "mokoi_game.hpp"
#include <cstring>


/* Game File Header
uint8_t[8] magic = {137, 'M', 'o', 'k', 'o', 'i', '1', 10}; // - magic[6] stores the file version
int8_t[256] name;
uint32_t id;
uint32_t logo_length; //png logo length
uint8_t * logo; // png logo data
uint32_t crc;

*/

static uint8_t mokoi_game_magic[6] = {137, 'M', 'o', 'k', 'o', 'i'};


mokoi_game::mokoi_game(std::string filename, std::string icon_file_path)
{
	this->version = '1';

	this->files_selection.push_back( file_wildcard("./game.", ".mokoi") );
	this->files_selection.push_back( file_wildcard("./preload/", ".txt") );

	this->files_selection.push_back( file_wildcard("./scripts/", ".mps", true) );
	this->files_selection.push_back( file_wildcard("./scripts/", ".inc", true) );
	this->files_selection.push_back( file_wildcard("./scripts/", ".options", true) );

	this->files_selection.push_back( file_wildcard("./scripts/", ".amx") );
	this->files_selection.push_back( file_wildcard("./scripts/", ".amx64") );
	this->files_selection.push_back( file_wildcard("./scripts/", ".js") );
	this->files_selection.push_back( file_wildcard("./scripts/", ".lua") );
	this->files_selection.push_back( file_wildcard("./scripts/", ".sq") );
	this->files_selection.push_back( file_wildcard("./scripts/", ".managed") );

	this->files_selection.push_back( file_wildcard("./lang/", ".txt") );
	this->files_selection.push_back( file_wildcard("./dialog/", ".txt") );
	this->files_selection.push_back( file_wildcard("./dialog/", ".ogg") );
	this->files_selection.push_back( file_wildcard("./dialog/", ".opus") );

	this->files_selection.push_back( file_wildcard("./sections/", ".txt") );
	this->files_selection.push_back( file_wildcard("./maps/", ".xml") );
	this->files_selection.push_back( file_wildcard("./maps/", ".entities") );

	this->files_selection.push_back( file_wildcard("./sprites/", ".png.txt") );
	this->files_selection.push_back( file_wildcard("./sprites/", ".png.xml") );
	this->files_selection.push_back( file_wildcard("./sprites/", ".png") );
	this->files_selection.push_back( file_wildcard("./masks/", ".xpm") );

	this->files_selection.push_back( file_wildcard("./soundfx/", "") );
	this->files_selection.push_back( file_wildcard("./music/", "") );

	this->load(filename);
}

void mokoi_game::sort_files()
{

}


bool mokoi_game::scan_header( elix::File * file )
{
	std::cout << "scan_header" << std::endl;
	uint8_t buf[8];
	file->Read( &buf, 1, 8 );
	/* magic */
	if ( memcmp(buf, mokoi_game_magic, 6) == 0 && buf[6] == this->version && buf[7] == 10 )
	{
		
		this->title.resize ( 255, 0 );
		file->Read( this->title, 255 );
		this->id = file->Read_uint32( true );
		/* PNG Icon */
		this->png_length = file->Read_uint32( true );
		if  ( this->png_length )
		{
			this->png = new uint8_t[this->png_length];
			file->Read( this->png, 1, this->png_length );
		}
		/* Checksum */
		this->checksum = file->Read_uint32( true );
		valid = true;

		std::cout << "Title:" << this->title << std::endl;
		std::cout << "ID:" << this->id << std::endl;
	}
	else
	{
		std::cout << "Invalid Game File" << std::endl;
		valid = false;
	}
	return valid;
}

bool mokoi_game::dump_header( elix::File * file )
{
	std::cout << "dump_header" << std::endl;
	/* magic */
	file->Write( (data_pointer)mokoi_game_magic, 1, 6 );
	file->Write( this->version );
	file->Write( (uint8_t)'\n' );

	/* id + title */
	file->Write( this->id );
	this->title.resize(255, 0);
	file->WriteString( this->title );

	/* PNG Icon */
	file->Write( this->png_length );
	file->Write( this->png, 1, this->png_length );

	/* Checksum */
	file->Write( this->checksum );

	return true;
}
