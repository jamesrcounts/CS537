#include <cstring>
#include <igloo/igloo.h>
#include <unistd.h>
#include "../Namer.h"
#include "../StringWriter.h"
#include "../reporters/Reporter.h"

using namespace igloo;
using namespace std;

class ApprovalMissingException : public std::exception
{
private:
    char *message;

    char *format( const std::string file )
    {
        size_t n = 1024;
        char s[n];
        int size = snprintf( s,
                             n,
                             "Failed Approval: Approval File \"%s\" Not Found.",
                             file.c_str() );
        char *t = new char[size + 1];
        std::strncpy( t, s, size + 1 );
        return t;
    }

public:
    ApprovalMissingException( std::string received, std::string approved )
        : message( format( approved ) )
    {
    }

    ApprovalMissingException( const ApprovalMissingException &a )
        : message( NULL )
    {
        size_t message_sz = std::strlen( a.message );
        message = new char[message_sz + 1];
        std::strncpy( message, a.message, message_sz + 1 );
    }

    ~ApprovalMissingException() throw()
    {
        delete[] message;
    }

    virtual const char *what() const throw()
    {
        return message;
    }
};

class FileApprover
{
protected:
    FileApprover();
    ~FileApprover();
public:
    static void verify( Namer n, StringWriter s, Reporter r )
    {

    }
};

Context( DescribeApprovalExceptions )
{
    Spec( ApprovalMissingExceptionHasAMessage )
    {
        ApprovalMissingException a( "r.txt", "a.txt" );
        Assert::That( a.what(),
                      Equals( "Failed Approval: Approval File \"a.txt\" Not Found." ) );
    }
};

Context( DescribeAFileApprover )
{
    Spec( ItVerifiesApprovedFileExists )
    {
        char buf[512];
        ssize_t readlink_ok = readlink( "/proc/self/cwd", buf, 512 );
        string dir( buf );

        Namer namer( dir, "DescribeAFileApprover.ItVerifiesApprovedFileExists" );
        StringWriter writer( "Hello" );
        TestReporter reporter;
        AssertThrows( ApprovalMissingException,
                      FileApprover::verify( namer, writer, reporter ) );
    }
    Spec( ItVerifiesFilesHaveEqualSize ) {}
    Spec( ItVerifiesEveryByteIsEqual ) {}
};
/*
describe('FileApprover', function () {

	describe('', function () {
		it('should verify two files match', function () {

			var writer = new StringWriter(config, "HELLO!");
			var reporter = new DoNothingReporter();

			FileApprover.verify(namer, writer, reporter);

		});
	});
});
*/
int main( int argc, char const *argv[] )
{
    return TestRunner::RunAllTests();
}