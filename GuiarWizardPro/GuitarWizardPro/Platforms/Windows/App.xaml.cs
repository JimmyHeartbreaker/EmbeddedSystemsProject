using GuitarWizardPro.Platforms.Windows;
using GuitarWizardPro.Services.Interfaces;
using Microsoft.UI.Xaml;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace GuitarWizardPro.WinUI
{
    /// <summary>
    /// Provides application-specific behavior to supplement the default Application class.
    /// </summary>
    public partial class App : MauiWinUIApplication
    {
        /// <summary>
        /// Initializes the singleton application object.  This is the first line of authored code
        /// executed, and as such is the logical equivalent of main() or WinMain().
        /// </summary>
        public App()
        {
            this.InitializeComponent();
        }

        protected override MauiApp CreateMauiApp()
        {
          return MauiProgram.CreateMauiApp(ServiceRegistrar);
        }

        private void ServiceRegistrar(IServiceCollection serviceCollection)
        {
            serviceCollection.AddSingleton<IAudioBluetoothDeviceService, AudioBluetoothDeviceService>();
            serviceCollection.AddSingleton<IAudioCapureService, AudioCaptureService>();
        }
    }

}
