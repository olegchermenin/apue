#define _XOPEN_SOURCE 500

#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

#define USERNAME_MAX 255
#define GROUPNAME_MAX 255

void print_file_info( char *path, struct stat *stat_ptr );
char *get_file_type( mode_t file_mode, char *filtype );
char *get_username( uid_t uid, char *username );
char *get_groupname( gid_t gid, char *groupname );

int main( int argc, char *argv[] )
{
   int i = 0;
   struct stat stat_buf;

   if( argc == 1 )
   {
      printf("finfo prints information about the specified files\n");
      printf( "Usage: %s ...\n", argv[0] );

      return 0;
   }

   for( int i = 1; i < argc; ++i)
   {
      if( lstat( argv[i], &stat_buf ) != 0 )
      {
         perror( argv[i] );
         continue;
      }

      print_file_info( argv[i], &stat_buf );
   }

   return 0;
}

void print_file_info( char *path, struct stat *stat_ptr )
{
   char *filetype;
   char username[USERNAME_MAX], groupname[GROUPNAME_MAX];

   if( S_ISLNK( stat_ptr->st_mode ) )
   {
      char orig_file[FILENAME_MAX];

      readlink( path, orig_file, FILENAME_MAX );
      orig_file[strlen( orig_file )] = '\0';

      printf( "%s -> %s\n", path, orig_file );
   }
   else
      printf( "%s\n", path);

   printf( "* Device: %ld\n", stat_ptr->st_dev );
   printf( "* Inode number: %ld\n", stat_ptr->st_ino );
   printf( "* Link count: %ld\n", stat_ptr->st_nlink );
   printf( "* File mode: %s\n", get_file_type( stat_ptr->st_mode, filetype ) );
   printf( "* User id: %d (%s)\n", stat_ptr->st_uid, get_username( stat_ptr->st_uid, username ) );
   printf( "* Group id: %d (%s)\n", stat_ptr->st_gid, get_groupname( stat_ptr->st_gid, groupname ) );

   /* print dev number if character or block special file */
   if( S_ISCHR( stat_ptr->st_mode ) || S_ISBLK( stat_ptr->st_mode ) ) 
      printf( "* Device number: %ld\n", stat_ptr->st_rdev );

   printf( "* File size: %ld\n", stat_ptr->st_size );
   printf( "* Block size: %ld\n", stat_ptr->st_blksize );
   printf( "* Number 512-byte blocks allocated: %ld\n", stat_ptr->st_blocks );
   printf( "* Last access: %s", ctime( &stat_ptr->st_atime ) );
   printf( "* Last modification: %s", ctime( &stat_ptr->st_mtime ) );
   printf( "* Last status change: %s", ctime( &stat_ptr->st_ctime ) );

   printf("\n");
}

char *get_file_type( mode_t file_mode, char *filetype )
{
   switch( file_mode & S_IFMT )
   {
      case S_IFREG:
         filetype = "regular";
         break;
      case S_IFDIR:
         filetype = "directory";
         break;
      case S_IFLNK:
         filetype = "symbolic link";
         break;
      case S_IFCHR:
         filetype = "character special";
         break;
      case S_IFBLK:
         filetype = "block special";
         break;
      case S_IFIFO:
         filetype = "FIFO";
         break;
      case S_IFSOCK:
         filetype = "socket";
         break;
      default:
         filetype = "unknown type";
         break;
   }

   return filetype;
}

char *get_username( uid_t uid, char *username )
{
   struct passwd *passwd_buf;

   if( ( passwd_buf = getpwuid( uid ) ) != NULL)
      strcpy( username, passwd_buf->pw_name );
   else
      username = "unknown";

   return username;
}

char *get_groupname( gid_t gid, char *groupname )
{
   struct group *group_buf;

   if( ( group_buf = getgrgid( gid ) ) != NULL)
      strcpy( groupname, group_buf->gr_name );
   else
      groupname = "unknown";

   return groupname;
}