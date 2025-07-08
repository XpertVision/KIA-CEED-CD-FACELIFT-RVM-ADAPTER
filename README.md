# KIA-CEED-CD-FACELIFT-RVM-ADAPTER
Arduino+CAN sketch for adaptation rear view camera from CEED CD PE (22MY facelift) to CEED CD non-facelift (20MY-21MY)

IMPORTANT: Suitable for all types of automatic transmissions. Tested with 7DCT. I do not guarantee 100% correct behavior on manual transmission.

Required:
1) Arduino Nano or another compatible AVR board
2) CAN board for Arduino with MCP2515
3) Arduino mcp2515 library (tested with version 1.2.1): https://github.com/autowp/arduino-mcp2515

Schema for connecting Arduino and CAN shield:<br/>
<pre>Arduino: D2  -> CAN shield: INT
Arduino: D13 -> CAN shield: SCK
Arduino: D11 -> CAN shield: SI
Arduino: D12 -> CAN shield: SO
Arduino: GND -> CAN shield: GND
Arduino: 5V  -> CAN shield: VCC</pre>

CAN board's CAN-H & CAN-L connect directly to the C-CAN bus in a car.
Arduino GND & VIN connect directly to the car's power line, which is enabled only when ACC is ON. (prevent battery power loss)
