# Bandwidth Monitor

## Description
A packet sniffer that uses a producer/consumer multithreaded model to capture data about the bandwidth used by a device
and compiles reports that are sent to the system administrator on a weekly basis.

## Setup
`libpcap` requires the following additional packages to be installed:
- `flex`
- `bison`

These packages can be installed using the following commands:
```bash
sudo apt install flex
sudo apt install bison
```
Then please make sure to pull the submodules using the following command:
```bash
git submodule update --init --recursive
```
## Build
To build the program, create a `build` directory and navigate into it. Then run:
```bash
cmake ..
make
```

## Execution
The program can now be run using:
```bash
sudo ./Bandwidth_Monitor 
```

To run it as a background process use:
```bash
sudo ./Bandwidth_Monitor &
```

## Sample Output
### Coming soon