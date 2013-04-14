#include <approvals/Approvals.h>
#include <igloo/igloo.h>
#include <packet.h>
#include <sstream>

using namespace igloo;
using namespace std;

string FormatPacket( packet_t &p )
{
    stringstream s;
    s << p;
    return s.str();
}

Context( DescribeAPacket )
{
    Spec( UsesASaneCheckSum )
    {
        uint16_t b[1] = { 0x0101};
        uint16_t sum = checksum( ( unsigned char * ) b, 2 );
        uint16_t sum_msg = sum + b[0];
        Assert::That( sum_msg, Equals( 0xFFFF ) );
        b[0] = sum_msg;
        sum = checksum( ( unsigned char * ) b, 2 );
        Assert::That( sum, Equals( 0x0000 ) );
    }

    Spec( HasACalculatedCheckSum )
    {
        packet_t p;
        bzero( ( void * ) &p, sizeof( p ) );
        p.len = htons( 7 + 12 );
        p.ackno = htonl( 4092 );
        p.seqno = htonl( 8903 );

        char data[7] = "Hello!";
        memcpy( p.data, data, 7 );

        p.cksum = packet_checksum( p );
        Approvals::Verify( FormatPacket( p ) );
    }

    Spec( CheckSumRechecksToZero )
    {
        packet_t p;
        bzero( ( void * )&p, sizeof( p ) );
        p.len = htons( 7 + 12 );
        p.ackno = htonl( 4092 );
        p.seqno = htonl( 8903 );

        char data[7] = "Hello!";
        memcpy( p.data, data, 7 );

        p.cksum = packet_checksum( p );
        uint16_t check = packet_checksum( p );
        Assert::That( check, Equals( 0x0000 ) );
    }

    Spec( ItHasTheFollowingFields )
    {
        packet_t p;
        bzero( ( void * ) &p, sizeof( p ) );

        p.cksum = 2;
        p.len = htons( 6 );
        p.ackno = htonl( 4092 );
        p.seqno = htonl( 8903 );

        char data[6] = "Hello";
        memcpy( p.data, data, 6 );

        Approvals::Verify( FormatPacket( p ) );
    };

    Spec( APacketCanBeBuiltByBuildPacket )
    {
        packet_t p;
        bzero( ( void * ) &p, sizeof( p ) );
        build_packet( &p, 10, 100, "Hello World!", 13 );

        Approvals::Verify( FormatPacket( p ) );
    }

    Spec( APacketCanBeParsedByParsePacket )
    {
        packet_t p;
        bzero( ( void * )&p, sizeof( p ) );
        build_packet( &p, 10, 101, "Hello Michael", 14 );

        packet_t q;
        bzero( ( void * )&q, sizeof( q ) );
        fill_packet( ( char * )&p, &q );

        Approvals::Verify( FormatPacket( q ) );
    }
};

int main( int argc, char const *argv[] )
{
    DefaultTestResultsOutput output;
    TestRunner runner( output );

    IglooMetadataListener listener;
    runner.AddListener( &listener );
    MinimalProgressTestListener mpl;
    runner.AddListener( &mpl );

    return runner.Run();
}

