using System.Text;

using (var fileWrite = File.OpenWrite("waveform.csv"))
{
    using (var fileWriter = new StreamWriter(fileWrite))
    {
        using (var fileRead = File.OpenRead("putty.log"))
        {
            using (var streamReader = new StreamReader(fileRead))
            {
                var lines = streamReader.ReadToEnd().Split(System.Environment.NewLine);
                foreach (var line in lines)
                {
                    if (line.Length == 32)
                    {
                        var bytes = Encoding.ASCII.GetBytes(line);
                        for (int i = 0; i < 16; i++)
                        {
                            fileWriter.Write(BitConverter.ToUInt16(bytes, i * 2));

                            fileWriter.WriteLine(',');

                        }

                    }
                }
            }
        }
    }
 
}