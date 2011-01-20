using System;
using System.Text;

namespace Vehicles
{
  public class SuperBlatMobile : BlatMobile
  {
    public override void ActivateTurboBoost()
    {
      ManualTestBlatMobile.Trace("C# SuperBlatMobile.ActivateTurboBoost called...\n");
    }

    public override void DeactivateTurboBoost()
    {
      ManualTestBlatMobile.Trace("C# SuperBlatMobile.DeactivateTurboBoost called...\n");
    }
  }

  public class ManualTestBlatMobile
  {
    public static void Trace(string s)
    {
      Console.Error.Write(s);
      System.Diagnostics.Debug.Write(s);
    }

    public static void TestBlatMobile(BlatMobile b1)
    {
       if (null == b1)
       {
          throw new System.Exception("error: null b1 not allowed in TestBlatMobile...\n");
       }

       Trace(System.String.Format("RefCount: {0}\n", b1.RefCount));

       Trace("Calling CycleTurbo from C#...\n");
       BlatMobile.CycleTurbo(b1);

       Trace("Calling CycleTurbo(null) from C#...\n");
       BlatMobile.CycleTurbo(null);

       Trace("Calling ActivateTurboBoost/DeactivateTurboBoost from C#...\n");
       b1.ActivateTurboBoost();
       b1.DeactivateTurboBoost();
       Trace(System.String.Format("RefCount: {0}\n", b1.RefCount));

       Trace(System.String.Format("Testing \"supported data types\" methods...\n"));
       b1.VUnsignedChar((byte)'a');
       b1.VUnsignedShort(0x11);
       b1.VUnsignedInt(0x2222);
       b1.VUnsignedLong(0x33333333);
       b1.VUnsignedInt64(0x4444444444444444);
       b1.VChar((sbyte)'b');
       b1.VShort(0x55);
       b1.VInt(0x6666);
       b1.VLong(0x77777777);
       b1.VInt64(System.Int64.MinValue);
       b1.VInt64(0);
       b1.VInt64(System.Int64.MaxValue);
       b1.VBool(true);
       b1.VFloat(9.9f);
       b1.VDouble(101010.101010101010101010);
       b1.VVoidPointer(System.IntPtr.Zero);
       b1.VVoidPointer((System.IntPtr) b1.GetCppThis().Handle);
       b1.VVoidPointer(System.IntPtr.Zero);
       b1.VObjectPointer(null);
       b1.VObjectPointer(b1);
       b1.VObjectPointer(null);
       Trace(System.String.Format("Testing \"supported data types\" methods... -- done\n"));
    }

    public static void MainImpl(string[] args)
    {
      Trace("\n");
      Trace("Test C# subclass of BlatMobile:\n");
      Trace("===============================\n");
      BlatMobile b1 = new SuperBlatMobile();
      TestBlatMobile(b1);
      Trace("\n");

      Trace("\n");
      Trace("Test direct wrapper of C++ BlatMobile:\n");
      Trace("======================================\n");
      b1 = new BlatMobile();
      TestBlatMobile(b1);
      Trace("\n");
   }

    public static void Main(string[] args)
    {
      try
      {
        Trace("CTEST_FULL_OUTPUT (Avoid ctest truncation of output)\n");
        Trace("Begin ManualTestBlatMobile.Main\n");

        MainImpl(args);

        System.GC.Collect();

        Trace("End ManualTestBlatMobile.Main\n");
      }
      catch(Exception exc)
      {
        Trace(exc.ToString());
        throw;
      }
    }
  }
}
