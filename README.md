# Osciloscope Snake

This report details the process of elaborating a system capable of playing the classic Snake game developed resorting to an oscilloscope to display graphics. The project was developed using an Arduino Uno and a digital oscilloscope with the display mode set to XY. A fixed priority preemptive micro-kernel was implemented to ensure stable and periodic refresh rate. A schedulability analysis of the various tasks was conducted, in order to guarantee the system met the real-time requirements.
