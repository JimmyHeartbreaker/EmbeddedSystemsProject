
using GuitarWizardPro.Services.Interfaces;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Runtime.Intrinsics.X86;
using System.Text;
using System.Threading.Tasks;

namespace GuitarWizardPro.Services
{
    public class AudioBufferService
    {
        public event EventHandler? BufferFull;


        public const int CAPACITY = 50*21;
        private unsafe short* primaryBuffer;
        private unsafe short* secondaryBuffer;
        private int bufferPosition = 0;

        public unsafe short* AudioBuffer => primaryBuffer;

        public unsafe AudioBufferService(IAudioCapureService audioCapureService)
        {
            audioCapureService.AudioFrameProcessed += AudioCapureService_AudioFrameProcessed;
            primaryBuffer = (short*)Marshal.AllocHGlobal(2 * CAPACITY);
            secondaryBuffer = (short*)Marshal.AllocHGlobal(2 * CAPACITY);
            Task.Factory.StartNew(OnBufferFull);
        }
        SemaphoreSlim _semaphore = new SemaphoreSlim(0);
        private async Task OnBufferFull()
        {
            while(true)
            {
                await _semaphore.WaitAsync();
                BufferFull?.Invoke(this, EventArgs.Empty);
            }
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
            byte* data = (byte*)e.Item1;
            int nItems = e.Item2 / 2;
            var copyLen = Math.Min(nItems, CAPACITY - bufferPosition);

            if (bufferPosition < CAPACITY)
            {
                short* outBuf = secondaryBuffer + bufferPosition;
                byte* outBufPos = (byte*)outBuf;

                byte* endBuf = outBufPos + 42;
                byte* bufPos = data;
                int i = 0;
                bool even = true;
                while (outBufPos < endBuf)
                {
                    byte* bp = bufPos + ((i * 3) >> 1);

                    if (even)
                    {
                        *outBufPos = *bp;
                        *(outBufPos + 1) = (byte)(((*(bp + 1)) & 0xF0) >> 4);
                    }
                    else
                    {
                        *outBufPos = (*(bp + 1));
                        *(outBufPos + 1) = (byte)((*bp) & 0x0F);
                    }

                    short* px = (short*)(outBufPos);
                    short raw = *px;
                    
                    //apply basic filtering
                  //  float yn = (yn1 * w0) + ((*px) * (1 - w0));
                    *px = Math.Min((short)4095, raw);
                    

                    //monitor activity by taking the absolute difference of the last sample and this sample
                    i++;
                    even = !even;
                    outBufPos += 2;
                }
                bufferPosition += 21;
                if (bufferPosition == CAPACITY)
                {
                    SwapBuffers();
                    _semaphore.Release();
                }
            }
        }



    }
}
