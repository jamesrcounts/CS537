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
void SwapIntPointers(int * a, int * b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

Context(SwappingIntPointers)
{
    Spec(TwoIntValuesAreSwapped)
    {
        int a = 37;
        int b = 42;
        SwapIntPointers(&a, &b);
        Assert::That(a, Equals(42));
        Assert::That(b, Equals(37));
    }

};

/*
As a server,
I want to listen on a socket,
So that I can answer requests.
*/
Context(ListeningOnSocket)
{
    // cppcheck-suppress unusedFunction
    void SetUp()
    {
        port = 80;
        address = string("144.37.12.45");
        client_socket = -1;
        connection_result = -1;

        client_socket = socket(AF_INET, SOCK_STREAM, 0);

        bzero(&server_address, sizeof(server_address));
        server_address.sin_family = AF_INET;
        server_address.sin_addr.s_addr = inet_addr(address.c_str());
        server_address.sin_port = htons(port);

        connection_result = connect(
                                client_socket,
                                (struct sockaddr *) &server_address,
                                sizeof(server_address));

        request = "GET / HTTP/1.0\n\n";
        written_bytes = write(client_socket, (void *)request.c_str(), request.size());

        buffer = new char[256];
        read_bytes = read(client_socket, buffer, 3);
        response = string(buffer);
        delete buffer;

    }

    Spec(GivenASocket)
    {
        Assert::That(client_socket, IsGreaterThan(-1));
    }

    Spec(AndAnAddress)
    {
        Assert::That(server_address.sin_family, Equals(AF_INET));
        Assert::That(server_address.sin_addr.s_addr, Equals(inet_addr(address.c_str())));
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
    string address;
    string request;
    string response;
    int port;
    int client_socket;
    int connection_result;
    ssize_t read_bytes ;
    ssize_t written_bytes;
    struct sockaddr_in server_address;
};

int main()
{
    return TestRunner::RunAllTests();
}