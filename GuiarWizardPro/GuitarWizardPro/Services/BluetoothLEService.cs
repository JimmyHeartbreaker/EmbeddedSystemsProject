using GuitarWizardPro.Services.Interfaces;
using GuitarWizardPro.ViewModel;
using Plugin.BLE;
using Plugin.BLE.Abstractions.Contracts;
using Plugin.BLE.Abstractions.EventArgs;
using Plugin.BLE.Abstractions.Extensions;
using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GuitarWizardPro.Services
{
    public class BluetoothLEService : IBluetoothLEService
    {
        public event EventHandler<DeviceEventArgs>? DeviceAdded;
        public event EventHandler<DeviceEventArgs>? DeviceRemoved;
        public event EventHandler<IDevice>? DeviceConnected;
        public event EventHandler<IDevice>? DeviceDisconnected;

        private readonly ConcurrentDictionary<Guid, IDevice> connectedDevices = new();

        private readonly ConcurrentDictionary<Guid, IDevice> connectableDevices = new();
        public IReadOnlyDictionary<Guid,IDevice> ConnectableDevices { get { return connectableDevices.AsReadOnly(); } }



        private readonly IBluetoothLE _bluetoothManager;
        private readonly IAdapter _adapter;

        
        public BluetoothLEService() 
        {
            _bluetoothManager = CrossBluetoothLE.Current;
           
            _bluetoothManager.StateChanged += OnStateChanged;
           
            _adapter = CrossBluetoothLE.Current.Adapter;
          
            _adapter.DeviceDiscovered += OnDeviceDiscovered;
            _adapter.DeviceAdvertised += OnDeviceDiscovered;
            _adapter.ScanTimeoutElapsed += Adapter_ScanTimeoutElapsed;
            _adapter.ScanMode = ScanMode.LowLatency;
            _adapter.DeviceBondStateChanged += _adapter_DeviceBondStateChanged;
            _adapter.DeviceDisconnected += _adapter_DeviceDisconnected;
            _adapter.DeviceConnected += _adapter_DeviceConnected
                ;
        }

        private void _adapter_DeviceConnected(object? sender, DeviceEventArgs e)
        {
            DeviceConnected?.Invoke(this, e.Device);
        }

        private void _adapter_DeviceDisconnected(object? sender, DeviceEventArgs e)
        {
            DeviceDisconnected?.DynamicInvoke(this, e.Device);
        }

        private void _adapter_DeviceBondStateChanged(object? sender, DeviceBondStateChangedEventArgs e)
        {

        }

        public async Task Connect(Guid deviceGuid)
        {
            if (!connectableDevices.TryGetValue(deviceGuid, out IDevice? device))
            {
                device = _adapter.DiscoveredDevices.FirstOrDefault(x => x.Id == deviceGuid);
                if (device == null)
                {
                    throw new Exception($"Could not find with device with id:{deviceGuid}");
                }
            }
            await _adapter.ConnectToDeviceAsync(device);

           
        }
        internal async Task Disconnect(Guid deviceGuid)
        {
            if (!connectableDevices.TryGetValue(deviceGuid, out IDevice? device))
            {
                device = _adapter.DiscoveredDevices.FirstOrDefault(x => x.Id == deviceGuid);
                if (device == null)
                {
                    throw new Exception($"Could not find with device with id:{deviceGuid}");
                }
            }
            await _adapter.DisconnectDeviceAsync(device);
        }
        public async Task Start()
        {
            await _adapter.StartScanningForDevicesAsync();
        }

        private void OnStateChanged(object? sender, BluetoothStateChangedArgs e)
        {
            
        }

        private void Adapter_ScanTimeoutElapsed(object? sender, EventArgs e)
        {
        }

        private void OnDeviceDiscovered(object? sender, DeviceEventArgs e)
        {
            if (e.Device.IsConnectable)
            {
                if (connectableDevices.TryAdd(e.Device.Id, e.Device))
                {
                    DeviceAdded?.Invoke(this, e);
                }
            }
        }

        
    }
}
