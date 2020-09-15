#ifndef CTAWARPADDRMAP_H
#define CTAWARPADDRMAP_H

#include "Global.h"
#include "Address.h"
#include "AbstractChip.h"
#include <map>
#include <set>

typedef std::set<Address> AddrSet;
typedef std::map<int, AddrSet> WarpAddrMap;

class CtaWarpAddrMap {

public:
    CtaWarpAddrMap(AbstractChip* chip_ptr);
    ~CtaWarpAddrMap();
    void printConfig(ostream& out);
    void print(ostream& out) const;
    void add(int cta_id, int wid, Address addr);
    void discard(Address addr);
    WarpAddrMap getWarpAddrMap(int cta_id);
    int count(int cta_id);

private:
    std::map<int, WarpAddrMap> cta_warp_addr_map;
    AbstractChip* m_chip_ptr;

};


// ******************* Definitions *******************

// Output operator definition
extern inline
ostream& operator<<(ostream& out, const CtaWarpAddrMap& obj)
{
  obj.print(out);
  out << flush;
  return out;
}


// ****************************************************************

#endif
