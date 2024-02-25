using SkiaSharp.Views.Maui.Controls;
using SkiaSharp;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using SkiaSharp.Views.Maui;

namespace GuitarWizardPro
{
    public class ImageEditor : SKCanvasView
    {
   
        SKBitmap _bitmap = new SKBitmap(1000,500,SKColorType.Rgba8888,SKAlphaType.Opaque);
        public ImageEditor()
        {
            _bitmap.SetPixel(0, 0, new SKColor(100, 100, 100, 255));
        }
       

        protected override void OnPaintSurface(SKPaintSurfaceEventArgs e)
        {
            
            var canvas = e.Surface.Canvas;
            var info = e.Info;
            canvas.Clear(new SKColor(0, 0, 0));
           // canvas.DrawCircle(0, 0, 100,new SKPaint(new SKFont() { }) { Color = new SKColor(100,100,100)});
         //   canvas.Clear();
            canvas.DrawBitmap(_bitmap, info.Rect);
        }


    }
}
