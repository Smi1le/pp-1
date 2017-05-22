﻿using System;
using System.IO;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DictionaryCompressor
{
    class Program
    {

        static void Main(string[] args)
        {
            if (args.Length != 2)
            {
                Console.WriteLine("Usage program.exe <input_file.txt> <output_file.txt>");
                return;
            }
            Console.WriteLine(args[0]);
            Console.WriteLine(args[1]);
            IOManager man = new IOManager(args[0], args[1]);
            Application app = new Application(new IOManager(args[0], args[1]));
            app.ProcessFile();
            app.OutputResultsInFile();
            Console.WriteLine("Results output in file");
            //string data = man.GetViewMappingFile();
            //Console.WriteLine(data);
            //Console.WriteLine(man.GetViewMappingFile().Length);

            /*using (StreamWriter sw = new StreamWriter("out.txt", false, System.Text.Encoding.Default))
            {
                sw.WriteLine(data);
            }*/
        }
    }
}
