/*******************************************************************************
 * CLI - A simple command line interface.
 * Copyright (C) 2013 Daniele Pallastrelli
 *
 * Boost Software License - Version 1.0 - August 17th, 2003
 *
 * Permission is hereby granted, free of charge, to any person or organization
 * obtaining a copy of the software and accompanying documentation covered by
 * this license (the "Software") to use, reproduce, display, distribute,
 * execute, and transmit the Software, and to prepare derivative works of the
 * Software, and to permit third-parties to whom the Software is furnished to
 * do so, all subject to the following:
 *
 * The copyright notices in the Software and this entire statement, including
 * the above license grant, this restriction and the following disclaimer,
 * must be included in all copies of the Software, in whole or in part, and
 * all derivative works of the Software, unless such copies or derivative
 * works are solely in the form of machine-executable object code generated by
 * a source language processor.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
 * SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
 * FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 ******************************************************************************/

#ifndef CLI_H_
#define CLI_H_
 
#include <iostream>
#include <string>
#include <vector>
#include <boost/function.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/bind.hpp>

namespace cli
{

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
    virtual bool Exec( const std::vector< std::string >& cmdLine ) = 0;
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
    bool Exec( const std::vector< std::string >& cmdLine )
    {
        if ( cmdLine[ 0 ] == name )
        {
            cli -> Current( this );
            return true;
        }
        return false;
    }
    bool ScanCmds( const std::vector< std::string >& cmdLine )
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
    bool Exec( const std::vector< std::string >& cmdLine )
    {
        if ( cmdLine[ 0 ] == name )
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
    bool Exec( const std::vector< std::string >& cmdLine )
    {
        if ( cmdLine.size() != 2 ) return false;
        if ( name == cmdLine[ 0 ] )
        {
            try
            {
                T arg = boost::lexical_cast< T >( cmdLine[ 1 ] );
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

inline void Cli::Run()
{
    std::string cmd;
    while ( true )
    {
        Prompt();
        std::getline( std::cin, cmd );
        std::vector< std::string > strs;
        boost::split( strs, cmd, boost::is_any_of( " \t" ), boost::token_compress_on );
        // remove null entries from the vector:
        strs.erase( 
            std::remove_if( 
                strs.begin(), 
                strs.end(), 
                boost::bind( &std::string::empty, _1 )
            ),
            strs.end()
        );
        if ( strs.size() == 0 ) break; // just hit enter
        if ( strs[ 0 ] == exitCmd ) break;
        bool found = false;
        found = current -> ScanCmds( strs );
        if ( !found ) std::cout << "Command unknown: " << cmd << std::endl;
    }
}

inline void Cli::Prompt()
{
    std::cout << current -> Prompt() << "> " << std::flush;
}

}; // namespace

#endif

