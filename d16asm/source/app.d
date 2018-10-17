import std.algorithm.searching;
import std.ascii;
import std.file;
import std.format;
import std.stdio;

const string sNumber = "0123456789";
const string sHexnumber = sNumber ~ "ABCDEFabcdef";
const string sIdentifier = "ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijklmnopqrstuvwxyz" ~ sNumber;
const string sDirective = sIdentifier;
const string sLabel = sIdentifier ~ ":";

const string[] keywords = [ "JMP", "RET", "POPSP", "PUSHSP" ];
const string[] directives = [ ".ORG", ".DW", ".DS", ".EQU" ];

bool isKeyword(string s) {
    string su;
    foreach( c; s ) { su ~= c.toUpper; }
    if (keywords.canFind(su)) {
        return true;
    }
    return false;
}

bool isDirective(string s) {
    string su;
    foreach( c; s ) { su ~= c.toUpper; }
    if (directives.canFind(su)) {
        return true;
    }
    return false;
}

struct Character {
    enum Type { None, Eof, Alpha, Digit, Plus, Minus, Slash, Asterix, Comma, Colon, Semicolon, Quote, DoubleQuote, Ws, Newline, Dot }

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
        } else if ( c == '.' ) {
            type = Type.Dot;
        } else if ( c == '*' ) {
            type = Type.Asterix;
        } else if ( c == ':' ) {
            type = Type.Colon;
        } else if ( c == '/' ) {
            type = Type.Slash;
        } else if ( c == ',' ) {
            type = Type.Comma;
        }
    }

    bool isValidDirective() {
        return sDirective.canFind(c);
    }

    bool isValidIdentifier() {
        return sIdentifier.canFind(c);
    }

    bool isValidLabel() {
        return sLabel.canFind(c);
    }

    bool isValidNumber() {
        return sNumber.canFind(c);
    }

    bool isValidHexnumber() {
        return sHexnumber.canFind(c);
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
        scan();
    }

    void scan() {
        uint line = 1;
        uint col = 0;
        Character c1;
        uint p;
        for( p = 0; p < filecontents.length; p++) {
            char c = filecontents[p];
            col = (c == '\n') ? 0 : col+1;
            c1 = Character(c, line, col, p);
            chars ~= c1;
            line += ( c == '\n' ) ? 1 : 0;
        }
        c1 = Character('\0', line, col, p);
        c1.type = Character.Type.Eof;
        chars ~= c1;
    }

    bool pop(ref Character c) {
        bool ret = peek(c);
        if( pos < filecontents.length ) {
            pos++;
        }
        return ret;
    }

    bool peek(ref Character c) {
        bool valid = ( pos < filecontents.length );
        if( valid ) {
            c = chars[pos];
        }
        return valid;
    }

    uint pos = 0;
    Character[] chars;
    ubyte[] filecontents;
}

struct Token {
    enum Type { None, Operator, Label, Separator, Keyword, Directive, Identifier, String, Number, Hexnumber, Ws, Newline, Comment, Eof }

    void append(char c) {
        cargo ~= c;
    }

    uint line;
    uint col;
    Type type;
    string cargo;
}

class Lexer {
    this(string fname) {
        Scanner scanner = new Scanner(fname);
        Character c, n;
        while( scanner.pop(c) ) {
            // String
            if( c.type == Character.Type.DoubleQuote || c.type == Character.Type.Quote ) {
                Character.Type type = c.type;
                Token newToken = Token(c.line, c.col, Token.Type.String);
                newToken.append(c.c);
                while(scanner.pop(c) && c.type != type) {
                    newToken.append(c.c);
                }
                if( c.type != type ) {
                    throw new Exception(format("Missing double quote %s:%s", newToken.line, newToken.col ));
                }
                newToken.append(c.c);
                tokens ~= newToken;
            }

            // Comment
            else if( c.type == Character.Type.Semicolon ) {
                Token newToken = Token(c.line, c.col, Token.Type.Comment);
                newToken.append(c.c);
                while( scanner.peek(c) && c.type != Character.Type.Newline ) {
                    scanner.pop(c);
                    newToken.append(c.c);
                }
                tokens ~= newToken;
            }

            // Operator
            else if( c.type == Character.Type.Plus || c.type == Character.Type.Minus ||
                     c.type == Character.Type.Asterix || c.type == Character.Type.Slash ) {
                Token newToken = Token(c.line, c.col, Token.Type.Operator);
                newToken.append(c.c);
                tokens ~= newToken;
            }

            // Separator
            else if( c.type == Character.Type.Comma ) {
                Token newToken = Token(c.line, c.col, Token.Type.Separator);
                newToken.append(c.c);
                tokens ~= newToken;
            }
            
            // Directive
            else if( c.type == Character.Type.Dot ) {
                // first assume Directive
                Token newToken = Token(c.line, c.col, Token.Type.Directive);
                newToken.append(c.c);
                while( scanner.peek(c) && c.type != Character.Type.Ws ) {
                    scanner.pop(c);
                    if( c.isValidDirective() ) {
                        newToken.append(c.c);
                    }
                }
                tokens ~= newToken;
            }

            // Identifier or Label
            else if( c.type == Character.Type.Alpha ) {
                // first assume identifier
                Token newToken = Token(c.line, c.col, Token.Type.Identifier);
                newToken.append(c.c);
                while( scanner.peek(n) && n.isValidIdentifier() ) {
                    scanner.pop(c);
                    newToken.append(c.c);
                }
                if( n.isValidLabel() ) {
                    scanner.pop(c);
                    newToken.append(c.c);
                    newToken.type = Token.Type.Label;
                }
                tokens ~= newToken;
            }

            // Number or Hex number
            else if( c.type == Character.Type.Digit ) {
                Token newToken = Token(c.line, c.col, Token.Type.Number);
                newToken.append(c.c);
                if ( c.c == '0' && scanner.peek(n) && n.c.toUpper == 'X' ) {
                    scanner.pop(c);
                    newToken.append(c.c);
                    newToken.type = Token.Type.Hexnumber;
                    while(scanner.peek(c) && c.isValidHexnumber() ) {
                        scanner.pop(c);
                        newToken.append(c.c);
                    }
                } else {
                    while(scanner.peek(c) && c.isValidNumber() ) {
                        scanner.pop(c);
                        newToken.append(c.c);
                    }
                }
                tokens ~= newToken;
            }

            else if( c.type == Character.Type.Ws || c.type == Character.Type.Newline ) {
                Token newToken = Token(c.line, c.col, Token.Type.Ws);
                newToken.append(c.c);
                tokens ~= newToken;
            }

            else if( c.type == Character.Type.Newline ) {
                Token newToken = Token(c.line, c.col, Token.Type.Newline);
                newToken.append(c.c);
                tokens ~= newToken;
            }
        }
    }

    void trim() {
        // delete all redundant whitespaces
        Token[] trimmed;
        uint ti = 0;
        trimmed ~= tokens[0];
        for( uint i=1; i+1<tokens.length; i++) {
           if(trimmed[ti].type == Token.Type.Ws && tokens[i].type == Token.Type.Ws ) {
               continue;
           }
           trimmed ~= tokens[i];
           ti++;
        }
        tokens = trimmed;
    }

    void convertIdentifiers() {
        // convert identifiers to keywords if possible
        foreach( ref t; tokens ) {
            if ( isKeyword(t.cargo) ) {
                t.type = Token.Type.Keyword;
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
    lexer.trim();
    lexer.convertIdentifiers();

    foreach( t; lexer.tokens ) {
        writeln(t.type, ": ", t.cargo);
    }

    return 0;
}
