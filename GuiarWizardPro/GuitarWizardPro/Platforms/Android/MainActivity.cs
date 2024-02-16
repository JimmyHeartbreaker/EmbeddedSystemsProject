using Android.App;
using Android.Content;
using Android.Content.PM;
using Android.OS;

namespace GuitarWizardPro
{
    [Activity(Theme = "@style/Maui.SplashTheme", MainLauncher = true, ConfigurationChanges = ConfigChanges.ScreenSize | ConfigChanges.Orientation | ConfigChanges.UiMode | ConfigChanges.ScreenLayout | ConfigChanges.SmallestScreenSize | ConfigChanges.Density)]
    public class MainActivity : MauiAppCompatActivity
    {
        protected override void OnCreate(Bundle? savedInstanceState)
        {
            base.OnCreate(savedInstanceState);

            BroadcastReceiver x;
            IEnumerable<ConnectionProfile> profiles = Connectivity.Current.ConnectionProfiles;

            if (profiles.Contains(ConnectionProfile.Bluetooth))
            {
                // Active Wi-Fi connection.
            }

        }
    }
}
