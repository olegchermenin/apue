/* Напишите свою версию функции dup2, которая реализует функциональность,
   описанную в разделе 3.12, но без использования функции fcntl.
   Предусмотрите обработку ошибок. */

#include <errno.h>
#include <stdio.h>
#include <unistd.h>

void dup_recursive( int fd, int fd2 );

int mydup2( int fd, int fd2 )
{
   int i;

   if( fd == -1 )
   {
      errno = EBADF;
      return -1;
   }

   close( fd2 );

   if( fd == fd2 )
      return fd2;

   // dup_recursive( fd, fd2 );

   return fd2;
}

void dup_recursive( int fd, int fd2 )
{
   int tmp_fd;
   
   if( fd == fd2 )
      return;
   else
      tmp_fd = dup( fd );

   dup_recursive( tmp_fd, fd2 );

   if( tmp_fd != fd2 )
      close( tmp_fd );
}