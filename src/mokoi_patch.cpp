#include "mokoi_patch.hpp"
#include <cstring>

/* Patch File Header
uint8_t[8] magic = {139, 'M', 'o', 'k', 'o', 'i', '1', 10}; // - magic[6] stores the file version
uint32_t game_id;
uint32_t game_crc;
uint32_t crc;
*/


static uint8_t mokoi_patch_magic[6] = {139, 'M', 'o', 'k', 'o', 'i'};

mokoi_patch::mokoi_patch(std::string filename, uint32_t game_id)
{
	this->version = '1';
	this->load(filename);
}


void mokoi_patch::sort_files()
{

}

bool mokoi_patch::scan_header( elix::File * file )
{
	std::cout << "scan_header" << std::endl;
	uint8_t buf[8];
	file->Read( &buf, 1, 8 );

	/* magic */
	if ( memcmp(buf, mokoi_patch_magic, 6) == 0 && buf[6] == this->version && buf[7] == 10 )
	{

		this->game_id = file->Read_uint32( true );

		/* Checksum */
		this->game_checksum = file->Read_uint32( true );
		this->checksum = file->Read_uint32( true );

		valid = true;

		std::cout << "Game ID:" << this->game_id << std::endl;
		std::cout << "Game Checksum:" << this->game_checksum << std::endl;
		std::cout << "Checksum:" << this->checksum << std::endl;
	}
	else
	{
		std::cout << "Invalid Game File" << std::endl;
		valid = false;
	}
	return valid;
}

bool mokoi_patch::dump_header( elix::File * file )
{
	std::cout << "dump_header" << std::endl;
	/* magic */
	file->Write( (data_pointer)mokoi_patch_magic, 1, 6 );
	file->Write( this->version );
	file->Write( (uint8_t)'\n' );

	/* id */
	file->Write( this->game_id );

	/* Checksum */
	file->Write( this->game_checksum );
	file->Write( this->checksum );

	return true;
}
