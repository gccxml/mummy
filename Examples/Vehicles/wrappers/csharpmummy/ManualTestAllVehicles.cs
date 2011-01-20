using System;
using System.Text;

namespace Vehicles
{
  public class ManualTestAllVehicles
  {
    static int PrintAbstractVehicle(string context, AbstractVehicle av)
    {
      string s = "";

      s += String.Format("context: {0}\n", context);
      s += String.Format("  av: {0}\n", av);
      s += String.Format("  VehicleType: {0}\n", av.VehicleType);
      s += String.Format("  Mileage: {0}\n", av.Mileage);
      s += String.Format("  VIN: {0}\n", av.VIN);
      s += String.Format("  TGetHasLicensePlate(): {0}\n", av.TGetHasLicensePlate());
      s += String.Format("  Registered: {0}\n", av.Registered);
      s += String.Format("  FuelLevel: {0}\n", av.FuelLevel);
      s += String.Format("  TGetCallback(): {0}\n", av.TGetCallback());
      s += String.Format("  TGetClientData(): {0}\n", av.TGetClientData());
      s += String.Format("  TGetManufacturerID(): {0}\n", av.TGetManufacturerID());
      s += "\n";

      System.Diagnostics.Debug.Write(s);
      Console.Error.Write(s);

      return 0;
    }


    public static void AvCallbackTestCallback(IntPtr arg0, uint arg1, IntPtr arg2, IntPtr arg3)
    {
       // no-op...
    }


    public static void Main(string[] args)
    {
      try
      {
        System.Diagnostics.Debug.Write("CTEST_FULL_OUTPUT (Avoid ctest truncation of output)\n");
        Console.Error.Write("CTEST_FULL_OUTPUT (Avoid ctest truncation of output)\n");

        using (AbstractVehicle v1 = new Vehicle())
        {
          v1.Registered = true;
          v1.TSetHasLicensePlate(true);
          PrintAbstractVehicle("Vehicle", v1);
        }

        using (AbstractVehicle v2 = new Mustang())
        {
          v2.Registered = false;
          v2.TSetHasLicensePlate(false);
          PrintAbstractVehicle("Mustang", v2);

          // For coverage of the "main-like" methods in Mustang.cxx:
          //
          Mustang m2 = v2 as Mustang;
          if (m2 != null)
          {
            m2.VirtualMainPlus(333.333, args, 333);
            m2.NonVirtualMain(args);
          }

          Mustang.main(args);
       }

        using (AbstractVehicle v3 = new GenericMotorCycle())
        {
          v3.VIN = "C# VIN";
          PrintAbstractVehicle("GenericMotorCycle", v3);
        }

        using (AbstractVehicle v4 = new GenericSUV())
        {
          PrintAbstractVehicle("GenericSUV", v4);

          // Add some additional coverage by calling these methods
          // that are not already called elsewhere:
          //
          int discard = v4.TGet_BTX_ETX_Int();
          if (0 == discard)
          {
            Console.Error.Write("discard is 0");
          }

          v4.TSetManufacturerID(VehicleDescriptor.ManufacturerID.EtcEtcEtc);
          v4.TSetCallback(new AbstractVehicle.AVCallback(AvCallbackTestCallback), System.IntPtr.Zero);
       }
      }
      catch(Exception exc)
      {
        string s = exc.ToString();
        System.Diagnostics.Debug.Write(s);
        Console.Error.Write(s);
        throw;
      }
    }
  }
}
