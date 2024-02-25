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

namespace GuitarWizardPro.WPF
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
        }
         [System.Runtime.InteropServices.DllImport("gdi32.dll")]
        public static extern bool DeleteObject(IntPtr hObject);
        public unsafe void Test()
        {
                System.Drawing.Point point = new System.Drawing.Point(320, 240);
                IntPtr hBmp;
                Bitmap bmp = new Bitmap(640, 480);
            Rectangle lockRegion = new Rectangle(0, 0, 640, 480);
                BitmapData data = bmp.LockBits(lockRegion, ImageLockMode.ReadWrite, System.Drawing.Imaging.PixelFormat.Format24bppRgb);
                byte* p;
            System.Drawing.safen
                p = (byte*)data.Scan0 + (point.Y * data.Stride) + (point.X * 3);
                p[0] = 0; //B pixel
                p[1] = 255; //G pixel
                p[2] = 255; //R pixel

                bmp.UnlockBits(data);

                //Convert the bitmap to BitmapSource for use with WPF controls
                hBmp = bmp.GetHbitmap();
                image.Source = System.Windows.Interop.Imaging.CreateBitmapSourceFromHBitmap(hBmp, IntPtr.Zero, Int32Rect.Empty, System.Windows.Media.Imaging.BitmapSizeOptions.FromEmptyOptions());
                image.Source.Freeze();
                DeleteObject(hBmp); //Clean up original bitmap
            
        }
    }
}