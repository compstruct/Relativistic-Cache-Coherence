#include "RubySlicc_RequestRecorder_interface.h"
#include "Global.h"
#include "System.h"

void request_recorder_allocate(int req_id, Address req_addr, Time req_issue_pts, Time req_issue_rts) {
    if (g_REQUEST_RECORDER_ENABLED)
        g_system_ptr->getRequestRecorder()->allocate(req_id, req_addr, req_issue_pts, req_issue_rts);
}

void request_recorder_set_reply_ts(int req_id, Time req_reply_pts, Time req_reply_rts, bool hit_in_L1, bool can_renew) {
    if (g_REQUEST_RECORDER_ENABLED)
        g_system_ptr->getRequestRecorder()->set_reply_ts(req_id, req_reply_pts, req_reply_rts, hit_in_L1, can_renew);
}
