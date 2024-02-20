using Plugin.BLE.Abstractions;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GuitarWizardPro.ViewModel
{
    public class GenericDeviceInformation : INotifyPropertyChanged
    {
        public string Name { get; set; } = string.Empty;
        public Guid Id { get; set; }

        private DeviceState deviceState;

        public event PropertyChangedEventHandler? PropertyChanged;

        public Plugin.BLE.Abstractions.DeviceState DeviceState
        {
            get => deviceState;
            set
            {
                deviceState = value;

                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(ConnectionState)));
            }
        }
        public string ConnectionState
        {
            get
            {
                return deviceState.ToString();
            }
        }
    }
}
