using Plugin.BLE.Abstractions.Contracts;
using Plugin.BLE.Abstractions.EventArgs;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GuitarWizardPro.Services.Interfaces
{
    public interface IBluetoothLEService
    {
        event EventHandler<DeviceEventArgs>? DeviceAdded;
        event EventHandler<DeviceEventArgs>? DeviceRemoved;
        event EventHandler<IDevice>? DeviceConnected;
        event EventHandler<IDevice>? DeviceDisconnected;
        IReadOnlyDictionary<Guid, IDevice> ConnectableDevices { get;  }
    }

    public class StubBluetoothLEService : IBluetoothLEService
    {
        public IReadOnlyDictionary<Guid, IDevice> ConnectableDevices => throw new NotImplementedException();

        public event EventHandler<DeviceEventArgs>? DeviceAdded;
        public event EventHandler<IDevice>? DeviceConnected;
        public event EventHandler<DeviceEventArgs>? DeviceRemoved;
        public event EventHandler<IDevice>? DeviceDisconnected;
    }
}
