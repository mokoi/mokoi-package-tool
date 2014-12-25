#ifndef MOKOI_GAME_H
#define MOKOI_GAME_H

#include "mokoi_package.hpp"

class mokoi_game : public mokoi_package
{
public:
	mokoi_game(std::string filename, std::string icon_file_path);


	bool scan_header( elix::File * file );
	bool dump_header( elix::File * file );
	void sort_files();

	std::string type() { return "mokoi_game"; }

private:
	uint32_t id;
	uint8_t * png;
	uint32_t png_length;
	uint32_t checksum;

	uint8_t version;

	std::string title;

};

#endif // MOKOI_GAME_H
