//===- SymbolTable.h --------------------------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLD_MACHO_SYMBOL_TABLE_H
#define LLD_MACHO_SYMBOL_TABLE_H

#include "Symbols.h"

#include "lld/Common/LLVM.h"
#include "llvm/ADT/CachedHashString.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/Object/Archive.h"
#include "llvm/Support/RWMutex.h"

#include <tbb/concurrent_hash_map.h>

namespace lld::macho {

class ArchiveFile;
class DylibFile;
class InputFile;
class ObjFile;
class InputSection;
class MachHeaderSection;
class Symbol;
class Defined;
class Undefined;

class HashCmp {
public:
  static size_t hash(const llvm::CachedHashStringRef &s) { return s.hash(); }

  static bool equal(const llvm::CachedHashStringRef &s1,
                    const llvm::CachedHashStringRef &s2) {
    return s1.hash() == s2.hash() && s1.size() == s2.size() &&
           (s1.data() == s2.data() ||
            memcmp(s1.data(), s2.data(), s1.size()) == 0);
  }
};

using SymbolMap =
    tbb::concurrent_hash_map<llvm::CachedHashStringRef, Symbol *, HashCmp>;

/*
 * Note that the SymbolTable handles name collisions by calling
 * replaceSymbol(), which does an in-place update of the Symbol via `placement
 * new`. Therefore, there is no need to update any relocations that hold
 * pointers the "old" Symbol -- they will automatically point to the new one.
 */
class SymbolTable {
public:
  Defined *addDefined(StringRef name, InputFile *, InputSection *,
                      uint64_t value, uint64_t size, bool isWeakDef,
                      bool isPrivateExtern, bool isThumb,
                      bool isReferencedDynamically, bool noDeadStrip,
                      bool isWeakDefCanBeHidden);
  Defined *addDefinedEager(StringRef name, InputFile *, InputSection *,
                           uint64_t value, uint64_t size, bool isWeakDef,
                           bool isPrivateExtern, bool isThumb,
                           bool isReferencedDynamically, bool noDeadStrip,
                           bool isWeakDefCanBeHidden);

  Defined *aliasDefined(Defined *src, StringRef target, InputFile *newFile,
                        bool makePrivateExtern = false);

  Symbol *addUndefined(StringRef name, InputFile *, bool isWeakRef);
  Symbol *addUndefinedEager(StringRef name, InputFile *, bool isWeakRef);
  void markLive(StringRef name, InputFile *file);

  Symbol *addCommon(StringRef name, InputFile *, uint64_t size, uint32_t align,
                    bool isPrivateExtern);
  Symbol *addCommonEager(StringRef name, InputFile *, uint64_t size,
                         uint32_t align, bool isPrivateExtern);

  Symbol *addDylib(StringRef name, DylibFile *file, bool isWeakDef, bool isTlv);
  Symbol *addDynamicLookup(StringRef name);

  Symbol *addLazyArchive(StringRef name, ArchiveFile *file,
                         const llvm::object::Archive::Symbol &sym);
  Symbol *addLazyObjFile(StringRef name, InputFile *file);
  Symbol *addLazyObject(StringRef name, InputFile &file);

  Defined *addSynthetic(StringRef name, InputSection *, uint64_t value,
                        bool isPrivateExtern, bool includeInSymtab,
                        bool referencedDynamically);

  constexpr SymbolMap &getSymbols() { return symMap; }
  Symbol *find(llvm::CachedHashStringRef name);
  Symbol *find(StringRef name) { return find(llvm::CachedHashStringRef(name)); }

private:
  std::pair<Symbol *, bool> insert(StringRef name, const InputFile *);
  tbb::concurrent_hash_map<llvm::CachedHashStringRef, Symbol *, HashCmp> symMap;
};

void reportPendingUndefinedSymbols();
void reportPendingDuplicateSymbols();

// Call reportPendingUndefinedSymbols() to emit diagnostics.
void treatUndefinedSymbol(const Undefined &, StringRef source);
void treatUndefinedSymbol(const Undefined &, const InputSection *,
                          uint64_t offset);

extern std::unique_ptr<SymbolTable> symtab;

} // namespace lld::macho

#endif
