using GuitarWizardPro.Services;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Documents;

namespace GuitarWizardPro
{
    public class FFTService : INotifyPropertyChanged
    {
        private readonly AudioBufferService audioBufferService;

        public unsafe FFTService(AudioBufferService audioBufferService)
        {
            this.audioBufferService = audioBufferService;
            audioBufferService.BufferFull += AudioBufferService_BufferFull;

        }

        const int Size = 1024;
        readonly double[] data = new double[Size];
        private int[] topFrequencies;

        public event PropertyChangedEventHandler? PropertyChanged;

        public int[] TopFrequencies
        {
            get => topFrequencies; set
            {
                topFrequencies = value;
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(TopFrequencies)));
            }
        }
        private unsafe void AudioBufferService_BufferFull(object? sender, EventArgs e)
        {
            var buffer = audioBufferService.AudioBuffer;

            for (int i =0;i < Size;i++)
            {
                data[i] = buffer[i]*1000;
            }
            Task.Factory.StartNew(() =>
            {
                System.Numerics.Complex[] spectrum = FftSharp.FFT.Forward(data);
                double[] psd = FftSharp.FFT.Power(spectrum);
                    
                double[] freq = FftSharp.FFT.FrequencyScale(psd.Length, 32000);

                TopFrequencies = psd.Zip(freq, (pow, freq) => (pow, freq)).OrderByDescending(x => x.pow).Where(x=>x.freq!=0).Select(x=>(int)Math.Round(x.freq)).Take(6).ToArray();

            });
        }

     

       
    }
}
