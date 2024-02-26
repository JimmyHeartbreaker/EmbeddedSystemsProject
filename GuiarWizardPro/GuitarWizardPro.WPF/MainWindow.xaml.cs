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
using GuitarWizardPro.Services.Interfaces;

namespace GuitarWizardPro.WPF
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private readonly UdpAudioCaptureService udpAudioCaptureService;
        private readonly AudioBufferService audioBufferService;
        private readonly SignalRenderer signalRenderer;
        private readonly int width = 1024;
        private readonly int height = 500;
        private readonly FFTService fFTService;
        private readonly WifiAuthBLEService wifiAuthBLEService;

        public MainWindow()
        {
            InitializeComponent();
           
             udpAudioCaptureService = new UdpAudioCaptureService();
            audioBufferService = new AudioBufferService(udpAudioCaptureService);

            var bitmap = new WriteableBitmap(width, height, 96, 96, PixelFormats.Bgr32, null);
            signalRenderer = new SignalRenderer(audioBufferService, bitmap, Dispatcher);           
            image.Source = bitmap;
            fFTService = new FFTService(audioBufferService);
            FFTItems.DataContext = fFTService;
             wifiAuthBLEService = new WifiAuthBLEService();

        }

        

        private async void image_Loaded(object sender, RoutedEventArgs e)
        {
            await wifiAuthBLEService.Start();
            udpAudioCaptureService.Start();

        }
    }
}