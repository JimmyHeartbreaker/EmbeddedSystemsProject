
using GuitarWizardPro.Services.Interfaces;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace GuitarWizardPro.Services
{
    public class AudioBufferService
    {
        public event EventHandler? BufferFull;


        public const int CAPACITY = 1024;
        private unsafe short* primaryBuffer;
        private unsafe short* secondaryBuffer;
        private int bufferPosition = 0;

        public unsafe short* AudioBuffer => primaryBuffer;

        public unsafe AudioBufferService(IAudioCapureService audioCapureService)
        {
            audioCapureService.AudioFrameProcessed += AudioCapureService_AudioFrameProcessed;
            primaryBuffer = (short*)Marshal.AllocHGlobal(2 * CAPACITY);
            secondaryBuffer = (short*)Marshal.AllocHGlobal(2 * CAPACITY);
        }

        private unsafe void SwapBuffers()
        {
            var temp = secondaryBuffer;
            secondaryBuffer = primaryBuffer;
            primaryBuffer = temp;
            bufferPosition = 0;

        }
        private unsafe void AudioCapureService_AudioFrameProcessed(object? sender, (nint, int) e)
        {
            short* data = (short*)e.Item1;
            int nItems = e.Item2 / 2;
            var copyLen = Math.Min(nItems, CAPACITY - bufferPosition);

            if (bufferPosition < CAPACITY)
            {
                Buffer.MemoryCopy(data, secondaryBuffer + bufferPosition, copyLen * 2, copyLen * 2);
                bufferPosition += copyLen;
                if (bufferPosition == CAPACITY)
                {
                    
                    BufferFull?.Invoke(this, EventArgs.Empty);
                    SwapBuffers();
                }
            }
        }



    }
}
