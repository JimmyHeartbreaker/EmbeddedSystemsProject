using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Runtime.Intrinsics.Arm;
using System.Text;
using System.Threading.Tasks;
using Windows.Devices.Bluetooth;
using Windows.Devices.Enumeration;
using Windows.Foundation.Metadata;
using Windows.Media.Audio;
using GuitarWizardPro.Services.Interfaces;


namespace GuitarWizardPro.Platforms.Windows
{
    public class AudioBluetoothDeviceService : IAudioBluetoothDeviceService
    {

        private readonly Dictionary<string, (ViewModel.AudioDeviceInformation, AudioPlaybackConnection)> audioPlaybackConnections = [];
        private readonly IDispatcher dispatcher;
        private readonly DeviceWatcher deviceWatcher;

        public event EventHandler<string>? ConnectionStateChanged;
        public ObservableCollection<ViewModel.AudioDeviceInformation> Devices { get; } = [];

        public AudioBluetoothDeviceService(IDispatcher dispatcher)
        {
            this.dispatcher = dispatcher;
            this.deviceWatcher = DeviceInformation.CreateWatcher(AudioPlaybackConnection.GetDeviceSelector());

            // Register event handlers before starting the watcher. 
            this.deviceWatcher.Added += this.DeviceWatcher_Added;
            this.deviceWatcher.Removed += this.DeviceWatcher_Removed;

            this.deviceWatcher.Start();
        }

        private void OnConnectionStateChanged(string connectionState)
        {
            ConnectionStateChanged?.Invoke(this, connectionState);
        }

        private void DeviceWatcher_Removed(DeviceWatcher sender, DeviceInformationUpdate args)
        {            
            if (this.audioPlaybackConnections.TryGetValue(args.Id, out (ViewModel.AudioDeviceInformation, AudioPlaybackConnection) device))
            {
                audioPlaybackConnections.Remove(args.Id);
                device.Item2.Dispose();
                dispatcher.Dispatch(() =>
                {
                    Devices.Remove(device.Item1);
                });
            }
        }

        private void DeviceWatcher_Added(DeviceWatcher sender, DeviceInformation args)
        {
            dispatcher.Dispatch(() =>
           {
               this.Devices.Add(new ViewModel.AudioDeviceInformation() { Name = args.Name, Id = args.Id });
           });

        }

        public async Task StartAsync(ViewModel.AudioDeviceInformation deviceInformation)
        {
            var deviceId = deviceInformation.Id;

            if (!this.audioPlaybackConnections.ContainsKey(deviceId))
            {
                // Create the audio playback connection from the selected device id and add it to the dictionary. 
                // This will result in allowing incoming connections from the remote device. 
                var playbackConnection = AudioPlaybackConnection.TryCreateFromId(deviceId);
                
                if (playbackConnection != null)
                {
                    // The device has an available audio playback connection. 
                    playbackConnection.StateChanged += this.AudioPlaybackConnection_ConnectionStateChanged;
                    this.audioPlaybackConnections.Add(deviceId, (deviceInformation, playbackConnection));
                    await playbackConnection.StartAsync();

                }
            }
        }

        private async void AudioPlaybackConnection_ConnectionStateChanged(AudioPlaybackConnection sender, object args)
        {
            await dispatcher.DispatchAsync( () =>
            {
                if (sender.State == AudioPlaybackConnectionState.Closed)
                {
                    OnConnectionStateChanged("Disconnected");
                }
                else if (sender.State == AudioPlaybackConnectionState.Opened)
                {
                    OnConnectionStateChanged("Connected");
                }
                else
                {
                    OnConnectionStateChanged("Unknown");
                }
            });
        }

        public async Task<bool> OpenAsync(ViewModel.AudioDeviceInformation deviceInformation)
        {
            if (this.audioPlaybackConnections.TryGetValue(deviceInformation.Id, out (ViewModel.AudioDeviceInformation, AudioPlaybackConnection) device))
            {
              
               return (await device.Item2.OpenAsync()).Status == AudioPlaybackConnectionOpenResultStatus.Success;
            }
            return false;

        }

        public void Disconnect(ViewModel.AudioDeviceInformation deviceInformation)
        {
            if (this.audioPlaybackConnections.TryGetValue(deviceInformation.Id, out (ViewModel.AudioDeviceInformation, AudioPlaybackConnection) device))
            {
                device.Item2.Dispose();
                device.Item2.StateChanged -= AudioPlaybackConnection_ConnectionStateChanged;
                audioPlaybackConnections.Remove(deviceInformation.Id);

            }
        }
    }
}
