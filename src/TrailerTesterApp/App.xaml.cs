using Microsoft.Extensions.DependencyInjection;

namespace TrailerTesterApp;

public partial class App : Application
{
	public App()
	{
		InitializeComponent();
	}

	protected override Window CreateWindow(IActivationState? activationState)
	{
		var mainPage = Handler?.MauiContext?.Services.GetService<MainPage>();
		return new Window(mainPage ?? new MainPage(new ViewModels.MainViewModel(new Services.BluetoothService())));
	}
}