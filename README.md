Connect 4 Game Using Nucleo Microcontroller

This project is a Connect 4 game implemented using C and a Nucleo microcontroller. The goal was to demonstrate proficiency in embedded systems programming, including bit manipulation, multiprogramming, and hardware integration. The game integrates LEDs, buttons, and an LCD display, all wired on a breadboard, to deliver a two-player experience with features like name input, high score tracking, and game restarts.

Features

    Two-Player Gameplay: Players can enter their names via serial input.
    High Score Tracking: Scores are recorded for competitive gameplay.
    Replayabiity: Reset the game and play again without reinitializing the hardware.
    Interactive Hardware: LEDs, buttons, and an LCD display create an engaging and functional experience.

Hardware and Implementation

    Microcontroller: Nucleo board used for input/output configuration through C programming.
    LCD Display: Used to display the game board, player names, and scores.
    Buttons and LEDs: Configured through breadboard wiring and used for token placement and feedback.
    Micro Speaker: For sound effects.
    Bit Manipulation: Output and input registers were configured using bit-shifting operations in C to handle game logic and hardware integration efficiently.

Collaboration and Contributions

As a team of three, we shared responsibilities while optimizing the use of the single microcontroller board during lab sessions:

Initial Setup: Collaboratively configured the buttons, LEDs, and LCD during lab sessions to establish the hardware foundation. Role Allocation: One member focused on implementing the draw_grid() function to dynamically generate the grid while reducing resource usage,
Developing the coin array logic for tracking token placement and Writing the winning condition checks for the game.
Another worked on button interactions and token dropping logic. I focused on: The visual aspect of the game, making the sprites for the animation. Music for the game and It's sound effects throughout.
All members contributed to debugging and refining the game logic collaboratively.

How to Play

Set Up the Hardware: Connect the buttons, LEDs, and LCD display to the Nucleo board using a breadboard as per the wiring diagram (to be included in the repository). Flash the microcontroller with the provided code.

Start the Game: Players enter their names through the serial input. Take turns pressing buttons to drop tokens into the grid. Watch the LEDs and LCD screen for visual feedback on moves.

Winning and Restarting: The game automatically checks for winning conditions after every turn. After a game ends, press the play again button, restart or quit.

Challenges and Solutions

    Hardware Limitations: Sharing a single Nucleo board required careful scheduling and collaboration to test code and hardware configurations.
    Resource Constraints: Redrawing the entire game grid consumed significant resources. To optimize, a single square of the grid was defined and reused via the draw_grid() function.
    Adaptability: Each team member frequently adapted to new tasks, including troubleshooting buttons, optimizing logic, and refining animations, ensuring a seamless experience.

