#ifndef RUBYSLICC_REQUESTRECORDER_INTERFACE_H
#define RUBYSLICC_REQUESTRECORDER_INTERFACE_H

#include "Global.h"
#include "Address.h"
#include "RequestRecorder.h"

void request_recorder_allocate(int req_id, Address req_addr, Time req_issue_pts, Time req_issue_rts);
void request_recorder_set_reply_ts(int req_id, Time req_reply_pts, Time req_reply_rts, bool hit_in_L1, bool can_renew);

#endif // RUBYSLICC_TCPREDICTOR_INTERFACE_H
