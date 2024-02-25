using GuitarWizardPro.Services;
using SkiaSharp;
using SkiaSharp.Views.Desktop;
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
        SKBitmap _bitmap = new SKBitmap(100, 50, SKColorType.Rgba8888, SKAlphaType.Opaque);

        public SignalPane(AudioBufferService bufferService)
        {
            this.audioBufferService = bufferService;
        }
        public void Draw(ICanvas canvas, RectF dirtyRect)
        {
            if (_bitmap == null)
                return;
          //  canvas.DrawImage(new SkiaSharp.SKImage(_bitmap), dirtyRect.X, dirtyRect.Y, dirtyRect.Width, dirtyRect.Height);
          
            //var buffer = audioBufferService.AudioBuffer;

            //for(int i = 0; i < 999; i++) 
            //{
            //    var data0 = buffer[i] / 10;
            //    var data1 = buffer[i + 1] / 10;
            //    canvas.DrawLine(i, data0, i + 1, data1);
            //}
        }

       
    }
}
