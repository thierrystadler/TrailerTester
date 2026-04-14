using Plugin.BLE;
using Plugin.BLE.Abstractions.Contracts;
using Plugin.BLE.Abstractions.EventArgs;
using Plugin.BLE.Abstractions.Exceptions;
using System.Text;

namespace TrailerTesterApp.Services;

public class BluetoothService : IBluetoothService
{
    private readonly IAdapter _adapter;
    private readonly IBluetoothLE _bluetoothLE;
    private IDevice _connectedDevice;
    private ICharacteristic _txCharacteristic;
    private ICharacteristic _rxCharacteristic;
    private readonly List<BluetoothDevice> _discoveredDevices = new();

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

        if (!_bluetoothLE.IsOn)
        {
            throw new Exception("Bluetooth is not enabled");
        }

        await _adapter.StartScanningForDevicesAsync();
        await Task.Delay(5000);
        await _adapter.StopScanningForDevicesAsync();

        return _discoveredDevices.ToList();
    }

    private void OnDeviceDiscovered(object sender, DeviceEventArgs e)
    {
        if (!string.IsNullOrEmpty(e.Device.Name) && 
            e.Device.Name.Contains("TrailerTester", StringComparison.OrdinalIgnoreCase))
        {
            var device = new BluetoothDevice
            {
                Name = e.Device.Name,
                Id = e.Device.Id.ToString(),
                Rssi = e.Device.Rssi
            };

            if (!_discoveredDevices.Any(d => d.Id == device.Id))
            {
                _discoveredDevices.Add(device);
            }
        }
    }

    public async Task<bool> ConnectAsync(BluetoothDevice device)
    {
        try
        {
            var bleDevice = _adapter.ConnectedDevices.FirstOrDefault(d => d.Id.ToString() == device.Id);
            
            if (bleDevice == null)
            {
                var knownDevice = await _adapter.ConnectToKnownDeviceAsync(Guid.Parse(device.Id));
                bleDevice = knownDevice;
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
            System.Diagnostics.Debug.WriteLine($"Connection failed: {ex.Message}");
            return false;
        }
    }

    private async Task SetupSerialCommunicationAsync()
    {
        var services = await _connectedDevice.GetServicesAsync();
        
        foreach (var service in services)
        {
            var characteristics = await service.GetCharacteristicsAsync();
            
            foreach (var characteristic in characteristics)
            {
                if (characteristic.CanWrite)
                {
                    _txCharacteristic = characteristic;
                }
                
                if (characteristic.CanUpdate)
                {
                    _rxCharacteristic = characteristic;
                    _rxCharacteristic.ValueUpdated += OnCharacteristicValueUpdated;
                    await _rxCharacteristic.StartUpdatesAsync();
                }
            }
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
