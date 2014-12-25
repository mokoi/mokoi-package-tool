#ifndef _ELIX_FILE_H_
#define _ELIX_FILE_H_

#include <cstdio>
#include <string>
#include <iostream>
#include "elix_intdef.hpp"

typedef FILE* file_pointer;
typedef void* data_pointer;

namespace elix {
	class File
	{
		public:
			File( std::string path, bool write = false);
			~File();
		private:
			file_pointer handle;
			bool writable;
			std::string error;

			std::string path;
			uint32_t _length;

			void SetError( std::string msg );

		public:
			std::string error_note;
			void (*ErrorCallback)(std::string);
			bool Exist();
			uint32_t Length();
			int32_t Seek( int32_t pos );
			int32_t Tell();
			bool EndOfFile();
			bool Read( data_pointer buffer, uint32_t size, uint32_t count );
			uint32_t Read2( data_pointer buffer, uint32_t size, uint32_t count );
			uint32_t ReadAll( data_pointer * buffer, bool addnull = false );
			uint32_t Read_uint32( bool sysvalue = false );
			uint16_t Read_uint16( bool sysvalue = false );
			uint8_t Read_uint8( );
			std::string ReadString();
			bool Read( std::string * str );
			bool Read( std::string & str, uint16_t max );
			bool ReadLine( std::string * str );
			bool Write( data_pointer buffer, uint32_t size, uint32_t count );
			bool Write( uint32_t value, bool sysvalue = false );
			bool Write( uint16_t value, bool sysvalue = false );
			bool Write( uint8_t value );
			bool Write( std::string value );
			bool WriteString( std::string value );
	};
}
#endif
