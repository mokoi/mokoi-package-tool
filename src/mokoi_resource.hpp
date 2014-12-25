#ifndef MOKOI_RESOURCE_H
#define MOKOI_RESOURCE_H

#include "mokoi_package.hpp"

class mokoi_resource : public mokoi_package
{
public:
	mokoi_resource(std::string filename, std::string file_title, std::string file_author);

	bool scan_header( elix::File * file );
	bool dump_header( elix::File * file );

	void sort_files();


	std::string type() { return "mokoi_resource"; }


private:
	uint32_t id;
	uint32_t checksum;

	uint8_t version;
	uint8_t category;

	std::string title;
	std::string author;


};






#endif // MOKOI_RESOURCE_H
