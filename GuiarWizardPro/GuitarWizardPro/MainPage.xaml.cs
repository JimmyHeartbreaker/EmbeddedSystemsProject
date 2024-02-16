using GuitarWizardPro.Services;
using GuitarWizardPro.ViewModel;
using MauiApp3.Services.Interfaces;
using System.Collections.Concurrent;
using System.Collections.ObjectModel;

namespace GuitarWizardPro
{
    public partial class MainPage : ContentPage
    {
        private ViewModel.MainPage viewModel;
        private AudioBufferService? audioBufferService;
        private FFTService fftService;

        public MainPage()
        {           
            InitializeComponent();
            viewModel = new ViewModel.MainPage();

           

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
            GraphicsView.Invalidate();
       
        }

        private async void EnableAudioPlaybackConnectionButton_Click(object sender, EventArgs e)
        {
            if (DeviceListView.SelectedItem is DeviceInformation deviceInformation)
            {
                await AudioBluetoothDeviceManager.StartAsync(deviceInformation);
                OpenAudioPlaybackConnectionButtonButton.IsEnabled = true;
            }
        }

        private async void OpenAudioPlaybackConnectionButtonButton_Click(object sender, EventArgs e)
        {
            if (DeviceListView.SelectedItem is DeviceInformation deviceInformation)
            {
                var status = await AudioBluetoothDeviceManager.OpenAsync(deviceInformation);
                if (status)
                {
                    // Notify that the AudioPlaybackConnection is connected. 
                    viewModel.ConnectionState = "Connected";
                }
                else
                {
                    // Notify that the connection attempt did not succeed. 
                    viewModel.ConnectionState = "Disconnected (attempt failed)";
                }
            }
        }

        private void ReleaseAudioPlaybackConnectionButton_Click(object sender, EventArgs e)
        {
            if (DeviceListView.SelectedItem is DeviceInformation deviceInformation)
            {
                AudioBluetoothDeviceManager.Disconnect(deviceInformation);
                viewModel.ConnectionState = "Disconnected";
                OpenAudioPlaybackConnectionButtonButton.IsEnabled = false;
            }
        }

        

        

        private async Task InitializeApp()
        {
            this.viewModel = new ViewModel.MainPage(AudioBluetoothDeviceManager);
            MainGrid.BindingContext = viewModel;
            await AudioCaptureService.Initialize();
            audioBufferService = new AudioBufferService(AudioCaptureService);
            audioBufferService.BufferFull += AudioBufferService_BufferFull;
            fftService = new FFTService(audioBufferService);
           
            this.GraphicsView.Drawable = new SignalPane(audioBufferService);
          
            AudioCaptureService.Start();
        }

        private async void GraphicsView_Loaded(object sender, EventArgs e)
        {
            await InitializeApp();
        }

      
    }
}
