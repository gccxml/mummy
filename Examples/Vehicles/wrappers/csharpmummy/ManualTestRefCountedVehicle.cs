using System;
using System.Text;

namespace Vehicles
{
  public class ManualTestRefCountedVehicle
  {
    public static void Trace(string s)
    {
      Console.Error.Write(s);
      System.Diagnostics.Debug.Write(s);
    }

    public static void MainImpl(string[] args)
    {
       RefCountedVehicle v1 = new RefCountedVehicle();
       if (null != v1)
       {
          Trace(System.String.Format("v1.RefCount: {0}\n", v1.RefCount));
       }

       RefCountedVehicle v2 = v1.ThisVehicle;
       if (null != v2)
       {
          Trace(System.String.Format("v2.RefCount: {0}\n", v2.RefCount));
       }
       v2 = v1.ThisVehicle;
       if (null != v2)
       {
          Trace(System.String.Format("v2.RefCount: {0}\n", v2.RefCount));
       }

       RefCountedVehicle v3 = v1.ThatVehicle;
       if (null != v3)
       {
          Trace(System.String.Format("v3.RefCount: {0}\n", v3.RefCount));
       }
       v3 = v1.ThatVehicle;
       if (null != v3)
       {
          Trace(System.String.Format("v3.RefCount: {0}\n", v3.RefCount));
       }

       RefCountedVehicle v4 = v1.TheOtherVehicle;
       if (null != v4)
       {
          Trace(System.String.Format("v4.RefCount: {0}\n", v4.RefCount));
       }
       v4 = v1.TheOtherVehicle;
       if (null != v4)
       {
          Trace(System.String.Format("v4.RefCount: {0}\n", v4.RefCount));
       }
       v4 = v1.TheOtherVehicle;
       if (null != v4)
       {
          Trace(System.String.Format("v4.RefCount: {0}\n", v4.RefCount));
       }
       v4 = v1.TheOtherVehicle;
       if (null != v4)
       {
          Trace(System.String.Format("v4.RefCount: {0}\n", v4.RefCount));
       }


       RefCountedVehicle v5 = v1.ThatVehicleAlias;
       if (null != v5)
       {
          Trace(System.String.Format("v5.RefCount: {0}\n", v5.RefCount));
       }
       v5 = v1.ThatVehicleAlias;
       if (null != v5)
       {
          Trace(System.String.Format("v5.RefCount: {0}\n", v5.RefCount));
       }


       RefCountedVehicle v6 = v1.ThatVehicleByAnyOtherName;
       if (null != v6)
       {
         Trace(System.String.Format("v6.RefCount: {0}\n", v6.RefCount));
       }
       v6 = v1.ThatVehicleByAnyOtherName;
       if (null != v6)
       {
         Trace(System.String.Format("v6.RefCount: {0}\n", v6.RefCount));
       }


       RefCountedVehicle v7 = RefCountedVehicle.Create();
       if (null != v7)
       {
         Trace(System.String.Format("v7.RefCount: {0}\n", v7.RefCount));
       }


       if (null != v1)
       {
          Trace(System.String.Format("v1.CppThis: {0}\n", v1.GetCppThis().Handle));
       }
       if (null != v2)
       {
          Trace(System.String.Format("v2.CppThis: {0}\n", v2.GetCppThis().Handle));
       }
       if (null != v3)
       {
          Trace(System.String.Format("v3.CppThis: {0}\n", v3.GetCppThis().Handle));
       }
       if (null != v4)
       {
          Trace(System.String.Format("v4.CppThis: {0}\n", v4.GetCppThis().Handle));
       }
       if (null != v5)
       {
         Trace(System.String.Format("v5.CppThis: {0}\n", v5.GetCppThis().Handle));
       }
       if (null != v6)
       {
         Trace(System.String.Format("v6.CppThis: {0}\n", v6.GetCppThis().Handle));
       }
       if (null != v7)
       {
         Trace(System.String.Format("v7.CppThis: {0}\n", v7.GetCppThis().Handle));
       }

       // Explicitly Dispose of the one we "new'ed" to try to get the Destroy
       // export layer function covered:
       //
       v1.Dispose();
    }

    public static void Main(string[] args)
    {
      try
      {
        Trace("CTEST_FULL_OUTPUT (Avoid ctest truncation of output)\n");
        Trace("Begin ManualTestRefCountedVehicle.Main\n");


        MainImpl(args);


        Trace("\n");
        Trace("Before System.GC.Collect:\n");
        Kitware.mummy.Runtime.Methods.Print();

        System.GC.Collect();
        System.GC.WaitForPendingFinalizers();

        Trace("\n");
        Trace("After System.GC.Collect:\n");
        Kitware.mummy.Runtime.Methods.Print();

        RefCountedVehicle.Check();

        Trace("\n");
        Trace("End ManualTestRefCountedVehicle.Main\n");
      }
      catch(Exception exc)
      {
        Trace(exc.ToString());
        throw;
      }
    }
  }
}
