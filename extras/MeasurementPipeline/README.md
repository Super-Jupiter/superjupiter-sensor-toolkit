# Optional combined measurement pipeline

Install BOTH packages: SuperJupiter Sensor Toolkit and SuperJupiter Embedded Reliability.
This extras example is intentionally outside examples/ so a normal single-package examples build
does not acquire an undeclared dependency. The core packages have no dependency on each other.

Synthetic readings and RAM storage only. Hardware validation pending. This does not acquire real
sensor values, implement network transport, or prove power-loss durability. It exercises all twenty
public components, including rollover, invalid values, dropped samples and acknowledgement retry.
Host result: 31 observations, 3 acknowledgements, 1100 covered ms. The pipeline header is identical
to the local host/ESP32/ESP32-S3/ESP32-C3 test fixture. Use ESP32 for this full example; the full
example is not claimed to fit Uno. Small independent module examples were compiled for Uno.
