#include "assemble.h"
#include <iostream>
#include <fstream>

typedef map<string,CmdReference*> ReferenceMap;
static ReferenceMap mapReferences;
static list<CmdBase*> cmdlist;
static uint16_t pc = 0;

static void defineIdentifier(CmdBase *base) {
    if( base->type != CmdBase::Label && base->type != CmdBase::Equ ) {
        cout << "ERROR: Not an identifier definition " << base->name << endl;
        return;
    }
    if( mapReferences.find(base->name) == mapReferences.end() ) {
        mapReferences[base->name] = static_cast<CmdReference*>(base);
    } else {
        cout << "ERROR: Multiple definition of identifier " << base->name << endl;
        exit(1);
    }
}

static void addReference( CmdBase *base ) {
    // add label/equ reference to identifiers
    if( base->type == CmdBase::Ident ) {
        CmdIdentifier *id = static_cast<CmdIdentifier*>(base);
        CmdBase *ref = mapReferences.at(id->name);
        id->setReference(ref);
    }
}

static void pass( bool first = false ) {
        for( auto c: cmdlist ) {
        switch( c->type ) {
            case CmdBase::Number: {
                CmdNumber* num = static_cast<CmdNumber*>(c);
                num->addr = pc;
                pc += (num->value < 0x8000) ? 1 : 2;
                break;
            }
            case CmdBase::Equ: {
                CmdEqu* equ = static_cast<CmdEqu*>(c);
                if( first ) {
                    defineIdentifier(equ);
                }
                break;
            }
            case CmdBase::Ds: {
                CmdDs* ds = static_cast<CmdDs*>(c);
                ds->addr = pc;
                pc += ds->size;
                break;
            }
            case CmdBase::Dw: {
                CmdDw* dw = static_cast<CmdDw*>(c);
                dw->addr = pc;
                pc += dw->getSize();
                break;
            }
            case CmdBase::Org: {
                CmdOrg* org = static_cast<CmdOrg*>(c);
                pc = org->addr;
                break;
            }
            case CmdBase::Keyword: {
                CmdKeyword* key = static_cast<CmdKeyword*>(c);
                key->addr = pc;
                pc++;
                break;
            }
            case CmdBase::Ident: {
                CmdIdentifier* id = static_cast<CmdIdentifier*>(c);
                id->addr = pc;
                pc += id->isExtended() ? 2 : 1;
                break;
            }
            case CmdBase::Label: {
                CmdLabel* label = static_cast<CmdLabel*>(c);
                label->addr = pc;
                if( first ) {
                    defineIdentifier(label);
                }
                break;
            }
            default: {
                cout << "ERROR: Unknown Type " << c->type << endl;
            }
        }
    }
}

static void firstPass() {
    pass(true);
    // now everything has an address
    // all identifiers need references to EQU definitions or labels
    for( auto id: cmdlist ) {
        addReference(id);
    }
    // check if every identifier now has a reference. if not, abort.
    for( auto it: cmdlist ) {
        if( it->type == CmdBase::Ident ) {
            auto id = static_cast<CmdIdentifier*>(it);
            if( !id->hasReference() ) {
                cout << "ERROR: " << id->name << " has no reference." << endl;
                exit(1);
            }
        }
    }
    // every identifier has a reference
}

static int countExtendedLabels() {
    int count = 0;
    for( auto c: cmdlist ) {
        if( c->type ==  CmdBase::Label ) {
            CmdLabel* l = static_cast<CmdLabel*>(c);
            count += l->isExtended() ? 1 : 0;
        }
    }
    return count;
}

void printReferences(ofstream& out) {
    out << "Symbols:" << endl;
    for( auto r: mapReferences ) {
        out << r.first << ": 0x" << r.second->addr << endl;
    }
}

int assemble( list<CmdBase*>& _lst, string fn ) {
    cmdlist = _lst;
    firstPass();
    // sort by address
    cmdlist.sort( []( const CmdBase *a, const CmdBase *b ) { return a->addr < b->addr; } );
    
    // performing passes to correct reference sizes until nothing changes anymore
    int last = 0;
    int count = countExtendedLabels();
    int passno = 2;
    while( count != last ) {
        cout << "Pass " << passno++ << endl;
        last = count;
        pass();
        count = countExtendedLabels();
    }

    // outputting files
    ofstream out(fn+".map");
    out << std::hex;
    printReferences(out);

    out << endl;

    out << "Program:" << endl;
    for( auto c: _lst ) {
        out << "0x" << c->addr << ": " << c->getString() << endl;
    }

    out << endl;

    out << "Program (sorted by address):" << endl;
    for( auto c: cmdlist ) {
        out << "0x" << c->addr << ": " << c->getString() << endl;
    }

    return 0;
}
