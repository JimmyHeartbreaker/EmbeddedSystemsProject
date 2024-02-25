using GuitarWizardPro.Services;
using GuitarWizardPro.Services.Interfaces;
using GuitarWizardPro.ViewModel;
using Microsoft.Maui.ApplicationModel;
using Microsoft.UI.Xaml.Media.Imaging;
using SkiaSharp;
using SkiaSharp.Views.Maui.Controls;
using System.Collections.Concurrent;
using System.Collections.ObjectModel;
using Windows.Graphics.Imaging;
using Windows.Storage.Streams;

namespace GuitarWizardPro
{
    public partial class MainPage : ContentPage
    {
        private BLEClientService bleService;
        private ViewModel.MainPage viewModel;
        private AudioBufferService? audioBufferService;

        public MainPage()
        {           
            InitializeComponent();
            viewModel = new ViewModel.MainPage();

            bleService = new BLEClientService();


        }
     
        private IWifiAuthBLEService WifiAuthBLEService
        {
            get
            {
                if (Handler?.MauiContext != null)
                {
                    return Handler.MauiContext.Services.GetRequiredService<IWifiAuthBLEService>();
                }
                else
                {
                    return new StubWifiAuthBLEService();
                }
            }
        }
        private IAudioCapureService AudioCaptureService
        {
            get
            {
                if (Handler?.MauiContext != null)
                {
                    return Handler.MauiContext.Services.GetRequiredService<IAudioCapureService>();
                }
                else
                {
                    return new StubAudioCaptureService();
                }
            }
        }
        private IAudioBluetoothDeviceService AudioBluetoothDeviceManager
        {
            get
            {
                if (Handler?.MauiContext != null)
                {
                    return Handler.MauiContext.Services.GetRequiredService<IAudioBluetoothDeviceService>();
                }
                else
                {
                    return new StubAudioBluetoothDeviceService();
                }
            }
        }
     

        private void AudioBufferService_BufferFull(object? sender, EventArgs e)
        {
            audioBufferService?.ResetBuffer();
            //GraphicsView.Invalidate();
       
        }

        private async void EnableAudioPlaybackConnectionButton_Click(object sender, EventArgs e)
        {
            //if (AudioDeviceListView.SelectedItem is AudioDeviceInformation deviceInformation)
            //{
            //    await AudioBluetoothDeviceManager.StartAsync(deviceInformation);
            //    OpenAudioPlaybackConnectionButtonButton.IsEnabled = true;
            //}
        }

        private async void OpenAudioPlaybackConnectionButtonButton_Click(object sender, EventArgs e)
        {
            //if (AudioDeviceListView.SelectedItem is AudioDeviceInformation deviceInformation)
            //{
            //    var status = await AudioBluetoothDeviceManager.OpenAsync(deviceInformation);
            //    if (status)
            //    {
            //        // Notify that the AudioPlaybackConnection is connected. 
            //        viewModel.AudioConnectionState = "Connected";
            //    }
            //    else
            //    {
            //        // Notify that the connection attempt did not succeed. 
            //        viewModel.AudioConnectionState = "Disconnected (attempt failed)";
            //    }
            //}
        }

        private void ReleaseAudioPlaybackConnectionButton_Click(object sender, EventArgs e)
        {
            //if (AudioDeviceListView.SelectedItem is AudioDeviceInformation deviceInformation)
            //{
            //    AudioBluetoothDeviceManager.Disconnect(deviceInformation);
            //    viewModel.AudioConnectionState = "Disconnected";
            //    OpenAudioPlaybackConnectionButtonButton.IsEnabled = false;
            //}
        }

        

        

        private async Task InitializeApp()
        {
            bleService.DeviceDisconnected += BleService_DeviceConnectionChanged;
            bleService.DeviceConnected += BleService_DeviceConnectionChanged;
            this.viewModel = new ViewModel.MainPage(AudioBluetoothDeviceManager, bleService,Dispatcher);
          //  MainGrid.BindingContext = viewModel;
            await AudioCaptureService.Initialize();
            audioBufferService = new AudioBufferService(AudioCaptureService);
            audioBufferService.BufferFull += AudioBufferService_BufferFull;
            var fftService = new FFTService(audioBufferService);
           
           // this.GraphicsView.Drawable = new SignalPane(audioBufferService);
          
            await WifiAuthBLEService.Start();

            AudioCaptureService.Start();

        }

        private void BleService_DeviceConnectionChanged(object? sender, Plugin.BLE.Abstractions.Contracts.IDevice e)
        {
            //if (GenericDeviceListView.SelectedItem is GenericDeviceInformation deviceInformation)
            //{
            //    if (e.Id == deviceInformation.Id)
            //    {
            //        UpdateConnectActionText(deviceInformation);
            //    }
            //}
        }

        private async void GraphicsView_Loaded(object sender, EventArgs e)
        {
            await InitializeApp();
        }

        private async void ConnectGeneric_Click(object sender, EventArgs e)
        {
            //if (GenericDeviceListView.SelectedItem is GenericDeviceInformation deviceInformation)
            //{
            //    switch(deviceInformation.DeviceState )
            //    {
            //        case Plugin.BLE.Abstractions.DeviceState.Connected:
            //        case Plugin.BLE.Abstractions.DeviceState.Connecting:
            //            await bleService.Disconnect(deviceInformation.Id);
            //            break;
            //        case Plugin.BLE.Abstractions.DeviceState.Limited:
            //        case Plugin.BLE.Abstractions.DeviceState.Disconnected:
            //            await bleService.Connect(deviceInformation.Id);
            //            break;

            //    }
            //    UpdateConnectActionText(deviceInformation);
            //}
        }

        private void GenericDeviceListView_ItemSelected(object sender, SelectedItemChangedEventArgs e)
        {
            //if (GenericDeviceListView.SelectedItem is GenericDeviceInformation deviceInformation)
            //{
            //    UpdateConnectActionText(deviceInformation);
            //}
        }

        private void UpdateConnectActionText(GenericDeviceInformation deviceInformation)
        {
            //if (deviceInformation.DeviceState == Plugin.BLE.Abstractions.DeviceState.Connected)
            //{
            //    ConnectGenericDevice.Text = "Disconnected";
            //}
            //else
            //{
            //    ConnectGenericDevice.Text = "Connect";
            //}
        }
        public async  Task<Stream> ConvertWriteableBitmapToRandomAccessStream(byte[] pixelsBytes, double width, double height)
        {
            var stream = new InMemoryRandomAccessStream();

            BitmapEncoder encoder = await BitmapEncoder.CreateAsync(BitmapEncoder.JpegEncoderId, stream);

            
            encoder.SetPixelData(BitmapPixelFormat.Bgra8, BitmapAlphaMode.Ignore, (uint)width, (uint)height, 96.0, 96.0, pixelsBytes);
            await encoder.FlushAsync();

            return stream.AsStream();
        }
 
        private async void GraphicsViewImage_Loaded(object sender, EventArgs e)
        {

         
        


        }

        private async void TapGestureRecognizer_Tapped(object sender, TappedEventArgs e)
        {
            //var pos = e.GetPosition(GraphicsView);
            //var bytePos = 1000 * (int)pos.Value.Y * 4 + (int)pos.Value.X * 4;
            //_frameBuffer[bytePos++] = 155;
            //_frameBuffer[bytePos++] = 155;
            //_frameBuffer[bytePos++] = 155;
            //_frameBuffer[bytePos] = 155;
            //
            //  skBitmap.SetPixel((int)pos.Value.X, (int)pos.Value.Y, new SKColor(0, 0, 0));
            // skBitmap.NotifyPixelsChanged();
        //     canvasView.InvalidateSurface();

        }

        private void canvasView_Loaded(object sender, EventArgs e)
        {
          //  Array.Fill<byte>(_frameBuffer, 100);
            //   stream = await ConvertWriteableBitmapToRandomAccessStream(pixelData, 1000, 500);
        
         //   skBitmap = new SkiaSharp.SKBitmap(1000, 500, SKColorType.Bgra8888, SKAlphaType.Opaque);

          
        }

        private void CanvasView_OnPaintSurface(object sender, SkiaSharp.Views.Maui.SKPaintSurfaceEventArgs e)
        {
            //SKImageInfo info = e.Info;
            //using (var data = SKData.CreateCopy(_frameBuffer))
            //using (var original = new SKBitmap(1000,500,true))
            //{
            //    // create a new bitmap using the memory
            //    original.InstallPixels(new SKImageInfo(1000, 500, SKColorType.Rgba8888), data.Data);
                
            //        e.Surface.Canvas.DrawBitmap(original, info.Rect);
                
            //}

            //if (skBitmap == null)
            //    return;
            //SKImageInfo info = e.Info;
            //SKSurface surface = e.Surface;
            //SKCanvas canvas = surface.Canvas;
            //canvas.DrawBitmap(skBitmap, info.Rect);
        }
    }
}
