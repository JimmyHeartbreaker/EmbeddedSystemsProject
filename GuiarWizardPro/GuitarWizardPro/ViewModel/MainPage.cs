using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using MauiApp3.Services.Interfaces;

namespace GuitarWizardPro.ViewModel
{
    public class MainPage : INotifyPropertyChanged
    {
        private readonly IAudioBluetoothDeviceService audioBluetoothDeviceManager;
        private string connectionState = "Disconnected";
        public ObservableCollection<ViewModel.DeviceInformation> Devices
        {
            get { return audioBluetoothDeviceManager.Devices; }
        }

        public string ConnectionState
        {
            get => connectionState;
            set
            {
                connectionState = value;
                if (PropertyChanged != null)
                {
                    PropertyChanged(this, new PropertyChangedEventArgs("ConnectionState"));
                }
            }
        }

        public event PropertyChangedEventHandler? PropertyChanged;

        public MainPage() 
        {
            audioBluetoothDeviceManager = new StubAudioBluetoothDeviceService();
        }
       

        public MainPage( IAudioBluetoothDeviceService? audioBluetoothDeviceManager)
        {
            if(audioBluetoothDeviceManager==null)
            {
                throw new ArgumentNullException("audioBluetoothDeviceManager");
            }
            
            this.audioBluetoothDeviceManager = audioBluetoothDeviceManager;
            this.audioBluetoothDeviceManager.ConnectionStateChanged += AudioBluetoothDeviceManager_ConnectionStateChanged;
        }

        private void AudioBluetoothDeviceManager_ConnectionStateChanged(object? sender, string e)
        {
            connectionState = e;
        }
    }
}
