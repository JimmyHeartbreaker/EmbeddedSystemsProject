
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


        const int CAPACITY = 1024;
        private unsafe short* buffer;
        private int bufferPosition = 0;

        public unsafe Span<short> AudioBuffer => new(buffer, CAPACITY);

        public unsafe AudioBufferService(IAudioCapureService audioCapureService)
        {
            audioCapureService.AudioFrameProcessed += AudioCapureService_AudioFrameProcessed;
            buffer = (short*)Marshal.AllocHGlobal(2 * CAPACITY);
        }
        int frames;
        public int FrameCount => frames;
        private unsafe void AudioCapureService_AudioFrameProcessed(object? sender, (nint, int) e)
        {
            short* data = (short*)e.Item1;
            int nItems = e.Item2 / 2;
            var copyLen = Math.Min(nItems, CAPACITY - bufferPosition);

            if (bufferPosition < CAPACITY)
            {
                Buffer.MemoryCopy(data, buffer + bufferPosition, copyLen * 2, copyLen * 2);
                bufferPosition += copyLen;
                if (bufferPosition == CAPACITY)
                {
                    
                    BufferFull?.Invoke(this, EventArgs.Empty);
                    ResetBuffer();
                }
            }
        }

        public void ResetBuffer()
        {
            frames++;
            bufferPosition = 0;
        }


    }
}
