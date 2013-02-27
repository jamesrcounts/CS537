/***************************************************
test.cpp

Jim Counts & Michael Ngyuen
CS 537 - Data Communications
Homework 1
**************************************************/

#include <igloo/igloo.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;
using namespace igloo;

/*
As a server
I want to listen on a socket
So that I can accept requests.
*/
Context(ServerListensToSocket)
{
    void Setup()
    {
        listen_id = -1;
        address = "127.0.0.1";
        port = 8888;
        listen_id = socket(AF_INET, SOCK_STREAM, 0);
        bzero(&server_address, sizeof(server_address));

        server_address.sin_family = AF_INET;
        server_address.sin_addr.s_addr   = inet_addr(address.c_str());
        server_address.sin_port          = htons(port);


    }
    Spec(GivenAFileDescriptor)
    {
        Assert::That(listen_id, !Equals(-1));
    }
    Spec(AndAnAddress)
    {
        Assert::That(server_address.sin_family, Equals(AF_INET));
        // Assert::That(server_address.sin_addr.s_addr, Equals(inet_addr(address.c_str())));
        //    Assert::That(server_address.sin_port, Equals(htons(port)));
    }
    Spec(AndANamedSocket)
    {
        Assert::That(bind_result, Equals(0));
    }
    Spec(WhenIListen) {}
    Spec(ThenIShouldNotGetAnErrorCode)
    {
        Assert::That(listen_result, Equals(0));
    }

    int bind_result;
    int listen_id;
    int listen_result;
    int port;

    string address;

    struct sockaddr_in server_address;
};

/*
void *clientHandler(void * arg)
{
    int server_socket = *(int*)(arg);
    cout << server_socket << endl;
    struct sockaddr_in client_address;

    while (1)
    {
        int connection = accept(
                             server_socket,
                             (struct sockaddr *)&client_address,
                             (socklen_t*) sizeof(client_address));
        if (connection < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }
            else
            {
                fprintf(
                    stderr,
                    "Error connection request refused, errno = %d (%s) \n",
                    errno,
                    strerror(errno));
                continue;
            }
        }

        int bytes_written = write(connection, "OK\n", 3);
        if(0 < bytes_written)
        {
            break;
        }
    }

    return 0;
}


As a server,
I want to listen on a socket,
So that I can answer requests.

Context(ListeningOnSocket)
{
	static string address;
	static int port;
    static struct sockaddr_in server_address;

    static void SetUpContext()
    {
        const int queue_size = 4;
        pthread_t tid;

        address = string("127.0.0.1");
port = 18888;

        server_address = SetupAddress(address, port);

        int server_socket = socket(AF_INET, SOCK_STREAM, 0);
        bind_result = bind(
                          server_socket,
                          (struct sockaddr *)&server_address,
                          sizeof(server_address));
        int listen_result = listen(server_socket, queue_size);
        int create_thread_result = pthread_create(
                                   &tid,
                                   NULL,
                                   clientHandler,
                                   (void *)&server_socket);


    }
    // cppcheck-suppress unusedFunction
    void SetUp()
    {

        port = 18888;
        address = string("127.0.0.1");

        bind_result = -1;
        client_socket = -1;
        connection_result = -1;
        create_thread_result = -1;
        listen_result = -1;
        server_socket = -1;

        SendRequest();
        GetResponse();
    }

    void TearDown()
    {
        close(client_socket);
        close(server_socket);
    }

    void GetResponse()
    {
        buffer = new char[256];
        read_bytes = read(client_socket, buffer, 3);
        fprintf(
            stderr,
            "Error unable to read socket, errno = %d (%s) \n",
            errno,
            strerror(errno));
        response = string(buffer);
        delete buffer;
    }

    void SetupAddress(string addr, int portNum)
    {
    	// server s_addr must be "any" see www.linuxhowtos.org/data/6/server.c
    	//http://tldp.org/LDP/LG/issue74/tougher.html
    	struct sockaddr_in inet_address;
        bzero(&inet_address, sizeof(inet_address));
        inet_address.sin_family = AF_INET;
        inet_address.sin_addr.s_addr = inet_addr(address.c_str());
        inet_address.sin_port = htons(port);
    }

    void SendRequest()
    {
        client_socket = socket(AF_INET, SOCK_STREAM, 0);

        connection_result = connect(
                                client_socket,
                                (struct sockaddr *) &server_address,
                                sizeof(server_address));

        request = "GET / HTTP/1.0\n\n";
        written_bytes = write(
                            client_socket,
                            (void *)request.c_str(),
                            request.size());
    }

    Spec(GivenAServerSocket)
    {
        Assert::That(server_socket, IsGreaterThan(-1));
        Assert::That(bind_result, Equals(0));
        Assert::That(listen_result, Equals(0));
    }

    Spec(AndAListeningThread)
    {
        Assert::That(create_thread_result, Equals(0));
    }

    Spec(GivenAClientSocket)
    {
        Assert::That(client_socket, IsGreaterThan(-1));
    }

    Spec(AndAnAddress)
    {
        Assert::That(server_address.sin_family, Equals(AF_INET));
        Assert::That(
            server_address.sin_addr.s_addr,
            Equals(inet_addr(address.c_str())));
        Assert::That(server_address.sin_port, Equals(htons(port)));
    }

    Spec(AndAConnection)
    {
        Assert::That(connection_result, Equals(0));
    }

    Spec(WhenISendARequest)
    {
        Assert::That((int)written_bytes, Equals((int)request.size()));
    }

    Spec(ThenIShouldGetAResponse)
    {
        Assert::That(read_bytes, !Equals(-1));
        Assert::That(response, Equals("OK\n"));
    }

    char * buffer;

    string request;
    string response;


    int bind_result;
    int client_socket;
    int connection_result;
    int create_thread_result;
    int listen_result;
    int server_socket;

    ssize_t read_bytes ;
    ssize_t written_bytes;
};
*/
int main()
{
    return TestRunner::RunAllTests();
}