using Foundation;
using MauiApp3.Services.Interfaces;

namespace GuitarWizardPro
{
    [Register("AppDelegate")]
    public class AppDelegate : MauiUIApplicationDelegate
    {
        protected override MauiApp CreateMauiApp() => MauiProgram.CreateMauiApp(ServiceRegistrar);
        private void ServiceRegistrar(IServiceCollection serviceCollection)
        {
            serviceCollection.AddTransient<IAudioBluetoothDeviceService, StubAudioBluetoothDeviceService>();
        }
    }
}
