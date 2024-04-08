// See https://aka.ms/new-console-template for more information
using System.Text;


using (var fileWrite = File.OpenWrite("waveform.csv"))
{
    using (var fileWriter = new StreamWriter(fileWrite))
    {
        using (var fileRead = File.OpenRead("waveform.log"))
        {
            using (var streamReader = new StreamReader(fileRead))
            {
                var lines = streamReader.ReadToEnd().Split(System.Environment.NewLine);
                foreach (var line in lines)
                {
                    if (line.Length == 42)
                    {
                        var bytes = Encoding.ASCII.GetBytes(line);
                        for (int i = 0; i < 21; i++)
                        {
                            fileWriter.Write(BitConverter.ToUInt16(bytes, i * 2));

                            fileWriter.WriteLine(',');

                        }

                    }
                }
            }
        }
    }
    //byte[] buffer = new byte[2];
    //while (fileRead.Read(buffer, 0, 2) > 0)
    //{

    //    var sample = BitConverter.ToUInt16(buffer);
    //    Console.WriteLine(sample);
    //}
}