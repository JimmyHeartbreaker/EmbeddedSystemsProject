using GuitarWizardPro.ViewModel;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MauiApp3.Services.Interfaces
{
    public interface IAudioBluetoothDeviceService
    {
        event EventHandler<string> ConnectionStateChanged;
        ObservableCollection<DeviceInformation> Devices { get; }

        Task StartAsync(DeviceInformation deviceInformation);
        Task<bool> OpenAsync(DeviceInformation deviceInformation);
        void Disconnect(DeviceInformation deviceInformation);
    }

    public class StubAudioBluetoothDeviceService : IAudioBluetoothDeviceService
    {
        public ObservableCollection<DeviceInformation> Devices { get; } = [];

        public event EventHandler<string>? ConnectionStateChanged;

        public void Disconnect(DeviceInformation deviceInformation)
        {
            throw new NotImplementedException();
        }

        public Task<bool> OpenAsync(DeviceInformation deviceInformation)
        {
            throw new NotImplementedException();
        }

        public Task StartAsync(DeviceInformation deviceInformation)
        {
            throw new NotImplementedException();
        }
    }

}
