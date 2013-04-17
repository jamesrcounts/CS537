#include <arpa/inet.h>
#include <igloo/igloo.h>
#include <approvals/Approvals.h>
#include <stdint.h>
#include <fcntl.h>

using namespace igloo;

enum rdt_state { WAITCALL0, WAITACK0, WAITCALL1, WAITACK1, };
struct packet
{
    uint16_t cksum; /* Ack and Data */
    uint16_t len;   /* Ack and Data */
    uint32_t ackno; /* Ack and Data */
    uint32_t seqno; /* Data only */
    char data[500]; /* Data only; Not always 500 unsigned chars, can be less */
};

typedef struct packet packet_t;

std::ostream &operator << ( std::ostream &out, packet_t &a )
{
    out << "cksum: " << a.cksum << std::endl;
    out << "len: " << ntohs( a.len ) << std::endl;
    out << "ackno: " << ntohl( a.ackno ) << std::endl;
    out << "seqno: " << ntohl( a.seqno ) << std::endl;
    out << "data: " << a.data << std::endl;
    return out;
}

uint16_t checksum( unsigned char *addr, uint32_t count )
{
    register uint32_t sum = 0;

    while ( count > 1 )
    {
        sum = sum + *( ( uint16_t * ) addr );
        addr += 2;
        count = count - 2;
    }

    if ( count > 0 )
    {
        sum = sum + *( ( unsigned char * ) addr );
    }

    while ( sum >> 16 )
    {
        sum = ( sum & 0xFFFF ) + ( sum >> 16 );
    }

    return( ~sum );
}

void rdt_sender_reset( rdt_state &s )
{
    s = WAITCALL0;
}

void rdt_sender_next( rdt_state &s )
{
    switch ( s )
    {
    case WAITCALL0:
        s = WAITACK0;
        break;
    case WAITACK0:
        s = WAITCALL1;
        break;
    case WAITCALL1:
        s = WAITACK1;
        break;
    case WAITACK1:
        s = WAITCALL0;
        break;
    }
}

int readpkt( packet_t &rcvpkt, const char *data, size_t size )
{
    uint16_t check = checksum( ( unsigned char * )data, size );

    if ( check != 0 )
    {
        return -1;
    }

    bzero( ( void * )&rcvpkt, sizeof( rcvpkt ) );
    memcpy( ( void * )&rcvpkt.len, data + 2, 2 );
    memcpy( ( void * )&rcvpkt, data, ntohs( rcvpkt.len ) );
    return ntohs( rcvpkt.len );
}

void rdt_recv( rdt_state &s, const char *data, size_t size )
{
    packet_t rcvpkt;
    int numbytes = readpkt( rcvpkt, data, size );

    if ( numbytes == -1 )
    {
        return;
    }

    switch ( s )
    {
    case WAITACK0:

        if ( rcvpkt.ackno == 1 )
        {
            return;
        }

        break;
    case WAITACK1:

        if ( rcvpkt.ackno == 0 )
        {
            return;
        }

        break;
    }

    rdt_sender_next( s );
}

void rdt_send( rdt_state &s, const char *data )
{
    rdt_sender_next( s );
}

void rdt_sender_timeout()
{

}

packet_t make_pkt( int ackno, const char *data, size_t size )
{
    packet_t p;
    bzero( ( void * )&p, sizeof( p ) );

    p.ackno = htonl( ackno );
    p.len = htons( size + 12 );
    memcpy( &p.data, data, size );
    p.cksum = checksum( ( unsigned char * )&p, size + 12 );
    return p;
}


Context( DescribeAnRdtSender )
{
    Spec( ItStartsInWaitCall0 )
    {
        rdt_state s;
        rdt_sender_reset( s );
        Assert::That( s, Equals( WAITCALL0 ) );
    }

    Spec( ItStaysInWaitCall0WhenRecvCalled )
    {
        rdt_state s = WAITCALL0;
        rdt_recv( s, "Data", 4 );
        Assert::That( s, Equals( WAITCALL0 ) );
    }

    Spec( ItStaysInWaitCall1WhenRecvCalled )
    {
        rdt_state s = WAITCALL1;
        rdt_recv( s, "Data", 4 );
        Assert::That( s, Equals( WAITCALL1 ) );
    }


    Spec( ItGoesToWaitAck0WhenSendCalled )
    {
        rdt_state s = WAITCALL0;
        rdt_send( s, "Data" );
        Assert::That( s, Equals( WAITACK0 ) );
    }

    Spec( ItSendsAPacketFromWaitCall0WhenCalled )
    {
        rdt_state s = WAITCALL0;
        int buffer = shm_open( "/ItSendsAPacketFromWaitCall0WhenCalled", O_RDWR, 0 );
        rdt_send( s, buffer, "Data" );
        lseek( buffer, 0, SEEK_SET );
        char p[512];
        bzero( p, 512 );
        read( buffer, p, 512 );
        packet_t q;
        readpkt( q, p, 512 );
        Approvals::Verify( FormatPacket( q ) );
    }
    Spec( ItGoesToWaitAck1WhenSendCalled )
    {
        rdt_state s = WAITCALL1;
        rdt_send( s, "Data" );
        Assert::That( s, Equals( WAITACK1 ) );
    }

    Spec( AfterWaitCall0ItMovesToWaitAck0 )
    {
        rdt_state s = WAITCALL0;
        rdt_sender_next( s );
        Assert::That( s, Equals( WAITACK0 ) );
    }

    Spec( ItStaysInWaitAck0WhenRecevingCorruptPacket )
    {
        rdt_state s = WAITACK0;
        packet_t p = make_pkt( s, "Data", 4 );
        p.cksum = 0xABCD;
        rdt_recv( s, ( char * )&p, ntohs( p.len ) );
        Assert::That( s, Equals( WAITACK0 ) );
    }

    Spec( ItStaysInWaitAck1WhenRecevingCorruptPacket )
    {
        rdt_state s = WAITACK1;
        packet_t p = make_pkt( s, "Data", 4 );
        p.cksum = 0xABCD;
        rdt_recv( s, ( char * )&p, ntohs( p.len ) );
        Assert::That( s, Equals( WAITACK1 ) );
    }

    Spec( ItStaysInWaitAck0WhenReceinvgWrongAck )
    {
        rdt_state s = WAITACK0;
        packet_t p = make_pkt( s, "Data", 4 );
        p.ackno = 1;
        p.cksum =  0x0000 ;
        p.cksum = checksum( ( unsigned char * )&p, ntohs( p.len ) );
        rdt_recv( s, ( char * )&p, ntohs( p.len ) );
        Assert::That( s, Equals( WAITACK0 ) );
    }

    Spec( ItStaysInWaitAck1WhenReceinvgWrongAck )
    {
        rdt_state s = WAITACK1;
        packet_t p = make_pkt( s, "Data", 4 );
        p.ackno = 0;
        p.cksum =  0x0000 ;
        p.cksum = checksum( ( unsigned char * )&p, ntohs( p.len ) );
        rdt_recv( s, ( char * )&p, ntohs( p.len ) );
        Assert::That( s, Equals( WAITACK1 ) );
    }

    Spec( ItStaysInWaitAck0OnTimeout )
    {
        rdt_state s = WAITACK0;
        rdt_sender_timeout();
        Assert::That( s, Equals( WAITACK0 ) );
    }

    Spec( ItStaysInWaitAck1OnTimeout )
    {
        rdt_state s = WAITACK1;
        rdt_sender_timeout();
        Assert::That( s, Equals( WAITACK1 ) );
    }

    Spec( AfterWaitAck0ItMovesToWaitCall1 )
    {
        rdt_state s = WAITACK0;
        rdt_sender_next( s );
        Assert::That( s, Equals( WAITCALL1 ) );
    }

    Spec( ItMovesToWaitCall1AfterReceivingAck0 )
    {
        rdt_state s = WAITACK0;
        packet_t p = make_pkt( 0, "", 0 );
        AssertThat( ntohl( p.ackno ), Equals( 0 ) );
        rdt_recv( s, ( char * )&p, ntohs( p.len ) );
        AssertThat( s, Equals( WAITCALL1 ) );
    }

    Spec( ItMovesToWaitCall0AfterReceivingAck1 )
    {
        rdt_state s = WAITACK1;
        packet_t p = make_pkt( 1, "", 0 );
        AssertThat( ntohl( p.ackno ), Equals( 1 ) );
        rdt_recv( s, ( char * )&p, ntohs( p.len ) );
        AssertThat( s, Equals( WAITCALL0 ) );
    }

    Spec( AfterWaitCall1ItMovesToWaitAck1 )
    {
        rdt_state s;
        rdt_sender_reset( s );

        for ( int i = 0; i < 3; ++i )
        {
            rdt_sender_next( s );
        }

        Assert::That( s, Equals( WAITACK1 ) );
    }

    Spec( AfterWaitAck1ItMovesToWaitCall0 )
    {
        rdt_state s;
        rdt_sender_reset( s );

        for ( int i = 0; i < 4; ++i )
        {
            rdt_sender_next( s );
        }

        Assert::That( s, Equals( WAITCALL0 ) );
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

