/* Copyright Daniele Pallastrelli 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 */

#include <iostream>
#include <string>
#include <vector>
#include <boost/function.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/bind.hpp>

class Menu; // forward declaration

class Cli
{
public:
    Cli() : current( NULL ), exitCmd( "exit" ) {}
    void Run();
    void Current( Menu* menu )
    {
        current = menu;
    }
    void Prompt();
private:
    Menu* current;
    const std::string exitCmd;
};

class Command
{
public:
    virtual bool Exec( const std::string& cmdLine ) = 0;
    virtual ~Command() {}
};

class Menu : public Command
{
public:
    Menu( Cli* _cli, const std::string& _name ) : cli( _cli ), name( _name ), parent( NULL ) {}
    Menu( Cli* _cli, Menu* _parent, const std::string& _name ) : cli( _cli ), name( _name ), parent( _parent ) {}
    void Add( Command* cmd )
    {
        cmds.push_back( cmd );
    }
    bool Exec( const std::string& cmdLine )
    {
        if ( cmdLine == name )
        {
            cli -> Current( this );
            return true;
        }
        return false;
    }
    bool ScanCmds( const std::string& cmdLine )
    {
        for ( Cmds::iterator i = cmds.begin(); i != cmds.end(); ++i )
            if ( ( *i ) -> Exec( cmdLine ) )  return true;
        if ( parent )
            if ( parent -> Exec( cmdLine ) ) return true;
        return false;
    }
    std::string Prompt() const
    {
        return name;
    }
private:
    Cli* cli;
    const std::string name;
    Menu* parent;
    typedef std::vector< Command* > Cmds;
    Cmds cmds;
};

class FuncCmd : public Command
{
public:
    FuncCmd( const std::string& _name, boost::function< void ( void )> _function ) : name( _name ), function( _function ) {}
    bool Exec( const std::string& cmdLine )
    {
        if ( cmdLine == name )
        {
            function();
            return true;
        }

        return false;
    }
private:
    const std::string name;
    const boost::function< void ( void )> function;
};

template < typename T >
class FuncCmd1 : public Command
{
public:
    FuncCmd1( const std::string& _name, boost::function< void ( T ) > _function ) : name( _name ), function( _function ) {}
    bool Exec( const std::string& cmdLine )
    {
        std::vector< std::string > strs;
        boost::split( strs, cmdLine, boost::is_any_of( " \t" ), boost::token_compress_on );
        if ( strs.size() != 2 ) return false;
        if ( name == strs[ 0 ] )
        {
            try
            {
                T arg = boost::lexical_cast< T >( strs[ 1 ] );
                function( arg );
            }
            catch( boost::bad_lexical_cast & )
            {
                return false;
            }
            return true;
        }

        return false;
    }
private:
    const std::string name;
    const boost::function< void ( T )> function;
};

void Cli::Run()
{
    std::string cmd;
    while ( true )
    {
        Prompt();
        std::getline( std::cin, cmd );
        if ( cmd == exitCmd ) break;
        bool found = false;
        found = current -> ScanCmds( cmd );
        if ( !found )
            std::cout << "Command unknown: " << cmd << std::endl;
    }
}

void Cli::Prompt()
{
    std::cout << current -> Prompt() << "> " << std::flush;
}



// ###################################################

void Show( void )
{
    std::cout << "Show" << std::endl;
}

void Dump( void )
{
    std::cout << "Dump" << std::endl;
}

void Stop( void )
{
    std::cout << "Stop" << std::endl;
}

void Run( int x )
{
    std::cout << "Run: " << x << std::endl;
}

class Application
{
public:
    void Start()
    {
        std::cout << "Application::Start" << std::endl;
    }
};

int main()
{
    Application app;

    using namespace boost;

    Cli cli;
        
    Menu rootMenu( &cli, "root" );
 
    Menu statusMenu( &cli, &rootMenu, "status" );
    statusMenu.Add( new FuncCmd( "show", Show ) );
    statusMenu.Add( new FuncCmd( "dump", Dump ) );

    Menu cmdMenu( &cli, &rootMenu, "cmd" );
    cmdMenu.Add( new FuncCmd( "stop", Stop ) );
    cmdMenu.Add( new FuncCmd( "start", bind( &Application::Start, &app ) ) );
    cmdMenu.Add( new FuncCmd1< int >( "run", bind( Run, _1 ) ) );
    
    rootMenu.Add( &statusMenu );
    rootMenu.Add( &cmdMenu );
    
    cli.Current( &rootMenu );
    cli.Run();
    
    return 0;
}
