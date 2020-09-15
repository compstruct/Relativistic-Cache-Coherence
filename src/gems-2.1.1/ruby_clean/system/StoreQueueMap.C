#include "StoreQueueMap.h"

StoreQueueMap::StoreQueueMap(AbstractChip* chip_ptr) {
    //store_queue_map = new std::map<Address, StoreQueue>;
    m_chip_ptr = chip_ptr;
}

StoreQueueMap::~StoreQueueMap() {
    //delete store_queue_map;
}

void 
StoreQueueMap::add(Address addr, DataBlock Datablk) {
    if (store_queue_map.count(addr) == 0) {
        store_queue_map[addr] = new StoreQueue();
    }
    store_queue_map[addr]->push(Datablk);
}

DataBlock
StoreQueueMap::return_queue_top(Address addr) {
    assert(store_queue_map.count(addr) > 0);
    DataBlock DataBlk = store_queue_map[addr]->front();
    discard(addr);
    return DataBlk;
}

void
StoreQueueMap::discard(Address addr) {
    assert(store_queue_map.count(addr) > 0);
    store_queue_map[addr]->pop();
    if (store_queue_map[addr]->size() == 0) {
        delete store_queue_map[addr];
	store_queue_map.erase(addr);
    }
}

void
StoreQueueMap::printConfig(ostream& out) {
    out << "L1StoreQueue" << endl;
}

