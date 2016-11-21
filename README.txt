

Assignment 3b: Input Handling in Pong
EGP-405-01
By David Jordan
Due 11/21/2016

Previously, the individual clients simulated their own movement and sent their positions to the server. But now, the client is only sending lists of "moves" to the server, and the server is fully authoritative, simulating ball AND player movement. The client accumulates input state and moves over time, sending an input packet once it has three moves. When the server receives these input packets, it adjusts player velocities and simulates their movement over time deltas. I had to change my speed units from pixels-per-frame to pixels-per-second (and acceleration to pixels-per-second-squared) to make them work better with time deltas of seconds. The server then sends updated positions back to the clients for them to display. Also, because input packets are only sent every three moves, and because I have not implemented any interpolation or client side prediction, the frame rate is visually 20 fps instead of 60 fps.
