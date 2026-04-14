using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Runtime.CompilerServices;
using System.Windows.Input;
using TrailerTesterApp.Services;

namespace TrailerTesterApp.ViewModels;

public class MainViewModel : INotifyPropertyChanged
{
    private readonly IBluetoothService _bluetoothService;
    private bool _isConnected;
    private bool _isScanning;
    private string _statusMessage;
    private string _connectedDeviceName;
    private bool _isPowerMode;
    private string _selectedLight;

    public ObservableCollection<BluetoothDevice> Devices { get; } = new();
    public ObservableCollection<string> Messages { get; } = new();

    public bool IsConnected
    {
        get => _isConnected;
        set { _isConnected = value; OnPropertyChanged(); }
    }

    public bool IsScanning
    {
        get => _isScanning;
        set { _isScanning = value; OnPropertyChanged(); }
    }

    public string StatusMessage
    {
        get => _statusMessage;
        set { _statusMessage = value; OnPropertyChanged(); }
    }

    public string ConnectedDeviceName
    {
        get => _connectedDeviceName;
        set { _connectedDeviceName = value; OnPropertyChanged(); }
    }

    public bool IsPowerMode
    {
        get => _isPowerMode;
        set
        {
            if (_isPowerMode != value)
            {
                _isPowerMode = value;
                OnPropertyChanged();
                _ = SetModeAsync(value);
            }
        }
    }

    public string SelectedLight
    {
        get => _selectedLight;
        set
        {
            if (_selectedLight != value)
            {
                _selectedLight = value;
                OnPropertyChanged();
            }
        }
    }

    public ICommand ScanCommand { get; }
    public ICommand ConnectCommand { get; }
    public ICommand DisconnectCommand { get; }
    public ICommand SetLightCommand { get; }
    public ICommand NextStepCommand { get; }

    public MainViewModel(IBluetoothService bluetoothService)
    {
        _bluetoothService = bluetoothService;
        _bluetoothService.MessageReceived += OnMessageReceived;
        _bluetoothService.ConnectionChanged += OnConnectionChanged;

        ScanCommand = new Command(async () => await ScanForDevicesAsync());
        ConnectCommand = new Command<BluetoothDevice>(async (device) => await ConnectToDeviceAsync(device));
        DisconnectCommand = new Command(async () => await DisconnectAsync());
        SetLightCommand = new Command<string>(async (light) => await SetLightAsync(light));
        NextStepCommand = new Command(async () => await NextStepAsync());

        StatusMessage = "Not connected";
    }

    private async Task ScanForDevicesAsync()
    {
        try
        {
            IsScanning = true;
            StatusMessage = "Scanning for devices...";
            Devices.Clear();

            var devices = await _bluetoothService.ScanForDevicesAsync();
            
            foreach (var device in devices)
            {
                Devices.Add(device);
            }

            StatusMessage = $"Found {devices.Count} device(s)";
        }
        catch (Exception ex)
        {
            StatusMessage = $"Scan failed: {ex.Message}";
        }
        finally
        {
            IsScanning = false;
        }
    }

    private async Task ConnectToDeviceAsync(BluetoothDevice device)
    {
        try
        {
            StatusMessage = $"Connecting to {device.Name}...";
            var success = await _bluetoothService.ConnectAsync(device);

            if (success)
            {
                IsConnected = true;
                ConnectedDeviceName = device.Name;
                StatusMessage = $"Connected to {device.Name}";
            }
            else
            {
                StatusMessage = "Connection failed";
            }
        }
        catch (Exception ex)
        {
            StatusMessage = $"Connection error: {ex.Message}";
        }
    }

    private async Task DisconnectAsync()
    {
        try
        {
            await _bluetoothService.DisconnectAsync();
            IsConnected = false;
            ConnectedDeviceName = string.Empty;
            StatusMessage = "Disconnected";
        }
        catch (Exception ex)
        {
            StatusMessage = $"Disconnect error: {ex.Message}";
        }
    }

    private async Task SetModeAsync(bool powerMode)
    {
        if (!IsConnected) return;

        try
        {
            var command = powerMode ? "MODE POWER" : "MODE TEST";
            await _bluetoothService.SendCommandAsync(command);
            StatusMessage = powerMode ? "Power mode activated" : "Test mode activated";
        }
        catch (Exception ex)
        {
            StatusMessage = $"Mode change failed: {ex.Message}";
        }
    }

    private async Task SetLightAsync(string light)
    {
        if (!IsConnected || IsPowerMode) return;

        try
        {
            await _bluetoothService.SendCommandAsync($"LIGHT {light}");
            SelectedLight = light;
            StatusMessage = $"Light set to {light}";
        }
        catch (Exception ex)
        {
            StatusMessage = $"Light change failed: {ex.Message}";
        }
    }

    private async Task NextStepAsync()
    {
        if (!IsConnected || IsPowerMode) return;

        try
        {
            await _bluetoothService.SendCommandAsync("NEXT");
            StatusMessage = "Next step";
        }
        catch (Exception ex)
        {
            StatusMessage = $"Next step failed: {ex.Message}";
        }
    }

    private void OnMessageReceived(object sender, string message)
    {
        MainThread.BeginInvokeOnMainThread(() =>
        {
            Messages.Add($"[{DateTime.Now:HH:mm:ss}] {message}");
            if (Messages.Count > 100)
            {
                Messages.RemoveAt(0);
            }
        });
    }

    private void OnConnectionChanged(object sender, bool isConnected)
    {
        MainThread.BeginInvokeOnMainThread(() =>
        {
            IsConnected = isConnected;
            if (!isConnected)
            {
                ConnectedDeviceName = string.Empty;
                StatusMessage = "Connection lost";
            }
        });
    }

    public event PropertyChangedEventHandler PropertyChanged;

    protected virtual void OnPropertyChanged([CallerMemberName] string propertyName = null)
    {
        PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
    }
}
