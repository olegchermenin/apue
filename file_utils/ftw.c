#define _XOPEN_SOURCE 500

#include <dirent.h>
#include <errno.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define PERCENT_100 100.0

typedef struct files_count 
{
   int count;
   int reg_count;
   int dir_count;
   int link_count;
   int char_count;
   int blk_count;
   int fifo_count;
   int sock_count;
} files_count_t;

int file_tree_walk( char *file_path, files_count_t *files_count );
int counting_files( const char *file_path, int file_attr, mode_t file_mode, files_count_t *files_count );
void print_file_stat( const files_count_t *files_count );
int get_path_max();

int main( int argc, char *argv[] )
{
   char *file_path;
   files_count_t files_count;
   int ftw_ret;

   if( argc != 2)
   {
      printf( "About: ftw prints information about files in specified directory\n" );
      printf( "Usage: %s <dir>\n", argv[0] );

      return 0;
   }

   /* working with the memory */
   memset( &files_count, 0, sizeof( files_count_t ) );
   if( ( file_path = malloc( get_path_max() ) ) == NULL )
   {
      perror( "malloc file_path" );
      return -1;
   }
   memset( file_path, 0, get_path_max() );
   strcpy( file_path, argv[1] ); /* strncpy? */

   ftw_ret = file_tree_walk( file_path, &files_count );
   free( file_path );
   print_file_stat( &files_count );
   
   return 0;
}

#define FTW_DIR 0 /* directory */
#define FTW_FND 1 /* file not a directory */
#define FTW_FNS 2 /* file not stat */
#define FTW_DNR 3 /* directory not read */

int file_tree_walk( char *file_path, files_count_t *files_count )
{
   struct stat stat_buf;
   DIR *dir_ptr;
   struct dirent *dirent_buf;

   if( lstat( file_path, &stat_buf ) != 0 )
      return counting_files( file_path, FTW_FNS, 0, NULL );

   if( !S_ISDIR( stat_buf.st_mode ) )
      return counting_files( file_path, FTW_FND, stat_buf.st_mode, files_count );
   else
   {
      counting_files( file_path, FTW_DIR, 0, files_count );
      if( ( strcmp( file_path, "/" ) ) != 0 )
      {
         strcat( file_path, "/" );
         file_path[strlen( file_path )] = '\0';
      }
   }

   if( ( dir_ptr = opendir( file_path ) ) == NULL )
      return counting_files( file_path, FTW_DNR, 0, NULL );

   while( ( dirent_buf = readdir( dir_ptr ) ) != NULL )
   {
      /* formation of the full file name */
      int null_byte_pos = strlen( file_path );
      int rc;

      if( !strcmp( dirent_buf->d_name, "." ) ||
          !strcmp( dirent_buf->d_name, ".." ) )
      {
         continue;
      }

      strcat( file_path, dirent_buf->d_name );
      file_path[strlen( file_path )] = '\0';
      
      rc = file_tree_walk( file_path, files_count );

      /* rc value check */
      file_path[null_byte_pos] = '\0';
   }

   closedir( dir_ptr );
   return 0;
}

int counting_files( const char *file_path, int file_attr, mode_t file_mode, files_count_t *files_count )
{
   int rc = 0;
   char err_msg[1024] = { 0 };

   switch ( file_attr )
   {
      case FTW_FND:
         switch( file_mode & S_IFMT )
         {
            case S_IFREG:
               files_count->reg_count++;
               break;
            case S_IFLNK:
               files_count->link_count++;
               break;
            case S_IFCHR:
               files_count->char_count++;
               break;
            case S_IFBLK:
               files_count->blk_count++;
               break;
            case S_IFIFO:
               files_count->fifo_count++;
               break;
            case S_IFSOCK:
               files_count->sock_count++;
               break;

            case S_IFDIR:
               printf( "[DIR IS NOT DIR] %s: something was wrong -- directory has other files attribute\n", file_path );
               break;
            default:
               printf( "[UNKNOWN FILE TYPE] %s\n", file_path );
               break;
         }
         files_count->count++;
         break;

      case FTW_DIR:
         files_count->dir_count++;
         files_count->count++;
         break;

      case FTW_DNR:
         sprintf( err_msg, "[DIR NOT READ] %s", file_path );
         perror( err_msg );
         break;

      case FTW_FNS:
         sprintf( err_msg, "[NOT STAT] %s", file_path );
         perror( err_msg );
         break;
      
      default:
         printf( "[UNKNOWN FILE TYPE] %s\n", file_path );
         break;
   }

   return rc;
}

void print_file_stat( const files_count_t *files_count )
{
   float reg_per = (float) files_count->reg_count / files_count->count * PERCENT_100;
   float dir_per = (float) files_count->dir_count / files_count->count * PERCENT_100;
   float link_per = (float) files_count->link_count / files_count->count * PERCENT_100;
   float char_per = (float) files_count->char_count / files_count->count * PERCENT_100;
   float blk_per = (float) files_count->blk_count / files_count->count * PERCENT_100;
   float fifo_per = (float) files_count->fifo_count / files_count->count * PERCENT_100;
   float sock_per = (float) files_count->sock_count / files_count->count * PERCENT_100;

   printf( "|------------------ RESULTS ------------------|\n");
   printf( "| Regular count            = %7d (%5.2f%%) |\n", files_count->reg_count, reg_per );
   printf( "| Directories count        = %7d (%5.2f%%) |\n", files_count->dir_count, dir_per );
   printf( "| Link count               = %7d (%5.2f%%) |\n", files_count->link_count, link_per );
   printf( "| Character special count  = %7d (%5.2f%%) |\n", files_count->char_count, char_per);
   printf( "| Block special count      = %7d (%5.2f%%) |\n", files_count->blk_count, blk_per );
   printf( "| FIFO count               = %7d (%5.2f%%) |\n", files_count->fifo_count, fifo_per );
   printf( "| Socket count             = %7d (%5.2f%%) |\n", files_count->sock_count, sock_per );
}

#define PATH_MAX_GUESS 4096

inline int get_path_max()
{
#ifdef PATH_MAX
   return PATH_MAX;
#else
{
   long posix_version = sysconf( _SC_VERSION );
   long xsi_version = sysconf( _SC_XOPEN_VERSION );
   int path_max = pathconf( "/", _PC_PATH_MAX );

   if( path_max <= 0 )
      path_max = PATH_MAX_GUESS;
   if( ( posix_version < 200112L ) && ( xsi_version < 4 ) )
      path_max++;

   return path_max;
}
#endif
}