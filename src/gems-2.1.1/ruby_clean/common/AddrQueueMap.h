#ifndef ADDRQUEUEMAP_H
#define ADDRQUEUEMAP_H

#include <queue>
#include <map>
#include "Address.h"

class AddrQueueMap {
public:
    AddrQueueMap() {};
    ~AddrQueueMap() {};
    void push(Address addr, int id) { m_queues[addr].push(id); }
    int pop(Address addr) { 
	assert(m_queues.count(addr) > 0);
	assert(!m_queues[addr].empty());
        int result = m_queues[addr].front(); 
	m_queues[addr].pop();
        if (m_queues[addr].empty())
	    m_queues.erase(addr);	
	
	return result; }

private:
    std::map< Address, std::queue<int> > m_queues;
};

// Output operator declaration
ostream& operator<<(ostream& out, const AddrQueueMap& obj);

// ******************* Definitions *******************

// Output operator definition
extern inline
ostream& operator<<(ostream& out, const AddrQueueMap& obj)
{
  //obj.print(out);
  out << flush;
  return out;
}

#endif 
