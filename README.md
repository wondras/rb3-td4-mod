# rb3-td4-mod

An Arduino-based mod for using Roland TD-4 drums with the Rock Band 3 MIDI Pro Adapter

# Features

1. Open hi-hat mapped to Blue cymbal

Many songs are charted with closed hi-hats on yellow and open on blue. The TD-4 kit
doesn't allow mapping the MIDI note for open hi-hat to any of the notes the MPA accepts
for the blue cymbal. This mod replaces the default open hi-hat note from the TD-4 with
the ride cymbal note the MPA uses for blue.

2. Double crash hits mapped to Green+Yellow

Many songs have double crashes mapped to Green+Yellow. It's not very satisfying to use
the hi-hat as a crash. Normally both crash cymbals on the kit are mapped to the green
cymbal in the game, but if you hit both at the same time, the mod sends Green+Yellow
instead.


# Construction

To use this mod, cut the trace between the optoisolator and the main processor in the
MIDI Pro Adapter, and connect the serial pins of a 3.3V Arduino between them. Arduino RX
goes to the optoisolator; Arduino TX goes to the processor.

The Arduino Pro Mini fits nicely inside the MPA, and can be powered using test pads in
the MPA for ground and 3.3V power.

This allows us to modify or suppress the MIDI commands coming from the drum kit before
they are sent to the MPA CPU.


# Configuration

1. Use the default MIDI note map on the TD-4.
2. Enable hi-hat pedal in Rock Band 3 game settings


# Operation

Just play! You will need to keep your foot on the hi-hat pedal in order to hit yellow
notes, but otherwise it will play as normal.


