#ifndef _InlineWrappingHints_h
#define _InlineWrappingHints_h

// All inline wrapping hint macros are prefixed with "iwh"
//

// If being "compiled" by gccxml, use the gccxml attribute:
//
#ifdef __GCCXML__
#define IWH_USE_GCCXML_ATTRIBUTE
#endif

// Documentation on gccxml attributes can be found by running gccxml --help.
// Source for that documentation is in "GXFront/gxDocumentation.cxx"
//
#ifdef IWH_USE_GCCXML_ATTRIBUTE
#define gxa(x) __attribute((gccxml(#x)))
#else
#define gxa(x)
#endif

//----------------------------------------------------------------------------
/// <summary>
/// Array size hint. Used to indicate that a C++ pointer-to-fundamental-type
/// is a fixed-and-known-at-compile-time size. Parameter to this hint should
/// be the size of the array in number of elements.
/// The array size hint is only valid on method return types and on method
/// parameter types.
/// </summary>
#define iwhArraySize(s) gxa(iwhArraySize##s)

//----------------------------------------------------------------------------
/// <summary>
/// The "already ref counted" hint. Used to indicate that an object pointer
/// returned from a method is already ref counted and that an eventual
/// UnRegister call is required by the caller.
/// The counted hint is only valid on method return types.
/// </summary>
#define iwhCounted gxa(iwhCounted)

//----------------------------------------------------------------------------
/// <summary>
/// Method event hint. Used to indicate that a C++ method is an "event get"
/// method. Typically such a method returns a pointer to a nested event
/// instance. Methods marked with this hint get transformed into C# event
/// members.
/// This hint is only valid on method return types.
/// </summary>
#define iwhEvent gxa(iwhEvent)

//----------------------------------------------------------------------------
/// <summary>
/// Exclude from wrapping hint. Used to prevent an otherwise-wrappable method
/// from being wrapped.
/// This hint is only valid on methods.
/// </summary>
#define iwhExclude gxa(iwhExclude)

//----------------------------------------------------------------------------
/// <summary>
/// Derive the wrapped class from the named interface, if the named interface
/// is one that is known/has meaning in the target language. This is used,
/// for example, to impart derivation knowledge for IEnumerable to collection
/// classes and IEnumerator to enumerator classes in C#. The C++ class
/// definition must have methods in it that satisfy the interface
/// requirements when wrapped into the target language.
/// This hint is only valid on class types.
/// </summary>
#define iwhImplementsInterface(i) gxa(iwhImplementsInterface##i)

//----------------------------------------------------------------------------
/// <summary>
/// Map this class to the given type. For example, "iwhMapToType(string)" is
/// used to map C++ string-like classes into native C# strings.
/// This hint is only valid on class types.
/// </summary>
#define iwhMapToType(t) gxa(iwhMapToType##t)

//----------------------------------------------------------------------------
/// <summary>
/// Hint that indicates to suppress public field get/set generation. Used to
/// enable writing field accessors manually for C# structs.
/// This hint is only valid on class types that also have a iwhUtility hint.
/// </summary>
#define iwhNoFieldAccess gxa(iwhNoFieldAccess)

//----------------------------------------------------------------------------
/// <summary>
/// Hint that indicates to wrap a method as a "property get" if that notion
/// has meaning in the target language. If not, wrap as a regular method.
/// This hint is only valid on methods.
/// </summary>
#define iwhPropGet gxa(iwhPropGet)

//----------------------------------------------------------------------------
/// <summary>
/// Hint that indicates to wrap a method as a "property set" if that notion
/// has meaning in the target language. If not, wrap as a regular method.
/// This hint is only valid on methods.
/// </summary>
#define iwhPropSet gxa(iwhPropSet)

//----------------------------------------------------------------------------
/// <summary>
/// Call the named method on an instance of a C++ string-like class to get a
/// temporary "char*" that can be used to marshal a copy of the string to the
/// target wrapped language.
/// This hint is only valid on class types that also have a
/// iwhMapToType(string) hint.
/// </summary>
#define iwhStringMethod(m) gxa(iwhStringMethod##m)

//----------------------------------------------------------------------------
/// <summary>
/// Exists for testing the attribute mechanism without actually assigning an
/// attribute that means something to an entity.
/// </summary>
#define iwhTestIWH gxa(iwhTestIWH)

//----------------------------------------------------------------------------
/// <summary>
/// Designates a class as a "utility" class. Utility classes get wrapped into
/// C# as structs. Methods of such classes are not wrapped, only "plain old
/// data" data members.
/// The utility hint is only valid on class definitions.
/// </summary>
#define iwhUtility gxa(iwhUtility)


//----------------------------------------------------------------------------
// For now, disable this one warning when building the mummy "export layer"
// cxx files... C4190 is "C-linkage specified, but returns UDT which is
// incompatible with C". The docs for this warning say it doesn't matter as
// long as the caller is built with C++, but I'm not 100% certain how the
// PInvoke layer from the .NET side of the fence actually calls into the
// export layer when a class/struct is returned by value.
//
// Here are the Microsoft docs for this warning:
// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/vccore/html/C4190.asp
//
// But we need to use extern "C" to avoid the automatic name mangling that
// occurs when not using it... (So that the actual name of the export is the
// same as the name of the exported function, and the PInvoke/DllImport layer
// in C# can find it using that name.) So, the point is to avoid the name
// mangling, not to be able to call *all* of these functions from "C" code...
//
// So, the cause of 4190 is using *both* extern "C" and returning class
// instances by value. It's unclear to me at this point what the best
// permanent resolution for eliminating this warning is...
//
// For now, simply suppress it until we figure out if it's truly important
// in our situation...
//
#ifdef MUMMY_EXPORT_LAYER_CXX
#ifdef _MSC_VER
#pragma warning ( disable : 4190 )
#endif
#endif

#endif
