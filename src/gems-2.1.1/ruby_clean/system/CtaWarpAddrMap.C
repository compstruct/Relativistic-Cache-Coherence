#include "CtaWarpAddrMap.h"

CtaWarpAddrMap::CtaWarpAddrMap(AbstractChip* chip_ptr) {
    m_chip_ptr = chip_ptr;
}

CtaWarpAddrMap::~CtaWarpAddrMap() {
}

void 
CtaWarpAddrMap::add(int cta_id, int wid, Address addr) {
    cta_warp_addr_map[cta_id][wid].insert(addr);
}

void
CtaWarpAddrMap::discard(Address addr) {
    for (std::map<int, WarpAddrMap>::iterator it1 = cta_warp_addr_map.begin(); it1 != cta_warp_addr_map.end(); it1++) {
         int cta_id = it1->first;
	 WarpAddrMap warp_addr_map = it1->second;
	 for (WarpAddrMap::iterator it2 = warp_addr_map.begin(); it2 != warp_addr_map.end(); it2++) {
	    int warp_id = it2->first;
	    AddrSet addr_set = it2->second;
	    if (addr_set.count(addr) != 0) {
	        addr_set.erase(addr);
		if (addr_set.empty()) {
		    warp_addr_map.erase(warp_id);
		    if (warp_addr_map.empty()) {
		        cta_warp_addr_map.erase(cta_id);
		    }
		}
	    }
	}
    }
}

WarpAddrMap
CtaWarpAddrMap::getWarpAddrMap(int cta_id) {
    assert(cta_warp_addr_map.count(cta_id) != 0);
    return cta_warp_addr_map[cta_id];
}

int
CtaWarpAddrMap::count(int cta_id) {
    return cta_warp_addr_map.count(cta_id);
}

void
CtaWarpAddrMap::printConfig(ostream& out) {
    out << "L1CtaWarpAddrMap" << endl;
}

