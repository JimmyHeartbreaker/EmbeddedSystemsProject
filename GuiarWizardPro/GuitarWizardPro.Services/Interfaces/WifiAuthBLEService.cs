using Plugin.BLE.Abstractions.Contracts;
using Plugin.BLE.Abstractions.EventArgs;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GuitarWizardPro.Services.Interfaces
{
    public interface IWifiAuthBLEService
    {
        Task<bool> Start();
    }

    public class StubWifiAuthBLEService : IWifiAuthBLEService
    {
        public Task<bool> Start()
        {
            throw new NotImplementedException();
        }
    }
}
