using TrailerTesterApp.ViewModels;

namespace TrailerTesterApp;

public partial class MainPage : ContentPage
{
	private readonly MainViewModel _viewModel;

	public MainPage(MainViewModel viewModel)
	{
		InitializeComponent();
		_viewModel = viewModel;
		BindingContext = _viewModel;
	}

	private void OnTestModeClicked(object sender, EventArgs e)
	{
		_viewModel.IsPowerMode = false;
	}

	private void OnPowerModeClicked(object sender, EventArgs e)
	{
		_viewModel.IsPowerMode = true;
	}
}
