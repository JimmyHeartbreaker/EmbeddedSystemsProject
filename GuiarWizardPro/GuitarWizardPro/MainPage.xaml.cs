using GuitarWizardPro.Services;
using GuitarWizardPro.Services.Interfaces;
using GuitarWizardPro.ViewModel;
using System.Collections.Concurrent;
using System.Collections.ObjectModel;

namespace GuitarWizardPro
{
    public partial class MainPage : ContentPage
    {
        private BluetoothLEService bleService;
        private ViewModel.MainPage viewModel;
        private AudioBufferService? audioBufferService;

        public MainPage()
        {           
            InitializeComponent();
            viewModel = new ViewModel.MainPage();

            bleService = new BluetoothLEService();


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
            if (AudioDeviceListView.SelectedItem is AudioDeviceInformation deviceInformation)
            {
                await AudioBluetoothDeviceManager.StartAsync(deviceInformation);
                OpenAudioPlaybackConnectionButtonButton.IsEnabled = true;
            }
        }

        private async void OpenAudioPlaybackConnectionButtonButton_Click(object sender, EventArgs e)
        {
            if (AudioDeviceListView.SelectedItem is AudioDeviceInformation deviceInformation)
            {
                var status = await AudioBluetoothDeviceManager.OpenAsync(deviceInformation);
                if (status)
                {
                    // Notify that the AudioPlaybackConnection is connected. 
                    viewModel.AudioConnectionState = "Connected";
                }
                else
                {
                    // Notify that the connection attempt did not succeed. 
                    viewModel.AudioConnectionState = "Disconnected (attempt failed)";
                }
            }
        }

        private void ReleaseAudioPlaybackConnectionButton_Click(object sender, EventArgs e)
        {
            if (AudioDeviceListView.SelectedItem is AudioDeviceInformation deviceInformation)
            {
                AudioBluetoothDeviceManager.Disconnect(deviceInformation);
                viewModel.AudioConnectionState = "Disconnected";
                OpenAudioPlaybackConnectionButtonButton.IsEnabled = false;
            }
        }

        

        

        private async Task InitializeApp()
        {
            bleService.DeviceDisconnected += BleService_DeviceConnectionChanged;
            bleService.DeviceConnected += BleService_DeviceConnectionChanged;
            this.viewModel = new ViewModel.MainPage(AudioBluetoothDeviceManager, bleService,Dispatcher);
            MainGrid.BindingContext = viewModel;
            await AudioCaptureService.Initialize();
            audioBufferService = new AudioBufferService(AudioCaptureService);
            audioBufferService.BufferFull += AudioBufferService_BufferFull;
            var fftService = new FFTService(audioBufferService);
           
            this.GraphicsView.Drawable = new SignalPane(audioBufferService);
          
            AudioCaptureService.Start(); 
            await bleService.Start();


        }

        private void BleService_DeviceConnectionChanged(object? sender, Plugin.BLE.Abstractions.Contracts.IDevice e)
        {
            if (GenericDeviceListView.SelectedItem is GenericDeviceInformation deviceInformation)
            {
                if (e.Id == deviceInformation.Id)
                {
                    UpdateConnectActionText(deviceInformation);
                }
            }
        }

        private async void GraphicsView_Loaded(object sender, EventArgs e)
        {
            await InitializeApp();
        }

        private async void ConnectGeneric_Click(object sender, EventArgs e)
        {
            if (GenericDeviceListView.SelectedItem is GenericDeviceInformation deviceInformation)
            {
                switch(deviceInformation.DeviceState )
                {
                    case Plugin.BLE.Abstractions.DeviceState.Connected:
                    case Plugin.BLE.Abstractions.DeviceState.Connecting:
                        await bleService.Disconnect(deviceInformation.Id);
                        break;
                    case Plugin.BLE.Abstractions.DeviceState.Limited:
                    case Plugin.BLE.Abstractions.DeviceState.Disconnected:
                        await bleService.Connect(deviceInformation.Id);
                        break;

                }
                UpdateConnectActionText(deviceInformation);
            }
        }

        private void GenericDeviceListView_ItemSelected(object sender, SelectedItemChangedEventArgs e)
        {
            if (GenericDeviceListView.SelectedItem is GenericDeviceInformation deviceInformation)
            {
                UpdateConnectActionText(deviceInformation);
            }
        }

        private void UpdateConnectActionText(GenericDeviceInformation deviceInformation)
        {
            if (deviceInformation.DeviceState == Plugin.BLE.Abstractions.DeviceState.Connected)
            {
                ConnectGenericDevice.Text = "Disconnected";
            }
            else
            {
                ConnectGenericDevice.Text = "Connect";
            }
        }
    }
}
