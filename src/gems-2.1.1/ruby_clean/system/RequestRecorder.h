#ifndef STOREQUEUEMAP_H
#define STOREQUEUEMAP_H

#include "Global.h"
#include "Address.h"
#include "AbstractChip.h"
#include <map>
#include <queue>

struct RequestRecorderEntry {
    Address request_addr;
    Time request_issue_pts;
    Time request_issue_rts;
    Time request_reply_pts;
    Time request_reply_rts;
    std::string hit_in_L1;
    std::string can_renew;
};

class RequestRecorder {

public:
    RequestRecorder();
    ~RequestRecorder();
    void printConfig(ostream& out) const;
    void print(ostream& out) const;
    void allocate(int req_id, Address req_addr, Time req_issue_pts, Time req_issue_rts);
    void set_reply_ts(int req_id, Time req_reply_pts, Time req_reply_rts, bool hit_in_L1, bool can_renew);
    void dump();

private:
    std::map<int, RequestRecorderEntry> m_requests;

};


// ******************* Definitions *******************

// Output operator definition
extern inline
ostream& operator<<(ostream& out, const RequestRecorder& obj)
{
  obj.print(out);
  out << flush;
  return out;
}


// ****************************************************************

#endif
