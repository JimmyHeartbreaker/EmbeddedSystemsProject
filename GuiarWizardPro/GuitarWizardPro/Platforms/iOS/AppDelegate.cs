using Foundation;

namespace GuitarWizardPro
{
    [Register("AppDelegate")]
    public class AppDelegate : MauiUIApplicationDelegate
    {
        protected override MauiApp CreateMauiApp() => MauiProgram.CreateMauiApp(ServiceRegistrar);

        private void ServiceRegistrar(IServiceCollection serviceCollection)
        {
            serviceCollection.AddTransient<IAudioBluetoothDeviceManager, StubAudioBluetoothDeviceManager>();
        }
    }
}
