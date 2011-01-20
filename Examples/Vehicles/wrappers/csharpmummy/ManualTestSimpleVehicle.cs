using System;
using System.Text;

namespace Vehicles
{
  public class ManualTestSimpleVehicle
  {
    public static void Trace(string s)
    {
      Console.Error.Write(s);
      System.Diagnostics.Debug.Write(s);
    }

    public static void Main(string[] args)
    {
      try
      {
        Trace("CTEST_FULL_OUTPUT (Avoid ctest truncation of output)\n");
        Trace("Begin ManualTestSimpleVehicle.Main\n");

        SimpleVehicle v1 = new SimpleVehicle();
        int i = v1.SimpleMethod();
        SimpleVehicle v2 = v1.ObjectMethod();

        Trace(System.String.Format("i: {0}\n", i));
        Trace(System.String.Format("v1.CppThis: {0}\n", v1.GetCppThis().Handle));
        Trace(System.String.Format("v2.CppThis: {0}\n",
          null == v2 ? System.IntPtr.Zero : v2.GetCppThis().Handle));

        v1 = null;
        v2 = null;
        System.GC.Collect();

        Trace("End ManualTestSimpleVehicle.Main\n");
      }
      catch(Exception exc)
      {
        Trace(exc.ToString());
        throw;
      }
    }
  }
}
