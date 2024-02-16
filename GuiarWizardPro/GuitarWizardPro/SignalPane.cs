using GuitarWizardPro.Services;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GuitarWizardPro
{
    internal class SignalPane : IDrawable
    {
        private readonly AudioBufferService audioBufferService;

        public SignalPane(AudioBufferService bufferService)
        {
            this.audioBufferService = bufferService;
        }
        public void Draw(ICanvas canvas, RectF dirtyRect)
        {
            var buffer = audioBufferService.AudioBuffer;
          
            for(int i = 0; i < buffer.Length-1; i++) 
            {
                var data0 = buffer[i] * 100000 + 250;
                var data1 = buffer[i+1] * 100000 + 250;
                canvas.DrawLine(i, data0, i + 1, data1);
            }
        }
    }
}
