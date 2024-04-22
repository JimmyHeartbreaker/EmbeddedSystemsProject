////// See https://aka.ms/new-console-template for more information
//using System.Text;
//using static System.Runtime.InteropServices.JavaScript.JSType;

//using(var fileRead = File.OpenRead("waveform.csv"))
//{
//    using(var reader = new StreamReader(fileRead))
//    {
//        var s = reader.ReadToEnd();
//        var data =s.Split(System.Environment.NewLine,StringSplitOptions.RemoveEmptyEntries).Select(v => double.Parse(v)).ToArray();
//        System.Numerics.Complex[] spectrum = FftSharp.FFT.Forward(data.Take(16384).ToArray());
//        double[] psd = FftSharp.FFT.Power(spectrum);

//        double[] freq = FftSharp.FFT.FrequencyScale(psd.Length, 32000);

//        var TopFrequencies = psd.Zip(freq, (pow, freq) => (pow, freq)).OrderByDescending(x => x.pow).Where(x => x.freq != 0 && x.freq < 1000).Select(x => (int)Math.Round(x.freq)).Take(100).ToArray();

//    }
//}

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
    //byte[] buffer = new byte[2];
    //while (fileRead.Read(buffer, 0, 2) > 0)
    //{

    //    var sample = BitConverter.ToUInt16(buffer);
    //    Console.WriteLine(sample);
    //}
}