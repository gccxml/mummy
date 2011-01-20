//----------------------------------------------------------------------------
//
//  $Id: MRSmokeTestMain.cs 470 2009-06-12 17:43:02Z hoffman $
//
//  $Author: hoffman $
//  $Date: 2009-06-12 13:43:02 -0400 (Fri, 12 Jun 2009) $
//  $Revision: 470 $
//
//  Copyright (C) 2007 Kitware, Inc.
//
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
namespace Kitware.mummy.Runtime
{
  /// <summary>
  /// </summary>
  public class SmokeTestMain
  {
    /// <summary>
    /// </summary>
    public static void Main(string[] args)
    {
      int i = 0;
      int n = 10000;
      bool strong = false;
      bool evensplit = false;
      bool pseudorandom = false;

      if (args.Length>0)
      {
        n = System.Convert.ToInt32(args[0]);
      }

      if (args.Length>1)
      {
        if ("--strong" == args[1])
        {
          strong = true;
        }
        else if ("--evensplit" == args[1])
        {
          evensplit = true;
        }
        else if ("--pseudorandom" == args[1])
        {
          pseudorandom = true;
        }
      }

      Kitware.mummy.Runtime.MRSmokeTest test = new Kitware.mummy.Runtime.MRSmokeTest(strong);
      Kitware.mummy.Runtime.MRSmokeTest testClone = test.Clone();

      Kitware.mummy.Runtime.Methods.Trace("looping...");

      for (i= 0; i<args.Length; ++i)
      {
        Kitware.mummy.Runtime.Methods.Trace(System.String.Format("arg{0}: {1}", i, args[i]));
      }

      System.Random r = new System.Random();

      for (i= 0; i<n; ++i)
      {
        if (evensplit)
        {
          strong = !strong;
        }
        else if (pseudorandom)
        {
          strong = (r.NextDouble() > 0.5 ? true : false);
        }

        test = new Kitware.mummy.Runtime.MRSmokeTest(strong);
      }

      if (null == testClone)
      {
        throw new System.Exception("error: testClone is null!");
      }

      test = null;
      testClone = null;

      uint teCount = Kitware.mummy.Runtime.Methods.GetTypeEntryCount();
      Kitware.mummy.Runtime.Methods.Trace(System.String.Format("teCount: {0}", teCount));

      Kitware.mummy.Runtime.Methods.Trace("about to call System.GC.Collect()...");

      System.GC.Collect();

      Kitware.mummy.Runtime.Methods.Trace("done with System.GC.Collect()...");
    }
  }
}
