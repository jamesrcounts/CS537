#include <iostream>
#include "Server.h"

using namespace std;

int main() {
	cout << "Running."<<endl;
    Server s( "127.0.0.1:8888", SOCK_NONBLOCK );

    string response( "HI" );

    while ( 1 )
    {
        int connfd = s.accept_connection();

        if ( 0 <= connfd )
        {
            // read request
            cout << "Reading... " << endl;
            string request = Socket::read_stream( connfd );
            cout << "Read" << endl;

            // write response
            if ( request == "HELLO" )
            {
                cout<<"Got HELLO"<<endl;
                Socket::write_stream( connfd, response );
                cout << "Responded" << endl;
            }
            close( connfd );

        }
    }
}