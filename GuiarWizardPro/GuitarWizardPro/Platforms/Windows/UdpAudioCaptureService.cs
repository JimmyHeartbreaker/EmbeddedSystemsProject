using System.Collections.ObjectModel;
using Windows.Devices.Enumeration;
using Windows.Media.Audio;
using Windows.Media.Capture;
using Windows.Media.Devices;
using Windows.Media.MediaProperties;
using Windows.Media.Render;
using Windows.Media.Transcoding;
using Windows.Storage;
using System.Runtime.InteropServices;
using WinRT;
using Windows.Media.Capture.Frames;
using Windows.Media;
using Windows.Foundation;
using Microsoft.Maui.Platform;
using System.Timers;
using System.Collections.Concurrent;
using System.Runtime.CompilerServices;
using Windows.UI.Popups;
using Microsoft.Maui.Storage;
using GuitarWizardPro.Services.Interfaces;
using System.Diagnostics;
using System.Net.Sockets;
using System.Net;
using Windows.Media.Protection.PlayReady;

namespace GuitarWizardPro.Platforms.Windows
{
   

    public  class UdpAudioCaptureService : IAudioCapureService
    {
        private readonly UdpClient udpClient;
        private IPEndPoint groupEP = new IPEndPoint(IPAddress.Any, 1234);

        public event EventHandler<(IntPtr, int)>? AudioFrameProcessed;
        
        public UdpAudioCaptureService()
        {
            udpClient = new UdpClient(1234,AddressFamily.InterNetwork);
            udpClient.JoinMulticastGroup(new IPAddress(new byte[] { 224, 1, 2, 3 }));
        }




        public unsafe void Start()
        {

          
            try
            {
                var initRecieved = udpClient.Receive(ref groupEP);
                udpClient.Connect(groupEP);

                Task.Factory.StartNew(ReceiveUdp);

            }
            catch (Exception e)
            {
                Console.WriteLine(e.ToString());
            }

        }

        SemaphoreSlim ss = new SemaphoreSlim(1);
        private async  void ReceiveUdp()
        {
            while (true)
            {
                var recieved = await udpClient.ReceiveAsync();
                unsafe
                {
                    fixed (byte* p = recieved.Buffer)
                    {

                        AudioFrameProcessed?.Invoke(this, ((nint)p, 64));
                     
                    }
                }
            }
        }

        public async Task Initialize()
        {

        }

      

        
    }

}
