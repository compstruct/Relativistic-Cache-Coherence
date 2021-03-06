
/*
    Copyright (C) 1999-2008 by Mark D. Hill and David A. Wood for the
    Wisconsin Multifacet Project.  Contact: gems@cs.wisc.edu
    http://www.cs.wisc.edu/gems/

    --------------------------------------------------------------------

    This file is part of the Ruby Multiprocessor Memory System Simulator,
    a component of the Multifacet GEMS (General Execution-driven
    Multiprocessor Simulator) software toolset originally developed at
    the University of Wisconsin-Madison.

    Ruby was originally developed primarily by Milo Martin and Daniel
    Sorin with contributions from Ross Dickson, Carl Mauer, and Manoj
    Plakal.

    Substantial further development of Multifacet GEMS at the
    University of Wisconsin was performed by Alaa Alameldeen, Brad
    Beckmann, Jayaram Bobba, Ross Dickson, Dan Gibson, Pacia Harper,
    Derek Hower, Milo Martin, Michael Marty, Carl Mauer, Michelle Moravan,
    Kevin Moore, Andrew Phelps, Manoj Plakal, Daniel Sorin, Haris Volos,
    Min Xu, and Luke Yen.
    --------------------------------------------------------------------

    If your use of this software contributes to a published paper, we
    request that you (1) cite our summary paper that appears on our
    website (http://www.cs.wisc.edu/gems/) and (2) e-mail a citation
    for your published paper to gems@cs.wisc.edu.

    If you redistribute derivatives of this software, we request that
    you notify us and either (1) ask people to register with us at our
    website (http://www.cs.wisc.edu/gems/) or (2) collect registration
    information and periodically send it to us.

    --------------------------------------------------------------------

    Multifacet GEMS is free software; you can redistribute it and/or
    modify it under the terms of version 2 of the GNU General Public
    License as published by the Free Software Foundation.

    Multifacet GEMS is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with the Multifacet GEMS; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
    02111-1307, USA

    The GNU General Public License is contained in the file LICENSE.

### END HEADER ###
*/

/*
 * System.C
 *
 * Description: See System.h
 *
 * $Id$
 *
 */


#include "System.h"
#include "Profiler.h"
#include "TCpredictor.h"
#include "LifetimeTrace.h"
#include "Network.h"
#include "Tester.h"
#include "GpusimInterface.h"
#include "SyntheticDriver.h"
#include "DeterministicDriver.h"
#include "Chip.h"
#include "Tracer.h"
#include "Protocol.h"

int Network::CONTROL_MESSAGE_SIZE = 8;
int Network::DATA_MESSAGE_SIZE = (64+8);

share_oracle::share_oracle(std::string filename) {
    unsigned addr;
    std::string sharing_type;

    std::ifstream file(filename.c_str());
    while (file.good()) {
        file >> addr >> sharing_type;
        if (!file.fail()) {
            if (sharing_type == "PV") {
                pv_blocks.insert(addr);
            } else if (sharing_type == "RO") {
                ro_blocks.insert(addr);
            } else if (sharing_type == "WO") {
                wo_blocks.insert(addr);
            } else if (sharing_type == "AO") {
                ao_blocks.insert(addr);
            } else if (sharing_type == "RW") {
                rw_blocks.insert(addr);
            } else {
                std::cerr << "PANIC: unknown sharing type " << sharing_type << " for address " << addr << std::endl;
            }
        }
    }
    file.close();
}

System::System()
{
  DEBUG_MSG(SYSTEM_COMP, MedPrio,"initializing");

  m_driver_ptr = NULL;
  m_profiler_ptr = new Profiler;

  m_TCpredictorContainer_ptr = new TCpredictorContainer(RubyConfig::numberOfL2Cache());

  m_LifetimeTrace_ptr = new LifetimeTrace();

  // NETWORK INITIALIZATION
  // create the network by calling a function that calls new
  m_network_ptr = Network::createNetwork(RubyConfig::numberOfChips());

  // Set the data message size
  Network::DATA_MESSAGE_SIZE = g_DATA_MESSAGE_SIZE;

  printf("CONTROL_MESSAGE_SIZE=%d\n", Network::CONTROL_MESSAGE_SIZE);
  printf("DATA_MESSAGE_SIZE=%d\n", Network::DATA_MESSAGE_SIZE);

  DEBUG_MSG(SYSTEM_COMP, MedPrio,"Constructed network");

  // CHIP INITIALIZATION
  m_chip_vector.setSize(RubyConfig::numberOfChips());// create the vector of pointers to processors
  for(int i=0; i<RubyConfig::numberOfChips(); i++) { // for each chip
    // create the chip
    m_chip_vector[i] = new Chip(i, m_network_ptr);
    DEBUG_MSG(SYSTEM_COMP, MedPrio,"Constructed a chip");
  }

  // These must be after the chips are constructed

  if (!g_SIMICS) {
    if (g_SYNTHETIC_DRIVER && !g_DETERMINISTIC_DRIVER) {
      m_driver_ptr = new SyntheticDriver(this);
    } else if (!g_SYNTHETIC_DRIVER && g_DETERMINISTIC_DRIVER) {
      m_driver_ptr = new DeterministicDriver(this);
    } else if (g_SYNTHETIC_DRIVER && g_DETERMINISTIC_DRIVER) {
      ERROR_MSG("SYNTHETIC and DETERMINISTIC DRIVERS are exclusive and cannot be both enabled");
    } else {
      // normally make tester object, otherwise make an opal interface object.
      if (1) {
        m_driver_ptr = new GpusimInterface(this);
      } else if (1) {
        m_driver_ptr = new Tester(this);
      } else {
          assert(0);
          //         m_driver_ptr = new OpalInterface(this);
      }
    }
  }

  m_tracer_ptr = new Tracer;

  if (XACT_MEMORY) {
      assert(0);
      /*
      m_xact_isolation_checker = new XactIsolationChecker;
      m_xact_commit_arbiter    = new XactCommitArbiter;
      m_xact_visualizer        = new XactVisualizer;
      */
  }

  m_request_recorder_ptr = new RequestRecorder();

  DEBUG_MSG(SYSTEM_COMP, MedPrio,"finished initializing");
  DEBUG_NEWLINE(SYSTEM_COMP, MedPrio);

  scheduler_stalled.resize(RubyConfig::numberOfL1Cache(), false);
  delay_counters.resize(RubyConfig::numberOfL1Cache(), 0);

  if (read_only_check_enabled()) {
      assert(READ_ONLY_TRACE_FILE != NULL);  
      m_share_oracle = new share_oracle(READ_ONLY_TRACE_FILE);
  }
}

System::~System()
{
  for (int i = 0; i < m_chip_vector.size(); i++) {
    delete m_chip_vector[i];
  }
  delete m_driver_ptr;
  delete m_network_ptr;
  delete m_profiler_ptr;
  delete m_tracer_ptr;
  delete m_TCpredictorContainer_ptr;
  delete m_LifetimeTrace_ptr;
  delete m_request_recorder_ptr;
  if (read_only_check_enabled())
      delete m_share_oracle;
}

void System::printConfig(ostream& out) const
{
  out << "\n================ Begin System Configuration Print ================\n\n";
  RubyConfig::printConfiguration(out);
  out << endl;
  getChip(0)->printConfig(out);
  m_network_ptr->printConfig(out);
  m_driver_ptr->printConfig(out);
  m_profiler_ptr->printConfig(out);
  m_TCpredictorContainer_ptr->printConfig(out);
  m_LifetimeTrace_ptr->printConfig(out);
  out << "\n================ End System Configuration Print ================\n\n";
}

void System::printStats(ostream& out)
{
  const time_t T = time(NULL);
  tm *localTime = localtime(&T);
  char buf[100];
  strftime(buf, 100, "%b/%d/%Y %H:%M:%S", localTime);

  out << "Real time: " << buf << endl;

  m_profiler_ptr->printStats(out);
  // Print mandatory queue message buffer stats
  for(int i=0; i<RubyConfig::numberOfChips(); i++) { // for each chip
    for(int p=0; p<RubyConfig::numberOfProcsPerChip(); p++) {
      m_chip_vector[i]->m_L1Cache_mandatoryQueue_vec[p]->printStats(out);
    }
  }
  // Print idealInv queue message buffer stats
  for(int i=0; i<RubyConfig::numberOfChips(); i++) { // for each chip
    if(m_chip_vector[i]->m_L1Cache_idealInvQueue_vec.size() > 0) {
      for(int p=0; p<RubyConfig::numberOfProcsPerChip(); p++) {
        m_chip_vector[i]->m_L1Cache_idealInvQueue_vec[p]->printStats(out);
      }
    }
  }
  m_network_ptr->printStats(out);
  m_driver_ptr->printStats(out);
  Chip::printStats(out);
  for(int i=0; i<RubyConfig::numberOfChips(); i++) { // for each chip
     m_chip_vector[i]->printBufferStats(out);
  }
  m_TCpredictorContainer_ptr->printStats(out);
  m_LifetimeTrace_ptr->printStats(out);
  m_request_recorder_ptr->dump();
}

void System::clearStats() const
{
  m_profiler_ptr->clearStats();
  m_network_ptr->clearStats();
  m_driver_ptr->clearStats();
  Chip::clearStats();
  for(int i=0; i<RubyConfig::numberOfChips(); i++) { // for each chip
    for(int p=0; p<RubyConfig::numberOfProcsPerChip(); p++) {
      m_chip_vector[i]->m_L1Cache_mandatoryQueue_vec[p]->clearStats();
      if(m_chip_vector[i]->m_L1Cache_idealInvQueue_vec.size() > 0)
          m_chip_vector[i]->m_L1Cache_idealInvQueue_vec[p]->clearStats();
    }
  }
  m_TCpredictorContainer_ptr->clearStats();
  m_LifetimeTrace_ptr->clearStats();
}

void System::recordCacheContents(CacheRecorder& tr) const
{
  for (int i = 0; i < m_chip_vector.size(); i++) {
    for (int m_version = 0; m_version < RubyConfig::numberOfProcsPerChip(); m_version++) {
      if (Protocol::m_TwoLevelCache) {
        m_chip_vector[i]->m_L1Cache_L1IcacheMemory_vec[m_version]->setAsInstructionCache(true);
        m_chip_vector[i]->m_L1Cache_L1DcacheMemory_vec[m_version]->setAsInstructionCache(false);
      } else {
        m_chip_vector[i]->m_L1Cache_cacheMemory_vec[m_version]->setAsInstructionCache(false);
      }
    }
    m_chip_vector[i]->recordCacheContents(tr);
  }
}

void System::opalLoadNotify()
{
    #if 0
    if (OpalInterface::isOpalLoaded()) {
    // change the driver pointer to point to an opal driver
    delete m_driver_ptr;
    m_driver_ptr  = new OpalInterface(this);
  }
    #endif
}

#ifdef CHECK_COHERENCE
// This code will check for cases if the given cache block is exclusive in
// one node and shared in another-- a coherence violation
//
// To use, the SLICC specification must call sequencer.checkCoherence(address)
// when the controller changes to a state with new permissions.  Do this
// in setState.  The SLICC spec must also define methods "isBlockShared"
// and "isBlockExclusive" that are specific to that protocol
//
void System::checkGlobalCoherenceInvariant(const Address& addr  )  {

  NodeID exclusive = -1;
  bool sharedDetected = false;
  NodeID lastShared = -1;

  for (int i = 0; i < m_chip_vector.size(); i++) {

    if (m_chip_vector[i]->isBlockExclusive(addr)) {
      if (exclusive != -1) {
        // coherence violation
        WARN_EXPR(exclusive);
        WARN_EXPR(m_chip_vector[i]->getID());
        WARN_EXPR(addr);
        WARN_EXPR(g_eventQueue_ptr->getTime());
        ERROR_MSG("Coherence Violation Detected -- 2 exclusive chips");
      }
      else if (sharedDetected) {
        WARN_EXPR(lastShared);
        WARN_EXPR(m_chip_vector[i]->getID());
        WARN_EXPR(addr);
        WARN_EXPR(g_eventQueue_ptr->getTime());
        ERROR_MSG("Coherence Violation Detected -- exclusive chip with >=1 shared");
      }
      else {
        exclusive = m_chip_vector[i]->getID();
      }
    }
    else if (m_chip_vector[i]->isBlockShared(addr)) {
      sharedDetected = true;
      lastShared = m_chip_vector[i]->getID();

      if (exclusive != -1) {
        WARN_EXPR(lastShared);
        WARN_EXPR(exclusive);
        WARN_EXPR(addr);
        WARN_EXPR(g_eventQueue_ptr->getTime());
        ERROR_MSG("Coherence Violation Detected -- exclusive chip with >=1 shared");
      }
    }
  }
}
#endif




