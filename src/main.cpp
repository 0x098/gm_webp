#include "GarrysMod/Lua/Interface.h"
#include <GarrysMod/Lua/Types.h>
#include <Platform.hpp>
#include <GarrysMod/ModuleLoader.hpp>
#include <GarrysMod/FactoryLoader.hpp>
#include <detouring/detours.h>
#include <scanning/symbolfinder.hpp>
#include <Windows.h>
#include <webp/encode.h>
#include <libpng16/png.h>

#ifndef _WIN32
#define __cdecl
#define __thiscall
#define __fastcall
#endif

typedef struct {
  const uint8_t * data;
  size_t size;
  size_t offset;
} PNGMemoryReader;

typedef unsigned char uchar;

using namespace GarrysMod;

SymbolFinder s;

void png_memory_read( png_structp png_ptr, png_bytep outBytes, size_t byteCountToRead ) {
  PNGMemoryReader * reader = ( PNGMemoryReader * ) png_get_io_ptr( png_ptr );
  if( reader->offset + byteCountToRead > reader->size ) {
    png_error( png_ptr, "Read Error" );
  }
  memcpy( outBytes, reader->data + reader->offset, byteCountToRead );
  reader->offset += byteCountToRead;
}

// Decode PNG from memory to RGBA pixel data
int decode_png_to_rgba( const uint8_t * png_data, size_t png_size, uint8_t ** out_rgba, int * width, int * height ) {
  png_structp png = png_create_read_struct( PNG_LIBPNG_VER_STRING, NULL, NULL, NULL );
  if( !png ) return 1;

  png_infop info = png_create_info_struct( png );
  if( !info ) {
    png_destroy_read_struct( &png, NULL, NULL );
    return 2;
  }

  if( setjmp( png_jmpbuf( png ) ) ) {
    png_destroy_read_struct( &png, &info, NULL );
    return 3;
  }

  PNGMemoryReader reader = { png_data, png_size, 0 };
  png_set_read_fn( png, &reader, png_memory_read );
  png_read_info( png, info );

  *width = png_get_image_width( png, info );
  *height = png_get_image_height( png, info );
  png_byte color_type = png_get_color_type( png, info );
  png_byte bit_depth = png_get_bit_depth( png, info );

  // Convert palette, grayscale, etc. to RGBA
  if( bit_depth == 16 )
    png_set_strip_16( png );
  if( color_type == PNG_COLOR_TYPE_PALETTE )
    png_set_palette_to_rgb( png );
  if( png_get_valid( png, info, PNG_INFO_tRNS ) )
    png_set_tRNS_to_alpha( png );
  if( color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8 )
    png_set_expand_gray_1_2_4_to_8( png );
  if( color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA )
    png_set_gray_to_rgb( png );

  png_read_update_info( png, info );

  size_t rowbytes = png_get_rowbytes( png, info );
  uint8_t * image_data = ( uint8_t * ) malloc( rowbytes * ( *height ) );
  if( !image_data ) {
    png_destroy_read_struct( &png, &info, NULL );
    return 4;
  }
  png_bytep * row_pointers = ( png_bytep * ) malloc( sizeof( png_bytep ) * ( *height ) );
  for( int i = 0; i < *height; i++ ) {
    row_pointers[ i ] = image_data + i * rowbytes;
  }

  png_read_image( png, row_pointers );
  free( row_pointers );
  png_destroy_read_struct( &png, &info, NULL );

  *out_rgba = image_data;
  return 0;
}

// Encode RGBA data to WebP


LUA_FUNCTION( webp ) {
  unsigned int size;
  const char * raw = LUA->GetString( 1, &size );
  float quality = ( float ) LUA->GetNumber( 2 );

  uint8_t * rgba_data = NULL;
  int width, height;
  int res = decode_png_to_rgba( ( const uint8_t * ) raw, size, &rgba_data, &width, &height );
  if( res != 0 ) {
    Msg( "ERRCODE: %i", res );
    LUA->ThrowError( "Failed to decode PNG" );
    return 0;
  }

  uint8_t * output_buffer = NULL;
  size_t output_size = WebPEncodeRGBA( rgba_data, width, height, width * 4, quality, &output_buffer );
  if( output_size == 0 || !output_buffer ) {
    LUA->ThrowError( "Failed to encode" );
    return 0;
  }
  LUA->PushString( ( const char * ) output_buffer, output_size );
  WebPFree( output_buffer );
  return 1;
}

GMOD_MODULE_OPEN() {
  //LUA->PushCFunction( webp );
  LUA->PushSpecial( GarrysMod::Lua::SPECIAL_GLOB );
  LUA->PushCFunction( webp );
  LUA->SetField( -2, "WEBP" );
  LUA->Pop( LUA->Top() );
  return 1;
}