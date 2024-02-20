using GuitarWizardPro.ViewModel;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GuitarWizardPro.Services.Interfaces
{
    public interface IAudioBluetoothDeviceService
    {
        event EventHandler<string> ConnectionStateChanged;
        ObservableCollection<AudioDeviceInformation> Devices { get; }

        Task StartAsync(AudioDeviceInformation deviceInformation);
        Task<bool> OpenAsync(AudioDeviceInformation deviceInformation);
        void Disconnect(AudioDeviceInformation deviceInformation);
    }

    public class StubAudioBluetoothDeviceService : IAudioBluetoothDeviceService
    {
        public ObservableCollection<AudioDeviceInformation> Devices { get; } = [];

        public event EventHandler<string>? ConnectionStateChanged;

        public void Disconnect(AudioDeviceInformation deviceInformation)
        {
            throw new NotImplementedException();
        }

        public Task<bool> OpenAsync(AudioDeviceInformation deviceInformation)
        {
            throw new NotImplementedException();
        }

        public Task StartAsync(AudioDeviceInformation deviceInformation)
        {
            throw new NotImplementedException();
        }
    }

}
