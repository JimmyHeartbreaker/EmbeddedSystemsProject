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
            audioSampleDataCharacteristic.ValueUpdated += AudioSampleDataCharacteristic_ValueUpdated;
            await audioSampleDataCharacteristic.StartUpdatesAsync();
        }

        SemaphoreSlim semaphoreSlim = new SemaphoreSlim(0);
        private void AudioSampleDataCharacteristic_ValueUpdated(object? sender, Plugin.BLE.Abstractions.EventArgs.CharacteristicUpdatedEventArgs e)
        {
            semaphoreSlim.Release();
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

        public async IAsyncEnumerable<byte[]> GetAudioDatasRaw()
        {
            while (audioSampleDataCharacteristic?.CanRead ?? false)
            {
                await semaphoreSlim.WaitAsync();
                var (data, resultCode) = await audioSampleDataCharacteristic.ReadAsync();
                if (data != null)
                {
                    
                        yield return data;
                    
                }
            }

        }
    }
}
