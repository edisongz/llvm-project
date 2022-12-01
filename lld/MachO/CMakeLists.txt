set(LLVM_TARGET_DEFINITIONS Options.td)
tablegen(LLVM Options.inc -gen-opt-parser-defs)
add_public_tablegen_target(MachOOptionsTableGen)

include_directories(${LLVM_MAIN_SRC_DIR}/../libunwind/include)

# use onetbb for concurrent symtab
add_library(libtbb STATIC IMPORTED)
set_target_properties(libtbb PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/../lib/libtbb.a)

# use mimalloc for create huge number of input files
add_library(libmimalloc STATIC IMPORTED)
set_target_properties(libmimalloc PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/../lib/libmimalloc.a)

add_lld_library(lldMachO
  Arch/ARM.cpp
  Arch/ARM64.cpp
  Arch/ARM64Common.cpp
  Arch/ARM64_32.cpp
  Arch/X86_64.cpp
  ConcatOutputSection.cpp
  Driver.cpp
  DriverUtils.cpp
  Dwarf.cpp
  EhFrame.cpp
  ExportTrie.cpp
  ICF.cpp
  InputFiles.cpp
  InputSection.cpp
  LTO.cpp
  MapFile.cpp
  MarkLive.cpp
  ObjC.cpp
  OutputSection.cpp
  OutputSegment.cpp
  Relocations.cpp
  SectionPriorities.cpp
  SymbolTable.cpp
  Symbols.cpp
  SyntheticSections.cpp
  Target.cpp
  UnwindInfoSection.cpp
  Writer.cpp

  LINK_COMPONENTS
  ${LLVM_TARGETS_TO_BUILD}
  BinaryFormat
  BitReader
  Core
  DebugInfoDWARF
  Demangle
  LTO
  MC
  ObjCARCOpts
  Object
  Option
  Passes
  Support
  TextAPI

  LINK_LIBS
  lldCommon
  ${LLVM_PTHREAD_LIB}
  ${XAR_LIB}

  DEPENDS
  MachOOptionsTableGen
  ${tablegen_deps}
  )

if(LLVM_HAVE_LIBXAR)
  target_link_libraries(lldMachO PRIVATE ${XAR_LIB} libtbb libmimalloc)
endif()
