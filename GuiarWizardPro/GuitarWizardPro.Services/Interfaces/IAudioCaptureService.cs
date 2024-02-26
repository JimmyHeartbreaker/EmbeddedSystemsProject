
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GuitarWizardPro.Services.Interfaces
{
    public interface IAudioCapureService
    {
        void Start();

        unsafe event EventHandler<(nint dataPtr, int length)>? AudioFrameProcessed;

    }

    public class StubAudioCaptureService : IAudioCapureService
    {

        public event EventHandler<(nint, int)>? AudioFrameProcessed;

  

        public void Start() { }


    }

}
