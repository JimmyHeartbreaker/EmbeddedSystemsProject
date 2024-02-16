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

namespace GuitarWizardPro.Platforms.Windows
{
    [ComImport]
    [Guid("5B0D3235-4DBA-4D44-865E-8F1D0E4FD04D")]
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    unsafe interface IMemoryBufferByteAccess
    {
        void GetBuffer(out byte* buffer, out uint capacity);
    }

    public  class AudioCaptureService : IAudioCapureService
    {

        public event EventHandler<(IntPtr, int)>? AudioFrameProcessed;
        
        private AudioGraph? graph;
        private AudioFrameOutputNode? frameOutputNodeResult;

       

        public unsafe void Start()
        {
            if(graph==null)
            {
                throw new Exception("graph not initialized");
            }
            var timer = new System.Timers.Timer(1000);
            timer.Elapsed += Timer_Elapsed;
            timer.Start();
            graph.Start();
            Task.Delay(1000).Wait();
           
        }

        private unsafe void Timer_Elapsed(object? sender, ElapsedEventArgs e)
        {
            if (frameOutputNodeResult == null)
                throw new Exception("frameOutputNodeResult==null");

            using var frame = frameOutputNodeResult.GetFrame();
            using var buffer = frame.LockBuffer(AudioBufferAccessMode.Read);
            using var memoryReference = buffer.CreateReference();

            memoryReference.As<IMemoryBufferByteAccess>().GetBuffer(out byte* dataInBytes, out uint capacityInBytes);
            AudioFrameProcessed?.Invoke(this, ((IntPtr)dataInBytes, (int)capacityInBytes));

        }

        public async Task Initialize()
        {
            
            // Create an AudioGraph with default settings
            var settings = new AudioGraphSettings(AudioRenderCategory.Media);
            var result = await AudioGraph.CreateAsync(settings);
            if (result.Status != AudioGraphCreationStatus.Success)
            {
                throw new Exception($"cannot create graph, status:{result.Status}");
            }
            graph = result.Graph;
            var  inputDevices = await DeviceInformation.FindAllAsync(MediaDevice.GetAudioCaptureSelector());
            var inputNodeResult =await graph.CreateDeviceInputNodeAsync(MediaCategory.Other, AudioEncodingProperties.CreatePcm(48000, 1, 16), inputDevices[0]);
            if (inputNodeResult.Status != AudioDeviceNodeCreationStatus.Success)
            {
                throw new Exception($"cannot create DeviceInputNode, status:{inputNodeResult.Status}");
            }
            // Create a device output node
            frameOutputNodeResult = graph.CreateFrameOutputNode(AudioEncodingProperties.CreatePcm(48000,1,16));
            inputNodeResult.DeviceInputNode.AddOutgoingConnection(frameOutputNodeResult);

           }

      

        
    }

}
