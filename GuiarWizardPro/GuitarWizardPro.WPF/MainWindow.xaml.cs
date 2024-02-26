using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Drawing;
using Rectangle = System.Drawing.Rectangle;
using System.Drawing.Imaging;
using GuitarWizardPro.Services;
using System.Runtime.CompilerServices;
using System.Data.Common;

namespace GuitarWizardPro.WPF
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
     //   private WriteableBitmap bitmap;
        private int width = 1024; // Width of the image
        private int height = 500; // Height of the image
        private readonly UdpAudioCaptureService udpAudioCaptureService;
        private readonly AudioBufferService audioBufferService;
        private readonly SemaphoreSlim drawSemaphore = new SemaphoreSlim(0);
       // private readonly Rectangle rect;

        public MainWindow()
        {
            InitializeComponent();
            RenderOptions.SetBitmapScalingMode(image, BitmapScalingMode.NearestNeighbor);
            RenderOptions.SetEdgeMode(image, EdgeMode.Aliased);
             udpAudioCaptureService = new UdpAudioCaptureService();
            audioBufferService = new AudioBufferService(udpAudioCaptureService);
            audioBufferService.BufferFull += AudioBufferService_BufferFull;
            Task.Factory.StartNew(UpdateScreen);
         //   rect = new Rectangle(0, 0, width, height);

        }

        private void UpdateScreen()
        {
            var bitmap = new WriteableBitmap(width, height, 96, 96, PixelFormats.Bgr32, null);
           Dispatcher.Invoke(()=>  image.Source = bitmap);

            int byteLength = width * height * 4;
            while (true)
            {
                drawSemaphore.Wait();



                bitmap.Lock();


                var buffer = audioBufferService.AudioBuffer;
                var scaler = 498 / 4096.0;

                var sqrt2Inv = 1 / Math.Sqrt(2);
                unsafe
                {
                    byte* ptr = (byte*)bitmap.BackBuffer;
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

                    Unsafe.InitBlockUnaligned((void*)bitmap.BackBuffer, 0, (uint)byteLength);
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
                bitmap.AddDirtyRect(new Int32Rect(0, 0, width, height));

                // Unlock the bits.
                bitmap.Unlock();
                //Clean up original bitmap
                
                // graphics.DrawImage(bitmap);
                // Call this same method but append THREAD2 to the text
                //     pictureBox.Invoke(() => pictureBox.Refresh());

                //   // Refresh the PictureBox to display changes

            }
        }

        private unsafe void AudioBufferService_BufferFull(object? sender, EventArgs e)
        {
            drawSemaphore.Release();
            // Lock the bitmap's bits.

        }

        private void image_MouseDown(object sender, MouseButtonEventArgs e)
        {
        }

        private void image_Loaded(object sender, RoutedEventArgs e)
        {
            udpAudioCaptureService.Start();

        }
    }
}