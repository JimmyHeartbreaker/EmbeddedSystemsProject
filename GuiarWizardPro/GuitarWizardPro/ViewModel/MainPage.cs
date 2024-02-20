using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using GuitarWizardPro.Services;
using GuitarWizardPro.Services.Interfaces;
using Microsoft.Maui.Dispatching;

namespace GuitarWizardPro.ViewModel
{
    public class MainPage : INotifyPropertyChanged
    {

        private readonly IDispatcher dispatcher;
        private readonly IAudioBluetoothDeviceService audioBluetoothDeviceManager;
        private readonly IBLEClientService bluetoothLEService;
        private BluetoothAudioServiceChannel? bluetoothAudioServiceChannel;
        public ObservableCollection<AudioDeviceInformation> AudioDevices
        {
            get { return audioBluetoothDeviceManager.Devices; }
        }
        public ObservableCollection<GenericDeviceInformation> GenericDevices
        {
            get; set;
        } = new ObservableCollection<GenericDeviceInformation>();

      

        private string audioConnectionState = "Disconnected";
        public string AudioConnectionState
        {
            get => audioConnectionState;
            set
            {
                audioConnectionState = value;
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs("ConnectionState"));
            }
        }

        public event PropertyChangedEventHandler? PropertyChanged;

        public MainPage() 
        {
            audioBluetoothDeviceManager = new StubAudioBluetoothDeviceService();
            bluetoothLEService = new StubBLEClientService();
            dispatcher = new StubDispatcher();
        }
       

        public MainPage( IAudioBluetoothDeviceService? audioBluetoothDeviceManager,Services.Interfaces.IBLEClientService bluetoothLEService, IDispatcher dispatcher)
        {
            ArgumentNullException.ThrowIfNull(audioBluetoothDeviceManager);
            ArgumentNullException.ThrowIfNull(bluetoothLEService);
            ArgumentNullException.ThrowIfNull(dispatcher);

            this.dispatcher = dispatcher;
            this.audioBluetoothDeviceManager = audioBluetoothDeviceManager;
            this.audioBluetoothDeviceManager.ConnectionStateChanged += AudioBluetoothDeviceManager_ConnectionStateChanged;
            this.bluetoothLEService = bluetoothLEService;
            bluetoothLEService.DeviceAdded += BleService_DeviceAdded;
            bluetoothLEService.DeviceConnected += BluetoothLEService_DeviceConnected; ;
            bluetoothLEService.DeviceDisconnected += BluetoothLEService_DeviceDisconnected;
           
        }


        private async void BluetoothLEService_DeviceConnected(object? sender, Plugin.BLE.Abstractions.Contracts.IDevice e)
        {
            var deviceVM = GenericDevices.FirstOrDefault(x => x.Id == e.Id);
            if (deviceVM != null)
            {
                deviceVM.DeviceState = e.State;

                
                bluetoothAudioServiceChannel = new BluetoothAudioServiceChannel( await e.GetServiceAsync(BluetoothServiceGuids.AudioSampleService));
                await bluetoothAudioServiceChannel.Initialize();

                var dataList = new List<short>();
                int count = 0;
                await foreach(var data in  bluetoothAudioServiceChannel.GetAudioDatasRaw())
                {
                    dataList.Add(BitConverter.ToInt16(data));
                    count++;
                    if (count > 2000)
                        break;
                }
            }

        }

        private void BluetoothLEService_DeviceDisconnected(object? sender, Plugin.BLE.Abstractions.Contracts.IDevice e)
        {
            var deviceVM = GenericDevices.FirstOrDefault(x => x.Id == e.Id);
            if(deviceVM!=null)
            {
                deviceVM.DeviceState = e.State;
            }
        }

        private void BleService_DeviceAdded(object? sender, Plugin.BLE.Abstractions.EventArgs.DeviceEventArgs e)
        {
            dispatcher.Dispatch(() =>
            {
                GenericDevices.Add(new GenericDeviceInformation()
                {
                    Id = e.Device.Id,
                    Name = e.Device.Name,
                    DeviceState = e.Device.State
                });
            });

        }
        private void AudioBluetoothDeviceManager_ConnectionStateChanged(object? sender, string e)
        {
            audioConnectionState = e;
        }
    }
}
