Introduction
------
Fixes for viewing demos and streaming from SourceTV for the client. Only for games `left4dead2` and `left4dead`

Installation
------
1. Get [latest sourcetvsupport-client release](https://github.com/A1mDev/sourcetvsupport-client/actions) (requires authentication) for your OS (`currently only supported on windows`)
2. Extract the zip file to folder `left4dead2/addons` or `left4dead/addons`
3. Add `-insecure` to your game launch options
4. Now you can view broadcasts and demos recorded by SourceTV

Notes
------
There is a limitation you will not be able to join any server with `VAC anti-cheat`, you will need to exit the game and remove the launch option `-insecure`

Fixed Issues
------
1) Missing arms models/blinking world model attachments during SourceTV footage playback [L4D1, L4D2]
2) Various sounds and effects (for example, explosions, destruction of the tank rock, etc) appearing at zero coordinates on the map  [L4D1, L4D2]
3) Missing body parts from infected commons, these body parts appeared at zero coordinates on the map [L4D1, L4D2]
5) Crash when minimizing the game while watching a demo or stream from SourceTV [L4D1, L4D2]
7) Wrong camera position, camera rotation, wrong view (first person, although it should be third person) in all cases. A few examples: during a player's incapacitation, if you are playing as an infected and attacking a victim, if you are a survivor and healing a teammate, if you are picking up a stone while playing a tank, etc [L4D1, L4D2]

Known Issues
------
1) When someone shoots, any player, flash effects due to shots always appear on the screen in different places. It doesn't matter if you're spectating a player or in a different view mode [L4D1, L4D2]
2) Entity 'env_fog_controller' is not displayed even if we are close to its location [L4D1?, L4D2]
