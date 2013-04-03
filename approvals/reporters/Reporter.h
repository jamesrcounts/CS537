#ifndef REPORTER_H
#define REPORTER_H

#include "CommandLauncher.h"

class Reporter
{
private:
    std::string cmd;
    CommandLauncher *l;

protected:
    Reporter( std::string command, CommandLauncher *launcher )
        : cmd( command ), l( launcher )
    {
    }

public:
    void report( std::string received, std::string approved )
    {
        std::vector<std::string> fullCommand;
        fullCommand.push_back( cmd );
        fullCommand.push_back( received );
        fullCommand.push_back( approved );
        l->launch( fullCommand );
    }
};

class MeldReporter : public Reporter
{
private:
    SystemLauncher launcher;

public:
    MeldReporter() : Reporter( "meld", &launcher ) {};
};

class TestReporter : public Reporter
{
public:
    DoNothingLauncher launcher;
    TestReporter() : Reporter( "fake", &launcher ) {};
};

#endif