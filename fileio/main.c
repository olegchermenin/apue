#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int mydup2( int fd, int fd2 );

int main( int argc, char *argv[] )
{
   int fd, fd2, fd3, fd4;

   if( argc == 1)
   {
      printf( "Usage: %s <file>\n", argv[0] );
      return 0;
   }

   if( ( fd = open( argv[1], O_RDONLY ) ) < 0 )
   {
      perror( "open" );
      // return -1;
   }

   if( ( fd2 = dup( fd ) ) < 0 )
   {
      perror( "dup" );
   }

   if( ( fd3 = dup2( fd, 10 ) ) < 0 )
   {
      perror( "dup2" );
   }

   if( ( fd4 = mydup2( fd, 10 ) ) < 0 )
   {
      perror( "mydup2" );
   }
   
#ifdef 0
   char buf[1024];
   if ( read( fd3, buf, 1024 ) < 1024 )
      perror( "read" );
#endif

   printf( "desriptors: fd = %d, fd2 = %d, fd3 = %d, fd4 = %d\n", fd, fd2, fd3, fd4 );

   return 0;
}