namespace TrailerTesterApp.Services;

public interface IBluetoothService
{
    event EventHandler<string> MessageReceived;
    event EventHandler<bool> ConnectionChanged;
    
    Task<List<BluetoothDevice>> ScanForDevicesAsync();
    Task<bool> ConnectAsync(BluetoothDevice device);
    Task DisconnectAsync();
    Task SendCommandAsync(string command);
    bool IsConnected { get; }
    string ConnectedDeviceName { get; }
}

public class BluetoothDevice
{
    public string Name { get; set; }
    public string Id { get; set; }
    public int Rssi { get; set; }
}
