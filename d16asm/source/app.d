import std.algorithm;
import std.ascii;
import std.conv;
import std.file;
import std.format;
import std.stdio;

const string sNumber = "0123456789";
const string sHexnumber = sNumber ~ "ABCDEFabcdef";
const string sIdentifier = "ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijklmnopqrstuvwxyz" ~ sNumber;
const string sDirective = sIdentifier;
const string sLabel = sIdentifier ~ ":";

const string[] directives = [ ".ORG", ".DW", ".DS", ".EQU" ];

ushort[string] dictIdentifier;/* = [
    "DUP": 0xA110, "SWAP": 0x80A0, "DROP": 0xC120, "JMPZ": 0xE650, "JMPL": 0xE750, "JMP": 0xC150,
    "CALL": 0xC190, "RET": 0x9050, "PUSHRS": 0xC100, "DROPRS": 0x9120, "POPRS": 0xB010, "LOAD": 0x8420,
    "STORE": 0xE860, "PUSHPC": 0xA210, "PUSHSP": 0xA310, "POPSP": 0x8140, "ADD": 0xC530, "ADC": 0x8581,
    "SUB": 0xC538, "SBC": 0x8589, "AND": 0xC532, "OR": 0xC533, "XOR": 0xC534, "INV": 0x8525,
    "LSL": 0xC536, "LSR": 0xC537
];*/

string toUpperCase( in string s ) {
    string su;
    foreach( c; s ) { su ~= c.toUpper; }
    return su;
}

ushort convertToUshort( string s ) {
    s = s.toUpperCase;
    if( s.length > 2 && s[0 .. 2] == "0X" ) {
        s = s[2 .. $];
        return to!ushort(s, 16);
    }
    return to!ushort(s, 10);
}

bool isDirective(string s) {
    s = toUpperCase(s);
    if (directives.canFind(s)) {
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
            else if( c.type == Character.Type.Ws ) {
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
        tokens ~= Token(c.line, c.col, Token.Type.Eof);
        trim();
        //convertIdentifiers();
    }

    private:
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

    Token[] tokens;
}

class CmdBase {
    public:

    enum Type { Base, Org, Ds, Dw, Equ, Number, Expression, Comment, String, Label, Identifier }
    enum Result { Error, Done, Next }

    this( Token t ) {
        CmdBase.add(t);
        type = Type.Base;
    }

    Result add( Token t ) {
        tokens ~= t;
        return Result.Done;
    }

    override string toString() const {
        return tokens[0].cargo;
    }

    Type type;
    Token[] tokens;
    
    ushort locateAddr;
}

class CmdOrg : CmdBase {
    enum State { Value, EndLine, Done }

    this( Token t ) {
        super(t);
        type = Type.Org;
    }

    override Result add( Token t ) {
        if( t.type == Token.Type.Ws ) {
            return Result.Next;
        }
        final switch( state ) {
            case State.Value:
                if( [Token.Type.Number, Token.Type.Hexnumber].canFind(t.type) ) {
                    tokens ~= t;
                    state = State.EndLine;
                    address = convertToUshort(t.cargo);
                    return Result.Next;
                }
                throw new Exception(format("ERROR: %s:%s Expected value", t.line, t.col ));
            case State.EndLine:
                if( [Token.Type.Newline, Token.Type.Eof].canFind(t.type)) {
                    state = State.Done;
                    return Result.Done;
                }
                if( t.type == Token.Type.Comment ) {
                    return Result.Next;
                }
                throw new Exception(format("ERROR: %s:%s Expected end of line", t.line, t.col ));
            case State.Done: break;
        }
        return Result.Error;
    }

    override string toString() const {
        string s = tokens[0].cargo;
        if( state == State.Done ) {
            s ~= " " ~ tokens[1].cargo;
        }
        return s;
    }

    ushort getAddress() {
        return address;
    }

    State state = State.Value;
    ushort address;
}

class CmdEqu : CmdBase {
    enum State { Ident, Value, LineEnd, Done }

    this( Token t ) {
        super(t);
        type = Type.Equ;
    }

    override Result add( Token t ) {
        if( t.type == Token.Type.Ws ) {
            return Result.Next;
        }
        final switch( state ) {
            case State.Ident:
                if( t.type == Token.Type.Identifier ) {
                    tokens ~= t;
                    state = State.Value;
                    return Result.Next;
                }
                throw new Exception(format("ERROR: %s:%s Expected identifier", t.line, t.col ));
            case State.Value:
                if( t.type == Token.Type.Number || t.type == Token.Type.Hexnumber ) {
                    tokens ~= t;
                    state = State.LineEnd;
                    return Result.Next;
                }
                if( t.type == Token.Type.Comment ) {
                    return Result.Next;
                }
                throw new Exception(format("ERROR: %s:%s Expected value", t.line, t.col ));
            case State.LineEnd:
                if( t.type == Token.Type.Comment ) {
                    return Result.Next;
                }
                if( t.type == Token.Type.Newline ) {
                    state = State.Done;
                    return Result.Done;
                }
                throw new Exception(format("ERROR: %s:%s Expected end of line", t.line, t.col ));
            case State.Done: break;
        }
        return Result.Error;
    }

    override string toString() const {
        string s = tokens[0].cargo;
        if( state == State.Done ) {
            s ~= " " ~ tokens[1].cargo ~ " " ~ tokens[2].cargo;
        }
        return s;
    }

    string getName() {
        return tokens[1].cargo;
    }

    ushort getValue() {
        return convertToUshort(tokens[2].cargo);
    }

    State state = State.Ident;
}

class CmdDs : CmdBase {
    this( Token t ) {
        super(t);
        type = Type.Ds;
    }

    override Result add( Token t ) {
        if( t.type == Token.Type.Ws ) {
            return Result.Next;
        }
        if( [Token.Type.Number, Token.Type.Hexnumber].canFind(t.type) ) {
            tokens ~= t;
            size = convertToUshort(t.cargo);
            return Result.Done;
        }
        throw new Exception(format("ERROR: %s:%s Expected value", t.line, t.col ));
    }

    override string toString() const {
        string s = tokens[0].cargo;
        if( tokens.length > 1 ) {
            s ~= " " ~ tokens[1].cargo;
        }
        return s;
    }

    ushort getSize() {
        return size;
    }

    ushort size;
}

class CmdDw : CmdBase {
    enum State { MandatoryValue, Value, Sep, Done }

    Token.Type[] validValues = [
        Token.Type.Number,
        Token.Type.Hexnumber,
        Token.Type.String,
        Token.Type.Identifier
    ];

    this( Token t ) {
        super(t);
        type = Type.Dw;
    }

    override Result add( Token t ) {
        if( t.type == Token.Type.Ws ) {
            return Result.Next;
        }
        final switch( state ) {
            case State.MandatoryValue:
                if( validValues.canFind(t.type) ) {
                    tokens ~= t;
                    convert(t);
                    state = State.Sep;
                    return Result.Next;
                }
                throw new Exception(format("ERROR: %s:%s Expected identifier or value", t.line, t.col ));
            case State.Value:
                if( validValues.canFind(t.type) ) {
                    tokens ~= t;
                    convert(t);
                    state = State.Sep;
                    return Result.Next;
                }
                throw new Exception(format("ERROR: %s:%s Expected identifier or value", t.line, t.col ));
            case State.Sep:
                if( t.type == Token.Type.Newline ) {
                    state = State.Done;
                    return Result.Done;
                }
                if( t.type == Token.Type.Separator ) {
                    state = State.Value;
                    return Result.Next;
                }
                throw new Exception(format("ERROR: %s:%s Expected ,", t.line, t.col ));
            case State.Done: break;
        }
        return Result.Error;
    }

    override string toString() const {
        string s = tokens[0].cargo;
        if( tokens.length > 1 ) {
            s ~= " " ~ tokens[1].cargo;
            for( uint i = 2; i<tokens.length; i++) {
                s ~= " " ~ tokens[i].cargo;
            }
        }
        return s;
    }

    void convert( Token t ) {
        switch( t.type ) {
            case Token.Type.Number: values ~= convertToUshort(t.cargo); break;
            case Token.Type.Hexnumber: values ~= convertToUshort(t.cargo); break;
            case Token.Type.Identifier:
            case Token.Type.String: 
            default:
        }
    }

    ushort getSize() {
        return cast(ushort)values.length;
    }

    ushort[] getData() {
        return values;
    }

    ushort[] values;
    State state = State.Value;
}

class CmdLabel : CmdBase {
    this( Token t ) {
        super(t);
        type = Type.Label;
    }

    override Result add( Token t ) {
        if( t.type == Token.Type.Label ) {
            tokens ~= t;
            return Result.Done;
        }
        throw new Exception(format("ERROR: %s:%s Expected label", t.line, t.col ));
    }

    override string toString() const {
        return tokens[0].cargo;
    }

    string getName() {
        return tokens[0].cargo[0..$-1];
    }
}

class CmdNumber : CmdBase {
    this( Token t ) {
        super(t);
        value = convertToUshort(t.cargo);
        type = Type.Number;
    }

    override Result add( Token t ) {
        throw new Exception(format("ERROR: %s:%s Expected (hex) number", t.line, t.col ));
    }

    ushort getValue() {
        return value;
    }

    override string toString() const {
        return value.to!string();
    }

    ushort value;
}


class CmdIdentifier : CmdBase {
    this( Token t ) {
        super(t);
        type = Type.Identifier;
    }

    override Result add( Token t ) {
        throw new Exception(format("ERROR: %s:%s whatever", t.line, t.col ));
    }

    string getName() {
        return tokens[0].cargo;
    }

    bool hasValue = false;
    ushort value;
}

struct Cell {
    ushort dat;
    bool free = true;
}

Cell[0x10000] cells;

void setMem( ushort addr, ushort dat ) {
    if( !cells[addr].free ) {
        throw new Exception(format("ERROR: Address 0x%04X not free", addr));
    }
    cells[addr].free = false;
    cells[addr].dat = dat;
}

int assemble( CmdBase[] cmd ) {
    ushort pc = 0;
    string[] undefined;
    // first pass
    foreach( c; cmd ) {
        writefln("0x%04X: %s", pc, c);
    
        final switch( c.type ) {
            case CmdBase.Type.Org: {
                pc = (cast(CmdOrg)c).getAddress();
                break;
            }
            case CmdBase.Type.Ds: {
                CmdDs ds = cast(CmdDs)c;
                ushort size = ds.getSize();
                ds.locateAddr = pc;
                pc += size;
                break;
            }
            case CmdBase.Type.Dw: {
                CmdDw dw = cast(CmdDw)c;
                ushort size = dw.getSize();
                dw.locateAddr = pc;
                pc += size;
                break;
            }
            case CmdBase.Type.Equ: {
                CmdEqu equ = cast(CmdEqu)c;
                string name = equ.getName().toUpperCase;
                ushort value = equ.getValue();
                dictIdentifier[name] = value;
                break;
            }
            case CmdBase.Type.Number: {
                CmdNumber num = cast(CmdNumber)c;
                num.locateAddr = pc;
                pc++;
                break;
            }
            case CmdBase.Type.Expression: {
                Token t = c.tokens[0];
                throw new Exception(format("ERROR: %s:%s Expressions not supported yet.", t.line, t.col ));
            }
            case CmdBase.Type.String: {
                Token t = c.tokens[0];
                throw new Exception(format("ERROR: %s:%s  Whats the string doing here??", t.line, t.col ));
            }
            case CmdBase.Type.Label: {
                CmdLabel label = cast(CmdLabel)c;
                label.locateAddr = pc;
                dictIdentifier[label.getName().toUpperCase] = pc;
                break;
            }
            case CmdBase.Type.Identifier: {
                CmdIdentifier ident = cast(CmdIdentifier)c;
                ident.locateAddr = pc;
                pc++;
                string name = ident.getName().toUpperCase;
                if( name in dictIdentifier ) {
                    writeln(name, ": ", dictIdentifier[name]);
                } else {
                    writeln(name, " not known yet.");
                }
                break;
            }
            case CmdBase.Type.Comment: break;
            case CmdBase.Type.Base: {
                Token t = c.tokens[0];
                throw new Exception(format("ERROR: %s:%s What is this???", t.line, t.col ));
            }
        } // switch
    } // foreach

    // looking for unknown identifiers
    foreach( ud; undefined ) {
        if( ud in dictIdentifier ) {
            undefined.remove(ud);
        }
    }
    if( undefined.length > 0 ) {
        foreach( ud; undefined ) {
            writeln(ud);
        }
        throw new Exception("ERROR: Undefined, aborting.");
    }

    writeln(dictIdentifier);

    // sort by address
    sort!((a,b) => a.locateAddr < b.locateAddr)(cmd);

    writeln("\nAssembling:");
    // fill memory
    foreach( c; cmd ) {
        writeln(c);
        writef("0x%04X: ", c.locateAddr);
        switch( c.type ) {
            case CmdBase.Type.Dw: {
                CmdDw dw = cast(CmdDw)c;
                writeln(dw);
                ushort[] data = dw.getData();
                for( ushort p = 0; p < data.length; p++ ) {
                    setMem( cast(ushort)(dw.locateAddr + p), data[p] );
                    writef("%04X ", data[p]);
                }
                writeln();
                break;
            }
            case CmdBase.Type.Number: {
                CmdNumber num = cast(CmdNumber)c;
                writefln("%04X", num.getValue());
                setMem( num.locateAddr, num.getValue() );
                break;
            }
            case CmdBase.Type.Identifier: {
                CmdIdentifier ident = cast(CmdIdentifier)c;
                ushort value = dictIdentifier[ident.getName().toUpperCase] ;
                writefln("%04X ", value);
                setMem( ident.locateAddr, value );
                break;
            }
            default: {}
        }
    }


    return 0;
}

int main(string[] args)
{
    if( args.length < 2 ) {
        writeln("Missing argument");
        return 1;
    }
    Lexer lexer = new Lexer(args[1]);

    dictIdentifier = [
        "DUP": 0xA110, "SWAP": 0x80A0, "DROP": 0xC120, "JMPZ": 0xE650, "JMPL": 0xE750, "JMP": 0xC150,
        "CALL": 0xC190, "RET": 0x9050, "PUSHRS": 0xC100, "DROPRS": 0x9120, "POPRS": 0xB010, "LOAD": 0x8420,
        "STORE": 0xE860, "PUSHPC": 0xA210, "PUSHSP": 0xA310, "POPSP": 0x8140, "ADD": 0xC530, "ADC": 0x8581,
        "SUB": 0xC538, "SBC": 0x8589, "AND": 0xC532, "OR": 0xC533, "XOR": 0xC534, "INV": 0x8525,
        "LSL": 0xC536, "LSR": 0xC537
    ];

    CmdBase[] commands;

    CmdBase cmd = null;
    foreach( t; lexer.tokens ) {
        if( cmd !is  null ) {
            CmdBase.Result res = cmd.add(t);
            final switch(res) {
                case CmdBase.Result.Error: writeln("Error!"); return 1;
                case CmdBase.Result.Done:
                    commands ~= cmd;
                    //writeln(cmd.toString());
                    cmd = null;
                    break;
                case CmdBase.Result.Next: break;
            }
        } else {
            if( t.type == Token.Type.Directive ) {
                const string directive = t.cargo.toUpperCase;
                if( directive == ".ORG" ) {
                    cmd = new CmdOrg(t);
                } else if( directive == ".EQU" ) {
                    cmd = new CmdEqu(t);
                } else if( directive == ".DS" ) {
                    cmd = new CmdDs(t);
                } else if( directive == ".DW" ) {
                    cmd = new CmdDw(t);
                }
            } else if( t.type == Token.Type.Label ) {
                auto cmdlabel = new CmdLabel(t);
                commands ~= cmdlabel;
                //writeln(cmdlabel.toString());
            } else if ( [Token.Type.Number, Token.Type.Hexnumber].canFind(t.type) ) {
                auto cmdnumber = new CmdNumber(t);
                commands ~= cmdnumber;
                //writeln(cmdnumber);
            } else if ( t.type == Token.Type.Identifier ) {
                    auto cmdidentifier = new CmdIdentifier(t);
                    commands ~= cmdidentifier;
                    //writeln(cmdidentifier);
            } else if ( [Token.Type.Comment, Token.Type.Newline, Token.Type.Ws].canFind(t.type) ) {
                // do nothing
            } else {
                throw new Exception(format("ERROR: %s:%s Unexpected whatever", t.line, t.col ));
            }
        }
    }

    assemble(commands);

    return 0;
}
