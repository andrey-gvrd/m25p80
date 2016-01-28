Library for M25P80 - 1MB Flash Memory IC.

This library is based on [this][1] API, but has a different implementation.

You'll need to supply:

1. Your own SPI function (`spi_transceive()`). I've used software version, ping me if you're interested, (it's pretty ugly);

2. Your own delay function (`delay_ms()`);

3. Optional: Your function for `DEBUG_SEND` macro. I've used both SEGGER RTT and hardware UART (STM's) before.

P.S.: Tests in `main.c` are provided mostly as API reference and haven't been run on actual hardware, but the library itself does work.

[1]: http://senstools.gforge.inria.fr/doxygen/group__m25p80.html
