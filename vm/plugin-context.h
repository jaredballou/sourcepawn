// vim: set sts=2 ts=8 sw=2 tw=99 et:
// 
// Copyright (C) 2006-2015 AlliedModders LLC
// 
// This file is part of SourcePawn. SourcePawn is free software: you can
// redistribute it and/or modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation, either version 3 of
// the License, or (at your option) any later version.
//
// You should have received a copy of the GNU General Public License along with
// SourcePawn. If not, see http://www.gnu.org/licenses/.
//
#ifndef _INCLUDE_SOURCEPAWN_V1CONTEXT_H_
#define _INCLUDE_SOURCEPAWN_V1CONTEXT_H_

#include "base-context.h"
#include "scripted-invoker.h"
#include "plugin-runtime.h"

namespace sp {

struct HeapTracker
{
  HeapTracker()
   : size(0),
     pBase(nullptr),
     pCur(nullptr)
  {}
  size_t size; 
  ucell_t *pBase; 
  ucell_t *pCur;
};

static const size_t SP_MAX_RETURN_STACK = 1024;

class Environment;
class PluginContext;

class PluginContext : public BasePluginContext
{
 public:
  PluginContext(PluginRuntime *pRuntime);
  ~PluginContext() override;

  bool Initialize();

 public: //IPluginContext
  int HeapAlloc(unsigned int cells, cell_t *local_addr, cell_t **phys_addr);
  int HeapPop(cell_t local_addr);
  int HeapRelease(cell_t local_addr);
  int FindNativeByName(const char *name, uint32_t *index);
  int GetNativeByIndex(uint32_t index, sp_native_t **native);
  uint32_t GetNativesNum();
  int FindPublicByName(const char *name, uint32_t *index);
  int GetPublicByIndex(uint32_t index, sp_public_t **publicptr);
  uint32_t GetPublicsNum();
  int GetPubvarByIndex(uint32_t index, sp_pubvar_t **pubvar);
  int FindPubvarByName(const char *name, uint32_t *index);
  int GetPubvarAddrs(uint32_t index, cell_t *local_addr, cell_t **phys_addr);
  uint32_t GetPubVarsNum();
  int LocalToPhysAddr(cell_t local_addr, cell_t **phys_addr);
  int LocalToString(cell_t local_addr, char **addr);
  int StringToLocal(cell_t local_addr, size_t chars, const char *source);
  int StringToLocalUTF8(cell_t local_addr, size_t maxbytes, const char *source, size_t *wrtnbytes);
  IPluginFunction *GetFunctionByName(const char *public_name);
  IPluginFunction *GetFunctionById(funcid_t func_id);
  cell_t *GetNullRef(SP_NULL_TYPE type);
  int LocalToStringNULL(cell_t local_addr, char **addr);
  IPluginRuntime *GetRuntime();
  cell_t *GetLocalParams();

  bool Invoke(funcid_t fnid, const cell_t *params, unsigned int num_params, cell_t *result);

  size_t HeapSize() const {
    return mem_size_;
  }
  uint8_t *memory() const {
    return memory_;
  }
  size_t DataSize() const {
    return data_size_;
  }
  PluginRuntime *runtime() const {
    return m_pRuntime;
  }

 public:
  bool IsInExec();

  static inline size_t offsetOfTracker() {
    return offsetof(PluginContext, tracker_);
  }
  static inline size_t offsetOfSp() {
    return offsetof(PluginContext, sp_);
  }
  static inline size_t offsetOfRuntime() {
    return offsetof(PluginContext, m_pRuntime);
  }
  static inline size_t offsetOfMemory() {
    return offsetof(PluginContext, memory_);
  }

  int32_t *addressOfSp() {
    return &sp_;
  }
  cell_t *addressOfFrm() {
    return &frm_;
  }
  cell_t *addressOfHp() {
    return &hp_;
  }

  cell_t frm() const {
    return frm_;
  }
  cell_t hp() const {
    return hp_;
  }

  int popTrackerAndSetHeap();
  int pushTracker(uint32_t amount);

  int generateArray(cell_t dims, cell_t *stk, bool autozero);
  int generateFullArray(uint32_t argc, cell_t *argv, int autozero);

  inline bool checkAddress(cell_t *stk, cell_t addr) {
    if (uint32_t(addr) >= mem_size_)
      return false;

    if (addr < hp_)
      return true;

    if (reinterpret_cast<cell_t *>(memory_ + addr) < stk)
      return false;

    return true;
  }

 private:
  PluginRuntime *m_pRuntime;
  uint8_t *memory_;
  uint32_t data_size_;
  uint32_t mem_size_;

  cell_t *m_pNullVec;
  cell_t *m_pNullString;

  // Tracker for local HEA growth.
  HeapTracker tracker_;

  // Stack, heap, and frame pointer.
  cell_t sp_;
  cell_t hp_;
  cell_t frm_;
};

} // namespace sp

#endif //_INCLUDE_SOURCEPAWN_V1CONTEXT_H_
