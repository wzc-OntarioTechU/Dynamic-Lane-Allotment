# Transaction/Transmission Spec

The purpose of this document is to describe the format of messages between the edge devices and the gateway.

## BT Strength messages (from edge to gateway)

These messages need to convey the station id, and pairs of the BT MAC address and signal strength.
They should be formatted as follows:
1. Start with station id
    1. Terminate with semicolon `;`
2. List of pairs
    1. Start with BT MAC
    2. Comma as separator `,`
    3. Signal Strength
    4. Terminate with semicolon `;`
    5. Move to next pair repeat from 2.1

## Lighting Signalling

These messages just need to convey to a station to enable its lights or disable them.
They should be formatted as follows:
- The station id
- A comma separator `,`
- A `0` or `1`
- A semicolon `;` terminator
