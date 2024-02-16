using Plugin.BLE.Abstractions.Contracts;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GuitarWizardPro.Services
{
    public class BluetoothAudioServiceChannel 
    {
        private readonly IService service;
        private ICharacteristic? audioSampleDataCharacteristic;

        public BluetoothAudioServiceChannel(Plugin.BLE.Abstractions.Contracts.IService service)
        {
            this.service = service;
        }

        public async Task Initialize()
        {
             audioSampleDataCharacteristic = await this.service.GetCharacteristicAsync(BluetoothCharacteristicGuids.AudioSampleDataCharacteristic);
        }

        public async Task<byte[]> GetAudioDataRaw()
        {
            if(audioSampleDataCharacteristic?.CanRead ?? false)
            {
                var (data, resultCode) = await audioSampleDataCharacteristic.ReadAsync();
                return data;
            }
            return [];
        }
    }
}
