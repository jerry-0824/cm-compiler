/*
 * Copyright (c) 2018, Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

//===----------------------------------------------------------------------===//
//
/// GenXSubtarget : subtarget information
/// -------------------------------------
/// 
/// GenXSubtarget is the GenX-specific subclass of TargetSubtargetInfo. It takes
/// features detected by the front end (what the Gen architecture is; what size
/// SVM pointers are), and exposes flags to the rest of the GenX backend for
/// various features (e.g. whether 64 bit operations are supported).
///
/// Where subtarget features are used is noted in the documentation of GenX
/// backend passes.
///
/// The flags exposed to the rest of the GenX backend are as follows. Most of
/// these are currently not used.
///
//===----------------------------------------------------------------------===//

#ifndef GENXSUBTARGET_H
#define GENXSUBTARGET_H

#include "llvm/ADT/StringSwitch.h"
#include "llvm/ADT/Triple.h"
#include "llvm/IR/CallingConv.h"
#include "llvm/Pass.h"
#include "llvm/CodeGen/TargetSubtargetInfo.h"
#include <string>

#define GET_SUBTARGETINFO_HEADER
#define GET_SUBTARGETINFO_ENUM
#include "GenXGenSubtargetInfo.inc"

namespace llvm {
class GlobalValue;
class StringRef;
class TargetMachine;

class GenXSubtarget final : public GenXGenSubtargetInfo {

protected:
  // TargetTriple - What processor and OS we're targeting.
  Triple TargetTriple;

  enum GenXTag {
    GENX_GENERIC,
    GENX_HSW,
    GENX_BDW,
    GENX_CHV,
    GENX_SKL,
    GENX_BXT,
    GENX_KBL,
    GENX_GLK,
    GENX_CNL,
    GENX_ICL,
    GENX_ICLLP,
  };

  // GenXVariant - GenX Tag identifying the variant to compile for
  GenXTag GenXVariant;

private:
  // DumpRegAlloc - True if we should dump register allocation information
  bool DumpRegAlloc;

  // SvmPtrIs64Bit - True if svmptr_t is 64 bit (otherwise it is 32 bit)
  bool SvmptrIs64Bit;

  // HasLongLong - True if subtarget supports long long type
  bool HasLongLong;

  // DisableJmpi - True if jmpi is disabled.
  bool DisableJmpi;

  // DisableVectorDecomposition - True if vector decomposition is disabled.
  bool DisableVectorDecomposition;

  // Only generate warning when callable is used in the middle of the kernel
  bool WarnCallable;


public:
  // This constructor initializes the data members to match that
  // of the specified triple.
  //
  GenXSubtarget(const Triple &TT, const std::string &CPU,
                const std::string &FS);

  // hasLongLong - true for Gen8+
  bool hasLongLong() { return HasLongLong; }

  // ParseSubtargetFeatures - Parses features string setting specified
  // subtarget options.  Definition of function is auto generated by tblgen.
  void ParseSubtargetFeatures(StringRef CPU, StringRef FS);

  // \brief Reset the features for the GenX target.
  void resetSubtargetFeatures(StringRef CPU, StringRef FS);

public:

  /// * isHSW - true if target is HSW
  bool isHSW() const { return GenXVariant == GENX_HSW; }

  /// * isBDW - true if target is BDW
  bool isBDW() const { return GenXVariant == GENX_BDW; }

  /// * isBDWplus - true if target is BDW or later
  bool isBDWplus() const { return GenXVariant >= GENX_BDW; }

  /// * isCHV - true if target is CHV
  bool isCHV() const { return GenXVariant == GENX_CHV; }

  /// * isSKL - true if target is SKL
  bool isSKL() const { return GenXVariant == GENX_SKL; }

  /// * isSKLplus - true if target is SKL or later
  bool isSKLplus() const { return GenXVariant >= GENX_SKL; }

  /// * isBXT - true if target is BXT
  bool isBXT() const { return GenXVariant == GENX_BXT; }


  /// * isKBL - true if target is KBL
  bool isKBL() const { return GenXVariant == GENX_KBL; }

  /// * isGLK - true if target is GLK
  bool isGLK() const { return GenXVariant == GENX_GLK; }

  /// * isCNL - true if target is CNL
  bool isCNL() const { return GenXVariant == GENX_CNL; }

  /// * isCNLplus - true if target is CNL or later
  bool isCNLplus() const { return GenXVariant >= GENX_CNL; }

  /// * isICL - true if target is ICL
  bool isICL() const { return GenXVariant == GENX_ICL; }

  /// * isICLLP - true if target is ICL LP
  bool isICLLP() const { return GenXVariant == GENX_ICLLP; }

  /// * dumpRegAlloc - true if we should dump Reg Alloc info
  bool dumpRegAlloc() const { return DumpRegAlloc; }

  /// * svmptrIs32Bit - true if svmptr_t is 32 bit
  bool svmptrIs32Bit() const { return !SvmptrIs64Bit; }

  /// * svmptrIs64Bit - true if svmptr_t is 64 bit
  bool svmptrIs64Bit() const { return SvmptrIs64Bit; }

  /// * hasLongLong - true if target supports long long
  bool hasLongLong() const { return HasLongLong; }

  /// * disableJmpi - true if jmpi is disabled.
  bool disableJmpi() const { return DisableJmpi; }

  /// * WaNoA32ByteScatteredStatelessMessages - true if there is no A32 byte
  ///   scatter stateless message.
  bool WaNoA32ByteScatteredStatelessMessages() const { return !isCNLplus(); }

  /// * disableVectorDecomposition - true if vector decomposition is disabled.
  bool disableVectorDecomposition() const { return DisableVectorDecomposition; }

  /// * warnCallable() - true if compiler only generate warning for 
  ///   callable in the middle
  bool warnCallable() const { return WarnCallable; }

  /// * hasIndirectGRFCrossing - true if target supports an indirect region
  ///   crossing one GRF boundary
  bool hasIndirectGRFCrossing() const { return isSKLplus(); }

  // Generic helper functions...
  const Triple &getTargetTriple() const { return TargetTriple; }

  bool isTargetDarwin() const { return TargetTriple.isOSDarwin(); }
  bool isTargetLinux() const { return TargetTriple.isOSLinux(); }

  bool isTargetWindowsMSVC() const {
    return TargetTriple.isWindowsMSVCEnvironment();
  }

  bool isTargetKnownWindowsMSVC() const {
    return TargetTriple.isKnownWindowsMSVCEnvironment();
  }

  bool isTargetWindowsCygwin() const {
    return TargetTriple.isWindowsCygwinEnvironment();
  }

  bool isTargetWindowsGNU() const {
    return TargetTriple.isWindowsGNUEnvironment();
  }

  bool isTargetCygMing() const { return TargetTriple.isOSCygMing(); }

  bool isOSWindows() const { return TargetTriple.isOSWindows(); }

};

class GenXSubtargetPass : public ImmutablePass {
  const GenXSubtarget *ST;
public:
  GenXSubtargetPass();
  GenXSubtargetPass(const GenXSubtarget &ST);
  ~GenXSubtargetPass();
  const GenXSubtarget *getSubtarget() const { return ST; }
  static char ID;
};

ImmutablePass *createGenXSubtargetPass(const GenXSubtarget &ST);

} // End llvm namespace

#endif
