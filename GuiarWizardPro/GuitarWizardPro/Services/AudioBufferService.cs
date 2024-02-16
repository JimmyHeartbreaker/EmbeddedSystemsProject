
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


        const int CAPACITY = 2000;
        private unsafe float* buffer;
        private int bufferPosition = 0;

        public unsafe Span<float> AudioBuffer => new(buffer, CAPACITY);

        public unsafe AudioBufferService(IAudioCapureService audioCapureService)
        {
            audioCapureService.AudioFrameProcessed += AudioCapureService_AudioFrameProcessed;
            buffer = (float*)Marshal.AllocHGlobal(4 * CAPACITY);
        }

        private unsafe void AudioCapureService_AudioFrameProcessed(object? sender, (nint, int) e)
        {
            float* data = (float*)e.Item1;
            int len = e.Item2 / 4;
            var copyLen = Math.Min(len, CAPACITY - bufferPosition);

            if (bufferPosition < CAPACITY)
            {
                Buffer.MemoryCopy(data, buffer + bufferPosition, copyLen * 4, copyLen * 4);
                bufferPosition += copyLen;

                if (bufferPosition == CAPACITY)
                {
                    BufferFull?.Invoke(this, EventArgs.Empty);
                }
            }
        }

        internal void ResetBuffer()
        {
            bufferPosition = 0;
        }


    }
}
