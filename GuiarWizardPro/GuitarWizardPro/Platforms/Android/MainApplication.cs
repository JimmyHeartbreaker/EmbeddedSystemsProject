using Android.App;
using Android.Runtime;
using MauiApp3.Services.Interfaces;

[assembly: UsesPermission(Android.Manifest.Permission.AccessNetworkState)]

namespace GuitarWizardPro
{
    [Application]
    public class MainApplication : MauiApplication
    {
        public MainApplication(IntPtr handle, JniHandleOwnership ownership)
            : base(handle, ownership)
        {
        }

        protected override MauiApp CreateMauiApp() => MauiProgram.CreateMauiApp(ServiceRegistrar);
        private void ServiceRegistrar(IServiceCollection serviceCollection)
        {
            serviceCollection.AddTransient<IAudioBluetoothDeviceService, StubAudioBluetoothDeviceService>();
        }
    }
}
