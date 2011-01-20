//----------------------------------------------------------------------------
//
//  $Id: WrappedObject.cs 2 2007-12-17 18:15:56Z david.cole $
//
//  $Author: david.cole $
//  $Date: 2007-12-17 13:15:56 -0500 (Mon, 17 Dec 2007) $
//  $Revision: 2 $
//
//  Copyright (C) 2007 Kitware, Inc.
//
//----------------------------------------------------------------------------

namespace Kitware.mummy.Runtime
{
  /// <summary>
  /// Base class for mummy-generated C# classes.
  /// </summary>
  public class WrappedObject : System.IDisposable
  {
    //------------------------------------------------------------------------
    /// <summary>
    /// MRClassNameKey should be the value that is returned by the C++ RTTI
    /// function typeid(*obj).name() for an unmanaged C++ object.
    /// </summary>
    public static readonly string MRClassNameKey = "no equivalent unmanaged class";

    //------------------------------------------------------------------------
    /// <summary>
    /// MRFullTypeName should be the same as the fully qualified name of this
    /// C# class...
    /// </summary>
    public const string MRFullTypeName = "Kitware.mummy.Runtime.WrappedObject";

    //------------------------------------------------------------------------
    /// <summary>
    /// A subclass should call Kitware.mummy.Runtime.Methods.RegisterType
    /// from its static constructor.
    /// </summary>
    static WrappedObject()
    {
    }

    //------------------------------------------------------------------------
    private System.Runtime.InteropServices.HandleRef CppThis;
    private bool CallDisposalMethod;

    //------------------------------------------------------------------------
    /// <summary>
    /// Protected constructor. Only called from derived classes.
    /// </summary>
    protected WrappedObject(System.IntPtr rawCppThis, bool callDisposalMethod, bool strong)
    {
       this.SetCppThis(rawCppThis, callDisposalMethod, strong);
    }

    /// <summary>
    /// Finalizer/destructor. Called automatically by the garbage collector.
    /// </summary>
    ~WrappedObject()
    {
      this.Dispose(false);
    }

    /// <summary>
    /// Get the HandleRef that links the C++ 'this' pointer of the
    /// underlying unmanaged object to 'this'.
    /// </summary>
    public System.Runtime.InteropServices.HandleRef GetCppThis()
    {
      return this.CppThis;
    }

    /// <summary>
    /// Set the C++ 'this' pointer of the underlying unmanaged object. DO NOT
    /// CALL THIS METHOD DIRECTLY. This method is called automatically as
    /// needed by generated code.
    /// </summary>
    protected void SetCppThis(System.IntPtr rawCppThis, bool callDisposalMethod, bool strong)
    {
      this.CppThis = new System.Runtime.InteropServices.HandleRef(this, rawCppThis);
      this.CallDisposalMethod = callDisposalMethod;

      if (rawCppThis != System.IntPtr.Zero)
      {
         Kitware.mummy.Runtime.Methods.AddTableReference(rawCppThis, this, strong);
      }
    }

    /// <summary>
    /// Clear the C++ 'this' pointer of the underlying unmanaged object. DO
    /// NOT CALL THIS METHOD DIRECTLY. This method is called automatically as
    /// needed by generated code.
    /// </summary>
    protected void ClearCppThis()
    {
      if (this.CppThis.Handle != System.IntPtr.Zero)
      {
         Kitware.mummy.Runtime.Methods.RemoveTableReference(this.CppThis.Handle);
      }

      this.CppThis = new System.Runtime.InteropServices.HandleRef();
      this.CallDisposalMethod = false;
    }

    /// <summary>
    /// Returns true if this instance of WrappedObject responsible for
    /// calling an unmanaged disposal method when it is being disposed of.
    /// DO NOT CALL THIS METHOD DIRECTLY. This method is called automatically
    /// as needed by generated code.
    /// </summary>
    protected bool GetCallDisposalMethod()
    {
      return this.CallDisposalMethod;
    }

    //------------------------------------------------------------------------
    /// <summary>
    /// Public implementation of IDisposable. You can call this method prior
    /// to a WrappedObject going out of scope (and prior to the garbage
    /// collector processing it) to force an early disconnection from the
    /// underlying unmanaged object. After calling this method, any attempt
    /// to cross the boundary to unmanaged code will likely result in an
    /// exception. If you do call this method, it should be the last thing
    /// you do with a WrappedObject...
    /// </summary>
    public void Dispose()
    {
      this.Dispose(true);
      System.GC.SuppressFinalize(this);
    }

    /// <summary>
    /// Generated subclasses override this method to call a disposal method
    /// in the unmanaged code.
    /// </summary>
    protected virtual void Dispose(bool disposing)
    {
    }
  }
}
