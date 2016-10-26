

Assignment 3: Create a basic pong game
EGP-405-01
By David Jordan
Due 10/26/2016

I decided to go with a variation on the game pong called Slime Volleyball, which was originally a singleplayer (or hotseat multiplayer) online game. In this game, there are two teams of players, or "slimes", and a ball. The players must hit the ball to bounce it over the net to have it land on their opponent's field in order to score a point. The scoring team gets to serve the next round. I believe Slime Volleyball was a suitable alternative to Pong because they both have the same game concepts: players (slimes/paddles), and a ball. They are also both "twitch" based games, where it is dependant on reflexes to play.

In terms of replication, the server initializes the world's metrics and rules in a GameConfig class, and sends that to the clients upon connection (the whole class is serialized). Both client and server play their part in updating the world state. The client keeps track of its own position and velocity, updating them based on key presses, and then sending that info to the server. The server holds the positions of the ball and all players, and it performs the ball physics, then relaying the ball and player positions back to the clients. 

I did implement a small amount of compression/serialization. In sending player positions/velocities, I created serialization and serialization functions in the Slime class. Because this is a side-scoll game with gravity, the players will frequently be on the ground. I took advantage of this by not serialization a player's y-position if he is on the ground, and also adding a single bit to tell if the player is on the ground or not. I did this similarly with player velocity, checking if its x-y components are equal to zero.

