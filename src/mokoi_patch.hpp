#ifndef MOKOI_PATCH_HPP
#define MOKOI_PATCH_HPP

#include "mokoi_package.hpp"

class mokoi_patch : public mokoi_package
{
public:
	mokoi_patch(std::string filename, uint32_t game_id);

	bool scan_header( elix::File * file );
	bool dump_header( elix::File * file );
	void sort_files();

	std::string type() { return "mokoi_patch"; }

private:
	uint32_t game_id;
	uint32_t game_checksum;
	uint32_t checksum;

	uint8_t version;


};

#endif // MOKOI_PATCH_HPP
