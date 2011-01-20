using System;
using System.Runtime.InteropServices; // DllImport and HandleRef both live here

namespace Vehicles
{
  public class ManualTestEvent
  {
    public static void Trace(string s)
    {
      Console.Error.Write(s);
      System.Diagnostics.Debug.Write(s);
    }

    public void FuelLevelChanged(AbstractVehicle sender, FuelLevelChangedEventArgs args)
    {
      Trace("\n");
      Trace("Managed code: FuelLevelChanged was called!!\n");
      Trace(String.Format("  this: {0}\n", this));
      Trace(String.Format("  sender.VIN: {0}\n", sender.VIN));
      Trace(String.Format("  args.Level: {0}\n", args.Level));
      Trace(String.Format("  args.OldLevel: {0}\n", args.OldLevel));
      Trace("\n");
    }

    public static void TestFloatArrayMethods(Vehicle v1)
    {
      float[] fa1;
      float[] fa2;
      int i;
      int n;

      Trace("\n");
      Trace("\n");

      Trace(String.Format("fa1\n"));
      fa1 = v1.InfoArray;
      foreach (float f in fa1)
      {
        Trace(String.Format("  f: {0}\n", f));
      }
      Trace("\n");

      i = 0;
      n = fa1.GetLength(0);
      for (i = 0; i < n; ++i)
      {
        fa1[i] = 2.2f;
      }
      fa1[0] = 1111.11111111f;
      fa1[fa1.Length-1] = 3333.33333333f;
      v1.InfoArray = fa1;

      Trace(String.Format("fa2\n"));
      fa2 = v1.InfoArray;
      foreach (float f in fa2)
      {
        Trace(String.Format("  f: {0}\n", f));
      }
      Trace("\n");
    }

    public static void Main(string[] args)
    {
      Trace("CTEST_FULL_OUTPUT (Avoid ctest truncation of output)\n");
      Trace("Begin ManualTestEvent.Main\n");

      ManualTestEvent te = new ManualTestEvent();
      Vehicle v1 = new Vehicle();
      v1.FuelLevelChanged += new AbstractVehicle.FuelLevelChangedEventHandler(te.FuelLevelChanged);
      v1.FuelLevel = 0.77;
      v1.FuelLevel = 0.77;
      v1.FuelLevel = 0.77;
      v1.FuelLevel = 1.0;
      v1.FuelLevel = 1.0;
      v1.FuelLevel = 1.0;

      TestFloatArrayMethods(v1);

      VehicleDescriptor desc = new VehicleDescriptor();

      try
      {
        desc = v1.TGetDescriptor();
      }
      catch(System.Exception exc)
      {
        Trace("info: Caught exception calling v1.TGetDescriptor...\n");
        Trace("      Is this an older version of mono with the 'struct cannot cross PInvoke boundary' bug?\n");
        Trace("\n");
        Trace(exc.ToString());
        Trace("\n");
        Trace("\n");
      }

      Trace(String.Format("desc.AMarker: {0}\n", desc.AMarker));
      Trace(String.Format("desc.BoolTest1: {0}\n", desc.BoolTest1));
      Trace(String.Format("desc.BoolTest2: {0}\n", desc.BoolTest2));
      Trace(String.Format("desc.c1: {0}\n", desc.c1));
      Trace(String.Format("desc.ColorCode: {0}\n", desc.ColorCode));
      Trace(String.Format("desc.d1: {0}\n", desc.d1));
      Trace(String.Format("desc.f1: {0}\n", desc.f1));
      Trace(String.Format("desc.i1: {0}\n", desc.i1));
      Trace(String.Format("desc.i64: {0}\n", desc.i64));
      Trace(String.Format("desc.l1: {0}\n", desc.l1));
      Trace(String.Format("desc.MileageEstimate: {0}\n", desc.MileageEstimate));
      Trace(String.Format("desc.NumberOfCylinders: {0}\n", desc.NumberOfCylinders));
      Trace(String.Format("desc.NumberOfDoors: {0}\n", desc.NumberOfDoors));
      Trace(String.Format("desc.s1: {0}\n", desc.s1));
      Trace(String.Format("desc.uc1: {0}\n", desc.uc1));
      Trace(String.Format("desc.ui1: {0}\n", desc.ui1));
      Trace(String.Format("desc.ui64: {0}\n", desc.ui64));
      Trace(String.Format("desc.ul1: {0}\n", desc.ul1));
      Trace(String.Format("desc.us1: {0}\n", desc.us1));
      Trace(String.Format("desc.enumMfgID: {0}\n", desc.enumMfgID));
      Trace(String.Format("desc.ZMarker: {0}\n", desc.ZMarker));

      desc.NumberOfCylinders = 16;
      desc.NumberOfDoors = 16;
      v1.TSetDescriptor(desc);

      desc.NumberOfCylinders = 32;
      desc.NumberOfDoors = 32;
      v1.TSetDescriptorRef(desc);

      desc.NumberOfCylinders = 64;
      desc.NumberOfDoors = 64;
      v1.TSetDescriptorConstRef(desc);

      // Silly prove-you-can-do-it tests and improve coverage, too:
      //
      FuelLevelChanged evt = new FuelLevelChanged();
      evt.ReturnMyLong();
      evt.ReturnMyNonClassLong();
      evt = null;

      FuelLevelChangedEventArgs eargs = new FuelLevelChangedEventArgs();
      eargs.Level = 0.88;
      eargs.OldLevel = 0.55;
      eargs = null;

      Trace("End ManualTestEvent.Main\n");

      System.GC.Collect();
    }
  }
}
