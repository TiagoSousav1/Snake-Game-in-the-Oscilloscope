# Osciloscope Snake

This project was developed using an Arduino Uno and a digital oscilloscope with the display mode set to XY. A fixed priority preemptive micro-kernel was implemented to ensure stable and periodic refresh rate. A schedulability analysis of the various tasks was conducted, in order to guarantee the system met the real-time requirements. For more details please read the "SEMB - report.pdf" file.

The final result can be observed in [a video uploaded to YouTube](https://www.youtube.com/watch?v=Z6TQhf5sGj8), where the game is smoothly running and features such as the game ending, the snake eating fruit and growing, the border detection and the user input flawlessly working. Moreover, the schedulability validates the 40Hz refresh rate of the game, under the given maximum score of 29 fruits, which was a goal set to ensure fluid gameplay.
