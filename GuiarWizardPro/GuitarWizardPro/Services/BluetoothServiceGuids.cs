using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GuitarWizardPro.Services
{
    public static class BluetoothServiceGuids
    {
        public static Guid AudioSampleService = Guid.Parse( "0d7322c5-377a-4f0c-8713-b1706ce81a1e");
    }
    public static class BluetoothCharacteristicGuids
    {
        public static Guid AudioSampleDataCharacteristic = Guid.Parse("0d7322c5-377a-4f0c-8713-b1706ce81a1e");
    }
}
