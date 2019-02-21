#include "FontAtlasGenerator.hpp"
#include "SDFGenerator.hpp"

#include <Crimild.hpp>

using namespace crimild;
using namespace crimild::tools::fontgen;

std::ostream &operator<<( std::ostream &out, Texture &texture )
{

	return out;
	
}

void buildHeaderFile( std::string fontName )
{
    std::ofstream out( fontName + ".hpp", std::ios::out );
    if ( !out.is_open() ) {
        CRIMILD_LOG_ERROR( "Cannot open file for writing: ", fontName, ".hpp" );
        return;
    }

    out << "#include <Rendering/Image.hpp>";
    out << "\n";
    out << "\nusing namespace crimild;";
    out << "\n";

	auto texture = crimild::alloc< Texture >( crimild::alloc< ImageTGA >( fontName + ".tga" ) );
    auto image = texture->getImage();
    out << "\nSharedPointer< Image > buildSystemFontTexture( void )";
    out << "\n{";
    out << "\n\tauto image = crimild::alloc< Image >( "
        << image->getWidth()
        << ", "
        << image->getHeight()
        << ", "
        << image->getBpp()
        << ", "
        << "nullptr"
        << ", "
        << ( image->getBpp() == 4 ? "Image::PixelFormat::RGBA" : "Image::PixelFormat::RGB" )
        << " );";
    out << "\n\tauto *data = image->getData();";
    crimild::Size count = image->getWidth() * image->getHeight() * image->getBpp();
    auto data = image->getData();
    for ( crimild::Size i = 0; i < count; ++i ) {
        if ( data[ i ] != 0 ) {
            out << "\n\tdata[ " << i << " ] = " << ( int ) data[ i ] << ";";
        }
    }
    out << "\n\treturn image;";
    out << "\n}\n";

	// build glyphs
	out << "\nstd::map< unsigned char, Font::Glyph > buildSystemFontGlyphs( void )";
	out << "\n{";
	out << "\n\tstd::map< unsigned char, Font::Glyph > glyphs;";

	std::ifstream glyphs( fontName + ".txt" );
	char buffer[ 1024 ];
	while ( !glyphs.eof() ) {
		glyphs.getline( buffer, 1024 );
		std::stringstream line;
		line << buffer;
		Font::Glyph glyph;
		int symbol;
		line >> symbol
			 >> glyph.width
			 >> glyph.height
			 >> glyph.bearingX
			 >> glyph.bearingY
			 >> glyph.advance
			 >> glyph.uOffset
			 >> glyph.vOffset
			 >> glyph.u
			 >> glyph.v;
		out << "\n\tglyphs[ " << symbol << " ] = { "
			<< symbol << ", "
			<< glyph.width << ", "
			<< glyph.height << ", "
			<< glyph.bearingX << ", "
			<< glyph.bearingY << ", "
			<< glyph.advance << ", "
			<< glyph.uOffset << ", "
			<< glyph.vOffset << ", "
			<< glyph.u << ", "
			<< glyph.v << " };";
	}
	
	out << "\n\treturn glyphs;";
	out << "\n}\n";
}

std::string extractFontName( std::string input )
{
	std::string filename = input.substr( input.find_last_of( "/" ) + 1 );
	std::string fontName = filename.substr( 0, filename.find_last_of( "." ) );
	return fontName;
}

int main( int argc, char **argv )
{
	Settings settings;
	settings.parseCommandLine( argc, argv );

	std::string fontFile = settings.get( "font_file", "" );
	if ( fontFile == "" ) {
		std::cout << "usage: " << argv[ 0 ] << " font_file=path/to/font.ttf [texture_size=number]" << std::endl;
		return -1;
	}

	std::cout << fontFile << std::endl;

	std::string fontName = extractFontName( fontFile );
	int size = settings.get( "texture_size", 1024 );

	FontAtlasGenerator fontAtlasGenerator( fontFile, size, size );
	if ( fontAtlasGenerator.execute( fontName ) ) {
#ifdef __APPLE__		
		system( ( std::string( "open \"" ) + fontName + ".tga\"" ).c_str() );
		system( ( std::string( "open \"" ) + fontName + ".txt\"" ).c_str() );
#endif
	}

    buildHeaderFile( fontName );

	SDFGenerator sdfGenerator;
	sdfGenerator.execute( fontName + ".tga" );

	return 0;
}

