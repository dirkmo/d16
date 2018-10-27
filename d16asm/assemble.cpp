#include "assemble.h"
#include <iostream>

typedef map<string,CmdReference*> IdentMap;
static list<CmdBase*> cmdlist;
static IdentMap mapIdent;
static uint16_t pc = 0;

static void defineIdentifier(CmdBase *base) {
    if( base->type != CmdBase::Label && base->type != CmdBase::Equ ) {
        cout << "ERROR: Not an identifier definition " << base->name << endl;
        return;
    }
    if( mapIdent.find(base->name) == mapIdent.end() ) {
        mapIdent[base->name] = static_cast<CmdReference*>(base);
    } else {
        cout << "ERROR: Multiple definition of identifier " << base->name << endl;
        exit(1);
    }
}

static void addReference( CmdBase *base ) {
    if( base->type == CmdBase::Ident ) {
        CmdIdentifier *id = static_cast<CmdIdentifier*>(base);
        CmdBase *ref = mapIdent.at(id->name);
        id->setReference(ref);
    }
}

static void firstPass() {
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
                defineIdentifier(equ);
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
                defineIdentifier(label);
                break;
            }
            default: {
                cout << "ERROR: Unknown Type " << c->type << endl;
            }
        }
    }
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

int assemble( list<CmdBase*> _lst ) {
    cmdlist = _lst;
    firstPass();
    cmdlist.sort( []( const CmdBase *a, const CmdBase *b ) { return a->addr < b->addr; } );
    for( auto c: cmdlist ) {
        printf("%04X: %s\n", c->addr, c->getString().c_str());
    }

    return 0;
}
