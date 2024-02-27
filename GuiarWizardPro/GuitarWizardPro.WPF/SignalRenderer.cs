using GuitarWizardPro.Services;
using System;
using System.Runtime.CompilerServices;
using System.Windows;
using System.Windows.Media.Imaging;
using System.Windows.Threading;

namespace GuitarWizardPro.WPF
{
    internal class SignalRenderer
    {
        private WriteableBitmap bitmap;
        private readonly int byteLength;
        private readonly double scaler;
        private readonly double sqrt2Inv;
        private int width = 1024;
        private int height = 500;
        private readonly AudioBufferService audioBufferService;
        private Dispatcher dispatcher;
        private readonly SemaphoreSlim ssStartWriteBits;
        private readonly SemaphoreSlim ssEndWriteBits;
        private readonly bool antialiasing = true;
        private unsafe byte* ptr;
        private CancellationTokenSource cancellationTokenSource;

        public SignalRenderer(AudioBufferService bufferService,WriteableBitmap writeableBitmap, Dispatcher dispatcher)
        {
            this.audioBufferService = bufferService;
            audioBufferService.BufferFull += AudioBufferService_BufferFull;
            this.dispatcher = dispatcher;
            bitmap = writeableBitmap;
            ssStartWriteBits = new SemaphoreSlim(0);
            ssEndWriteBits = new SemaphoreSlim(0);
            byteLength = width * height * 4;
            scaler = 498 / 4096.0;
            sqrt2Inv = 1 / Math.Sqrt(2);
          //  Task.Factory.StartNew(UpdatePixels);
            cancellationTokenSource = new CancellationTokenSource();

        }

        private async void AudioBufferService_BufferFull(object? sender, EventArgs e)
        {
            await UpdateBitmap();
        }
        
        private unsafe void UpdatePixels()
        {
            var buffer = audioBufferService.AudioBuffer;
            while (true)
            {
                ssEndWriteBits.Release();
                ssStartWriteBits.Wait();

                DrawPixels(buffer);
            }
        }

        private unsafe void DrawPixels(short* buffer)
        {
            void plotPoint(double x, double y)
            {
                var floorX = (int)x;
                var floorY = (int)y;
                int screenBufPos = (width * (floorY << 2) + (floorX << 2));

                byte newValue = 255;
                ptr[screenBufPos++] = newValue;
                ptr[screenBufPos++] = newValue;
                ptr[screenBufPos] = newValue;
            }

            void plotAntiAliasedPoint(double x, double y)
            {
                var floorX = (int)Math.Floor(x);
                var ceilX = (int)Math.Ceiling(x);
                var floorY = (int)Math.Floor(y);
                var ceilY = (int)Math.Ceiling(y);

                var percentFX = Math.Pow(Math.Abs(x - floorX), 2);
                var percentFY = Math.Pow(Math.Abs(y - floorY), 2);
                var percentCX = Math.Pow(Math.Abs(x - ceilX), 2);
                var percentCY = Math.Pow(Math.Abs(y - ceilY), 2);

                void plotPoint(double percentXSqrd, double percentYSqrd, int roundedX, int roundedY)
                {
                    var percent = 1 - Math.Sqrt(percentXSqrd + percentYSqrd) * sqrt2Inv;

                    int screenBufPos = (width * (roundedY << 2) + (roundedX << 2));
                    var brightness = percent * 255;


                    int curValue = ptr[screenBufPos];
                    byte newValue = (byte)Math.Min(255, (brightness) + (curValue));
                    ptr[screenBufPos++] = newValue;
                    ptr[screenBufPos++] = newValue;
                    ptr[screenBufPos] = newValue;

                }
                plotPoint(percentFX, percentFY, floorX, floorY);
                if (floorX != ceilX)
                {
                    plotPoint(percentCX, percentFY, ceilX, floorY);
                    if (floorY != ceilY)
                    {
                        plotPoint(percentFX, percentCY, floorX, ceilY);
                        plotPoint(percentCX, percentCY, ceilX, ceilY);
                    }
                }
                else if (floorY != ceilY)
                {
                    plotPoint(percentFX, percentCY, floorX, ceilY);
                }

            }
            Action<double, double> plotFunction = antialiasing ? plotAntiAliasedPoint : plotPoint;

            Unsafe.InitBlockUnaligned(ptr, 0, (uint)byteLength);
            var zoom = 4;
            for (int i = 0; i < AudioBufferService.CAPACITY / zoom - 1; i++)
            {
                var y0 = 498 - (int)(buffer[i] * scaler);
                var y1 = 498 - (int)(buffer[i + 1] * scaler);
                var x0 = i * zoom;
                var x1 = (i + 1) * zoom;
                var yChange = (y1 - y0);

                var xChangePerY = zoom / (double)Math.Abs(yChange);

                if (y1 == y0 || xChangePerY >= 1)
                {
                    var xChange = (x1 - x0);
                    var yChangePerX = yChange / (double)Math.Abs(xChange);
                    double y = y0;
                    for (double x = x0; x < x1; x++, y += yChangePerX)
                    {
                        plotFunction(x, y);
                    }

                }
                else if (y1 > y0)
                {
                    double x = x0;
                    for (double y = y0; y < y1; y++, x += xChangePerY)
                    {
                        plotFunction(x, y);
                    }
                }
                else
                {
                    double x = x0;
                    for (double y = y0; y > y1; y--, x += xChangePerY)
                    {
                        plotFunction(x, y);
                    }
                }

            }
        }

        SemaphoreSlim ss = new SemaphoreSlim(1);
        private async Task UpdateBitmap()
        {

            try
            {
                if(ss.CurrentCount>1)
                {
                    return;
                }
                await ss.WaitAsync();
                await dispatcher.InvokeAsync(() =>
                {
                    if (cancellationTokenSource.Token.IsCancellationRequested)
                        return;

                    bitmap.Lock();
                    unsafe
                    {
                        ptr = (byte*)bitmap.BackBuffer;
                        DrawPixels(audioBufferService.AudioBuffer);
                    }
                    
                  //  ssStartWriteBits.Release();
                  //  ssEndWriteBits.Wait();
                    bitmap.AddDirtyRect(new Int32Rect(0, 0, width, height));
                    bitmap.Unlock();
                }, DispatcherPriority.Render, cancellationTokenSource.Token);
                ss.Release();
            }
            catch (TaskCanceledException e)
            {

            }
                        

             
        }
       
    }
}
