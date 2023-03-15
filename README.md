# Analog pulse detector

This Arduino sketch detects and counts pulses using the board's ADC inputs. It
writes to the serial port (to a connected PC) when pulses are detected, and also
debugging output.

I'm using this to monitor our water meters, which have pulse outputs (reed
switches), and are currently connected to heat meters, which I don't wish to
disconnect. Thus I need to be able to detect LOW pulses (to ~0V) without
applying a pullup, as that would disturb the heat meters (and possibly damage
them).

Configuration is by modifying the [sketch](analog_pulse_detector.ino) and
changing the following constants defined at the top:

* `DEBUG`: enables debug logging of current values, and additional debugging
  messages/comments.
* `NUM_CHANNELS`: the number of ADC pins (pulsing devices) to monitor.
* `CHANNEL_PINS`: which ADC pin to sample for each channel (e.g. A0, A1, A2...).
* `ENABLE_INTERNAL_PULLUP`: true to enable the internal pull-up to 5V on each
  channel, false to disable it.
* `LOG_CURRENT_VALUES_INTERVAL`: how often to debug log the current values, in
  milliseconds.
* `MIN_PULSE_LENGTH`: number of milliseconds that new state must be held to
  record it.
* `PULSE_VALUE_THRESHOLD`: the ADC reading above which to register a HIGH value
  (below this is LOW). The ADC range is 0-1023 which usually represents 0-5V, so
  100 is about 0.5V.
* `COUNT_FALLING_EDGE`: if true, a pulse will be registered on its falling edge
  (end of a HIGH pulse, or start of a LOW pulse), and the time since the last
  pulse will be the time since the last falling edge.
