using System;
using System.Text;

namespace Vehicles
{
  public class ManualTestSignatureSeries
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
        Trace("Begin ManualTestSignatureSeries.Main\n");

        SignatureSeries s1 = new SignatureSeries();

        System.IntPtr ptr = System.IntPtr.Zero;

        // For "int" :
        //
        int intArg;

//public void Method_int_param_const(int arg)
        Trace("============================================================\n");
        intArg = 101;
        Trace(System.String.Format("calling Method_int_param_const\n"));
        s1.Method_int_param_const(intArg);

//public void Method_int_param_constdataconstpointer(ref int arg)
        Trace("============================================================\n");
        intArg = 102;
        Trace(System.String.Format("calling Method_int_param_constdataconstpointer\n"));
        s1.Method_int_param_constdataconstpointer(ptr);

//public void Method_int_param_constdatapointer(ref int arg)
        Trace("============================================================\n");
        intArg = 103;
        Trace(System.String.Format("calling Method_int_param_constdatapointer\n"));
        s1.Method_int_param_constdatapointer(ptr);

//public void Method_int_param_constref(ref int arg)
        Trace("============================================================\n");
        intArg = 104;
        Trace(System.String.Format("calling Method_int_param_constref\n"));
        s1.Method_int_param_constref(ref intArg);

//public void Method_int_param_dataconstpointer(ref int arg)
        Trace("============================================================\n");
        intArg = 105;
        Trace(System.String.Format("calling Method_int_param_dataconstpointer\n"));
        s1.Method_int_param_dataconstpointer(ptr);

//public void Method_int_param_datapointer(ref int arg)
        Trace("============================================================\n");
        intArg = 106;
        Trace(System.String.Format("calling Method_int_param_datapointer\n"));
        s1.Method_int_param_datapointer(ptr);

//public void Method_int_param_plain(int arg)
        Trace("============================================================\n");
        intArg = 107;
        Trace(System.String.Format("calling Method_int_param_plain\n"));
        s1.Method_int_param_plain(intArg);

//public void Method_int_param_ref(ref int arg)
        Trace("============================================================\n");
        intArg = 108;
        Trace(System.String.Format("calling Method_int_param_ref\n"));
        s1.Method_int_param_ref(ref intArg);
        Trace(System.String.Format("{0} (after returning from Method_int_param_ref)\n", intArg));

//public int Method_int_return_const()
        Trace("============================================================\n");
        intArg = 0;
        s1.Method_int_param_plain(109); // push to other side...
        Trace(System.String.Format("calling Method_int_return_const\n"));
        intArg = s1.Method_int_return_const();
        Trace(System.String.Format("{0}\n", intArg));

//public IntPtr Method_int_return_constdataconstpointer()
        Trace("============================================================\n");
        intArg = 0;
        s1.Method_int_param_plain(110); // push to other side...
        Trace(System.String.Format("calling Method_int_return_constdataconstpointer\n"));
        ptr = s1.Method_int_return_constdataconstpointer();
        intArg = System.Runtime.InteropServices.Marshal.ReadInt32(ptr);
        Trace(System.String.Format("{0} (via pointer {1})\n", intArg, ptr));

//public IntPtr Method_int_return_constdatapointer()
        Trace("============================================================\n");
        intArg = 0;
        s1.Method_int_param_plain(111); // push to other side...
        Trace(System.String.Format("calling Method_int_return_constdatapointer\n"));
        ptr = s1.Method_int_return_constdatapointer();
        intArg = System.Runtime.InteropServices.Marshal.ReadInt32(ptr);
        Trace(System.String.Format("{0} (via pointer {1})\n", intArg, ptr));

//public IntPtr Method_int_return_constref()
        Trace("============================================================\n");
        intArg = 0;
        s1.Method_int_param_plain(112); // push to other side...
        Trace(System.String.Format("calling Method_int_return_constref\n"));
        ptr = s1.Method_int_return_constref();
        intArg = System.Runtime.InteropServices.Marshal.ReadInt32(ptr);
        Trace(System.String.Format("{0} (via reference {1})\n", intArg, ptr));

//public IntPtr Method_int_return_dataconstpointer()
        Trace("============================================================\n");
        intArg = 0;
        s1.Method_int_param_plain(113); // push to other side...
        Trace(System.String.Format("calling Method_int_return_dataconstpointer\n"));
        ptr = s1.Method_int_return_dataconstpointer();
        intArg = System.Runtime.InteropServices.Marshal.ReadInt32(ptr);
        Trace(System.String.Format("{0} (via pointer {1})\n", intArg, ptr));

//public IntPtr Method_int_return_datapointer()
        Trace("============================================================\n");
        intArg = 0;
        s1.Method_int_param_plain(114); // push to other side...
        Trace(System.String.Format("calling Method_int_return_datapointer\n"));
        ptr = s1.Method_int_return_datapointer();
        intArg = System.Runtime.InteropServices.Marshal.ReadInt32(ptr);
        Trace(System.String.Format("{0} (via pointer {1})\n", intArg, ptr));

//public int Method_int_return_plain()
        Trace("============================================================\n");
        intArg = 0;
        s1.Method_int_param_plain(115); // push to other side...
        Trace(System.String.Format("calling Method_int_return_plain\n"));
        intArg = s1.Method_int_return_plain();
        Trace(System.String.Format("{0}\n", intArg));

//public IntPtr Method_int_return_ref()
        Trace("============================================================\n");
        intArg = 0;
        s1.Method_int_param_plain(116); // push to other side...
        Trace(System.String.Format("calling Method_int_return_ref\n"));
        ptr = (System.IntPtr) s1.Method_int_return_ref();
        intArg = System.Runtime.InteropServices.Marshal.ReadInt32(ptr);
        Trace(System.String.Format("{0} (via reference {1})\n", intArg, ptr));


        try
        {
           s1.ThrowExceptionOnPurpose("throw some text across the boundary...");
        }
        catch (Oops oops1)
        {
           Trace("\n");
           Trace("retrieved text from exception:\n");
           Trace("==============================\n");
           Trace("\n");
           Trace(oops1.Description());
           Trace("\n");
           Trace("==============================\n");
           Trace("\n");
           Trace(oops1.ToString());
           Trace("\n");
           Trace("==============================\n");
           Trace("\n");
           Trace("exception caught and 'handled'... continuing...\n");

           // Clone it for coverage...!!
           // (and then just let it go...)
           //
           Oops oopsClone = oops1.Clone();
           if (null == oopsClone)
           {
             Trace("\n");
             Trace("==============================\n");
             Trace("\n");
             Trace("oopsClone is null...");
           }

           Trace("\n");
           Trace("==============================\n");
           Trace("\n");
           Trace("past the Clone call...\n");
        }


        s1 = null;
        System.GC.Collect();

        Trace("End ManualTestSignatureSeries.Main\n");
      }
      catch (Vehicles.Oops oops2)
      {
        Trace(oops2.Description());
        Trace(oops2.ToString());
        throw;
      }
      catch (Exception exc)
      {
        Trace(exc.ToString());
        throw;
      }
    }
  }
}
