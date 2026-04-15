using Plugin.BLE;
using Plugin.BLE.Abstractions.Contracts;
using Plugin.BLE.Abstractions.EventArgs;
using Plugin.BLE.Abstractions;
using Plugin.BLE.Abstractions.Exceptions;
using System.Text;

namespace TrailerTesterApp.Services;

public class BluetoothService : IBluetoothService
{
    private static readonly Guid UartServiceUuid = Guid.Parse("6E400001-B5A3-F393-E0A9-E50E24DCCA9E");
    private static readonly Guid UartRxUuid = Guid.Parse("6E400002-B5A3-F393-E0A9-E50E24DCCA9E");
    private static readonly Guid UartTxUuid = Guid.Parse("6E400003-B5A3-F393-E0A9-E50E24DCCA9E");

    private readonly IAdapter _adapter;
    private readonly IBluetoothLE _bluetoothLE;
    private IDevice _connectedDevice;
    private ICharacteristic _txCharacteristic;
    private ICharacteristic _rxCharacteristic;
    private readonly List<BluetoothDevice> _discoveredDevices = new();
    private readonly Dictionary<string, IDevice> _nativeDevices = new();
    private CancellationTokenSource _scanCts;

    public event EventHandler<string> MessageReceived;
    public event EventHandler<bool> ConnectionChanged;

    public bool IsConnected => _connectedDevice?.State == Plugin.BLE.Abstractions.DeviceState.Connected;
    public string ConnectedDeviceName => _connectedDevice?.Name ?? string.Empty;

    public BluetoothService()
    {
        _bluetoothLE = CrossBluetoothLE.Current;
        _adapter = CrossBluetoothLE.Current.Adapter;
        _adapter.DeviceDiscovered += OnDeviceDiscovered;
        _adapter.DeviceConnected += OnDeviceConnected;
        _adapter.DeviceDisconnected += OnDeviceDisconnected;
    }

    public async Task<List<BluetoothDevice>> ScanForDevicesAsync()
    {
        _discoveredDevices.Clear();
        _nativeDevices.Clear();
        _scanCts?.Cancel();
        _scanCts = new CancellationTokenSource();

        await EnsurePermissionsAsync();

        if (!_bluetoothLE.IsOn)
        {
            throw new Exception("Bluetooth is not enabled");
        }

        _adapter.ScanTimeout = 5000;

        try
        {
            await _adapter.StartScanningForDevicesAsync(
                serviceUuids: new[] { UartServiceUuid },
                cancellationToken: _scanCts.Token);
        }
        catch (OperationCanceledException)
        {
            // Scan was stopped early because a device was found
        }

        return _discoveredDevices.ToList();
    }

    private void OnDeviceDiscovered(object sender, DeviceEventArgs e)
    {
        var name = e.Device.Name;
        if (string.IsNullOrEmpty(name)) return;

        var device = new BluetoothDevice
        {
            Name = name,
            Id = e.Device.Id.ToString(),
            Rssi = e.Device.Rssi
        };

        if (!_discoveredDevices.Any(d => d.Id == device.Id))
        {
            _discoveredDevices.Add(device);
            _nativeDevices[device.Id] = e.Device;
            _scanCts?.Cancel();
        }
    }

    private static async Task EnsurePermissionsAsync()
    {
#if ANDROID
        var status = await Permissions.CheckStatusAsync<Permissions.Bluetooth>();
        if (status != PermissionStatus.Granted)
        {
            status = await Permissions.RequestAsync<Permissions.Bluetooth>();
        }

        var locationStatus = await Permissions.CheckStatusAsync<Permissions.LocationWhenInUse>();
        if (locationStatus != PermissionStatus.Granted)
        {
            locationStatus = await Permissions.RequestAsync<Permissions.LocationWhenInUse>();
        }

        if (status != PermissionStatus.Granted || locationStatus != PermissionStatus.Granted)
        {
            throw new Exception("Bluetooth and location permissions are required for scanning");
        }
#elif IOS
        // iOS permissions are handled via Info.plist and system prompts
        await Task.CompletedTask;
#else
        await Task.CompletedTask;
#endif
    }

    public async Task<bool> ConnectAsync(BluetoothDevice device)
    {
        try
        {
            await EnsurePermissionsAsync();

            // Stop any ongoing scan — Android won't connect while scanning
            await _adapter.StopScanningForDevicesAsync();
            await Task.Delay(200);

            var connectParams = new ConnectParameters(forceBleTransport: true);

            var bleDevice = _adapter.ConnectedDevices.FirstOrDefault(d => d.Id.ToString() == device.Id);

            if (bleDevice == null && _nativeDevices.TryGetValue(device.Id, out var nativeDevice))
            {
                System.Diagnostics.Debug.WriteLine($"Connecting via native device...");
                await _adapter.ConnectToDeviceAsync(nativeDevice, connectParams);
                bleDevice = nativeDevice;
            }

            if (bleDevice == null)
            {
                System.Diagnostics.Debug.WriteLine($"Connecting via known device ID...");
                bleDevice = await _adapter.ConnectToKnownDeviceAsync(Guid.Parse(device.Id), connectParams);
            }

            if (bleDevice != null)
            {
                _connectedDevice = bleDevice;
                await SetupSerialCommunicationAsync();
                return true;
            }

            return false;
        }
        catch (DeviceConnectionException ex)
        {
            System.Diagnostics.Debug.WriteLine($"Connection failed (DeviceConnectionException): {ex.Message}");
            return false;
        }
        catch (Exception ex)
        {
            System.Diagnostics.Debug.WriteLine($"Connection failed: {ex.GetType().Name}: {ex.Message}");
            return false;
        }
    }

    private async Task SetupSerialCommunicationAsync()
    {
        var service = await _connectedDevice.GetServiceAsync(UartServiceUuid);
        if (service == null)
        {
            throw new Exception("UART service not found on device");
        }

        _txCharacteristic = await service.GetCharacteristicAsync(UartRxUuid);
        _rxCharacteristic = await service.GetCharacteristicAsync(UartTxUuid);

        if (_rxCharacteristic != null && _rxCharacteristic.CanUpdate)
        {
            _rxCharacteristic.ValueUpdated += OnCharacteristicValueUpdated;
            await _rxCharacteristic.StartUpdatesAsync();
        }
    }

    private void OnCharacteristicValueUpdated(object sender, CharacteristicUpdatedEventArgs e)
    {
        var data = e.Characteristic.Value;
        var message = Encoding.UTF8.GetString(data);
        MessageReceived?.Invoke(this, message);
    }

    public async Task DisconnectAsync()
    {
        if (_connectedDevice != null)
        {
            if (_rxCharacteristic != null)
            {
                await _rxCharacteristic.StopUpdatesAsync();
            }
            
            await _adapter.DisconnectDeviceAsync(_connectedDevice);
            _connectedDevice = null;
            _txCharacteristic = null;
            _rxCharacteristic = null;
        }
    }

    public async Task SendCommandAsync(string command)
    {
        if (_txCharacteristic == null || !IsConnected)
        {
            throw new Exception("Not connected to device");
        }

        var data = Encoding.UTF8.GetBytes(command + "\n");
        await _txCharacteristic.WriteAsync(data);
    }

    private void OnDeviceConnected(object sender, DeviceEventArgs e)
    {
        ConnectionChanged?.Invoke(this, true);
    }

    private void OnDeviceDisconnected(object sender, DeviceEventArgs e)
    {
        ConnectionChanged?.Invoke(this, false);
    }
}
