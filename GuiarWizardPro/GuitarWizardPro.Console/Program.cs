using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Input;
using System.Runtime.CompilerServices;
using System.Windows.Media.Media3D;
using System.Windows.Threading;
using GuitarWizardPro.Services;

namespace GuitarWizardPro.Console
{
    class Program
    {

        private static  UdpAudioCaptureService udpAudioCaptureService;
        private static  AudioBufferService audioBufferService;
        private static  SemaphoreSlim drawSemaphore = new SemaphoreSlim(0);

        static WriteableBitmap writeableBitmap;
        static Window w;
        static Image i;

        private static int width = 1024; // Width of the image
        private static int height = 500; // Height of the image

        [STAThread]
        static void Main(string[] args)
        {
            i = new Image();
            RenderOptions.SetBitmapScalingMode(i, BitmapScalingMode.NearestNeighbor);
          //  RenderOptions.SetEdgeMode(i, EdgeMode.Aliased);

            w = new Window();
            w.Width = width;
            w.Height = height;
               
            w.Content = i;
            w.Show();

            writeableBitmap = new WriteableBitmap(
                (int)w.ActualWidth,
                (int)w.ActualHeight,
                96,
                96,
                PixelFormats.Bgr32,
                null);

            i.Source = writeableBitmap;

            i.Stretch = Stretch.None;
            i.HorizontalAlignment = HorizontalAlignment.Left;
            i.VerticalAlignment = VerticalAlignment.Top;

            i.MouseMove += new MouseEventHandler(i_MouseMove);
            i.MouseLeftButtonDown +=
                new MouseButtonEventHandler(i_MouseLeftButtonDown);
            i.MouseRightButtonDown +=
                new MouseButtonEventHandler(i_MouseRightButtonDown);

            w.MouseWheel += new MouseWheelEventHandler(w_MouseWheel);

            udpAudioCaptureService = new UdpAudioCaptureService();
            audioBufferService = new AudioBufferService(udpAudioCaptureService);
            audioBufferService.BufferFull += AudioBufferService_BufferFull;
       //     Task.Factory.StartNew(UpdateScreen);
            udpAudioCaptureService.Start();
            Application app = new Application();
            app.Run();
        }

        private static void AudioBufferService_BufferFull(object? sender, EventArgs e)
        {
            w.Dispatcher.Invoke(UpdateScreen);
       //     drawSemaphore.Release();
        }

        private static void UpdateScreen()
        {

            int byteLength = width * height * 4;
          //  while (true)
           // {
             //   drawSemaphore.Wait();



                writeableBitmap.Lock();


                var buffer = audioBufferService.AudioBuffer;
                var scaler = 498 / 4096.0;

                var sqrt2Inv = 1 / Math.Sqrt(2);
                unsafe
                {
                    byte* ptr = (byte*)writeableBitmap.BackBuffer;
                    void plotPointAntiAliasedPoint(double x, double y)
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

                    Unsafe.InitBlockUnaligned((void*)writeableBitmap.BackBuffer, 0, (uint)byteLength);
                    var zoom = 8;
                    for (int i = 0; i < buffer.Length / zoom - 1; i++)
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
                                plotPointAntiAliasedPoint(x, y);
                            }

                        }
                        else if (y1 > y0)
                        {
                            double x = x0;
                            for (double y = y0; y < y1; y++, x += xChangePerY)
                            {
                                plotPointAntiAliasedPoint(x, y);
                            }
                        }
                        else
                        {
                            double x = x0;
                            for (double y = y0; y > y1; y--, x += xChangePerY)
                            {
                                plotPointAntiAliasedPoint(x, y);
                            }
                        }

                    }
                }
                // Manipulate the pixel buffer


                // Copy the RGB values back to the bitmap
                // System.Runtime.InteropServices.Marshal.Copy(rgbValues, 0, ptr, bytes);
                writeableBitmap.AddDirtyRect(new Int32Rect(0, 0, width, height));

                // Unlock the bits.
                writeableBitmap.Unlock();
                //Clean up original bitmap

                // graphics.DrawImage(bitmap);
                // Call this same method but append THREAD2 to the text
                //     pictureBox.Invoke(() => pictureBox.Refresh());

                //   // Refresh the PictureBox to display changes

            //}
        }


        // The DrawPixel method updates the WriteableBitmap by using
        // unsafe code to write a pixel into the back buffer.
        static void DrawPixel(MouseEventArgs e)
        {
      
        }

        static void ErasePixel(MouseEventArgs e)
        {
            byte[] ColorData = { 0, 0, 0, 0 }; // B G R

            Int32Rect rect = new Int32Rect(
                    (int)(e.GetPosition(i).X),
                    (int)(e.GetPosition(i).Y),
                    1,
                    1);

            writeableBitmap.WritePixels(rect, ColorData, 4, 0);
        }

        static void i_MouseRightButtonDown(object sender, MouseButtonEventArgs e)
        {
            ErasePixel(e);
        }

        static void i_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            DrawPixel(e);
        }

        static void i_MouseMove(object sender, MouseEventArgs e)
        {
            if (e.LeftButton == MouseButtonState.Pressed)
            {
                DrawPixel(e);
            }
            else if (e.RightButton == MouseButtonState.Pressed)
            {
                ErasePixel(e);
            }
        }

        static void w_MouseWheel(object sender, MouseWheelEventArgs e)
        {
            System.Windows.Media.Matrix m = i.RenderTransform.Value;

            if (e.Delta > 0)
            {
                m.ScaleAt(
                    1.5,
                    1.5,
                    e.GetPosition(w).X,
                    e.GetPosition(w).Y);
            }
            else
            {
                m.ScaleAt(
                    1.0 / 1.5,
                    1.0 / 1.5,
                    e.GetPosition(w).X,
                    e.GetPosition(w).Y);
            }

            i.RenderTransform = new MatrixTransform(m);
        }
    }
}