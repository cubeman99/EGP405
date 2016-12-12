
Assignment 3c: Client-Side Prediction
EGP-405-01
By David Jordan
Due 12/11/2016


------------------------------------------------------------------------------
Controls:

Left/Right Arrows ... Move horizontally
Up Arrow ............ Jump
Page Up/Down ........ Adjust number of moves sent per input packet
P ................... Enable/disable Client-side Prediction
R ................... Enable/disable Server Reconciliation
E ................... Enable/disable Entity Interpolation
Escape .............. Quit game


------------------------------------------------------------------------------
Introduction:

I decided to go with a variation on the game pong called Slime Volleyball, which
was originally a singleplayer (or hotseat multiplayer) online game. In this game,
there are two teams of players, or "slimes", and a ball. The players must hit the
ball to bounce it over the net to have it land on their opponent's field in order
to score a point. The scoring team gets to serve the next round. Many players can
be on a single team, but there must be at least one player per team for the game
to start I believe Slime Volleyball was a suitable alternative to Pong because
they both have the same game concepts: players (slimes/paddles), and a ball. They
are also both "twitch" based games, where it is dependant on reflexes to play.

I used a simplified version of my own application framework in order to implement
this project. It has code for math, graphics, and input. It uses SDL, OpenGL,
glew, lodepng, and FreeType2. One side note is that it is currently using
OpenGL's fixed-function pipeline. I planned to move it to using shaders to do the
2D graphics, but I never did, although there are the classes for it (Shader,
Renderer, etc.).


------------------------------------------------------------------------------
Networking Overview:

The server is authoritative. The players, the ball, and the game reules are
handled by the server. There are multiple packets for events (player joined,
player left, team Scored, team serve, etc.), one for input (sent by the client),
and one for state replication (sent by the server). There are also NetworkManager
classes for the client and server, although I was having difficulty keeping them
de-coupled from the game code and ended up making them somewhat clunky.

I did implement a small amount of compression/serialization. In sending player
positions/velocities, I created serialization and serialization functions in the
Slime (player) class. Because this is a side-scroll game with gravity, the players
will frequently be on the ground. I took advantage of this by not serialization a
player's y-position if he is on the ground, and also adding a single bit to tell if
the player is on the ground or not. I did this similarly with player velocity,
checking if its x-y components are equal to zero.


--------------------------
Player Input & Movement:

The client is a dumb-terminal. It accumulates player input which it puts into
Moves. It groups moves into a MoveList, sent as an input packet to the server. The
server receives this packet and then simulates each move in the move-list. Each
move has time information to remember how much time passed between moves, so it
can be simulated easier by the server. The updated positions and velocities are
then broadcasted to all clients.


------------------------------------------------------------------------------
Client Side Prediction:

This game has an authoritative server. In order to add client side prediction, I
made it so both the client and server have a class called GameWorld, which contains
information about the state of the game, and functions to simulate it. This means
the client can also simulate the game locally, to predict what the results of its
inputs would be. 

By default the client queues up three frames of input/movement into a MoveList to
send in an input packet to the server. Once the server receives this packet, it
processes each of its moves and simulates player movement. After which, that
player's state is then sent back to the client, along with the current state of the
ball and every other player. While the client is waiting for a state update packet,
it simulates its own player's movement with the more immediate input it can gather.
Because the client and server are running the same code to simulate movement, they
should be mostly in-sync.

When the client receives the state update packet from the server, it then set its
position/velocity to the actual values which the server calculated. One problem
with this is that if the client and server are frequently diverging, then these
state updates will cause a jittery effect. This problem is handled with Server
Reconciliation.

Client side prediction can be enabled/disabled with the key "P".
With it disabled, 

The number of moves queued and sent per input packet can be adjusted
with "Page Up" and "Page Down".


------------------------------------------------------------------------------
Server Reconciliation:

This method comes from an article which I found very useful: "Fast-Paced
Multiplayer", by Gabriel Gambetta.

http://www.gabrielgambetta.com/fpm2.html

The problem is that when the client receives a state update packet to adjust its
player's state, this packet itself is in the past. Server Reconciliation is
responsible for determining at what point in the past was the state update from,
so it can then re-simulate all the moves that came afterwards. This keeps the
player state synchronized with the server without messing up the client's
prediction efforts.

To implement this, I assigning numbers to individual Moves. Each move is one
move-number above the previous. The client now keeps a separate, "uncreconciled"
MoveList which it adds moves to every frame, and erases moves from it upon
receiving a state update from the server. The server also now sends a move-number
of the last-received move from the client, so that the client knows exactly how
far back the server is. The client can then remove any move from the
uncreconciled MoveList with numbers smaller than the server's last-receive
move-number. Then it can re-simulate the remaining moves in that list,
re-predicting the player's updated state in the present.

Server Reconciliation be enabled/disabled with the key "R".
It can only take effect if client-side-prediction is also enabled.
With Server Reconciliation is disabled, a feint jittering can be noticed.


------------------------------------------------------------------------------
Entity Interpolation:

This was inspired by the Valve Developer Wiki Page called "Source Multiplayer
Networking".

https://developer.valvesoftware.com/wiki/Source_Multiplayer_Networking

In my game, a client only sends an input packet once every three frames (by
default). Once the server receives this packet, it simulates it all at once, and
broadcasts state updates for that player the very same frame. This means that all
other clients will see that player with blocky movement, and they can't predict
it's movement the same way they can predict their own players' movement.

The solution to this is for a client to simulate other players in the past, with
interpolation. Instead of immediately updating their poisitions, the client queues
up the player states, recording their time steps as well. Once the client has at
least to states recorded for a single player, it can then interpolate between the
two states' positions and velocities using the difference in time steps.

Currently, this is only implemented with player states, and not with the ball
state. I figured that since the ball is updated by the server at 60 fps, then it
isn't as important to interpolate.

Entity Interpolation be enabled/disabled with the key "E".

