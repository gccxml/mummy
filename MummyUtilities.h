//----------------------------------------------------------------------------
// MummyUtilities.h
//
// Author(s) : David Cole
//
// Copyright (C) 2006-2007 Kitware, Inc.
//----------------------------------------------------------------------------

#ifndef MummyUtilities_h
#define MummyUtilities_h

// Suppress nearly meaningless warnings:
//
#ifdef _MSC_VER
#pragma warning(disable:4127)
  // 4127 is suppressed because it warns on every single stl container iteration
  // loop that the "conditional expression is constant"...
#endif

#include "cableContext.h"
#include "cxxFundamentalType.h"

#include "gxsys/ios/iosfwd"
#include "gxsys/stl/string"

extern const std::string ANNOTATION_TEXT;

namespace cable
{
  class Class;
  class Constructor;
  class FunctionType;
  class Type;
}

void Trace(const char *s);
void Emit(gxsys_ios::ostream &os, const char *s);
void EmitInt(gxsys_ios::ostream &os, const int i);
void EmitUint(gxsys_ios::ostream &os, const unsigned int i);
void EmitIndent(gxsys_ios::ostream &os, const unsigned int n = 1);
void EmitFile(gxsys_ios::ostream &os, const char *filename);
void WriteToFile(const char *filename, const char *s);
bool IsChar(const cable::Type *t);
bool IsFundamental(const cable::Type *t, cxx::FundamentalType::Id tid);
bool IsObject(const cable::Type *t);
bool IsVoid(const cable::Type *t);
bool HasMapToType(const cable::Type *t);
gxsys_stl::string GetMapToType(const cable::Type *t);
gxsys_stl::string GetStringMethod(const cable::Type *t);
bool IsCharPointer(const cable::Type *t);
bool IsCharPointerPointer(const cable::Type *t);
bool IsFundamentalPointer(const cable::Type *t, cxx::FundamentalType::Id tid);
bool IsFundamentalPointerPointer(const cable::Type *t, cxx::FundamentalType::Id tid);
bool IsObjectPointer(const cable::Type *t);
bool IsObjectPointerReference(const cable::Type *t);
bool IsVoidPointer(const cable::Type *t);
const char *GetAccessString(cable::Context::Access access);
const cable::Class *GetParentClass(const cable::Class *c);
bool ClassIsA(const cable::Class *c, const gxsys_stl::string& parent);
bool ValidateBaseClasses(const cable::Class *c);
gxsys_stl::string GetSimpleName(const cable::Named *n);
gxsys_stl::string GetFullyQualifiedName(const cable::Named *n, const char *sep);
gxsys_stl::string GetFullyQualifiedNameForCPlusPlus(const cable::Named *n);
gxsys_stl::string GetFullyQualifiedNameForCSharp(const cable::Named *n);
gxsys_stl::string GetFullyQualifiedCPlusPlusTypeIdName(const cable::Named *n);
bool EquivalentTypedefNameExists(const cable::Class* c, const cable::FunctionType *target, gxsys_stl::string& s);
bool HasAttribute(const cable::SourceObject *o, const char *attr);
bool IsUtilityClass(const cable::Class *c);
gxsys_stl::string ExtractAttribute(const gxsys_stl::string& atts, const gxsys_stl::string& attBase);
gxsys_stl::string ExtractAttribute(const cable::SourceObject *o, const gxsys_stl::string& attBase);
gxsys_stl::string ExtractArraySize(const gxsys_stl::string& atts);
gxsys_stl::string ExtractImplementsInterface(const gxsys_stl::string& atts);
gxsys_stl::string ExtractMapToType(const cable::SourceObject *o);
gxsys_stl::string ExtractStringMethod(const cable::SourceObject *o);
gxsys_stl::string GetMappedTypeName(const cable::Class *c, bool fullyQualified);
gxsys_stl::string GetWrappedClassName(const cable::Class *c);
gxsys_stl::string GetWrappedClassNameFullyQualified(const cable::Class *c);
void SuppressMsg(const int n);
bool ShouldLogMsg(const int n);
void LogMsg(const gxsys_stl::string& file, const unsigned long line, const gxsys_stl::string& label, const int n);
int GetErrorCount();
int GetFirstErrorValue();
int GetNthErrorValue(int n);
int GetMostRecentErrorValue();
const cable::Constructor* FindNonAbstractPublicDefaultConstructor(const cable::Class *c);
gxsys_stl::string GetCPlusPlusZeroInitializerExpression(const cable::Type *t);
gxsys_stl::string GetCsharpZeroInitializerExpression(const cable::Type *t);
bool BlockContains(const gxsys_stl::vector<gxsys_stl::string>& block, const char *value);
bool ShouldEmitComment(const char *comment);
gxsys_stl::string EncodeStringForXml(const char *s);
void EmitDocumentationBlock(gxsys_ios::ostream &os, const gxsys_stl::vector<gxsys_stl::string>& block, const unsigned int indent, bool isClassDoc = false);
gxsys_stl::string ExtractDerivedName(const char *s, const cable::Named *n, bool verbose);
bool HasAnnotation(const cable::SourceObject *o, const gxsys_stl::string& tag);

#endif
