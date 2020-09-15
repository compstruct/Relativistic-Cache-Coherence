#include "RubySlicc_Util.h"
#include "RubySlicc_Util2.h"
#include "Chip.h"
#include "protocol_name.h"
#include "RubySlicc_ComponentMapping.h"
#include "AccessPermission.h"

// Ideally invalidate address in all L1s
void ideal_inv_all_L1s(NodeID chip_id, Address addr) {
    assert(chip_id < RubyConfig::numberOfChips());
    for(int i=0; i<RubyConfig::numberOfProcsPerChip(); i++) {
        Sequencer* targetSequencer_ptr = g_system_ptr->getChip(chip_id)->getSequencer(i);
        targetSequencer_ptr->makeIdealInvRequest(
                CacheMsg( addr,
                   addr,
                   CacheRequestType_IDEAL_INV,
                   Address(0),    // PC
                   AccessModeType_UserMode,   // User/supervisor mode
                   0,   // Size in bytes of request
                   PrefetchBit_No, // Not a prefetch
                   0,              // Version number
                   addr,   // Virtual Address
                   0,              // SMT thread
		   0,
		   0,
		   0,
                   0,          // TM specific - timestamp of memory request
                   false,      // TM specific - whether request is part of escape action
                   MemorySpaceType_NULL,    // memory space
                   false,                    // profiled yet?
                   0,         // access mask
                   0                  // memfetch pointer
                   )
                );
    }
}


// Ideally invalidate address in sharer L1s
void ideal_inv_sharer_L1s(NodeID chip_id, Address addr, NetDest Sharers) {
    assert(chip_id < RubyConfig::numberOfChips());
    while(Sharers.count() > 0) {
        MachineID L1Machine = Sharers.smallestElement(MachineType_L1Cache);
        int core_id = machineIDToVersion(L1Machine);
        assert(core_id < RubyConfig::numberOfL1CachePerChip());
        Sequencer* targetSequencer_ptr = g_system_ptr->getChip(chip_id)->getSequencer(core_id);
        targetSequencer_ptr->makeIdealInvRequest(
            CacheMsg( addr,
               addr,
               CacheRequestType_IDEAL_INV,
               Address(0),    // PC
               AccessModeType_UserMode,   // User/supervisor mode
               0,   // Size in bytes of request
               PrefetchBit_No, // Not a prefetch
               0,              // Version number
               addr,   // Virtual Address
               0,              // SMT thread
	       0,
	       0,
	       0,
               0,          // TM specific - timestamp of memory request
               false,      // TM specific - whether request is part of escape action
               MemorySpaceType_NULL,    // memory space
               false,                    // profiled yet?
               0,         // access mask
               0                  // memfetch pointer
               )
        );

        Sharers.remove(L1Machine);
    }
}


// Ideally invalidate address in all sharers except requester
void ideal_inv_sharer_L1s_minus_requester(NodeID chip_id, Address addr, NetDest Sharers, MachineID requester) {
    Sharers.remove(requester);
    ideal_inv_sharer_L1s(chip_id, addr, Sharers);
}


// Check if protocol is ideal coherence
bool protocol_is_ideal_coherence() {
    // Check that first part of protocol name is 'ideal_coherence'
    return (strncmp(CURRENT_PROTOCOL, "ideal_coherence", 15) == 0);
}

// Check if benchmark contains memory fences
bool benchmark_contains_membar() { return BENCHMARK_CONTAINS_MEMBAR; }

// Get a list of true L1 sharers (by probing the L1s)
NetDest get_true_sharers(NodeID chip_id, Address addr) {
    NetDest true_sharers;
    assert(chip_id < RubyConfig::numberOfChips());
    for(int i=0; i<RubyConfig::numberOfProcsPerChip(); i++) {
        if(g_system_ptr->getChip(chip_id)->getL1DCache(i)->isTagPresent(addr))
            true_sharers.add(getL1MachineID(i));
    }
    return true_sharers;
}

//Following functions are for L2 response dealy queue
bool is_delay_queue_enabled() {
    return g_DELAY_QUEUE_ENABLED;
}

bool is_scheduler_stalled(int core_id) {
    return g_system_ptr->is_scheduler_stalled(core_id);
}

void inc_delay_counters(int core_id) {
    g_system_ptr->inc_delay_counters(core_id);
}

int get_delay_counters(int core_id) {
    return g_system_ptr->get_delay_counters(core_id);
}

int delay_queue_transitions_per_cycle() {
    return DELAY_QUEUE_TRANSITIONS_PER_RUBY_CYCLE;
}

int delay_queue_threshold() {
    return DELAY_QUEUE_THRESHOLD;
}

//For Tardis Oracle Predictor
bool can_be_renewed(NodeID chip_id, Address addr, Time l1_rts) {
    assert(chip_id < RubyConfig::numberOfChips());
    Chip* n = dynamic_cast<Chip*>(g_system_ptr->getChip(chip_id));
    MachineID l2_mach = map_L2ChipId_to_L2Cache(addr, n->getID());
    int l2_ver = l2_mach.num%RubyConfig::numberOfL2CachePerChip();
    assert(n->m_L2Cache_L2cacheMemory_vec[l2_ver] != NULL);
    bool in_l2 = n->m_L2Cache_L2cacheMemory_vec[l2_ver]->isTagPresent(addr);
    if (in_l2 && n->m_L2Cache_L2cacheMemory_vec[l2_ver]->getPermission(addr) == AccessPermission_Read_Write) {
        Time l2_wts = n->m_L2Cache_L2cacheMemory_vec[l2_ver]->getWts(addr);
	if (time_to_int(l2_wts) < time_to_int(l1_rts)) {
	    return true;
	} else {
	    return false;
	} 
    } else {
        return false; 
    }
}

Time ideally_extend_rts(NodeID chip_id, Address addr, Time pts) {
    assert(chip_id < RubyConfig::numberOfChips());
    Chip* n = dynamic_cast<Chip*>(g_system_ptr->getChip(chip_id));
    MachineID l2_mach = map_L2ChipId_to_L2Cache(addr, n->getID());
    int l2_ver = l2_mach.num%RubyConfig::numberOfL2CachePerChip();
    assert(n->m_L2Cache_L2cacheMemory_vec[l2_ver] != NULL);
    assert(n->m_L2Cache_L2cacheMemory_vec[l2_ver]->isTagPresent(addr));
    assert(n->m_L2Cache_L2cacheMemory_vec[l2_ver]->getPermission(addr) == AccessPermission_Read_Write);
    Time l2_rts = n->m_L2Cache_L2cacheMemory_vec[l2_ver]->getRts(addr);
    n->m_L2Cache_L2cacheMemory_vec[l2_ver]->setRts(addr, getTimeMax(l2_rts, getTimePlusTime(pts, int_to_time(CL_FIXED_LEASE))));
    l2_rts = n->m_L2Cache_L2cacheMemory_vec[l2_ver]->getRts(addr);
    return l2_rts;
}

bool read_only_check_enabled() {
    return g_system_ptr->read_only_check_enabled();
}

bool is_readonly(int addr) {
    return g_system_ptr->is_readonly(addr);
}

bool is_private(int addr) {
    return g_system_ptr->is_private(addr);
}
