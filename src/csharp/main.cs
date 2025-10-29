using System;
using System.IO;
using System.Diagnostics;

class NewlineCounter
{
    static ulong CountNewlines(byte[] buffer, int len)
    {
        ulong count = 0;
        int i = 0;
        
        // 8바이트씩 처리
        while (i + 8 <= len)
        {
            ulong chunk = BitConverter.ToUInt64(buffer, i);
            
            // SWAR 알고리즘
            ulong x = chunk ^ 0x0A0A0A0A0A0A0A0AUL;
            x = (x - 0x0101010101010101UL) & ~x & 0x8080808080808080UL;
            count += ((x >> 7) & 0x0101010101010101UL) * 0x0101010101010101UL >> 56;
            
            i += 8;
        }
        
        // 남은 바이트
        while (i < len)
        {
            if (buffer[i] == '\n') count++;
            i++;
        }
        
        return count;
    }
    
    static void Main(string[] args)
    {
        if (args.Length < 1)
        {
            Console.Error.WriteLine("Usage: {0} <file>", System.AppDomain.CurrentDomain.FriendlyName);
            Environment.Exit(1);
        }
        
        if (!File.Exists(args[0]))
        {
            Console.Error.WriteLine("File not found: {0}", args[0]);
            Environment.Exit(1);
        }
        
        FileInfo fileInfo = new FileInfo(args[0]);
        Console.WriteLine("File size: {0} bytes", fileInfo.Length);
        
        Stopwatch stopwatch = Stopwatch.StartNew();
        
        const int buffer_size = 64 * 1024 * 1024;  // 64MB
        byte[] buffer = new byte[buffer_size];
        
        ulong total = 0;
        
        using (FileStream file = new FileStream(args[0], FileMode.Open, FileAccess.Read, FileShare.Read, buffer_size))
        {
            int n;
            while ((n = file.Read(buffer, 0, buffer_size)) > 0)
            {
                total += CountNewlines(buffer, n);
            }
        }
        
        stopwatch.Stop();
        
        Console.WriteLine("Line count: {0}", total);
        Console.WriteLine("Time: {0:F2} ms", stopwatch.Elapsed.TotalMilliseconds);
    }
}