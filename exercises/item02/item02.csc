<?xml version="1.0" encoding="UTF-8"?>
<cooja-simulation>

    <!-- Simulation Configuration -->
    <sim-config>
        <name>Item02 Simulation</name>
        <description>Button press simulation with temperature reading and LED control.</description>
        <time>300</time> <!-- Set total simulation time to 300 seconds -->
        <timeunit>seconds</timeunit>
    </sim-config>

    <!-- Network Configuration -->
    <network>
        <node>
            <type>mote</type>
            <mote-type>firefly</mote-type> <!-- Use the Firefly mote type (adjust according to the actual hardware) -->
            <position>
                <x>0</x>
                <y>0</y>
            </position>
            <application>
                <file>../item02.c</file> <!-- Path to your C file -->
                <mote-type>firefly</mote-type>
            </application>
        </node>
    </network>

    <!-- Radio Configuration -->
    <radio>
        <radio-type>cc2420</radio-type> <!-- Radio type, adjust as needed -->
        <tx-power>0</tx-power> <!-- Transmit power (adjust based on your needs) -->
    </radio>

    <!-- Sensor Configuration -->
    <sensors>
        <!-- Temperature Sensor (simulated) -->
        <sensor>
            <sensor-type>temperature</sensor-type> <!-- Sensor type -->
            <activation-time>0</activation-time> <!-- Activation time in seconds -->
        </sensor>
    </sensors>

    <!-- Clock Configuration -->
    <clock>
        <time>100</time> <!-- Time step for simulation (in milliseconds) -->
    </clock>

    <!-- Serial Configuration -->
    <serial>
        <port>COM1</port> <!-- Serial port for output -->
    </serial>

</cooja-simulation>
