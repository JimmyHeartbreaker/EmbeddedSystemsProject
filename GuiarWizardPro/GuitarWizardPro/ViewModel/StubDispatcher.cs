

namespace GuitarWizardPro.ViewModel
{
    internal class StubDispatcher : IDispatcher
    {
        public bool IsDispatchRequired => throw new NotImplementedException();

        public IDispatcherTimer CreateTimer()
        {
            throw new NotImplementedException();
        }

        public bool Dispatch(Action action)
        {
            throw new NotImplementedException();
        }

        public bool DispatchDelayed(TimeSpan delay, Action action)
        {
            throw new NotImplementedException();
        }
    }
}