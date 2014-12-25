/****************************
Copyright © 2006-2012 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#include <cstdio>
#include <cstring>
#include "elix_file.hpp"
#include "elix_endian.hpp"

namespace elix {
	File::File(std::string filename, bool write)
	{
		this->path = filename;
		this->handle = fopen(this->path.c_str(), (write ? "wb" : "rb") );
		this->_length = 0;
		this->ErrorCallback = NULL;
		if ( !this->handle )
		{
			this->error = "Can't open '" + this->path + "'";
			this->writable = false;
		}
		else
		{
			this->writable = write;
			if ( !this->writable )
			{
				fseek( (FILE *)this->handle, 0, SEEK_END );
				this->_length = ftell( (FILE *)this->handle );
				fseek( (FILE *)this->handle, 0, SEEK_SET );
			}
		}
	}

	File::~File()
	{
		if ( this->handle )
		{
			fclose( (FILE*)this->handle );
		}
	}

	bool File::Exist()
	{
		return (this->handle ? true : false);
	}

	uint32_t File::Length()
	{
		int32_t len = 0;
		int32_t pos = ftell( (FILE *)this->handle );
		fseek( (FILE *)this->handle, 0, SEEK_END );
		len = ftell( (FILE *)this->handle );
		fseek( (FILE *)this->handle, pos, SEEK_SET );
		return (uint32_t) len;
	}

	int32_t File::Seek( int32_t pos )
	{
		return fseek( (FILE *)this->handle, (int32_t)pos, 0 );
	}

	int32_t File::Tell()
	{
		return ftell( (FILE *)this->handle );
	}

	bool File::EndOfFile()
	{
		if ( this->handle )
		{
			if ( feof( (FILE *)this->handle ) == 0 )
			{
				return false;
			}
		}
		return true;
	}

	bool File::Read( data_pointer buffer, uint32_t size, uint32_t count )
	{
		if ( !Exist() )
			return false;
		if ( !buffer )
		{
			if ( this->ErrorCallback )
				this->ErrorCallback( " No Buffer. " );
			std::cerr << __FUNCTION__ << ": No Buffer." << std::endl;
			return false;
		}

		if ( (count * size) > this->_length )
		{
			count = this->_length / size;
		}

		uint32_t read = fread(buffer, size, count, (FILE *)this->handle);

		if ( read == count )
		{
			return true;
		}
		else
		{
			if ( this->ErrorCallback )
				this->ErrorCallback( "Read failed." );
			std::cerr << __FUNCTION__ << ": Read only  " << read << " byte(s), instead of " << (size*count) << " byte(s) from " << this->path << ". " << this->error_note << std::endl;
		}
		return false;
	}

	uint32_t File::Read2( data_pointer buffer, uint32_t size, uint32_t count )
	{
		if ( !this->handle || this->EndOfFile() || !this->_length )
		{
			return 0;
		}
		if ( !buffer )
		{
			buffer = new char[size*count];
		}
		uint32_t read = fread(buffer, size, count, (FILE *)this->handle);
		return read;
	}

	uint32_t File::ReadAll( data_pointer * buffer, bool addnull )
	{
		if ( !this->handle || this->EndOfFile() || !this->_length )
		{
			return false;
		}

		if ( addnull )
		{
			*buffer = new char[this->_length+1];
			memset(*buffer, 0,this->_length+1);
		}
		else
		{
			*buffer = new char[this->_length];
			memset(*buffer, 0,this->_length);
		}
		if ( *buffer != NULL )
		{

			if ( fread(*buffer, this->_length, 1, (FILE *)this->handle) )
			{
				//if ( addnull )
				//	buffer[this->_length] = 0;
				return this->_length;
			}
		}
		return 0;
	}

	uint32_t File::Read_uint32( bool sysvalue )
	{
		if ( !this->handle || this->EndOfFile() )
		{
			if ( this->ErrorCallback )
				this->ErrorCallback( "EndOfFile." );
			return 0;
		}
		uint32_t value = 0;
		uint32_t read = fread(&value, sizeof(uint32_t), 1, (FILE *)this->handle);
		if ( !read )
		{
			if ( this->ErrorCallback )
				this->ErrorCallback( "Read failed." );
			std::cerr << __FUNCTION__ << ": Reading only '" << read << "' from '" << this->path << "'. " << std::endl;
			return 0;
		}
		if ( sysvalue )
		{
			return elix::endian::host32( value );
		}
		return value;
	}

	uint16_t File::Read_uint16( bool sysvalue )
	{
		if ( !this->handle || this->EndOfFile() )
		{
			if ( this->ErrorCallback )
				this->ErrorCallback( "EndOfFile." );
			return 0;
		}
		uint16_t value = 0;
		if ( !fread(&value, sizeof(uint16_t), 1, (FILE *)this->handle) )
		{
			if ( this->ErrorCallback )
				this->ErrorCallback( "Read failed." );
			std::cerr << __FUNCTION__ << ": Error reading from '" << this->path << "'. " << this->error_note << std::endl;
			return 0;
		}
		if ( sysvalue )
		{
			return elix::endian::host16( value );
		}
		return value;
	}

	uint8_t File::Read_uint8( )
	{
		if ( !this->handle || this->EndOfFile() )
		{
			if ( this->ErrorCallback )
				this->ErrorCallback( "EndOfFile." );
			return 0;
		}

		uint8_t value = 0;

		if ( !fread( &value, sizeof(uint8_t), 1, (FILE *)this->handle ) )
		{
			if ( this->ErrorCallback )
				this->ErrorCallback( "Read failed." );
			if ( !this->EndOfFile() )
				std::cerr << __FUNCTION__ << ": Error reading from '" << this->path << "'. " << std::endl;
			return 0;
		}
		if ( this->EndOfFile() )
			return 0;
		else
			return value;
	}

	std::string File::ReadString()
	{
		std::string str;
		this->Read( &str );
		return str;
	}

	bool File::Read( std::string * str )
	{
		str->clear();
		uint8_t value = 255;
		while ( value )
		{
			value = this->Read_uint8( );
			if ( value && !this->EndOfFile() )
			{
				str->append( 1, value );
			}
		}
		return true;
	}

	bool File::Read( std::string & str, uint16_t max )
	{
		str.clear();
		uint8_t value = 255;
		uint16_t pos = 0;
		while ( pos < max )
		{
			value = this->Read_uint8( );
			if ( value && !this->EndOfFile() )
			{
				str.append( 1, value );
			}
			pos++;
		}
		return true;
	}

	bool File::ReadLine( std::string * str )
	{
		if (!this->handle || this->EndOfFile() )
		{
			return false;
		}
		uint8_t value = 0;
		uint32_t pos = this->Tell();

		while ( this->_length > pos )
		{
			fread( &value, sizeof(uint8_t), 1, (FILE *)this->handle );
			if ( this->EndOfFile() || value == 0 )
				return false;
			else if ( value == 10 )
				return true;
			else
				str->append( 1, value );
			pos++;
		}
		return (bool)str->length();
	}

	bool File::Write( data_pointer buffer, uint32_t size, uint32_t count )
	{
		if ( !Exist() )
			return false;
		return (fwrite( buffer, size, count, (FILE *)this->handle ) ? true : false);
	}

	bool File::Write( uint32_t value, bool sysvalue )
	{
		if ( !Exist() )
			return false;
		if ( !sysvalue )
		{
			uint32_t uvalue = elix::endian::big32( value );
			return ( fwrite( &uvalue, sizeof(uint32_t), 1, (FILE *)this->handle ) ? true : false );
		}
		else
		{
			return ( fwrite( &value, sizeof(uint32_t), 1, (FILE *)this->handle ) ? true : false );
		}
	}

	bool File::Write( uint16_t value, bool sysvalue )
	{
		if ( !Exist() )
			return false;
		if ( !sysvalue )
		{
			uint16_t uvalue = elix::endian::big16( value );
			return ( fwrite( &uvalue, sizeof(uint16_t), 1, (FILE *)this->handle ) ? true : false );
		}
		else
		{
			return ( fwrite( &value, sizeof(uint16_t), 1, (FILE *)this->handle ) ? true : false );
		}
	}

	bool File::Write( uint8_t value )
	{
		if ( Exist() )
			return ( fwrite( &value, sizeof(uint8_t), 1, (FILE *)this->handle ) ? true : false );
		return false;
	}

	bool File::Write( std::string value )
	{
		if ( !Exist() )
			return false;
		bool result = false;
		if ( value.length() )
		{
			result = ( fwrite( value.c_str(), sizeof(uint8_t), value.length(), (FILE *)this->handle ) ? true : false );
		}
		result = this->Write( (uint8_t)'\0' );
		return result;
	}

	bool File::WriteString( std::string value )
	{
		if ( !Exist() )
			return false;
		bool result = false;
		if ( this->handle )
		{
			result = ( fwrite( value.c_str(), sizeof(uint8_t), value.length(), (FILE *)this->handle ) ? true : false );
		}
		return result;
	}
}
