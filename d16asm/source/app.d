import std.ascii;
import std.file;
import std.format;
import std.stdio;

struct Character {
    enum Type { None, Eof, Alpha, Digit, Plus, Minus, Semicolon, Quote, DoubleQuote, Ws, Newline }

    this( char c, uint line, uint col, uint pos) {
        this.c = c;
        this.line = line;
        this.col = col;
        this.pos = pos;
        if( isAlpha(c) ) {
            type = Type.Alpha;
        } else if( c == ';' ) {
            type = Type.Semicolon;
        } else if( c == ' ' || c == '\t' ) {
            type = Type.Ws;
        } else if( c == '+' ) {
            type = Type.Plus;
        } else if( c == '-' ) {
            type = Type.Minus;
        } else if( c == '\r' || c == '\n' ) {
            this.c = ' ';
            type = Type.Newline;
        } else if( isDigit(c) ) {
            type = type.Digit;
        } else if( c == '"' ) {
            type = Type.DoubleQuote;
        } else if( c == '\'' ) {
            type = Type.Quote;
        }
    }

    string toString() {
        return format( "%4s %3s %6s %s %s", line, col, pos, c, type );
    }

    char c;
    uint line;
    uint col;
    uint pos;
    Type type = Type.None;
}

class Scanner {
    this(string fname) {
        writefln("Opening %s", fname);
        filecontents = cast(ubyte[])read(fname);
    }

    bool get(ref Character cc) {
        pos++;
        char c = '\0';
        if( pos < filecontents.length ) {
            c = filecontents[pos];
        }
        if( c == '\n' ) {
            col = 0;
        } else {
            col++;
        }
        cc = Character(c, line, col, pos);
        if( c == '\n' ) {
            line++;
        }
        if( pos >= filecontents.length ) {
            cc.type = Character.Type.Eof;
        }
        return cc.type != Character.Type.Eof;
    }

    uint line = 0;
    uint col = 0;
    uint pos = 0;
    ubyte[] filecontents;
}

struct Token {
    enum Type { None, Label, Mnemonic, Directive, Identifier, String, Number, Ws, Comment, Eof }

    uint line;
    uint col;
    Type type;
}

class Lexer {
    this(string fname) {
        Scanner scanner = new Scanner(fname);
        Character c;
        while( scanner.get(c) ) {
            if( c.type == Character.Type.DoubleQuote ) {
                Token newToken = Token(c.line, c.col, Token.Type.String);
                tokens ~= newToken;
                while(scanner.get(c) && c.type != Character.Type.DoubleQuote){}
                if( c.type != Character.Type.DoubleQuote ) {
                    throw new Exception(format("Missing double quote %s:%s", newToken.col, newToken.line));
                }
            }
        }
    }
    Token[] tokens;
}

int main(string[] args)
{
    if( args.length < 2 ) {
        writeln("Missing argument");
        return 1;
    }
    Lexer lexer = new Lexer(args[1]);

    return 0;
}
