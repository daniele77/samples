// see http://norasandler.com/2017/11/29/Write-a-Compiler.html

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <boost/filesystem.hpp>

namespace detail
{
    inline std::string FormatMsg(const std::string& msg, std::size_t line, std::size_t col)
    {
        std::ostringstream oss;
        oss << "Line " << line << ", col " << col << ": " << msg;
        return oss.str();
    }
} // detail

/** Base class for all compiler errors. Derives from
* @c std::runtime_error. Call member function @c what to get human
* readable message associated with the error.
*/
class CompilerError : public std::runtime_error
{
public:
    /** Instantiate a CompilerError instance with the given message.
    * @param what The message to associate with this error.
    */
    CompilerError( const std::string &what ) : 
        std::runtime_error( what ) 
    {
    }

    ~CompilerError() throw()
    {
    }
};

/** Error indicating a lexical error scanning a configuration.
*   Derives from CompilerError.
*/
class LexicalError : public CompilerError
{
public:
    LexicalError(const std::string& msg, std::size_t line, std::size_t col) :
        CompilerError(detail::FormatMsg(msg, line, col)) {}
};

/** Error indicating a syntax error parsing a configuration.
*   Derives from CompilerError.
*/
class SyntaxError : public CompilerError
{
public:
    /// Instantiate a SyntaxError
    SyntaxError(const std::string& msg, std::size_t line, std::size_t col) :
        CompilerError(detail::FormatMsg(msg, line, col)) {}
};

// Represent a token
struct Token
{
    // terminals
    enum Type { 
        keyword,           // return, int, float ...
        identifier,        // <id>     [a-zA-Z_] [a-zA-Z0-9_:<>]*
        open_parenthesis,  // (
        close_parenthesis, // )
        open_brace,        // {
        close_brace,       // }
        semicolon,         // ;
        assign,            // =
        int_literal,       // [0-9]+
        char_literal,      // '.'
        string_literal,    // ".*"
        negation,          // -
        bitwise_complement,// ~
        logical_negation,  // !
        done               // EOF
    };
    Type type;
    std::string lexem;
    Token( Type t, const std::string& l = std::string() ) : type( t ), lexem( l ) {}
    // return a string explaining the type of a token
    static std::string Description( Type t )
    {
        switch ( t )
        {
            case keyword: return "<keyword>"; break;
            case identifier: return "<identifier>"; break;
            case open_parenthesis: return "("; break;
            case close_parenthesis: return ")"; break;
            case open_brace: return "{"; break;
            case close_brace: return "}"; break;
            case semicolon: return ";"; break;
            case assign: return "="; break;
            case int_literal: return "<int_literal>"; break;
            case char_literal: return "<char_literal>"; break;
            case string_literal: return "<string_literal>"; break;
            case negation: return "-"; break;
            case bitwise_complement: return "~"; break;
            case logical_negation: return "!"; break;
            case done: return "<EOF>"; break;
        }
        return "???"; // can't reach this point
    }

    static const std::vector<std::string> keywords;
};

const std::vector<std::string> Token::keywords = { "return", "int", "float" };

// Split an input stream into a sequence of token.
// Each call at Split::Next method returns the next token
// (or throws a LexicalError if the next token is unknown).
class TokenSource
{
public:
    explicit TokenSource(std::istream& in) : input(in), lineno(1), column(1) {}
    // throw LexicalError
    Token Next()
    {
        while ( true )
        {
            char c = input.peek();
            switch ( c )
            {
                case ' ': case '\t': Consume(); break;
                case '\n': NewLine(); Consume(); break;
                case '(': Consume(); return Token( Token::open_parenthesis ); break;
                case ')': Consume(); return Token( Token::close_parenthesis ); break;
                case '{': Consume(); return Token( Token::open_brace ); break;
                case '}': Consume(); return Token( Token::close_brace ); break;
                case ';': Consume(); return Token( Token::semicolon ); break;
                case '=': Consume(); return Token( Token::assign ); break;
                case '-': Consume(); return Token( Token::negation ); break;
                case '~': Consume(); return Token( Token::bitwise_complement ); break;
                case '!': Consume(); return Token( Token::logical_negation ); break;
                case '\'': 
                    {
                        Consume(); // '
                        c = input.peek();
                        // TODO manage quoted chars
                        Consume(); // char
                        if ( input.peek() == '\'' ) Consume(); // '
                        else throw LexicalError( "Missing terminating char closing", lineno, column );
                        return Token( Token::char_literal, std::string( 1, c ) );
                    }
                    break;
                case '"':
                    {
                        std::string value;
                        Consume();
                        c = input.peek();
                        while ( c != '"' && !input.eof() && c != '\n' )
                        {
                            value += c;
                            Consume();
                            c = input.peek();
                        }
                        if ( c == '"' ) Consume();
                        else throw LexicalError( "Missing terminating string closing", lineno, column );
                        return Token( Token::string_literal, value );
                    }
                    break;
                default:
                {
                    if ( isalpha( c ) || c == '_' )
                    {
                        std::string id;
                        while ( isalnum( c ) || c == '_' || c == ':' || c == '<' || c == '>' )
                        {
                            id += c;
                            Consume();
                            c = input.peek(); // next char...
                        }
                        if (std::find(Token::keywords.begin(), Token::keywords.end(), id) != Token::keywords.end())
                            return Token(Token::keyword, id);
                        else return Token( Token::identifier, id );
                    }
                    else if ( input.eof() )
                        return Token( Token::done );
                    else if ( isdigit( c ) || c == '-' || c == '+' )
                    {
                        std::string num;
                        std::size_t sepcount = 0;

                        num += c;
                        Consume();
                        c = input.peek(); // next char...

                        while ( isdigit( c ) || c == '.' )
                        {
                            if ( c == '.' ) 
                                if ( ++sepcount > 1 )
                                    throw LexicalError( "Floating point invalid (too many '.')", lineno, column );
                            num += c;
                            Consume();
                            c = input.peek(); // next char...
                        }
                        return Token( Token::int_literal, num );
                    }
                    else
                        throw LexicalError( "Unrecognized character", lineno, column );
                }
            }
        }
    }

    std::size_t Line() const { return lineno; }
    std::size_t Col() const { return column; }

private:
    void NewLine()
    {
        ++lineno;
        column = 1;
    }
    void Consume()
    {
        input.get();
        ++column;
    }
    std::istream& input;
    std::size_t lineno;
    std::size_t column;
};

/////////////////////////////////////////////////////////////

/*
GRAMMAR

<program> ::= <function>
<function> ::= "int" <id> "(" ")" "{" <statement> "}"
<statement> ::= "return" <exp> ";"
<exp> ::= <unary_op> <exp> | <int_literal>
<unary_op> ::= "!" | "~" | "-"

*/

template <typename SemanticActions>
class Grammar
{
public:
    Grammar(std::istream& in, SemanticActions& sa) : 
        input(in), lookahead(Token::done), actions(sa) 
    {}

    // throws SyntaxError
    // <program> ::= <function>
    // <function> ::= "int" <id> "(" ")" "{" <statement> "}"
    void Parse()
    {
        lookahead = input.Next();
        Match(Token::keyword);
        const std::string funName = NextLexem();
        Match(Token::identifier);
        Match(Token::open_parenthesis);
        Match(Token::close_parenthesis);
        Match(Token::open_brace);
        Statement();
        Match(Token::close_brace);
        Match(Token::done);
        actions.Function(funName);
    }
private:
    // <statement> ::= "return" <exp> ";"
    void Statement()
    {
        const std::string returnKeyword = NextLexem();
        if (returnKeyword != "return")
            throw SyntaxError( "expecting return, got " + returnKeyword, input.Line(), input.Col() );
        Match(Token::keyword);
        Expression();
        Match(Token::semicolon);
        actions.Return();
    }

    // <exp> ::= <unary_op> <exp> | <int_literal>
    void Expression()
    {
        if (lookahead.type == Token::int_literal)
        {
            const std::string intLiteral = NextLexem();
            Match(Token::int_literal);
            actions.IntLiteral(intLiteral);
        }
        else // TODO
            throw SyntaxError( "expecting int literal, got " + Token::Description(lookahead.type), input.Line(), input.Col() );        
    }

#if 0
    // loadexpr -> <load> <value>
    void LoadExpr()
    {
        Match( Token::load );
        const std::string value = NextLexem();
        Match( Token::value );
        actions.Load( value );
    }
    // assignexpr     ->  <id> <assign> <create> <id> <optparamlist> | <id> depexpr
    // optparamlist   ->  <empty> | <open> attrlist <close>
    // attrlist       ->  <empty> | attrassign moreattrassign
    // attrassign     ->  <id> <assign> <value>
    // moreattrassign ->  <empty> | <attrsep> attrassign    
    // depexpr        ->  <collsep> <id> <assign> <id>
    void AssignExpr()
    {
        const std::string lvalue = NextLexem();
        Match( Token::id );
        switch ( lookahead.type )
        {
            case Token::assign:
            {
                Match( Token::assign );
                Match( Token::create );
                const std::string rvalue = NextLexem();
                Match( Token::id );
                if ( lookahead.type == Token::open )
                {
                    Match( Token::open );
                    while ( lookahead.type == Token::id )
                    {
                        const std::string attId = NextLexem();
                        Match( Token::id );
                        Match( Token::assign );
                        const std::string attValue = NextLexem();
                        Match( Token::value );
                        actions.AssignAttribute( lvalue, attId, attValue );
                        if ( lookahead.type == Token::attrsep ) Match( Token::attrsep );
                    }
                    Match( Token::close );
                }
                actions.Create( lvalue, rvalue );
                break;
            }
            case Token::collsep:
            {
                Match( Token::collsep );
                const std::string dep = NextLexem();
                Match( Token::id );
                Match( Token::assign );
                const std::string rvalue = NextLexem();
                Match( Token::id );
                actions.AssignDep( lvalue, dep, rvalue );
                break;
            }
            default:
                throw SyntaxError( "expecting = or .", input.Line(), input.Col() ); // TODO
        }
    }
#endif
    void Match( Token::Type t )
    {
        if ( lookahead.type == t ) lookahead = input.Next();
        else throw SyntaxError( "expecting token " + Token::Description( t ) + ". Got " + Token::Description(lookahead.type), input.Line(), input.Col() ); // TODO error msg (e.g., "expecting t")
    }
    std::string NextLexem() const
    {
        return lookahead.lexem;
    }
    TokenSource input;
    Token lookahead;
    SemanticActions& actions;
};

/////////////////////////////////////////////////////////////

using namespace std;

class DumpActions {};

namespace AST
{
    class Node
    {
    public:
        virtual ~Node() = default;
        virtual void Emit(ostream& out) = 0;
    };

    class IntLiteral : public Node
    {
    public:
        explicit IntLiteral(const string& _value) : value(_value) {}
        void Emit(ostream& out) override { out << "movl $" << value << ", %eax\n"; }
    private:
        const string value;
    };

    class Return : public Node
    {
    public:
        explicit Return(unique_ptr<Node> _exp) : exp(move(_exp)) {}
        void Emit(ostream& out) override
        { 
            exp->Emit(out);
            out << "ret\n";
        }
    private:
        unique_ptr<Node> exp;
    };

    class Function : public Node
    {
    public:
        Function(const string& _funcName, unique_ptr<Node> _body) : funcName(_funcName), body(move(_body)) {}
        void Emit(ostream& out) override
        { 
            out <<
                ".globl " + funcName + "\n"
                "" + funcName + ":\n";
            body->Emit(out);
        }
    private:
        const string funcName;
        unique_ptr<Node> body;
    };
};

class ASTActions
{
public:
    void Function(const string& funcName)
    {
        current = make_unique<AST::Function>(funcName, move(current));        
    }
    void Return()
    {
        current = make_unique<AST::Return>(move(current));
    }
    void IntLiteral(const std::string& value)
    {
        current = make_unique<AST::IntLiteral>(value);
    }
    void Dump(std::ostream& o) const 
    {
        current->Emit(o);
    }
private:
    unique_ptr<AST::Node> current;
};

class ASMActions
{
public:
    void Prologue()
    {}
    void Epilogue()
    {
    }
    void Function(const std::string& funcName)
    {
        output +=
            ".globl " + funcName + "\n"
            "" + funcName + ":\n";        
    }
    void Return(const std::string& value)
    {
        output +=
        "movl $" + value + ", %eax\n"
        "ret\n";
    }
    void Dump(std::ostream& o) const { o << output << std::flush; }
private:
    std::string output;
};

/////////////////////////////////////////////////////////////

/*
class AST
{
};

std::unique_ptr<AST> Parse(const std::string& inputFile)
{
    auto ast = std::make_unique<AST>();
    return ast;
}

std::string GenerateAsm(std::unique_ptr<AST> ast)
{
    const std::string funcName = "main";
    const std::string retValue = "42";

    const std::string asmContent =
        ".globl " + funcName + "\n"
        "" + funcName + ":\n"
        "movl $" + retValue + ", %eax\n"
        "ret\n";

    return asmContent;
}
*/

int main(int argc, char* argv[])
{
    try
    {
        if (argc != 2)
        {
            std::cerr << "No input file" << std::endl;
            return 1;
        }

        std::string fileName = argv[1];
        
        if ( boost::filesystem::extension(fileName) != ".c" )
        {
            std::cerr << "Only files with extension .c are allowed" << std::endl;
            return 1;
        }

        //
        using Actions = ASTActions;
        Actions actions;
        //actions.Prologue();
        std::ifstream input(fileName);
        if (!input)
        {
            std::cerr << "File " << fileName << " not found" << std::endl;
            return 1;
        }
        Grammar<Actions> grammar(input, actions);
        grammar.Parse();
        //actions.Epilogue();
        std::cout << "End Parsing" << std::endl;

        fileName = boost::filesystem::change_extension(fileName, ".s").string();
        std::ofstream out(fileName);
        actions.Dump(out);

        // 

        /*
        auto ast = Parse(fileName);
        
        fileName = boost::filesystem::change_extension(fileName, ".s").string();

        std::ofstream out(fileName);
        out << GenerateAsm(std::move(ast));
*/
    }
    catch (const std::exception& e)
    {
        std::cerr << "Exception:\n" << e.what() << std::endl;
    }

    return 0;
}
