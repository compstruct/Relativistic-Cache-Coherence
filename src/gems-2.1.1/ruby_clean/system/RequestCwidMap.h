#ifndef REQUESTCWIDMAP_H
#define REQUESTCWIDMAP_H

#include "Address.h"
#include "AbstractChip.h"
#include <map>
#include <queue>
#include <utility>

typedef std::map< Address, std::queue< std::pair<int, int> > > AddressReqCwidQueueMap;

class RequestCwidMap {
public:
    RequestCwidMap(AbstractChip* chip_ptr);
    ~RequestCwidMap();
    void printConfig(ostream& out);
    void print(ostream& out) const;
    void push(Address addr, int cta_id, int wid);
    int returnTopCtaID(Address addr);
    int returnTopWarpID(Address addr);
    void pop(Address addr);

private:
    AddressReqCwidQueueMap addr_cwid_queue_map;
    AbstractChip* m_chip_ptr;

};

// ******************* Definitions *******************

// Output operator definition
extern inline
ostream& operator<<(ostream& out, const RequestCwidMap& obj)
{
  obj.print(out);
  out << flush;
  return out;
}


// ****************************************************************

#endif 
