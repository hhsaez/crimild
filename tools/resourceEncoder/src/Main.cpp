#include <iostream>
#include <cstdio>
#include <string>

void encode( std::string inFilePath, std::string outFilePath )
{
	FILE *in = fopen( inFilePath.c_str(), "rb" );
	FILE *out = fopen( outFilePath.c_str(), "w" );

	const size_t BUFFER_SIZE = 8;
	unsigned char buffer[ BUFFER_SIZE ];
	size_t count;

	fprintf( out, "unsigned char RESOURCE_BYTES[] = {\n" );

	while ( !feof( in ) ) {
		count = fread( buffer, 1, BUFFER_SIZE, in );
		fprintf( out, "\t" );
		for ( int n = 0; n < count; n++ ) {
			fprintf( out, "0x%02X, ", buffer[ n ] );
		}
		fprintf( out, "\n" );
	}
	
	fprintf( out, "};\n\n" );

	fclose( in );
	fclose( out );
}

int main( int argc, char **argv )
{
	if ( argc < 2 ) {
		std::cerr << "Usage: resourceEncoder in [out]\n";
		return -1;
	}

	std::string inFilePath = argv[ 1 ];
	
	std::string outFilePath = "out.inc";
	if ( argc >= 3 ) {
		outFilePath = argv[ 2 ];
	}
	
	encode( inFilePath, outFilePath );
	return 0;
}

