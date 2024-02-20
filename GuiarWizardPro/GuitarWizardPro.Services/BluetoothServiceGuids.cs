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
        public static Guid WifiAuthService = Guid.Parse("0d7322c5-377a-4f0c-0000-b1706ce81a2e");

    }
    public static class BluetoothCharacteristicGuids
    {

        public static Guid UsernameCharacteristic = Guid.Parse("0d7322c5-377a-4f0c-0001-b1706ce81a2e");
        public static Guid PasswordCharacteristic = Guid.Parse("0d7322c5-377a-4f0c-0002-b1706ce81a2e");
        public static Guid SSIDCharacteristic = Guid.Parse("0d7322c5-377a-4f0c-0003-b1706ce81a2e");
        public static Guid SecurityTypeCharacteristic = Guid.Parse("0d7322c5-377a-4f0c-0004-b1706ce81a2e");
        public static Guid EncryptionTypeCharacteristic = Guid.Parse("0d7322c5-377a-4f0c-0005-b1706ce81a2e");
        public static Guid SecurityKeyCharacteristic = Guid.Parse("0d7322c5-377a-4f0c-0006-b1706ce81a2e");


        public static Guid AudioSampleDataCharacteristic = Guid.Parse("0d7322c5-377a-4f0c-8713-b1706ce81a1e");
    }
}
