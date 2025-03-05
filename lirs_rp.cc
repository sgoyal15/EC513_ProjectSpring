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
 * 2. Redistributions in binary form must reproduce the above copyright notice,
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

#include "mem/cache/replacement_policies/lirs_rp.hh"

#include <cassert>
#include <memory>

#include "params/LIRSRP.hh"
#include "sim/cur_tick.hh"


namespace gem5
{

namespace replacement_policy
{

LIRS::LIRS(const Params &p)
  : Base(p)
{
}

void
LIRS::invalidate(const std::shared_ptr<ReplacementData>& replacement_data)
{
    // Reset state to default
    std::static_pointer_cast<LIRSReplData>(replacement_data)->bt = LIR;
    std::static_pointer_cast<LIRSReplData>(replacement_data)->replacement_data = nullptr;
    std::static_pointer_cast<LIRSReplData>(replacement_data)->resident = 0;
}

void
LIRS::touch(const std::shared_ptr<ReplacementData>& replacement_data) const 
{
  //should not use this function
  panic("LIRS requires access to data structures");
}
void
LIRS::touch(const std::shared_ptr<ReplacementData>& replacement_data,const PacketPtr pkt){
  //cast data to make using helper functions easier
  std::shared_ptr<LIRSReplData> dataCast = std::static_pointer_cast<LIRSReplData>(replacement_data);
  LIRSReplData data;
  data.bt = dataCast->bt;
  data.resident = dataCast->resident;
  data.replacement_data = dataCast -> replacement_data;

  //assumed hit already

  //checking block type
  if(data.bt == LIR){
    //if the last block in stack is the replacement_data, stackPrune()
    LIRSReplData lastInS = S.back();
    if(lastInS.replacement_data == data.replacement_data){
      stackPrune();
    }
    //move replacement data to begining of stack
    S.erase(S.begin() + findEntryS(data));
    S.insert(S.begin(),data);
  }
  else if(data.bt == HIR){
    //replacement data is type resident

    //search for data in stack
    int isInS = findEntryS(data);
    if(isInS){

      //data is in stack -> set to LIR take off of queue
      data.bt = LIR;
      Q.erase(Q.begin() + findEntryQ(data));

      //check block type of back of stack
      if(S.back().bt == LIR){

	//back of stack is LIR -> push to queue
	Q.push_back(S.back());

	//check if queue is full
	if(Q.size() > MAXQUEUESIZE){

	  //hold front of queue
	  int stackfind = findEntryS(Q.front());

	  //evict from queue and set to non-resident
	  Q.erase(Q.begin());
	  S[stackfind].resident = 0;
	}
	stackPrune();
      }
    }
    else{

      //move data to back of queue
      Q.erase(Q.begin() + findEntryQ(data));
      Q.push_back(data);

      //check queue size
      if(Q.size() > MAXQUEUESIZE){

	//queue is full-> hold front of queue and evict -> set to non-resident
	int stackfind = findEntryS(Q.front());
	Q.erase(Q.begin());
	S[stackfind].resident = 0;
      }
    }

    //move data to front of stack
    S.erase(S.begin() + findEntryS(data));
    S.insert(S.begin(),data);
  }
}

void
LIRS::reset(const std::shared_ptr<ReplacementData>& replacement_data)const{
  panic("LIRS requires access to data structures");
}
void
LIRS::reset(const std::shared_ptr<ReplacementData>& replacement_data,const PacketPtr pkt) 
{

  //cast data 
  std::shared_ptr<LIRSReplData> dataCast = std::static_pointer_cast<LIRSReplData>(replacement_data);
  LIRSReplData data;
  data.bt = dataCast->bt;
  data.resident = dataCast->resident;
  data.replacement_data = dataCast->replacement_data;

  //is stack greater than MAXLIRSSIZE
  if(S.size() >= MAXLIRSSIZE){

    //check queue
    if(Q.size() >= MAXQUEUESIZE){

      //queue is full 
      if(findEntryS(data)){

	//change data to LIR
	data.bt = LIR;

	//check bottom of stack for LIR
	if(S.back().bt == LIR){

	  //change to HIR and add to queue
	  S.back().bt = HIR;
	  Q.push_back(S.back());
	  stackPrune();
	}
      }
    }
    else{

      //change data to HIR and place in queue
      data.bt = HIR;
      Q.insert(Q.begin(),data);
    }
    //add data to front of stack
    S.insert(S.begin(),data);
  }
  else{
    //change data to LIR and place in front of stack
    data.bt = LIR;
    S.insert(S.begin(),data);
  }
}

ReplaceableEntry*
LIRS::getVictim(const ReplacementCandidates& candidates) const
{
  //very bad lucky guess implementation DOES NOT WORK
    // There must be at least one replacement candidate
    assert(candidates.size() > 0);
    ReplaceableEntry* victim = nullptr;
    for(const auto& candidate:candidates){
      if(std::static_pointer_cast<LIRSReplData>(candidate->replacementData) == Q[0].replacement_data){
	//got lucky, eviction target is in candidates
	victim = candidate;
	break;
      }
    }
    if(victim == nullptr){
      for(const auto& candidate:candidates){
	if(std::static_pointer_cast<LIRSReplData>(candidate->replacementData)->bt == HIR){
	  //got unlucky, maybe evict HIR?
	  victim = candidate;
	}
      }
    }
    else{
      //return got lucky
      return victim;
    }
    //got unlucky just return first candidate
    return candidates[0];
}
  
std::shared_ptr<ReplacementData>
LIRS::instantiateEntry()
{
    return std::shared_ptr<ReplacementData>(new LIRSReplData());
}



void
LIRS::stackPrune()  {

  //remove from stack until back of stack is LIR
  while(S.back().bt == HIR){
    S.pop_back();
  }
}

int
LIRS::findEntryS(LIRSReplData data){

  //look for data in stack
  for(int i = 0; i < S.size() -1 ;i++){
    if(S[i].replacement_data == data.replacement_data){
      return i;
    }
  }
  return -1;
}

int
LIRS::findEntryQ(LIRSReplData data){

  //look for data in queue
  for(int i = 0;i<Q.size() -1; i++){
    if(Q[i].replacement_data = data.replacement_data){
      return i;
    }
  }
  return -1;
}
  
}//rep policy
}//gem5
 
