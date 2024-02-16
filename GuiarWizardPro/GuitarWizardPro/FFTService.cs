using GuitarWizardPro.Services;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace GuitarWizardPro
{
    public class FFTService 
    {
        private readonly AudioBufferService audioBufferService;

        public unsafe FFTService(AudioBufferService audioBufferService)
        {
            this.audioBufferService = audioBufferService;
            audioBufferService.BufferFull += AudioBufferService_BufferFull;

        }

        const int Size = 1024;
        readonly double[] data = new double[Size];
        private void AudioBufferService_BufferFull(object? sender, EventArgs e)
        {
            var buffer = audioBufferService.AudioBuffer;
            for(int i =0;i < Size;i++)
            {
                data[i] = buffer[i]*1000;
            }
            Task.Factory.StartNew(() =>
            {
                    System.Numerics.Complex[] spectrum = FftSharp.FFT.Forward(data);
                    double[] psd = FftSharp.FFT.Power(spectrum);
                    
                    double[] freq = FftSharp.FFT.FrequencyScale(psd.Length, 48000);

                var zipped = psd.Zip(freq, (pow, freq) => (pow, freq)).OrderByDescending(x => x.pow).ToArray();

            });
        }

     

       
    }
}
