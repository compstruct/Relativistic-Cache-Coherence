#ifndef STOREQUEUEMAP_H
#define STOREQUEUEMAP_H

#include "Global.h"
#include "Address.h"
#include "DataBlock.h"
#include "AbstractChip.h"
#include <map>
#include <queue>

typedef std::queue<DataBlock> StoreQueue;

class StoreQueueMap {

public:
    StoreQueueMap(AbstractChip* chip_ptr);
    ~StoreQueueMap();
    void printConfig(ostream& out);
    void print(ostream& out) const;
    void add(Address addr, DataBlock DataBlk);
    DataBlock return_queue_top(Address addr);
    void discard(Address addr);

private:
    std::map<Address, StoreQueue*> store_queue_map;
    AbstractChip* m_chip_ptr;

};


// ******************* Definitions *******************

// Output operator definition
extern inline
ostream& operator<<(ostream& out, const StoreQueueMap& obj)
{
  obj.print(out);
  out << flush;
  return out;
}


// ****************************************************************

#endif
