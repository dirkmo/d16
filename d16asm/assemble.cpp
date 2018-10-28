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
    // add label/equ reference to identifiers
    if( base->type == CmdBase::Ident ) {
        CmdIdentifier *id = static_cast<CmdIdentifier*>(base);
        CmdBase *ref = mapIdent.at(id->name);
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

static void collectUpperRefs( list<CmdReference*>& list ) {
    for( auto l: cmdlist ) {
        if( (l->type == CmdBase::Label && l->addr >= 0x8000) ||
            (l->type == CmdBase::Equ   && l->value >= 0x8000) ) {
            list.push_back(static_cast<CmdLabel*>(l));
        }
    }
}

int assemble( list<CmdBase*> _lst ) {
    cmdlist = _lst;
    firstPass();
    // sort by address
    cmdlist.sort( []( const CmdBase *a, const CmdBase *b ) { return a->addr < b->addr; } );
    for( auto c: cmdlist ) {
        printf("%04X: %s\n", c->addr, c->getString().c_str());
    }
    // collect labels and Equ's in upper memory region (>0x7FFF). References to these labels
    // are 2 bytes long.
    
    cout << endl << "References to upper area:" << endl;
    list<CmdReference*> upperRefs;
    collectUpperRefs( upperRefs );
    for( auto l: upperRefs ) {
        cout << l->name << endl;
    }
    
    cout << endl;
    // performing pass to correct reference sizes
    pass();
    for( auto c: cmdlist ) {
        printf("%04X: %s\n", c->addr, c->getString().c_str());
    }

    return 0;
}
