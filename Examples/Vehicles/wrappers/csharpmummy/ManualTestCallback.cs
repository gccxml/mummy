using System;
using System.Runtime.InteropServices; // DllImport and HandleRef both live here

namespace Vehicles
{
  public class ManualTestCallback
  {
//    AbstractVehicle.AbstractVehicle_Callback cb = null;

    public static void Trace(string s)
    {
      Console.Error.Write(s);
      System.Diagnostics.Debug.Write(s);
    }

    public static void StaticCallback(IntPtr sender, uint eid, IntPtr mobj, IntPtr calldata)
    {
      GCHandle gch = (GCHandle) mobj;
      object obj = gch.Target;

      Trace("Managed code: StaticCallback was called!!\n");

      Trace(String.Format("  sender: {0:x}\n", (uint) sender));
      Trace(String.Format("  eid: {0}\n", eid));
      Trace(String.Format("  obj: {0}\n", obj==null ? "(null)" : obj.ToString()));
      Trace(String.Format("  calldata: {0:x}\n", (uint) calldata));

      if (1 == eid)
      {
        Trace(String.Format("  interpreted calldata: '{0}'\n", Marshal.ReadInt32(calldata)));
      }
      if (2 == eid)
      {
        Trace(String.Format("  interpreted calldata: '{0}'\n", Marshal.PtrToStringAnsi(calldata)));
      }
      if (3 == eid)
      {
        Trace(String.Format("  interpreted calldata: '{0}'\n", (double) Marshal.PtrToStructure(calldata, typeof(double))));
      }
      if (4 == eid)
      {
        Trace(String.Format("  (un)interpreted calldata: '{0:x}'\n", (uint) calldata));
      }
    }

    public virtual void VirtualCallback(IntPtr sender, uint eid, IntPtr mobj, IntPtr calldata)
    {
      Trace("Managed code: VirtualCallback was called!!\n");
      StaticCallback(sender, eid, mobj, calldata);
    }

    public virtual void VirtualMain(string[] args)
    {
      try
      {
        Trace("Begin ManualTestCallback.Main\n");

        Vehicle v1 = new Vehicle();
        GCHandle gch = GCHandle.Alloc(this);

        Trace(String.Format("v1: {0:x}\n", (uint) v1.GetCppThis().Handle));
        //Trace(String.Format("cb: {0}\n", cb == null ? "(null)" : cb.ToString()));

        uint i = 0;
        uint maxi = 1999005;//v1.GetMaxIndex();

        Trace("Set/GetValue loop...\n");
        //v1.SetCallback(cb, gch);
        for (i= 0; i<maxi; ++i)
          {
          v1.TSetValue(i, v1.TGetValue(i+1));
          if (0 == i)
            {
            //v1.SetCallback(null, gch);
            }
          }
        Trace(String.Format("  i: {0}\n", i));

        Trace("Set/GetDoubleValue loop...\n");
        //v1.SetCallback(cb, gch);
        for (i = 0; i < maxi; ++i)
          {
          v1.TSetDoubleValue(i, v1.TGetDoubleValue(i+1));
          if (0 == i)
            {
            //v1.SetCallback(null, gch);
            }
          }
        Trace(String.Format("  i: {0}\n", i));

        Trace("Set/GetStringValue loop...\n");
        //v1.SetCallback(cb, gch);
        for (i = 0; i < maxi; ++i)
          {
          v1.TSetStringValue(i, v1.TGetStringValue(i+1));
          if (0 == i)
            {
            //v1.SetCallback(null, gch);
            }
          }
        Trace(String.Format("  i: {0}\n", i));

        Trace("Set/GetObjectValue loop...\n");
        //v1.SetCallback(cb, gch);
        for (i = 0; i < maxi; ++i)
          {
          v1.TSetObjectValue(i, v1.TGetObjectValue(i+1));
          if (0 == i)
            {
            //v1.SetCallback(null, gch);
            }
          }
        Trace(String.Format("  i: {0}\n", i));

        Trace("MethodWithBoolRefArg loop...\n");
        for (i = 0; i < 30; ++i)
          {
          byte arg = 0;
          v1.MethodWithBoolRefArg(ref arg);
          Trace(String.Format("  i: {0}  --  arg: \n", i, (arg != 0 ? "true" : "false")));
          }
        Trace(String.Format("  i: {0}\n", i));

        gch.Free();

        Trace("End ManualTestCallback.Main\n");
      }
      catch(Exception exc)
      {
        Trace(exc.ToString());
        throw;
      }
    }

    public static void Main(string[] args)
    {
      ManualTestCallback tcb;

      Trace("CTEST_FULL_OUTPUT (Avoid ctest truncation of output)\n");

      Trace("\n");
      Trace("With *VIRTUAL* callback...\n");
      tcb = new ManualTestCallback();
      //tcb.cb = new AbstractVehicle.AbstractVehicle_Callback(tcb.VirtualCallback);
      tcb.VirtualMain(args);

      Trace("\n");
      Trace("With *STATIC* callback...\n");
      tcb = new ManualTestCallback();
      //tcb.cb = new AbstractVehicle.AbstractVehicle_Callback(StaticCallback);
      tcb.VirtualMain(args);

      Trace("\n");
      Trace("*WITHOUT ANY* callback...\n");
      tcb = new ManualTestCallback();
      tcb.VirtualMain(args);
    }
  }
}
