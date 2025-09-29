import asyncio
from bleak import BleakClient, BleakScanner

UART_RX_UUID = "6E400003-B5A3-F393-E0A9-E50E24DCCA9E" # RX characteristic UUID - shouldn't need to change?

def handle_rx(sender, data):
    text = data.decode().strip()
    try:
        #print(text)
        print(text)
        #nodeNum, ax, ay, az, gx, gy, gz = map(float, text.split(','))
        #nodeNum, ax, ay, az = map(float, text.split(','))
        #sprint(f"Node {int(nodeNum)}: Accel: ({ax:.2f}, {ay:.2f}, {az:.2f}) m/s² | Gyro {gx} {gy} {gz}")
    except Exception:
        print("FAIL: ", text)
        pass

async def connect_and_listen(address):
    while True:
        try:
            async with BleakClient(address) as client:
                print(f"Connected to {address}")
                await client.start_notify(UART_RX_UUID, handle_rx)
                while client.is_connected:
                    await asyncio.sleep(1)
        except Exception as e:
            print(f"Disconnected or failed to connect: {e}")
            print("Retrying in 3 seconds...")
            await asyncio.sleep(3)

async def main():
    print("Scanning for devices...")
    connectedDevices = []
    tasks = []
    expectedNumberOfDevices = 2
    
    while expectedNumberOfDevices > len(connectedDevices):
        devices = await BleakScanner.discover()
        for d in devices:
            if d and d.name and d.name.startswith("Feather") and d not in connectedDevices:
                print(f"Connecting to new device: {d.name} ({d.address})")
                connectedDevices.append(d)
                # Create a task to run connect_and_listen concurrently
                task = asyncio.create_task(connect_and_listen(d.address))
                tasks.append(task)
        if len(connectedDevices) < expectedNumberOfDevices:
            print("Still awaiting connections, scanning again in 3 seconds...")
            await asyncio.sleep(3)
    
    print(f"Connected to all {len(connectedDevices)} devices. Listening for data...")
    # Wait for all tasks to complete (they won't unless there's an unrecoverable error)
    await asyncio.gather(*tasks)

asyncio.run(main())
