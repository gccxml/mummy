//----------------------------------------------------------------------------
//
//  $Id: CreateTestMain.cs 109 2008-05-07 15:14:27Z david.cole $
//
//  $Author: david.cole $
//  $Date: 2008-05-07 11:14:27 -0400 (Wed, 07 May 2008) $
//  $Revision: 109 $
//
//  Copyright (C) 2007 Kitware, Inc.
//
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
namespace Kitware.mummy.Runtime
{
  /// <summary>
  /// </summary>
  public class CreateTestMain
  {
    /// <summary>
    /// </summary>
    public static void Main(string[] args)
    {
      int i = 0;
      int n = 10000;

      if (args.Length>0)
      {
        n = System.Convert.ToInt32(args[0]);
      }

      Kitware.mummy.Runtime.MRSmokeTest test = new Kitware.mummy.Runtime.MRSmokeTest(true);
      Kitware.mummy.Runtime.Methods.Trace(System.String.Format("test.GetCppThis().Handle: {0}", test.GetCppThis().Handle));
      Kitware.mummy.Runtime.Methods.Trace(System.String.Format("test.GetCppThis().Wrapper: {0}", test.GetCppThis().Wrapper));

      Kitware.mummy.Runtime.MRSmokeTest testClone = test.Clone();
      Kitware.mummy.Runtime.Methods.Trace(System.String.Format("testClone.GetCppThis().Handle: {0}", testClone.GetCppThis().Handle));
      Kitware.mummy.Runtime.Methods.Trace(System.String.Format("testClone.GetCppThis().Wrapper: {0}", testClone.GetCppThis().Wrapper));

      // After calling Clone, there should be a valid CreateWrappedObject type index
      // of 0 registered with the mummy.Runtime... Try to create an object and see
      // what we get:
      //
      Kitware.mummy.Runtime.Methods.Trace("create...");
      bool created;
      object obj = Kitware.mummy.Runtime.Methods.CreateWrappedObject(
        // uint status, uint index, uint rawRefCount, System.IntPtr rawCppThis, bool callDisposalMethod
        0, 0, 0, System.IntPtr.Zero, true, out created);
      Kitware.mummy.Runtime.Methods.Trace(System.String.Format("obj: {0}", obj));

      test = (Kitware.mummy.Runtime.MRSmokeTest) obj;
      Kitware.mummy.Runtime.Methods.Trace(System.String.Format("test.GetCppThis().Handle: {0}", test.GetCppThis().Handle));
      Kitware.mummy.Runtime.Methods.Trace(System.String.Format("test.GetCppThis().Wrapper: {0}", test.GetCppThis().Wrapper));

      // Do it again to verify that the type got cached during the first call to Create...
      //
      obj = Kitware.mummy.Runtime.Methods.CreateWrappedObject(
        0, 0, 0, System.IntPtr.Zero, true, out created);
      Kitware.mummy.Runtime.Methods.Trace(System.String.Format("obj: {0}", obj));

      test = (Kitware.mummy.Runtime.MRSmokeTest) obj;
      Kitware.mummy.Runtime.Methods.Trace(System.String.Format("test.GetCppThis().Handle: {0}", test.GetCppThis().Handle));
      Kitware.mummy.Runtime.Methods.Trace(System.String.Format("test.GetCppThis().Wrapper: {0}", test.GetCppThis().Wrapper));

      // now loop (like in the other smoke test) the number of times given on
      // the command line so we can compare performance of this create methodology
      // with the performance of a strictly "new-based" strategy...
      //
      for (i= 0; i<n; ++i)
      {
        test = (Kitware.mummy.Runtime.MRSmokeTest)
           Kitware.mummy.Runtime.Methods.CreateWrappedObject(
              0, 0, 0, System.IntPtr.Zero, true, out created);
      }

      test = null;
      testClone = null;


      Kitware.mummy.Runtime.Methods.Trace("about to call System.GC.Collect()...");
      System.GC.Collect();
      Kitware.mummy.Runtime.Methods.Trace("done with System.GC.Collect()...");
    }
  }
}
