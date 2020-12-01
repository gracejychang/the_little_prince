# The Little Prince - A Lost Rose

FINAL MILESTONE DOCUMENTATION CAN BE FOUND HERE:
https://docs.google.com/document/d/1n9uFXPnAeAFsp7g4wUlMvMOfxu19ZIXuN32gXGw5f1s/edit?usp=sharing

NOTE: the game now uses opencv library. As such, users who pull the game from git will need to have opencv installed on their local machine in order to run the game. If using mac and homebrew is installed, users can install by running "brew install opencv@4" on their command line.

DONE FOR MILESTONE 4 (DEC. 6)
- implement prince attack
- implement enemy attack
- fix Windows platform bugs
- make narration
- make sure of user flow
- update audio
- update ground image
- animation for enemies
- visual indicator of max 5 lives
- wrong screen after prince die
- star bar improvement
- barrel explosion (advanced graphics)
- fix margin 
- fix title
- spawn enemies in a reasonable position

DONE FOR MILESTONE 3 (NOV. 8)
- Hearts, stars and enemies disappear when prince goes off screen and then comes back
- Another key is spawned after prince takes it and portal appears, leave the screen and then comes back (two key issue)
- If prince goes off screen and comes back repeatedly, key doesn't show up
- If prince goes off screen and comes back to screen with key in it, the key respawns in a different position
- Key generates on first screen of level, making gameplay too easy
- Key will show up next to wall, which makes portal spawn on a wall and make it impossible to advance to next level ):
- Sometimes enemies spawns on top of walls
- Star bar not showing on upper left corner of screen
- Implement more levels (generate .txt files)
- Implement background sound
- Make legit intro screen
- Make end screen for when prince dies
- Merge branch with other implementations
- put in arrow keys for where prince should advance
- on_key press implementation
- fix restart button
- prince/wall collision bounceback
- have a fixed number of enemies and stars spawned for each level (don't have all stars spawn an enemy because the game is really hard right now) 
- track how many of the total amount of stars have been collected (i.e. 1/3 stars collected etc.)
- make sure the position of the stars and enemies are far enough from the star position of the prince
- reset key bug: more and more enmeny and stars 

DONE FOR MILESTONE 2 (Oct 21)
- implemented tutorial level
- implement condition for not respawning stars when collected after leaving the map and coming back to it 
- implemented screen state (intro, in-btwn-level)
- fixed invisible portal bug
- heart bar for health (on diff branch)
- star bar for points (on diff branch)
- star animation when collected
- refactored 

