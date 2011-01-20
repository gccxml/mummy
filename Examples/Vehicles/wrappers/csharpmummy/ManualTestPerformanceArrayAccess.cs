using System;
using System.Text;

namespace Vehicles
{
  public class ManualTestPerformanceArrayAccess
  {
    public static void Trace(string s)
    {
      Console.Error.Write(s);
      System.Diagnostics.Debug.Write(s);
    }

    public static void ReportTiming(string s, TimeSpan ts, uint count)
    {
      Trace(String.Format("\n"));
      Trace(String.Format("Managed Code Performance Results:\n"));
      Trace(String.Format("  data type: {0}\n", s));
      Trace(String.Format("  seconds: {0}\n", (double)ts.Ticks / (double)TimeSpan.TicksPerSecond));
      Trace(String.Format("  count: {0:N}\n", count));
      Trace(String.Format("  operations per second: {0:N}\n", (double)count / ((double)ts.Ticks / (double)TimeSpan.TicksPerSecond)));
      Trace(String.Format("\n"));
      Trace(String.Format("\n"));
    }

    public static void WriteNativeCsharpArrayElements(int length, int maxi)
    {
      System.TimeSpan ts;
      System.DateTime begin;

      int i = 0;
      float lf = 0.0F;
      float[] csaf = new float[length];

      Trace("Native C# array write access 'csaf[i] = lf' loop...\n");
      begin = System.DateTime.Now;
      for (i = 0; i < maxi; ++i)
      {
        csaf[i % length] = lf;
      }
      ts = System.DateTime.Now - begin;
      ReportTiming("float", ts, (uint)maxi);
    }

    public unsafe static void WriteCppArrayElements(int length, int maxi, Vehicle v1)
    {
      try
      {
        System.TimeSpan ts;
        System.DateTime begin;

        int i = 0;
        float* af = (float*)v1.InfoArrayBase();
        float lf = 0.0F;


        Trace("Direct C++ array write access from C# 'af[i] = lf' loop...\n");
        begin = System.DateTime.Now;
        for (i = 0; i < maxi; ++i)
        {
          af[i % length] = lf;
        }
        ts = System.DateTime.Now - begin;
        ReportTiming("float", ts, (uint)maxi);


        Trace("Unchecked vtable write access 'v1.SetInfoArrayElementUnchecked(i, lf)' loop...\n");
        begin = System.DateTime.Now;
        for (i = 0; i < maxi; ++i)
        {
          v1.TSetInfoArrayElementUnchecked(i % length, lf);
        }
        ts = System.DateTime.Now - begin;
        ReportTiming("float", ts, (uint)maxi);


        Trace("Unchecked vtable read access 'lf = v1.GetInfoArrayElementUnchecked(i)' loop...\n");
        begin = System.DateTime.Now;
        for (i = 0; i < maxi; ++i)
        {
          lf = v1.TGetInfoArrayElementUnchecked(i % length);
        }
        ts = System.DateTime.Now - begin;
        ReportTiming("float", ts, (uint)maxi);


        Trace("Checked vtable write access 'v1.SetInfoArrayElementChecked(i, lf)' loop...\n");
        begin = System.DateTime.Now;
        for (i = 0; i < maxi; ++i)
        {
          v1.TSetInfoArrayElementChecked(i % length, lf);
        }
        ts = System.DateTime.Now - begin;
        ReportTiming("float", ts, (uint)maxi);


        Trace("Checked vtable read access 'lf = v1.GetInfoArrayElementChecked(i)' loop...\n");
        begin = System.DateTime.Now;
        for (i = 0; i < maxi; ++i)
        {
          lf = v1.TGetInfoArrayElementChecked(i % length);
        }
        ts = System.DateTime.Now - begin;
        ReportTiming("float", ts, (uint)maxi);
      }
      catch (Exception exc)
      {
        Trace(exc.ToString());
        throw;
      }
    }

    public static void Main(string[] args)
    {
      Trace("CTEST_FULL_OUTPUT (Avoid ctest truncation of output)\n");
      Trace("Begin ManualTestPerformanceArrayAccess.Main\n");
      Trace("\n");

      Vehicle v1 = new Vehicle();
      int length = (int)v1.TGetInfoArrayLength();
      int maxi = 3 * (int)v1.MaxIndex;

      WriteNativeCsharpArrayElements(length, maxi);

      WriteCppArrayElements(length, maxi, v1);

      Trace("End ManualTestPerformanceArrayAccess.Main\n");
    }
  }
}
