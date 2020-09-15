#include "RequestRecorder.h"

RequestRecorder::RequestRecorder() {
    if (g_REQUEST_RECORDER_ENABLED) {
        FILE * outfile = fopen("mem_pts_stats.txt", "a+");
        fprintf(outfile, "%-10s%-18s%-18s%-18s%-18s%-18s%-8s%-12s\n", "req_id", "addr", "issue_pts", "issue_rts", "reply_pts", "reply_rts", "L1_hit", "can_renew");
        fflush(outfile);
        fclose(outfile);
    }
}

RequestRecorder::~RequestRecorder() {}

void 
RequestRecorder::allocate(int req_id, Address req_addr, Time req_issue_pts, Time req_issue_rts) {
    RequestRecorderEntry rre;
    rre.request_addr = req_addr;
    rre.request_issue_pts = req_issue_pts;
    rre.request_issue_rts = req_issue_rts;
    rre.request_reply_pts = 0;
    rre.request_reply_rts = 0;
    rre.hit_in_L1 = "Miss";
    rre.can_renew = "No";
    m_requests[req_id] = rre; 

}

void RequestRecorder::set_reply_ts(int req_id, Time req_reply_pts, Time req_reply_rts, bool hit_in_L1, bool can_renew) {
    assert(m_requests.count(req_id) > 0);
    m_requests[req_id].request_reply_pts = req_reply_pts;
    m_requests[req_id].request_reply_rts = req_reply_rts;
    m_requests[req_id].hit_in_L1 = hit_in_L1 ? "Hit" : "Miss";
    m_requests[req_id].can_renew = can_renew ? "Yes" : "No";
}

void
RequestRecorder::dump() {
    if (g_REQUEST_RECORDER_ENABLED) {
        FILE * outfile = fopen("mem_pts_stats.txt", "a+");
        for (std::map<int , RequestRecorderEntry>::iterator iter = m_requests.begin(); 
             iter != m_requests.end();
             ++iter) {
            int req_id = iter->first;
            RequestRecorderEntry rre =  iter->second;
            fprintf(outfile, "%-10d", req_id);
            fprintf(outfile, "%-18lld", rre.request_addr.getAddress());
            fprintf(outfile, "%-18lld", rre.request_issue_pts);
            fprintf(outfile, "%-18lld", rre.request_issue_rts);
            fprintf(outfile, "%-18lld", rre.request_reply_pts);
            fprintf(outfile, "%-18lld", rre.request_reply_rts);
	    fprintf(outfile, "%-8s",    rre.hit_in_L1.c_str());
	    fprintf(outfile, "%-12s\n", rre.can_renew.c_str());
        }
        fflush(outfile);
        fclose(outfile);
        m_requests.clear();
    }
}

void 
RequestRecorder::printConfig(ostream& out) const {
    out << "RequestRecorder" << endl;
}
