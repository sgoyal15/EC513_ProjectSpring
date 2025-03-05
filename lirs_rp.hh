/*
 * Copyright (c) 2013-2015 Advanced Micro Devices, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reprodce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __MEM_CACHE_REPLACEMENT_POLICIES_LIRS_RP_HH__
#define __MEM_CACHE_REPLACEMENT_POLICIES_LIRS_RP_HH__
#define MAXQUEUESIZE 1024
#define MAXLIRSSIZE (1<<20) - (MAXQUEUESIZE)

#include <memory>

#include "base/types.hh"
#include "mem/cache/replacement_policies/base.hh"
namespace gem5
{

struct LIRSRPParams;

namespace replacement_policy
{

class LIRS : public Base
{
  protected:
    /** LIRS-specific implementation of replacement data. */

  //enumeration of the two main block types
  typedef enum{
	       HIR,
	       LIR
  }block_type;

  //local replacement data struct 
  struct LIRSReplData : ReplacementData
    {
        
      block_type bt;
      bool resident;
      std::shared_ptr<ReplacementData> replacement_data;
        /**
         * Default constructor. Invalidate data.
         */
      LIRSReplData() : bt(LIR),resident(1),replacement_data(nullptr) {}
    };

  public:
    typedef LIRSRPParams Params;
    LIRS(const Params &p);
    ~LIRS() = default;

  //queue and stack vectors
  std::vector<LIRSReplData> Q;
  std::vector<LIRSReplData> S;

  //helper functions
  void stackPrune();
  int findEntryS(LIRSReplData data);
  int findEntryQ(LIRSReplData data);

  
  /*  *
     * @param replacement_data Replacement data to be invalidated.
     */
    void invalidate(const std::shared_ptr<ReplacementData>& replacement_data)
                                                                    override;

    /**
     * Touch an entry to update its replacement data.
     * Sets its last touch tick as the current tick.
     *
     * @param replacement_data Replacement data to be touched.
     */
  void touch(const std::shared_ptr<ReplacementData>& replacement_data) const
                                                                     override;
  void touch(const std::shared_ptr<ReplacementData>& replacement_data, const PacketPtr pkt)override;
    /**
     * Reset replacement data. Used when an entry is inserted.
     * Sets its last touch tick as the current tick.
     *
     * @param replacement_data Replacement data to be reset.
     */
    void reset(const std::shared_ptr<ReplacementData>& replacement_data) const override;
  void reset(const std::shared_ptr<ReplacementData>& replacement_data, const PacketPtr pkt)override;
    /**
     * Find replacement victim using LRU timestamps.
     *
     * @param candidates Replacement candidates, selected by indexing policy.
     * @return Replacement entry to be replaced.
     */
    ReplaceableEntry* getVictim(const ReplacementCandidates& candidates) const
                                                                     override;

    /**
     * Instantiate a replacement data entry.
     *
     * @return A shared pointer to the new replacement data.
     */
    std::shared_ptr<ReplacementData> instantiateEntry() override;
};

}
}


#endif // __MEM_CACHE_REPLACEMENT_POLICIES_LIRS_RP_HH__
