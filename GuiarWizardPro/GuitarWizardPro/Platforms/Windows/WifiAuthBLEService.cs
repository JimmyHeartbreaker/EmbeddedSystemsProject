using GuitarWizardPro.Services.Interfaces;
using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using Windows.ApplicationModel.Core;
using Windows.Devices.Bluetooth;
using Windows.Devices.Bluetooth.GenericAttributeProfile;
using Windows.Foundation;
using Windows.Storage.Streams;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Maui.Platform;
using Microsoft.VisualBasic;
using GuitarWizardPro.Services;
using Plugin.BLE.UWP;
using Windows.UI.WebUI;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Text;
using System.Net.Sockets;
using System.Net;
namespace GuitarWizardPro.Platforms.Windows
{
    public class WifiAuthBLEService : Services.Interfaces.IWifiAuthBLEService
    {
        static byte[] username = Encoding.UTF8.GetBytes("hardaker-j@ulster.ac.uk");
        static byte[] password = Encoding.UTF8.GetBytes("EveryWeekIsAG00dWeek");
        static byte[] ssid = Encoding.UTF8.GetBytes("EggTart");
        static byte[] networkSecurityKey = Encoding.UTF8.GetBytes("Jun0Am3l1aCh0i");
        static byte[] securityType = Encoding.UTF8.GetBytes("WPA2-Personal");
        static byte[] encryptionType = Encoding.UTF8.GetBytes("AES");

        public static GattLocalCharacteristicParameters GattParameters(string dsecription,byte[] value) => new()
        {
            CharacteristicProperties =  GattCharacteristicProperties.Read ,
            ReadProtectionLevel = GattProtectionLevel.Plain,
            StaticValue = value.AsBuffer(),
            
            UserDescription = dsecription
        };

        private GattServiceProvider? serviceProvider;

        public WifiAuthBLEService() 
        {
        }

        public static string GetLocalIPAddress()
        {
            var host = Dns.GetHostEntry(Dns.GetHostName());
            foreach (var ip in host.AddressList)
            {
                if (ip.AddressFamily == AddressFamily.InterNetwork)
                {
                    return ip.ToString();
                }
            }
            throw new Exception("No network adapters with an IPv4 address in the system!");
        }
        public async Task<bool> Start()
        {
            // BT_Code: Initialize and starting a custom GATT Service using GattServiceProvider.
            GattServiceProviderResult serviceResult = await GattServiceProvider.CreateAsync(BluetoothServiceGuids.WifiAuthService);

            if (serviceResult.Error == BluetoothError.Success)
            {
                serviceProvider = serviceResult.ServiceProvider;
            }
            else
            {
                return false;
            }
            
            GattLocalCharacteristicResult result = await serviceProvider.Service.CreateCharacteristicAsync(BluetoothCharacteristicGuids.UsernameCharacteristic, GattParameters("Username",username));
            
            result = await serviceProvider.Service.CreateCharacteristicAsync(BluetoothCharacteristicGuids.PasswordCharacteristic, 
                GattParameters("Password", password));
            result = await serviceProvider.Service.CreateCharacteristicAsync(BluetoothCharacteristicGuids.SSIDCharacteristic,
                GattParameters("SSID", ssid));
            result = await serviceProvider.Service.CreateCharacteristicAsync(BluetoothCharacteristicGuids.SecurityTypeCharacteristic,
                GattParameters("SecurityType", securityType));
            result = await serviceProvider.Service.CreateCharacteristicAsync(BluetoothCharacteristicGuids.EncryptionTypeCharacteristic,
                GattParameters("Encryption", encryptionType));
            result = await serviceProvider.Service.CreateCharacteristicAsync(BluetoothCharacteristicGuids.SecurityKeyCharacteristic,
                GattParameters("SecurityKey", networkSecurityKey));
            result = await serviceProvider.Service.CreateCharacteristicAsync(BluetoothCharacteristicGuids.SecurityKeyCharacteristic,
                GattParameters("SecurityKey", networkSecurityKey));
            result = await serviceProvider.Service.CreateCharacteristicAsync(BluetoothCharacteristicGuids.IpAddressCharacteristic,
                GattParameters("IpAddress", Encoding.UTF8.GetBytes(GetLocalIPAddress())));
            result.Characteristic.ReadRequested += Characteristic_ReadRequested
                ;
            // BT_Code: Indicate if your sever advertises as connectable and discoverable.
            GattServiceProviderAdvertisingParameters advParameters = new()
            {
                // IsConnectable determines whether a call to publish will attempt to start advertising and 
                // put the service UUID in the ADV packet (best effort)
                IsConnectable = true,
                
                // IsDiscoverable determines whether a remote device can query the local device for support 
                // of this service
                IsDiscoverable = true
            };
            serviceProvider.AdvertisementStatusChanged += ServiceProvider_AdvertisementStatusChanged;
            serviceProvider.StartAdvertising(advParameters);

            return true;
        }

        private void Characteristic_ReadRequested(GattLocalCharacteristic sender, GattReadRequestedEventArgs args)
        {
            
        }

        private void ServiceProvider_AdvertisementStatusChanged(GattServiceProvider sender, GattServiceProviderAdvertisementStatusChangedEventArgs args)
        {
        }
    }
}
