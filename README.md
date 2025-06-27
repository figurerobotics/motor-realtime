# motor-realtime

## Getting the code

Note this project uses git submodules, clone with:
```console
git clone --recurse-submodules https://github.com/unhuman-io/motor-realtime.git
```
And pull with
```console
git pull --recurse-submodules
```
or set `git config submodule.recurse=true`.

## Introduction

This project contains helps interface with a kernel driver to communicate to motor 
controllers over USB and an API to help communicate with the motor 
controllers. The kernel driver is available at github at unhuman-io/usb_rt_driver. 
The API will install a shared library motor_manager.so and include files: motor.h 
and motor_manager.h. An example is installed in `share/motor-realtime/example`. A 
pybind11 python interface is created and installed in `share/motor-realtime` 
and python examples are in `share/motor-realtime/python` 

`motor_util` is a command line utility that is also installed. It uses 
the motor_manager library and enumerates any connected devices. 
Example:
```console
$ motor_util
1 connected motor
         Dev      Name  Serial number                             Version           Path
----------------------------------------------------------------------------------------
   /dev/mtr1        J1           0001          0.2.1 Sep  4 2019 13:48:21    1-4.2.1.3.1
```
`motor_util` also allows for simple reading and writing to the motors with the set and read commands. Tab completion and -h help is supported. Note that when setting you always need to set the mode or else it 
will default to `open`. Some examples are: 
```console
$ motor_util set --mode current --current 1
$ motor_util read
```
A series of more example commands in in [motor_util_examples.md](doc/motor_util_examples.md)

`motor_usbmon` is a tool for reading USB traffic to and from the motor controllers. 
It uses the Linux kernel usbmon modules for getting the traffic data and `motor_usbmon` 
parse and outputs the data in text csv type format. An example usage is:
```console
$ motor_util read --read-write-statistics > /dev/null &  # generates many USB reads and writes
$ sudo modprobe usbmon
$ lsusb
...
Bus 001 Device 047: ID 3293:0100 Unhuman Inc. Obot G474 Motor
...
$ sudo motor_usbmon -d 47
...
 47 So, 672393,  2, 36131, 0, 0, 0, 0, 0, 0, 
 47 Co, 672477,  2, 36131, 0, 0, 0, 0, 0, 0, 
 47 Si, 673213, 82, 
 47 Ci, 673380, 82, 1081646015, 36131, -0.654975, -0.121237, -0.693877, -0.693877, 2, 0, 3.88449, 3.88044, 
...
```
This shows a submit out (So) on endpoint 2 with the `host_timestamp` data 36131 plus a text version of 
the MotorCommand struct. The callback out (Co) is received 84 us later meaning that's the time time a 
successful USB ACK was received from the motor controller. The request for a MotorStatus
packet is a submit in (Si) and receives a callback in (Ci) 167 us later with the associated status. 
Note that 36131 is returned from the motor controller which is the basic loopback handshaking 
implemented in the motor controller. `motor_usbmon` is most helpful for debugging a separate 
process that is communicating to the motor controllers.

Install the command line tools using:
```console
sudo apt install -y linux-headers-$(uname -r)
curl https://raw.githubusercontent.com/unhuman-io/obot/main/install-obot.sh > install-obot.sh
chmod +x install-obot.sh
./install-obot.sh
```

## Python module installation
Change to the directory you want your virtual environment, then create and activate venv:
```console
python3 -m venv .venv
source .venv/bin/activate
```

Checkout the user/chrisxu/python-venv branch of figurerobotics/motor-realtime:
https://github.com/figurerobotics/motor-realtime/tree/user/chrisxu/python-venv 
```console
git clone https://github.com/figurerobotics/motor-realtime
cd motor-realtime
```


Remove previous builds and build with virtual environment activated: 
```console
rm -rf build
mkdir build && cd build
cmake -DBUILD_PYTHON_API=1 -DCMAKE_INSTALL_PREFIX=$VIRTUAL_ENV ..
make -j
sudo make install
```

Check if motor python module works:
```console
python3
```
```python
>>> import motor
>>> dir(motor)
['Admittance', 'Chirp', 'ClearFaults', 'Command', 'Crash', 'Current', 'CurrentTuning', 'CurrentTuningCommand', 'Damped', 'DriverDisable', 'DriverEnable', 'Fault', 'FindLimits', 'HardwareBrake', 'Impedance', 'JointPosition', 'ModeDesired', 'Motor', 'MotorError', 'MotorFlags', 'MotorManager', 'MotorStatusLarge', 'NoMode', 'Open', 'PhaseLock', 'Position', 'PositionTuning', 'PositionTuningCommand', 'Random', 'Reset', 'RoundRobinData', 'Sine', 'Sleep', 'Square', 'State', 'Status', 'StepperCurrent', 'StepperMode', 'StepperTuning', 'StepperVelocity', 'StepperVelocityCommand', 'StepperVoltage', 'TextAPIItem', 'Torque', 'Triangle', 'Tuning', 'TuningCommand', 'TuningMode', 'Velocity', 'Voltage', '__doc__', '__file__', '__loader__', '__name__', '__package__', '__spec__', 'diff_encoder', 'diff_mcu_time', 'get_config_dir', 'max_api_packet_size', 'mode_color']
```

Install pybind11-stubgen for VScode autocomplete:
```console
pip install pybind11-stubgen
```

Find where the motor python module .so file is stored and cd there:
```console
python -c "import motor; print(motor.__file__)"
```
You may have a different python version: 
```console
cd .venv/lib/python3.12/site-packages
```

Build the stubs for the motor module:
```console
pybind11-stubgen  motor -o . \
  --enum-class-locations "^StepperMode$:motor.StepperMode" \
  --enum-class-locations "^ModeDesired$:motor.ModeDesired"
```

Reload VSCode. Python scripts that import motor should now autocomplete :D
