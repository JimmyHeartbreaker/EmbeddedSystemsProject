using GuitarWizardPro.Services;
using System.Diagnostics;
using System.Drawing;
using System.Drawing.Imaging;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using static System.Net.Mime.MediaTypeNames;
using static System.Windows.Forms.VisualStyles.VisualStyleElement;

namespace GuitarWizardPro.WinForms
{
    public partial class Form1 : Form
    {
        private Bitmap bitmap;
        private int width = 1024; // Width of the image
        private int height = 500; // Height of the image
        private readonly UdpAudioCaptureService udpAudioCaptureService;
        private readonly AudioBufferService audioBufferService;
        private readonly SemaphoreSlim drawSemaphore = new SemaphoreSlim(0);

        public Form1()
        {
         
            InitializeComponent();
            InitializeBitmap();
            udpAudioCaptureService = new UdpAudioCaptureService();
            audioBufferService = new AudioBufferService(udpAudioCaptureService);
            audioBufferService.BufferFull += AudioBufferService_BufferFull;
            Task.Factory.StartNew(UpdateScreen);
             rect = new Rectangle (0, 0, width, height);
        }
        int frames = 0;
        private Rectangle rect;

        private void UpdateScreen()
        {
            int byteLength = width * height*4;
            while (true)
            {
                frames++;
                drawSemaphore.Wait();
                if (frames % 10 == 0)
                {
                    Framerate.Invoke(new Action(() => { Framerate.Text = frames.ToString(); audioFrameCounter.Text = udpAudioCaptureService.frames.ToString(); }));
                }

              
                BitmapData bmpData = bitmap.LockBits(rect, ImageLockMode.WriteOnly, bitmap.PixelFormat);
             

                var buffer = audioBufferService.AudioBuffer;
                var scaler = 498 / 4096.0;
             
                var sqrt2Inv = 1/Math.Sqrt(2);
                unsafe
                {
                    byte* ptr = (byte*)bmpData.Scan0;
                    void plotPointAntiAliasedPoint(double x, double y)
                    {
                        var floorX = (int)Math.Floor(x);
                        var ceilX = (int)Math.Ceiling(x);
                        var floorY = (int)Math.Floor(y);
                        var ceilY = (int)Math.Ceiling(y);

                        var percentFX = Math.Pow(Math.Abs(x - floorX),2);
                        var percentFY = Math.Pow(Math.Abs(y - floorY),2);
                        var percentCX = Math.Pow(Math.Abs(x - ceilX), 2);
                        var percentCY = Math.Pow(Math.Abs(y - ceilY), 2);

                        void plotPoint(double percentXSqrd, double percentYSqrd,int roundedX, int roundedY)
                        {
                            var percent = 1 - Math.Sqrt(percentXSqrd + percentYSqrd)* sqrt2Inv;
                      
                            int screenBufPos = (width * (roundedY<<2) + (roundedX<<2));
                            var brightness = percent * 255;


                            int curValue = ptr[screenBufPos];
                            byte newValue = (byte)Math.Min(255,(brightness) + (curValue));
                            ptr[screenBufPos++] = newValue;
                            ptr[screenBufPos++] = newValue;
                            ptr[screenBufPos] = newValue;

                        }
                        plotPoint(percentFX, percentFY,floorX, floorY);
                        if(floorX!=ceilX)
                        { 
                            plotPoint(percentCX, percentFY, ceilX, floorY);
                            if (floorY != ceilY)
                            {
                                plotPoint(percentFX, percentCY, floorX, ceilY);
                                plotPoint(percentCX, percentCY, ceilX, ceilY);
                            }
                        }
                        else if(floorY != ceilY)
                        {
                            plotPoint(percentFX, percentCY, floorX, ceilY);
                        }

                    }
                    Unsafe.InitBlockUnaligned((void*) bmpData.Scan0, 0, (uint)byteLength);
                    var zoom = 8;
                    for (int i = 0; i < buffer.Length/ zoom - 1; i++)
                    {
                        var y0 = 498 - (int)(buffer[i] * scaler);
                        var y1 = 498 - (int)(buffer[i + 1] * scaler);
                        var x0 = i* zoom;
                        var x1 = (i + 1) * zoom;
                        var yChange = (y1 - y0);
                        
                        var xChangePerY = zoom / (double)Math.Abs(yChange );
                    
                        if(y1==y0 || xChangePerY >=1 )
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

                // Unlock the bits.
                bitmap.UnlockBits(bmpData);
               // graphics.DrawImage(bitmap);
                    // Call this same method but append THREAD2 to the text
                pictureBox.Invoke(() => pictureBox.Refresh());
            
                //   // Refresh the PictureBox to display changes

            }
        }
        //var yFloor = (int)Math.Floor(y);
        //var yRemainderFloor = y - yFloor;


        //x += xChangePerY;

        //                        var xFloor = (int)Math.Floor(x);
        //var xRemainderFloor = x - xFloor;
        //uint darkness = (uint)(white * (xRemainderFloor + yRemainderFloor));
        //uint invDarkness = (uint)(255 - white * (xRemainderFloor + yRemainderFloor));

        //var xCeil = (int)Math.Ceiling(x);
        //uint floorColour = 0xFF000000 | (darkness << 16) | (darkness << 8) | (darkness);
        //uint ceilingColour = 0xFF000000 | (invDarkness << 16) | (invDarkness << 8) | (invDarkness);
        //int screenBufPos1 = (width * yFloor + xFloor);
        //int screenBufPos2 = (width * yFloor + xCeil);
        //ptr[screenBufPos1] = floorColour;
        //                        ptr[screenBufPos2] = ceilingColour;
      
        private unsafe void AudioBufferService_BufferFull(object? sender, EventArgs e)
        {
            drawSemaphore.Release();
            // Lock the bitmap's bits.
          
        }

        private void InitializeBitmap()
        {
            bitmap = new Bitmap(width, height, PixelFormat.Format32bppRgb);
           
            pictureBox.Image = bitmap; // 'pictureBox' is a WinForms PictureBox control
            
            pictureBox.MouseClick += PictureBox_MouseClick; ;
        }

        private void PictureBox_MouseClick(object? sender, MouseEventArgs e)
        {
            udpAudioCaptureService.Start();
        }


      

        

    }
}
