### Library for M25P80 - 1MB Flash Memory IC.

This library is based on [this][1] API, but has a different implementation.

### Usage

You'll need to supply:

1. Your own SPI function (`spi_transceive()`). I've used software version, ping me if you're interested, (it's pretty ugly);

2. Your own delay function (`delay_ms()`);

3. Optional: Your function for `DEBUG_SEND` macro. I've used both SEGGER RTT and hardware UART (STM's) before.

P.S.: Tests in `main.c` are provided mostly as API reference and haven't been run on actual hardware, but the library itself does work.

### Internals

Let's look at M25P80 memory structure. Its 1MB are made up of 4096 pages, 256 bytes each.

There are several things to look out for:

- Bits can go 0 -> 1 individually, but 1 -> 0 only sector-wise

<pre>
┌─Sector 01─────────────┐
│ ┌────┐┌────┐   ┌────┐ │
│ │Page││Page│...│Page│ │
│ │0001││0002│   │0256│ │
│ └────┘└────┘   └────┘ │
├─Sector 02─────────────┤
│ ┌────┐┌────┐   ┌────┐ │
│ │Page││Page│...│Page│ │
│ │0257││0258│   │0512│ │
│ └────┘└────┘   └────┘ │
└───────────────────────┘
            :
┌─Sector 16─────────────┐
│ ┌────┐┌────┐   ┌────┐ │
│ │Page││Page│...│Page│ │
│ │3840││3841│   │4096│ │
│ └────┘└────┘   └────┘ │
└───────────────────────┘
</pre>

[1]: http://senstools.gforge.inria.fr/doxygen/group__m25p80.html
