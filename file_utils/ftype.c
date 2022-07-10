#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

void print_file_type( char *path, mode_t file_mode );

int main( int argc, char *argv[] )
{
   int i = 0;
   struct stat stat_buf;

   if( argc == 1 )
   {
      printf("ft prints information about the type of the specified file\n");
      printf( "Usage: %s ...\n", argv[0] );
      return 0;
   }

   for( int i = 1; i < argc; ++i)
   {
      if( stat( argv[i], &stat_buf ) != 0 )
      {
         perror( argv[i] );
         continue;
      }

      print_file_type( argv[i], stat_buf.st_mode );
   }

   return 0;
}

void print_file_type( char *path, mode_t file_mode )
{
   char *type_str;

   switch( file_mode & S_IFMT )
   {
      case S_IFREG:
         type_str = "regular file";
         break;
      case S_IFDIR:
         type_str = "directory file";
         break;
      case S_IFLNK:
         type_str = "symbolic link file";
         break;
      case S_IFCHR:
         type_str = "character special file";
         break;
      case S_IFBLK:
         type_str = "block special file";
         break;
      case S_IFIFO:
         type_str = "FIFO file";
         break;
      case S_IFSOCK:
         type_str = "socket file";
         break;
      default:
         type_str = "unknown file type";
         break;
   }

   printf( "%s is a %s\n", path, type_str );
}