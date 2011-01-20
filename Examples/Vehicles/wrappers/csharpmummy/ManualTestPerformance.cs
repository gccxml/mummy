using System;
using System.Text;

namespace Vehicles
{
  public class ManualTestPerformance
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
      //Trace(String.Format("  ticks: {0:N}\n", ts.Ticks));
      Trace(String.Format("  count: {0:N}\n", count));
      //Trace(String.Format("  ticks per operation: {0}\n", (double)ts.Ticks/(double)count));
      Trace(String.Format("  operations per second: {0:N}\n", (double)count / ((double)ts.Ticks / (double)TimeSpan.TicksPerSecond)));
      Trace(String.Format("\n"));
    }

    public static void Main(string[] args)
    {
      try
      {
        System.TimeSpan ts;
        System.DateTime begin;

        Trace("CTEST_FULL_OUTPUT (Avoid ctest truncation of output)\n");
        Trace("Begin ManualTestPerformance.Main\n");

        Vehicle v1 = new Vehicle();

        uint i = 0;
        uint maxi = 2999006;//v1.GetMaxIndex();

        Trace("Set/GetValue loop...\n");
        begin = System.DateTime.Now;
        for (i= 0; i<maxi; ++i)
          {
          v1.TSetValue(i, v1.TGetValue(i+1));
          }
        ts = System.DateTime.Now - begin;
        ReportTiming("int", ts, maxi);

        Trace("Set/GetDoubleValue loop...\n");
        begin = System.DateTime.Now;
        for (i = 0; i < maxi; ++i)
          {
          v1.TSetDoubleValue(i, v1.TGetDoubleValue(i+1));
          }
        ts = System.DateTime.Now - begin;
        ReportTiming("double", ts, maxi);

        Trace("Set/GetStringValue loop...\n");
        begin = System.DateTime.Now;
        for (i = 0; i < maxi; ++i)
          {
          v1.TSetStringValue(i, v1.TGetStringValue(i+1));
          }
        ts = System.DateTime.Now - begin;
        ReportTiming("string", ts, maxi);

        Trace("Set/GetObjectValue loop...\n");
        begin = System.DateTime.Now;
        for (i = 0; i < maxi; ++i)
          {
          v1.TSetObjectValue(i, v1.TGetObjectValue(i+1));
          }
        ts = System.DateTime.Now - begin;
        ReportTiming("object", ts, maxi);

        Trace("End ManualTestPerformance.Main\n");
      }
      catch(Exception exc)
      {
        Trace(exc.ToString());
        throw;
      }
    }
  }
}
