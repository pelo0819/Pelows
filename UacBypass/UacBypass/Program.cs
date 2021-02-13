using System;
using System.IO;
using Microsoft.Win32;
using System.Linq;

namespace UacBypass
{
    class Program
    {
        static void Main(string[] args)
        {
#if false
            string cont = "Good Morning";
            File.WriteAllText(@"C:\Windows\System32\test_2\test3.txt", cont);
            Console.WriteLine("Hello World!");
#endif
            var path = "SOFTWARE\\Microsoft\\Windows\\Notepad\\Capabilities";
            RegistryKey key = Registry.LocalMachine.OpenSubKey(path);
            //Console.WriteLine(key.GetValue("ApplicationName"));
            var names = key.GetValueNames().ToList();
            //names?.ToList().ForEach(_ =>
            //{
            //    Console.WriteLine(_);
            //});

            foreach(var (n, idx) in names.Select((n, idx) => (n, idx)))
            {
                var value = key.GetValue(n);
                Console.WriteLine($"name:{n}, value:{value}");
            }

            key = Registry.LocalMachine.CreateSubKey("SOFTWARE\\TestWork\\Test");
            key.SetValue("TestName", "TestValue");

            key.Close();
        }
    }
}
