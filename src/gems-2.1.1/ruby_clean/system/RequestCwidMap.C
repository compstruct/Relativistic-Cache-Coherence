#include "RequestCwidMap.h"

RequestCwidMap::RequestCwidMap(AbstractChip* chip_ptr) { m_chip_ptr = chip_ptr; }
RequestCwidMap::~RequestCwidMap() {}

void
RequestCwidMap::push(Address addr, int cta_id, int wid) {
    addr_cwid_queue_map[addr].push(std::make_pair(cta_id, wid));
}

int
RequestCwidMap::returnTopCtaID(Address addr) {
    assert(addr_cwid_queue_map[addr].size() > 0);
    return addr_cwid_queue_map[addr].front().first;
}

int
RequestCwidMap::returnTopWarpID(Address addr) {
    assert(addr_cwid_queue_map[addr].size() > 0);
    return addr_cwid_queue_map[addr].front().second;
}

void 
RequestCwidMap::pop(Address addr) {
    assert(addr_cwid_queue_map[addr].size() > 0);
    addr_cwid_queue_map[addr].pop();
    if (addr_cwid_queue_map[addr].size() == 0) {
        addr_cwid_queue_map.erase(addr);
    }
}

void 
RequestCwidMap::printConfig(ostream& out) {
    out << "L1RequestCwidMap" << endl;
}
