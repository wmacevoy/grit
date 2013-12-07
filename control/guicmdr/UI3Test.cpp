#include <string>
#include <stdlib.h>
#include <iostream>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <cassert>
#include <sys/wait.h>

using namespace std;

enum PIPE_FILE_DESCRIPTERS
{
  READ_FD  = 0,
  WRITE_FD = 1
};

enum CONSTANTS
{
  BUFFER_SIZE = 100
};

int main(int argc,char **argv)
{
  int       parentToChild[2];
  int       childToParent[2];
  pid_t     pid;
  string    dataReadFromChild;
  char      buffer[ BUFFER_SIZE + 1 ];
  size_t   readResult;
  int       status;
  int       count=0;

  assert(0==pipe(parentToChild));
  assert(0==pipe(childToParent));

  switch ( pid = fork() )
  {
    case -1:
      cerr << "Fork failed";
      exit(-1);

    case 0: /* Child */
      assert(-1!=dup2( parentToChild[ READ_FD  ], STDIN_FILENO  ) );
      assert(-1!=dup2( childToParent[ WRITE_FD ], STDOUT_FILENO ) );
      assert(-1!=dup2( childToParent[ WRITE_FD ], STDERR_FILENO ) );
      assert(0==close( parentToChild [ WRITE_FD ] ) );
      assert(0==close( childToParent [ READ_FD  ] ) );

          /*   file,  arg0,  arg1,   arg2 */
      execlp(  "./Surrogate", "./Surrogate", NULL );

      cerr << "This line should never be reached!!!";
      exit(-1);


    default: /* Parent */
	  string command("Test\n");
	  string exitCommand("exit\n");
      cout << "Child " << pid << " process running..." << endl;

      assert(0==close( parentToChild [ READ_FD  ] ) );
      assert(0==close( childToParent [ WRITE_FD ] ) );

      while ( true )
      {
		write(parentToChild[WRITE_FD],command.c_str(),command.size());
		count++;
		if (count==10) 
		  write(parentToChild[WRITE_FD],exitCommand.c_str(),exitCommand.size());
        switch ( readResult = read( childToParent[ READ_FD ],
                                    buffer, BUFFER_SIZE ) )
        {
          case 0: /* End-of-File, or non-blocking read. */
            cout << "End of file reached..."         << endl
                 << "Data received was ("
                 << dataReadFromChild.size() << "):" << endl
                 << dataReadFromChild                << endl;

            assert( pid==waitpid( pid, & status, 0 ) );

            cout << endl
                 << "Child exit staus is:  " << WEXITSTATUS(status) << endl
                 << endl;

            exit(0);


          case -1:
            if ( (errno == EINTR) || (errno == EAGAIN) )
            {
              errno = 0;
              break;
            }
            else
            {
              cerr <<  "read() failed";
              exit(-1);
            }

          default:
            dataReadFromChild . append( buffer, readResult );
            break;
        }
      } /* while ( true ) */
  } /* switch ( pid = fork() )*/
}
