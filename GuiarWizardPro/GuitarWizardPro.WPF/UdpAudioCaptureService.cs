
using System.Net.Sockets;
using System.Net;
using GuitarWizardPro.Services.Interfaces;

namespace GuitarWizardPro.WPF
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




        public  void Start()
        {

          
            try
            {
                var initRecieved = udpClient.Receive(ref groupEP);

                Task.Factory.StartNew(ReceiveUdp);

            }
            catch (Exception e)
            {
                Console.WriteLine(e.ToString());
            }

        }
        public int frames;
        private async  Task ReceiveUdp()
        {
            try
            {
                while (true)
                {
                    var recieved = await udpClient.ReceiveAsync();
                    unsafe
                    {
                        fixed (byte* p = recieved.Buffer)
                        {

                            AudioFrameProcessed?.Invoke(this, ((nint)p, 128));
                            frames++;
                        }
                    }
                }
            }
            catch(Exception e)
            {
                throw;
            }
        }
    }
}
