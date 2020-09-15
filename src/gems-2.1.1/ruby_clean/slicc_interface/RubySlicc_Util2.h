#include "Global.h"
#include "Address.h"
#include "NodeID.h"
#include "Sequencer.h"
#include "RubyConfig.h"
#include "NetDest.h"

class Chip;


// Ideally invalidate address in all L1s
void ideal_inv_all_L1s(NodeID chip_id, Address addr);

// Ideally invalidate address in sharer L1s
void ideal_inv_sharer_L1s(NodeID chip_id, Address addr, NetDest Sharers);

// Ideally invalidate address in all sharers except requester
void ideal_inv_sharer_L1s_minus_requester(NodeID chip_id, Address addr, NetDest Sharers, MachineID requester);

// Check if protocol is ideal coherence
bool protocol_is_ideal_coherence();

// Check if benchmark contains memory fences
bool benchmark_contains_membar();

// Get a list of true L1 sharers (by probing the L1s)
NetDest get_true_sharers(NodeID chip_id, Address addr);

//For delay queue of L2 resposne
bool is_delay_queue_enabled();
bool is_scheduler_stalled(int core_id);
void inc_delay_counters(int core_id);
int get_delay_counters(int core_id);
int delay_queue_transitions_per_cycle();
int delay_queue_threshold();

//For Tardis Oracle Predictor
bool can_be_renewed(NodeID chip_id, Address addr, Time l1_rts);
Time ideally_extend_rts(NodeID chip_id, Address addr, Time pts);

bool read_only_check_enabled();
bool is_readonly(int addr);
bool is_private(int addr);
