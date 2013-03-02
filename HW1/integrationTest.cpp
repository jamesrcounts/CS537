#include <igloo/igloo.h>

#include "Client.h"
#include "Server.h"

using namespace std;
using namespace igloo;

void *run_server( void *state )
{
    Server s( "127.0.0.1:8888" );

    string response( "HI" );

    while ( 1 )
    {
        cout << "Wating for client ... " << endl;
        int connfd = s.accept_connection();
        cout << "Connection id: " << connfd << ":" << endl;

        if ( 0 <= connfd )
        {
            // read request
            cout << "Reading... " << endl;
            string request = Socket::read_stream( connfd );
            cout << "Read" << endl;

            // write response
            if ( request == "HELLO" )
            {
                Socket::write_stream( connfd, response );
            }
        }

        close( connfd );
    }

    return NULL;
}

int main()
{
    return TestRunner::RunAllTests();
}


//      if ( connfd < 0 )
//     {
//         if ( errno == EINTR )
//         {
//             continue;
//         }
//         else
//         {
//             fprintf( stderr,
//                      "Error connection request refused, errno = %d (%s) \n",
//                      errno,
//                      strerror( errno ) );
//         }
//     }

//     int create_result = pthread_create( &tid,
//                                         NULL,
//                                         clientHandler,
//                                         ( void * )&connfd );

//     if ( create_result != 0 )
//     {
//         fprintf( stderr,
//                  "Error unable to create thread, errno = %d (%s) \n",
//                  errno,
//                  strerror( errno ) );
//     }


Context( GetResponseFromServer )
{
    Spec( ServerShouldBeRunning )
    {
        // int thread_result = -1;
        // pthread_t thread;

        // thread_result = pthread_create( &thread, NULL, &run_server, NULL );
        // AssertThat( thread_result, Equals( 0 ) );

        sleep( 1 );
        cout << "Creating client... " << endl;
        Client c( "127.0.0.1:8888" );
        cout << "Connecting" << endl;
        c.socket_connect();
        cout << "Connected" << endl;
        sleep( 1 );
        cout << "Sending" << endl;
        c.send( "HELLO" );
        cout << "Sent" << endl;
        c.receive();
        AssertThat( c.last_received(), IsGreaterThan( 0 ) );
        AssertThat( c.last_response(), Equals( "HI" ) );
        // pthread_cancel( thread );
    }
    // Given A Running Server
    // And A Client
    // When I say hello
    // Then the server should say hi
};